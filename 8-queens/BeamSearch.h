#pragma once
#include "Solver.h"
#include <random>


namespace ntf {
	class BeamSearch : public Solver
	{
	public:
		BeamSearch(const SolverParam& param) : Solver("Beam Search", param) {}

		Solution Solve(
			const std::vector<olc::vi2d>& figuresPositions,
			const SolverParam& param,
			const std::shared_ptr<Heuristic> heuristic
		) override
		{
			auto startTime = HighResClock::now();
			int generatedStatesCount = 0;

			if (heuristic->EvaluateBoard(figuresPositions) == 0) {
				return {
					figuresPositions,
					TakeTimeStamp(startTime),
					generatedStatesCount,
				};
			}

			std::default_random_engine randomEngine;
			randomEngine.seed(static_cast<uint32_t>(HighResClock::now().time_since_epoch().count()));

			const int beamWidth = param.value;

			std::uniform_int_distribution<int> distribution(0, static_cast<int>(figuresPositions.size() - 1));

			SearchStatesQueue queue;
			
			std::shared_ptr<SearchState> currentState(std::make_shared<SearchState>());
			std::unordered_map<std::string, bool> visitedStates{};

			for (int i = 0; i < beamWidth; i++) {

				int randomCol = distribution(randomEngine);
				int randomRow = distribution(randomEngine);

				*currentState = std::move(GenerateState(figuresPositions, { randomCol, randomRow }, heuristic));
				generatedStatesCount++;

				if (currentState->heuristicValue == 0) {
					return {
						currentState->figuresPositions,
						TakeTimeStamp(startTime),
						generatedStatesCount,
					};
				}

				queue.push(std::move(*currentState));
				visitedStates.insert({ currentState->Serialize(), true });
			}

			while (!queue.empty()) {

				SearchStatesQueue subQueue;

				while (!queue.empty()) {
					*currentState = queue.top();
					queue.pop();

					if (currentState->heuristicValue == 0) {
						return {
							currentState->figuresPositions,
							TakeTimeStamp(startTime),
							generatedStatesCount,
						};
					}

					for (auto& position : currentState->figuresPositions) {

						for (auto& result : heuristic->EvaluateColumn(position, currentState->figuresPositions)) {
							subQueue.push(std::move(GenerateState(currentState->figuresPositions, result.position, heuristic)));
							generatedStatesCount++;
						}
					}
				}

				for (int i = 0; i < beamWidth && !subQueue.empty(); i++) {

					*currentState = subQueue.top();
					subQueue.pop();

					std::string stateKey = std::move(currentState->Serialize());

					if (visitedStates.find(stateKey) != visitedStates.end())
						continue;

					queue.push(std::move(*currentState));
					visitedStates.insert({ stateKey, true });
				}
			}

			return { {}, TakeTimeStamp(startTime), generatedStatesCount };
		}
	};
}
