/*
Ludo Game Engine
Copyright (C) 2016 TwinDrills

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "Core/Public/Types/Array.h"
#include "Core/Public/Time/Timeout.h"

#include "Core/Public/Parallel/Types/ConcurrentQueue.h"
#include "Core/Public/Parallel/Thread.h"
#include "Core/Public/Parallel/Semaphore.h"
#include "Core/Public/Parallel/Thread.h"

#include <functional>

namespace Ludo {

/** \brief Represents an job instance that has been created by a job scheduler. Can be used to manipulate 
 *	   and retrieve information on the job from the job scheduler.
 */
struct JobHandle
{
private:
	friend class JobScheduler;

	int Index;
	int Version;
};

/** \brief Allows the creation and scheduling of jobs, with complex dependencies, which are executed in parallel
 *         on a pool of threads created during initialization.
 */
class JobScheduler
{
public:

	/// \brief Type of function that can be used as the callback when the job runs.
	typedef std::function<void()> JobCallback;

	/* \brief Maximum number of jobs that can be active at any given time. 
	 *
	 * If more jobs than this are queued the thread that the job is being queued on will
	 * stall until a job has completed and made space for this thread.
	 *
         * Increase if required, there are minimal memory usage repercussions from increasing this.
	 */
	const static int MaxJobCount = 1 * 1024;

private:

	// Internal representation of a active job. 
	struct Job
	{
		int                     Version;
		JobCallback             Callback;
		int                     DependenciesPending;
		Array<JobHandle>        Dependencies;
		Array<JobHandle>        Dependents;
		bool                    Completed;
		bool                    Enqueued;
	};

	Array<Thread*> m_Threads;

	int m_JobVersionCounter;

	ConcurrentQueue<int, MaxJobCount> m_QueuedJobs;

	Job m_Jobs[MaxJobCount];
	ConcurrentQueue<int, MaxJobCount> m_FreeJobIndices;

	Semaphore m_WorkSemaphore;

	bool m_Aborting;

private:
	
	/// \brief Entry point that all threads in the pool start at.
	void ThreadEntryPoint();
		
	/* \brief Attempts to allocate a job if space is available.
	 *
	 * \param Handle Reference to handle that should represent the allocated job.
	 *
         * \returns True if job was allocated successfully.
	 */
	bool AllocateJob(JobHandle& Handle);
	
	/* \brief Attempts to get an internal represenation of a job given a job handle. 
	 *
	 * \param Handle Job handle representing job to retrieve.
	 *
         * \returns  Pointer to job object if handle could be resolved, otherwise nullptr.
	 *
         *  If the job has finished and another job has taken its slot, it may no longer be possible
	 *  to get the job object.
	 */
	Job* GetJob(JobHandle Handle);
			
	/* \brief Attempts to run the next available job in the queue.
	 *
         * \returns True if a job was available and run, otherwise false.
	 */
	bool RunJob();

public:

	/** \brief Constructs the scheduler and creates the thread pool.
	 *
	 *  \param ThreadCount Number of worker threads that should be added to the thread pool.
	 */
	JobScheduler(int ThreadCount);

	/// \brief Destructor.
	~JobScheduler();

	/** \brief Creates a job that the scheduler will execute in future.
	 *
	 *  \param Callback Callback that is invoked. when the job runs, this is where the core body
	 *                  of work should be executed.
	 *
	 *  \returns Handle to newly created job.
	 *
	 *  The job will not be run until its has been enqueued and its dependencies fulfilled.
	 */
	JobHandle CreateJob(JobCallback Callback = nullptr);

	/** \brief Adds a dependency to a job so it will not be executed until the dependency is run.
	 *
	 *  \param Dependent Job to add dependency to, this job will not execute until the dependency is run.
	 *  \param Parent    Job that will be added as a dependency, the job will not execute until this one has run.
         */
	void AddDependency(JobHandle Dependent, JobHandle Parent);

	/** \brief Enqueues a job so it can be executed. This also causes all dependent children to be 
	 *         enqueued as well.
	 *
	 *  \param Handle Handle of job to enqueue.
	 */
	void Enqueue(JobHandle Handle);

	/** \brief Suspends execution of the current thread until the given job completes.
	 *
	 *  \param Handle  Handle to job to wait on.
	 *  \param Timeout Maximum amount of time to wait.
	 *
         *  \returns Returns true if we resumed due to job completition, otherwise returns false
	 *           if we resumed due to timeout or spurious wakeup.
	 *
         *  While waiting for jobs this thread may pickup and run other pending jobs to try and clear
	 *  the job queue faster. As such this function may not resume immediately after the job
	 *  being waited on completes.
	 */
	bool Wait(JobHandle Handle, TimeSpan Timeout = TimeSpan::Infinite);

	/** \brief Determines if the given job has completed yet.
	 *
	 *  \param Handle  Handle to job to get completion status of.
	 *
         *  \returns True if the job is completed.
	 *
         *  If the job handle is no longer valid because another job has taken over the
	 *  internal job slot, we assume the job has completed.
	 */
	bool IsComplete(JobHandle Handle);

};

};
