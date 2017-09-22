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

/** \brief Result that can be returned from an iteration of a ParallelFor element. Determines how
 *	  future elements in the list should be executed.
 */
enum class ParallelForTaskResult
{
	/// \brief If returned the parallel for loop continues executing.
	Continue,	
	
	/* \brief If returned the parallel for loop stops executing and returns. This is primary used
	 *	  to stop executing tasks due to an error.
	 */
	Break,		
				
};

/** \brief Result of a ParallelFor invocation. Determines if the loop was broken out of or
 *         completed normally.
 */
enum class ParallelForResult
{
	/// \brief Indicates at least one iteration of the loop returned a Break result.
	Broken,
	
	/// \brief Indicates all iterations of the loop completed successfully.
	Complete
};

/** \brief Takes a list of items and iterates over it, in parallel, and runs a given function on each item.
 * 
 *  \tparam TaskType	Type of element in the list. Usually compiler infered.
 *  \param  Scheduler   Task scheduler that will be used to run each task in parallel.
 *  \param  Tasks       List of elements that will be iterated over.
 *  \param  Callback    Callback that will be called for each element.
 *
 *  \returns Parallel for result that determines if a callback has requested to break out of the loop, or
 *           if each element executed normally.
 *
 *  \example
 *
 *  Array<char> letters = { 'H', 'E', 'L', 'L', 'O', ' ', 'W', 'O', 'R', 'L', 'D' };
 *  JobScheduler scheduler(8);
 *
 *  ParallelForResult result = ParallelFor(scheduler, letters, [](char& value){
 *      printf("%c", value);
 *  });
 *
 */
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
