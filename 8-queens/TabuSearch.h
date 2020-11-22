#pragma once
#include "Solver.h"


namespace ntf {
	class TabuSearch : public Solver
	{
	public:
		TabuSearch(const SolverParam& param) : Solver("Tabu Search", param) {}

		Solution Solve(
			const std::vector<olc::vi2d>& figuresPositions,
			const SolverParam& param,
			const std::shared_ptr<Heuristic> heuristic
		) override
		{
			auto startTime = HighResClock::now();
			int generatedStatesCount = 0;

			int tabuListMaxSize = param.value;
			uint64_t popDepth = 1;

			std::shared_ptr<SearchState> bestFit(new SearchState{
				figuresPositions,
				heuristic->EvaluateBoard(figuresPositions)
			});

			std::unordered_map<std::string, bool> tabuList{ {bestFit->Serialize(), true} };
			SearchStatesQueue fitnessQueue;

			fitnessQueue.push(*bestFit);

			while (bestFit->heuristicValue != 0 && !fitnessQueue.empty()) {

				for (auto& position : bestFit->figuresPositions) {

					for (auto& result : heuristic->EvaluateColumn(position, bestFit->figuresPositions)) {

						auto currentState = std::move(GenerateState(bestFit->figuresPositions, result.position, heuristic));
						generatedStatesCount++;

						if (tabuList.find(currentState.Serialize()) != tabuList.end())
							continue;

						fitnessQueue.push(currentState);
					}
				}

				SearchState localBestFit = fitnessQueue.top();
				fitnessQueue.pop();

				if (localBestFit < *bestFit)
					*bestFit = localBestFit;

				else {
					SearchState localSecondBestFit;
					
					for (uint64_t i = 0; i < popDepth && !fitnessQueue.empty(); i++) {
						localSecondBestFit = fitnessQueue.top();
						fitnessQueue.pop();
					}

					*bestFit = localSecondBestFit;
					tabuList.insert({ localBestFit.Serialize(), true });

					popDepth++;
				}
				
				if (tabuList.size() > tabuListMaxSize)
					tabuList.erase(tabuList.begin());
			}

			if (bestFit->heuristicValue != 0)
				return { {}, TakeTimeStamp(startTime), generatedStatesCount };
			
			return {
				bestFit->figuresPositions,
				TakeTimeStamp(startTime),
				generatedStatesCount,
			};
		}
	};
}
