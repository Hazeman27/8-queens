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
			std::unordered_map<std::string, bool> visitedStates{};

			for (int i = 0; i < beamWidth; i++) {

				int randomCol = distribution(randomEngine);
				int randomRow = distribution(randomEngine);

				auto state = GenerateState(figuresPositions, { randomCol, randomRow }, heuristic);
				generatedStatesCount++;

				if (state.heuristicValue == 0) {
					return {
						state.figuresPositions,
						TakeTimeStamp(startTime),
						generatedStatesCount,
					};
				}

				queue.push(state);
				visitedStates.insert({ state.Serialize(), true });
			}

			while (!queue.empty()) {

				SearchStatesQueue subQueue;

				while (!queue.empty()) {
					auto [figuresPositions, stateHeuristicValue] = queue.top();
					queue.pop();

					if (stateHeuristicValue == 0) {
						return {
							figuresPositions,
							TakeTimeStamp(startTime),
							generatedStatesCount,
						};
					}

					for (auto& position : figuresPositions) {

						auto results = heuristic->EvaluateColumn(position, figuresPositions);

						for (auto& result : results) {
							subQueue.push(GenerateState(figuresPositions, result.position, heuristic));
							generatedStatesCount++;
						}
					}
				}

				for (int i = 0; i < beamWidth && !subQueue.empty(); i++) {

					auto state = subQueue.top();
					subQueue.pop();

					std::string stateKey = state.Serialize();

					if (visitedStates.find(stateKey) != visitedStates.end())
						continue;

					queue.push(state);
					visitedStates.insert({ stateKey, true });
				}
			}

			return { {}, TakeTimeStamp(startTime), generatedStatesCount };
		}
	};
}
