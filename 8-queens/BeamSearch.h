#pragma once
#include "Solver.h"
#include <random>


namespace ntf {
	struct BeamSearch : public Solver {
		BeamSearch(const SolverParam& param) : Solver("Beam Search", param) {}

		Solution Solve(const std::vector<olc::vi2d>& figuresPositions, const SolverParam& param, Heuristic* heuristic) override {
			auto startTime = HighResClock::now();
			int generatedStatesCount = 0;

			auto newState = [&](const std::vector<olc::vi2d>& positions, const olc::vi2d& movePos) {

				std::vector<olc::vi2d> newPositions(positions);
				newPositions[movePos.x].y = movePos.y;

				uint32_t newStateHeuristicValue = heuristic->EvaluateBoard(newPositions);
				generatedStatesCount++;

				return SearchState{ newPositions, newStateHeuristicValue };
			};

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

			SearchStatesQueue queue{};
			std::unordered_map<std::string, bool> visitedStates{};

			for (int i = 0; i < beamWidth; i++) {

				int randomCol = distribution(randomEngine);
				int randomRow = distribution(randomEngine);

				auto state = newState(figuresPositions, { randomCol, randomRow });

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

				SearchHeuristicValuesQueue subQueue{};

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

					for (auto& position : figuresPositions)
						subQueue.push({ figuresPositions, heuristic->GetColumnMinValue(position, figuresPositions) });
				}

				for (int i = 0; i < beamWidth && !subQueue.empty(); i++) {

					auto [positions, heuristicValue] = subQueue.top();
					subQueue.pop();

					auto state = newState(positions, heuristicValue.position);
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
