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

#include "Core/Public/Parallel/Jobs/JobScheduler.h"

namespace Ludo {

/// \brief TODO
enum class ParallelForTaskResult
{
	Continue,
	Break,
};

/// \brief TODO
enum class ParallelForResult
{
	Broken,
	Complete
};

/// \brief TODO
template <typename TaskType>
ParallelForResult ParallelFor(JobScheduler* Scheduler, Array<TaskType>& Tasks, std::function<ParallelForTaskResult(TaskType& TaskType)> Callback)
{
	ParallelForResult Result = ParallelForResult::Complete;

	JobHandle GroupTask = Scheduler->CreateJob();

	for (int i = 0; i < Tasks.Length(); i++)
	{
		JobHandle IterTask = Scheduler->CreateJob([&Result, &Tasks, Callback, i] () {
			
			if (Result == ParallelForResult::Broken)
			{
				return;
			}

			ParallelForTaskResult TaskResult = Callback(Tasks[i]);
			if (TaskResult == ParallelForTaskResult::Break)
			{
				Result = ParallelForResult::Broken;
			}
		});

		Scheduler->AddDependency(GroupTask, IterTask);
	}

	Scheduler->Enqueue(GroupTask);
	Scheduler->Wait(GroupTask);

	return Result;
}

};