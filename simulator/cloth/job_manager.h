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

namespace nv {
namespace cloth {
class Solver;
}
}  // namespace nv

namespace vox {
namespace cloth {
/// Dummy task that can be used as end node in a task graph.
class DummyTask : public physx::PxTask {
public:
    DummyTask() {
        mFinished = false;
        mTm = nullptr;
    }

    DummyTask(physx::PxTaskManager *tm) {
        mFinished = false;
        mTm = tm;
        mTm->submitUnnamedTask(*this);
    }

    ~DummyTask() {
        mTm = nullptr;
    }  // Way to catch race conditions. Will usually crash on nullptr if the task gets deleted before the taskmanager is
       // done with it.

    virtual void run() override {}

    virtual void release() override {
        physx::PxTask::release();
        mFinished = true;
        mWaitEvent.notify_all();
    }

    virtual const char *getName() const override { return "DummyTask"; }

    void Reset(physx::PxTaskManager *tm) {
        mFinished = false;
        mTm = tm;
        mTm->submitUnnamedTask(*this);
    }

    /// Use Wait to block the calling thread until this task is finished and save to delete
    void Wait() {
        std::mutex eventLock;
        std::unique_lock<std::mutex> lock(eventLock);
        while (!mFinished) {
            mWaitEvent.wait(lock);
        }
    }

private:
    std::condition_variable mWaitEvent;
    bool mFinished;
};

class CpuDispatcher : public physx::PxCpuDispatcher {
    virtual void submitTask(physx::PxBaseTask &task) {
        task.run();
        task.release();
    }

    virtual uint32_t getWorkerCount() const { return 1; }
};

class JobManager;

class Job {
public:
    Job() = default;

    Job(const Job &);

    ~Job() { mValid = false; }

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

    std::function<void(Job *)> mFunction;
    JobManager *mParent;
    std::atomic_int mRefCount;

    bool mFinished;
    std::mutex mFinishedLock;
    std::condition_variable mFinishedEvent;
    bool mValid = true;
};

// this Job is a dependency to another job
class JobDependency : public Job {
public:
    void SetDependentJob(Job *job) { mDependendJob = job; }

    virtual void Execute() override {
        auto dependendJob = mDependendJob;
        Job::Execute();
        dependendJob->RemoveReference();
    }

private:
    Job *mDependendJob;
};

class JobManager {
public:
    JobManager() {
        mWorkerCount = 8;
        mWorkerThreads = new std::thread[mWorkerCount];
        mQuit = false;

        for (int i = 0; i < mWorkerCount; i++) mWorkerThreads[i] = std::thread(JobManager::WorkerEntryPoint, this);
    }

    ~JobManager() {
        if (!mQuit) Quit();
    }

    void Quit() {
        std::unique_lock<std::mutex> lock(mJobQueueLock);
        mQuit = true;
        lock.unlock();
        mJobQueueEvent.notify_all();
        for (int i = 0; i < mWorkerCount; i++) {
            mWorkerThreads[i].join();
        }
        delete[] mWorkerThreads;
    }

    template <int count, typename F>
    void ParallelLoop(F const &function) {
        /*for(int i = 0; i < count; i++)
         function(i);*/
        Job finalJob;
        finalJob.Initialize(this, std::function<void(Job *)>(), count);
        JobDependency jobs[count];
        for (int j = 0; j < count; j++) {
            jobs[j].Initialize(this, [j, &finalJob, function](Job *) { function(j); });
            jobs[j].SetDependentJob(&finalJob);
            jobs[j].RemoveReference();
        }
        finalJob.Wait();
    }

    static void WorkerEntryPoint(JobManager *parrent);

private:
    friend class Job;

    void Submit(Job *job);

    int mWorkerCount;
    std::thread *mWorkerThreads;

    std::mutex mJobQueueLock;
    std::queue<Job *> mJobQueue;
    std::condition_variable mJobQueueEvent;
    bool mQuit;
};

class MultithreadedSolverHelper {
public:
    void Initialize(nv::cloth::Solver *solver, JobManager *jobManager);

    void StartSimulation(float dt);

    void WaitForSimulation();

private:
    Job mStartSimulationJob;
    Job mEndSimulationJob;
    std::vector<JobDependency> mSimulationChunkJobs;

    float mDt;

    nv::cloth::Solver *mSolver;
    JobManager *mJobManager;
};

}  // namespace cloth
}  // namespace vox