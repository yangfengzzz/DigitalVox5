//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <task/PxTask.h>
#include <task/PxTaskManager.h>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <regex>
#include <thread>
#include <vector>

#include "simulator/cloth/callback_implementations.h"
#include "vox.cloth/foundation/PxErrorCallback.h"
#include "vox.cloth/foundation/PxVec3.h"
#include "vox.cloth/foundation/PxVec4.h"

namespace nv::cloth {
class Solver;
}  // namespace nv::cloth

namespace vox::cloth {
/// Dummy task that can be used as end node in a task graph.
class DummyTask : public physx::PxTask {
public:
    DummyTask() {
        m_finished_ = false;
        mTm = nullptr;
    }

    explicit DummyTask(physx::PxTaskManager *tm) {
        m_finished_ = false;
        mTm = tm;
        mTm->submitUnnamedTask(*this);
    }

    ~DummyTask() override {
        mTm = nullptr;
    }  // Way to catch race conditions. Will usually crash on nullptr if the task gets deleted before the task-manager
       // is done with it.

    void run() override {}

    void release() override {
        physx::PxTask::release();
        m_finished_ = true;
        m_wait_event_.notify_all();
    }

    [[nodiscard]] const char *getName() const override { return "DummyTask"; }

    void Reset(physx::PxTaskManager *tm) {
        m_finished_ = false;
        mTm = tm;
        mTm->submitUnnamedTask(*this);
    }

    /// Use Wait to block the calling thread until this task is finished and save to delete
    void Wait() {
        std::mutex event_lock;
        std::unique_lock<std::mutex> lock(event_lock);
        while (!m_finished_) {
            m_wait_event_.wait(lock);
        }
    }

private:
    std::condition_variable m_wait_event_;
    bool m_finished_;
};

class CpuDispatcher : public physx::PxCpuDispatcher {
    void submitTask(physx::PxBaseTask &task) override {
        task.run();
        task.release();
    }

    [[nodiscard]] uint32_t getWorkerCount() const override { return 1; }
};

class JobManager;

class Job {
public:
    Job() = default;

    Job(const Job &);

    ~Job() { m_valid_ = false; }

    void Initialize(JobManager *parent,
                    std::function<void(Job *)> function = std::function<void(Job *)>(),
                    int refcount = 1);

    void Reset(int refcount = 1);  // Call this before reusing a job that doesn't need to be reinitialized
    virtual void Execute();

    void AddReference();

    void RemoveReference();

    void Wait();  // Block until job is finished
private:
    virtual void ExecuteInternal() {}

    std::function<void(Job *)> m_function_;
    JobManager *m_parent_{};
    std::atomic_int m_ref_count_{};

    bool m_finished_{};
    std::mutex m_finished_lock_;
    std::condition_variable m_finished_event_;
    bool m_valid_ = true;
};

// this Job is a dependency to another job
class JobDependency : public Job {
public:
    void SetDependentJob(Job *job) { m_depended_job_ = job; }

    void Execute() override {
        auto depended_job = m_depended_job_;
        Job::Execute();
        depended_job->RemoveReference();
    }

private:
    Job *m_depended_job_;
};

class JobManager {
public:
    JobManager() {
        m_worker_count_ = 8;
        m_worker_threads_ = new std::thread[m_worker_count_];
        m_quit_ = false;

        for (int i = 0; i < m_worker_count_; i++)
            m_worker_threads_[i] = std::thread(JobManager::WorkerEntryPoint, this);
    }

    ~JobManager() {
        if (!m_quit_) Quit();
    }

    void Quit() {
        std::unique_lock<std::mutex> lock(m_job_queue_lock_);
        m_quit_ = true;
        lock.unlock();
        m_job_queue_event_.notify_all();
        for (int i = 0; i < m_worker_count_; i++) {
            m_worker_threads_[i].join();
        }
        delete[] m_worker_threads_;
    }

    template <int count, typename F>
    void ParallelLoop(F const &function) {
        /*for(int i = 0; i < count; i++)
         function(i);*/
        Job final_job;
        final_job.Initialize(this, std::function<void(Job *)>(), count);
        JobDependency jobs[count];
        for (int j = 0; j < count; j++) {
            jobs[j].Initialize(this, [j, &final_job, function](Job *) { function(j); });
            jobs[j].SetDependentJob(&final_job);
            jobs[j].RemoveReference();
        }
        final_job.Wait();
    }

    static void WorkerEntryPoint(JobManager *parent);

private:
    friend class Job;

    void Submit(Job *job);

    int m_worker_count_;
    std::thread *m_worker_threads_;

    std::mutex m_job_queue_lock_;
    std::queue<Job *> m_job_queue_;
    std::condition_variable m_job_queue_event_;
    bool m_quit_;
};

class MultithreadedSolverHelper {
public:
    void Initialize(nv::cloth::Solver *solver, JobManager *job_manager);

    void StartSimulation(float dt);

    void WaitForSimulation();

private:
    Job m_start_simulation_job_;
    Job m_end_simulation_job_;
    std::vector<JobDependency> m_simulation_chunk_jobs_;

    float m_dt_;

    nv::cloth::Solver *m_solver_;
    JobManager *m_job_manager_;
};

}  // namespace vox::cloth