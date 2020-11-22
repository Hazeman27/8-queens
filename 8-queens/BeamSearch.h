#pragma once
#include "Solver.h"
#include <random>


namespace ntf {
	using Clock = std::chrono::high_resolution_clock;

	struct BeamSearch : public Solver {
		BeamSearch(const SolverParam& param) : Solver("Beam Search", param) {}

		Solution Solve(const std::vector<olc::vi2d>& figuresPositions, const SolverParam& param, Heuristic* heuristic) override {
			auto startTime = Clock::now();
			int generatedStatesCount = 0;

			std::default_random_engine randomEngine;
			randomEngine.seed(static_cast<uint32_t>(Clock::now().time_since_epoch().count()));

			const int beamWidth = param.value;

			std::uniform_int_distribution<int> distribution(0, static_cast<int>(figuresPositions.size() - 1));

			SearchStatesQueue queue{};
			std::unordered_map<std::string, bool> visitedStates{};

			auto newState = [&](const std::vector<olc::vi2d> positions, const olc::vi2d movePos) {

				std::vector<olc::vi2d> newPositions(positions);
				newPositions[movePos.x].y = movePos.y;

				uint32_t newStateHeuristicValue = heuristic->evaluateBoard(newPositions);
				generatedStatesCount++;

				return SearchState{ newPositions, newStateHeuristicValue };
			};

			auto duration = [&]() {
				return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - startTime);
			};

			for (int i = 0; i < beamWidth; i++) {

				int randomCol = distribution(randomEngine);
				int randomRow = distribution(randomEngine);

				auto state = newState(figuresPositions, { randomCol, randomRow });

				queue.push(state);
				visitedStates.insert({ state.Serialize(), true });
			}

			while (!queue.empty()) {

				SearchHeuristicValuesQueue subQueue{};

				while (!queue.empty()) {
					auto& [figuresPositions, stateHeuristicValue] = queue.top();
					queue.pop();

					if (stateHeuristicValue == 0) {
						return {
							figuresPositions,
							duration(),
							generatedStatesCount,
						};
					}

					for (auto& position : figuresPositions) {

						auto values = heuristic->evaluateColumn(position, figuresPositions);
						std::sort(values.begin(), values.end());

						subQueue.push({ figuresPositions, values[0] });
					}
				}

				for (int i = 0; i < beamWidth && !subQueue.empty(); i++) {

					auto& [positions, heuristicValue] = subQueue.top();
					subQueue.pop();

					auto state = newState(positions, heuristicValue.position);
					std::string stateKey = state.Serialize();

					if (visitedStates.find(stateKey) != visitedStates.end())
						continue;

					queue.push(state);
					visitedStates.insert({ stateKey, true });
				}
			}

			return FAILED_SOLUTION;
		}
	};
}
