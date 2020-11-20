#pragma once
#include "olcPixelGameEngine.h"

namespace ntf {
	struct HeuristicValue {
		olc::vi2d position;
		uint32_t  value;

		bool operator == (const HeuristicValue& other) {
			return position == other.position && value == other.value;
		}
	};

	using HeuristicFunction = std::vector<HeuristicValue>(*)(uint32_t, const std::vector<olc::vi2d>&);

	struct Heuristic {
		std::string name;
		HeuristicFunction function;
	};

	std::vector<HeuristicValue> LocalThreatsHeuristic(uint32_t trgIndex, const std::vector<olc::vi2d>& positions);
	std::vector<HeuristicValue> GlobalThreatsHeuristic(uint32_t trgIndex, const std::vector<olc::vi2d>& positions);
}