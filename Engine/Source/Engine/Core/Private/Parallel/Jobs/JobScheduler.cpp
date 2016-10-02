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

#include "PCH.h"

#include "Core/Public/Parallel/Jobs/JobScheduler.h"

namespace Ludo {

JobScheduler::JobScheduler(int ThreadCount)
	: m_Aborting(false)
{
	Assert(ThreadCount > 0);

	// Push all empty job indices.
	for (int i = 0; i < MaxJobCount; i++)
	{
		m_FreeJobIndices.Push(i);
	}

	// Boot up all threads.
	for (int i = 0; i < ThreadCount; i++)
	{
		String ThreadName = String::Format("Job Pool Thread %i", i);

		Thread* NewThread = LD_ARENA_NEW(PlatformMemoryArena, Thread);
		NewThread->SetName(ThreadName);
		m_Threads.Add(NewThread);

		NewThread->Start([this](Thread* Self) { 
			LD_UNUSED_PARAMETER(Self);
			ThreadEntryPoint(); 
		});
	}
}

JobScheduler::~JobScheduler()
{
	m_Aborting = true;
	m_WorkSemaphore.Signal(m_Threads.Length());
	
	for (Thread* CurrentThread : m_Threads)
	{
		CurrentThread->Join();
		LD_ARENA_DELETE(PlatformMemoryArena, CurrentThread);
	}
	m_Threads.Empty();
}

bool JobScheduler::AllocateJob(JobHandle& Handle)
{
	if (m_FreeJobIndices.Pop(&Handle.Index))
	{
		m_Jobs[Handle.Index].Version = Atomic::Increment(&m_JobVersionCounter);
		m_Jobs[Handle.Index].Completed = false;
		m_Jobs[Handle.Index].Enqueued = false;
		m_Jobs[Handle.Index].DependenciesPending = 0;
		m_Jobs[Handle.Index].Dependencies.Empty();
		m_Jobs[Handle.Index].Dependents.Empty();
		Handle.Version = m_Jobs[Handle.Index].Version;
		return true;
	}
	return false;
}

JobHandle JobScheduler::CreateJob(JobCallback Callback)
{
	JobHandle Handle;
	bool bResult = AllocateJob(Handle);
	AssertMsg(bResult, "Unable to allocate job index, possibly to many jobs queued.");

	if (bResult)
	{
		m_Jobs[Handle.Index].Callback = Callback;
	}

	return Handle;
}

JobScheduler::Job* JobScheduler::GetJob(JobHandle Handle)
{
	if (m_Jobs[Handle.Index].Version != Handle.Version)
	{
		return nullptr;
	}
	return &m_Jobs[Handle.Index];
}

void JobScheduler::AddDependency(JobHandle Dependent, JobHandle Parent)
{
	Job* DependentJob = GetJob(Dependent);
	Job* ParentJob = GetJob(Parent);

	AssertMsg(DependentJob != nullptr, "Dependent job handle is no longer valid - its probably already been executed. Add job dependencies before enqueing them.");
	AssertMsg(ParentJob != nullptr, "Parent job handle is no longer valid - its probably already been executed. Add job dependencies before enqueing them.");

	AssertMsg(!DependentJob->Enqueued, "Dependent job has already been enqueued. Add job dependencies before enqueing jobs.");
	AssertMsg(!ParentJob->Enqueued, "Dependent job has already been enqueued. Add job dependencies before enqueing jobs.");

	DependentJob->DependenciesPending++;
	DependentJob->Dependencies.Add(Parent);
	ParentJob->Dependents.Add(Dependent);
}

void JobScheduler::Enqueue(JobHandle Handle)
{
	// Ensure we aren't already enqueued.
	Job* ParentJob = GetJob(Handle);

	AssertMsg(ParentJob != nullptr, "Parent job handle is no longer valid - its probably already been executed. Add job dependencies before enqueing them.");
	AssertMsg(!ParentJob->Enqueued, "Dependent job has already been enqueued. Add job dependencies before enqueing jobs.");

	if (ParentJob->DependenciesPending <= 0)
	{
		ParentJob->Enqueued = true;

		if (!m_QueuedJobs.Push(Handle.Index))
		{
			ConstantAssertMsg("Failed to enqueue job, job queue is probably full, increasing MaxJobCount may be required.");
		}
	}

	// Enqueue children first.
	for (JobHandle ChildHandle : ParentJob->Dependencies)
	{
		Job* ChildJob = GetJob(ChildHandle);
		if (ChildJob != nullptr && !ChildJob->Enqueued)
		{
			Enqueue(ChildHandle);
		}
	}

	m_WorkSemaphore.Signal();
}

bool JobScheduler::IsComplete(JobHandle Handle)
{
	if (m_Jobs[Handle.Index].Completed ||
		m_Jobs[Handle.Index].Version != Handle.Version)
	{
		return true;
	}
	return false;
}

void JobScheduler::Wait(JobHandle Handle, TimeSpan TimeoutDuration)
{
	Timeout WaitTimeout(TimeoutDuration);

	while (!IsComplete(Handle))
	{
		if (!RunJob())
		{
			Atomic::RelaxCpu();
		}

		if (WaitTimeout.Finished())
		{
			break;
		}
	}
}

bool JobScheduler::RunJob()
{
	int JobIndex = 0;
	if (m_QueuedJobs.Pop(&JobIndex))
	{
		if (m_Jobs[JobIndex].Callback != nullptr)
		{
			m_Jobs[JobIndex].Callback();
		}

		for (JobHandle& Handle : m_Jobs[JobIndex].Dependents)
		{
			int Index = Atomic::Decrement(&m_Jobs[Handle.Index].DependenciesPending);
			if (Index == 0)
			{
				Enqueue(Handle);
			}
		}

		m_Jobs[JobIndex].Completed = true;

		m_FreeJobIndices.Push(JobIndex);
		return true;
	}

	return false;
}

void JobScheduler::ThreadEntryPoint()
{
	while (!m_Aborting)
	{
		m_WorkSemaphore.Wait();
		if (!RunJob())
		{
			Atomic::RelaxCpu();
		}
	}
}

}; // namespace Ludo