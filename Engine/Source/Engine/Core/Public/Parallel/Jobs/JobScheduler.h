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

/// \brief TODO
struct JobHandle
{
private:
	friend class JobScheduler;

	int Index;
	int Version;
};

/// \brief TODO
class JobScheduler
{
public:

	/// \brief TODO
	typedef std::function<void()> JobCallback;

	/// \brief TODO
	const static int NoTimeout = -1;

	/// \brief TODO
	const static int MaxJobCount = 1 * 1024;

private:

	struct Job
	{
		int					Version;
		JobCallback			Callback;
		int					DependenciesPending;
		Array<JobHandle>	Dependencies;
		Array<JobHandle>	Dependents;
		bool				Completed;
		bool				Enqueued;
	};

	Array<Thread*> m_Threads;

	int m_JobVersionCounter;

	ConcurrentQueue<int, MaxJobCount> m_QueuedJobs;

	Job m_Jobs[MaxJobCount];
	ConcurrentQueue<int, MaxJobCount> m_FreeJobIndices;

	Semaphore m_WorkSemaphore;

	bool m_Aborting;

private:
	void ThreadEntryPoint();
	bool AllocateJob(JobHandle& Handle);
	Job* GetJob(JobHandle Handle);
	bool RunJob();

public:

	/// \brief TODO
	JobScheduler(int ThreadCount);

	/// \brief TODO
	~JobScheduler();

	/// \brief TODO
	JobHandle CreateJob(JobCallback Callback = nullptr);

	/// \brief TODO
	void AddDependency(JobHandle Dependent, JobHandle Parent);

	/// \brief TODO
	void Enqueue(JobHandle Handle);

	/// \brief TODO
	void Wait(JobHandle Handle, TimeSpan Timeout = TimeSpan::Infinite);

	/// \brief TODO
	bool IsComplete(JobHandle Handle);

};

};