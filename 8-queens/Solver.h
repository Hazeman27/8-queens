#pragma once
#include "ChessBoard.h"

namespace ntf {
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

    struct SolutionStep {
        uint32_t figureIndex;
        olc::vi2d position;
    };

    struct Solution {
        std::vector<SolutionStep> steps = {};
        TimePoint duration;
        uint32_t generatedStatesCount;
    };

    struct SolverParam {
        bool isUsed = false;
        std::string name = "K param";
        int32_t max = INT32_MAX;
        int32_t min = INT32_MIN;
        int32_t value = 0;
    };

    using SolverFunction = Solution(*)(const std::vector<olc::vi2d>&, const SolverParam&, Heuristic*);

    struct Solver {
        std::string name;
        SolverFunction function;
        SolverParam param;
    };
}