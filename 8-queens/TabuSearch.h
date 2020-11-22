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

			SearchState bestFit{
				figuresPositions,
				heuristic->EvaluateBoard(figuresPositions)
			};

			std::unordered_map<std::string, bool> tabuList{ {bestFit.Serialize(), true} };
			SearchStatesQueue fitnessQueue;

			fitnessQueue.push(bestFit);

			while (bestFit.heuristicValue != 0 && !fitnessQueue.empty()) {

				std::vector<SearchState> possibleStates;

				for (auto& position : bestFit.figuresPositions) {
					auto results = heuristic->EvaluateColumn(position, bestFit.figuresPositions);

					for (auto& result : results) {
						auto state = GenerateState(bestFit.figuresPositions, result.position, heuristic);
						generatedStatesCount++;

						if (tabuList.find(state.Serialize()) != tabuList.end())
							continue;

						fitnessQueue.push(state);
					}
				}

				SearchState localBestFit = fitnessQueue.top();
				fitnessQueue.pop();

				if (localBestFit < bestFit)
					bestFit = localBestFit;

				else {
					SearchState secondBestFit = fitnessQueue.top();
					fitnessQueue.pop();
					
					bestFit = secondBestFit;
					tabuList.insert({ localBestFit.Serialize(), true });
				}
				
				if (tabuList.size() > tabuListMaxSize)
					tabuList.erase(tabuList.begin());
			}

			if (bestFit.heuristicValue != 0)
				return { {}, TakeTimeStamp(startTime), generatedStatesCount };

			return {
				bestFit.figuresPositions,
				TakeTimeStamp(startTime),
				generatedStatesCount,
			};
		}
	};
}
