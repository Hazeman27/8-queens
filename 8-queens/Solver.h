#pragma once
#include "ChessBoard.h"

namespace ntf {
	using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

	struct SolutionStep {
		uint32_t figureIndex;
		olc::vi2d position;
	};

	struct Solution {
		std::vector<SolutionStep> steps;
		TimePoint duration;
		uint32_t generatedStatesCount;
	};

	using SolverFunction = Solution(*)(const olc::vi2d&, const HeuristicFunction);
	
	struct Solver {
		std::string name;
		SolverFunction function;
	};
}