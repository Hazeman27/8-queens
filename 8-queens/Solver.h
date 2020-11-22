#pragma once
#include "olcPixelGameEngine.h"
#include "Heuristic.h"
#include <queue>


namespace ntf {
    struct SearchState {
        std::vector<olc::vi2d> figuresPositions;
        uint32_t heuristicValue;

        bool operator == (const SearchState& other) const {
            for (size_t i = 0; i < figuresPositions.size(); i++) {
                if (figuresPositions[i] != other.figuresPositions[i])
                    return false;
            }
            
            return heuristicValue == other.heuristicValue;
        }

        bool operator < (const SearchState& other) const {
            return heuristicValue < other.heuristicValue;
        }

        bool operator > (const SearchState& other) const {
            return heuristicValue > other.heuristicValue;
        }

        std::string Serialize()
        {
            std::string serial = "";

            for (auto& position : figuresPositions)
                serial += "[" + std::to_string(position.x) + ";" + std::to_string(position.y) + "]";
            
            return serial;
        }
    };

    struct SearchHeuristicValue {
        std::vector<olc::vi2d> figuresPositions;
        HeuristicValue heuristicValue;

        bool operator == (const SearchHeuristicValue& other) const {
            for (size_t i = 0; i < figuresPositions.size(); i++) {
                if (figuresPositions[i] != other.figuresPositions[i])
                    return false;
            }

            return heuristicValue == other.heuristicValue;
        }

        bool operator < (const SearchHeuristicValue& other) const {
            return heuristicValue < other.heuristicValue;
        }

        bool operator > (const SearchHeuristicValue& other) const {
            return heuristicValue > other.heuristicValue;
        }
    };

    using SearchStatesQueue = std::priority_queue<SearchState, std::vector<SearchState>, std::greater<SearchState>>;
    using SearchHeuristicValuesQueue = std::priority_queue<SearchHeuristicValue, std::vector<SearchHeuristicValue>, std::greater<SearchHeuristicValue>>;

    struct Solution {
        std::vector<olc::vi2d> figuresPositions;
        std::chrono::milliseconds duration;
        int generatedStatesCount;

        bool operator == (const Solution& other) const {
            for (size_t i = 0; i < figuresPositions.size(); i++) {
                if (figuresPositions[i] != other.figuresPositions[i])
                    return false;
            }

            return duration == other.duration && generatedStatesCount == generatedStatesCount;
        }
    };

    const Solution FAILED_SOLUTION = { { {-1, -1} }, std::chrono::milliseconds(0), -1 };

    struct SolverParam {
        bool isUsed = false;
        std::string name = "K param";
        int32_t max = INT32_MAX;
        int32_t min = INT32_MIN;
        int32_t value = 0;
    };

    struct Solver {
        std::string name;
        SolverParam param;

        Solver(const std::string& name, const SolverParam& param) : name(name), param(param) {}

        virtual Solution Solve(const std::vector<olc::vi2d>& figuresPositions, const SolverParam& param, Heuristic* heuristic) = 0;
    };
}