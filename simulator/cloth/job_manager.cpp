//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "simulator/cloth/job_manager.h"

#include <utility>

#include "vox.cloth/NvCloth/Solver.h"

namespace vox::cloth {
void Job::Initialize(JobManager *parent, std::function<void(Job *)> function, int refcount) {
    m_function_ = std::move(function);
    m_parent_ = parent;
    Reset(refcount);
}

Job::Job(const Job &job) {
    m_function_ = job.m_function_;
    m_parent_ = job.m_parent_;
    m_ref_count_.store(job.m_ref_count_);
    m_finished_ = job.m_finished_;
}

void Job::Reset(int refcount) {
    m_ref_count_ = refcount;
    m_finished_ = false;
}

void Job::Execute() {
    if (m_function_)
        m_function_(this);
    else
        ExecuteInternal();

    {
        std::lock_guard<std::mutex> lock(m_finished_lock_);
        m_finished_ = true;
    }
    m_finished_event_.notify_one();
}

void Job::AddReference() { m_ref_count_++; }

void Job::RemoveReference() {
    int ref_count = --m_ref_count_;
    if (0 == ref_count) {
        m_parent_->Submit(this);
    }
    assert(ref_count >= 0);
}

void Job::Wait() {
    std::unique_lock<std::mutex> lock(m_finished_lock_);
    m_finished_event_.wait(lock, [this]() { return m_finished_; });
    lock.unlock();
}

void JobManager::WorkerEntryPoint(JobManager *parent) {
    while (true) {
        Job *job;
        {
            std::unique_lock<std::mutex> lock(parent->m_job_queue_lock_);
            while (parent->m_job_queue_.empty() && !parent->m_quit_) parent->m_job_queue_event_.wait(lock);

            if (parent->m_quit_) return;

            job = parent->m_job_queue_.front();
            parent->m_job_queue_.pop();
        }
        job->Execute();
    }
}

void JobManager::Submit(Job *job) {
    m_job_queue_lock_.lock();
    m_job_queue_.push(job);
    m_job_queue_lock_.unlock();
    m_job_queue_event_.notify_one();
}

void MultithreadedSolverHelper::Initialize(nv::cloth::Solver *solver, JobManager *job_manager) {
    m_solver_ = solver;
    m_job_manager_ = job_manager;
    m_end_simulation_job_.Initialize(m_job_manager_, [this](Job *) { m_solver_->endSimulation(); });

    m_start_simulation_job_.Initialize(m_job_manager_, [this](Job *) {
        m_solver_->beginSimulation(m_dt_);
        for (int j = 0; j < m_solver_->getSimulationChunkCount(); j++) m_simulation_chunk_jobs_[j].RemoveReference();
    });
}

void MultithreadedSolverHelper::StartSimulation(float dt) {
    m_dt_ = dt;

    if (m_solver_->getSimulationChunkCount() != m_simulation_chunk_jobs_.size()) {
        m_simulation_chunk_jobs_.resize(m_solver_->getSimulationChunkCount(), JobDependency());
        for (int j = 0; j < m_solver_->getSimulationChunkCount(); j++) {
            m_simulation_chunk_jobs_[j].Initialize(m_job_manager_, [this, j](Job *) { m_solver_->simulateChunk(j); });
            m_simulation_chunk_jobs_[j].SetDependentJob(&m_end_simulation_job_);
        }
    } else {
        for (int j = 0; j < m_solver_->getSimulationChunkCount(); j++) m_simulation_chunk_jobs_[j].Reset();
    }

    m_start_simulation_job_.Reset();
    m_end_simulation_job_.Reset(m_solver_->getSimulationChunkCount());
    m_start_simulation_job_.RemoveReference();
}

void MultithreadedSolverHelper::WaitForSimulation() {
    if (m_solver_->getSimulationChunkCount() == 0) return;

    m_end_simulation_job_.Wait();
}

}  // namespace vox::cloth
