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
		) override {
			return {};
		}
	};
}
