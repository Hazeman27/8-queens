#pragma once
#include "olcPixelGameEngine.h"
#include "Heuristic.h"
#include <queue>


namespace ntf {
 
    using HighResClock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using Microseconds = std::chrono::microseconds;

    struct SearchState {
        std::vector<olc::vi2d> figuresPositions;
        uint32_t heuristicValue;

        bool operator == (const SearchState& other) const {
            if (figuresPositions.size() != other.figuresPositions.size())
                return false;

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
            if (figuresPositions.size() != other.figuresPositions.size())
                return false;

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
        Microseconds duration = std::chrono::microseconds::zero();

        int generatedStatesCount;

        bool operator == (const Solution& other) const {
            if (figuresPositions.size() != other.figuresPositions.size())
                return false;

            for (size_t i = 0; i < figuresPositions.size(); i++) {
                if (figuresPositions[i] != other.figuresPositions[i])
                    return false;
            }

            return duration == other.duration && generatedStatesCount == generatedStatesCount;
        }
    };

    struct SolverParam {
        bool isUsed = false;
        std::string name = "Solver param";
        int32_t min = INT32_MIN;
        int32_t max = INT32_MAX;
        int32_t defaultValue = 0;
        int32_t value = 0;
    };

    class Solver : public std::enable_shared_from_this<Solver>
    {
    public:
        std::string name;
        SolverParam param;

        Solver(const std::string& name, const SolverParam& param) : name(name), param(param)
        {}

        Solver(std::string&& name, SolverParam&& param) : name(std::move(name)), param(std::move(param))
        {}

        static Microseconds TakeTimeStamp(TimePoint startTime) {
            return std::chrono::duration_cast<Microseconds>(HighResClock::now() - startTime);
        };

        static SearchState GenerateState(
            const std::vector<olc::vi2d>& figuresPositions,
            const olc::vi2d& movePosition,
            const std::shared_ptr<Heuristic> heuristic
        ) {
            std::vector<olc::vi2d> newPositions(figuresPositions);
            newPositions[movePosition.x].y = movePosition.y;

            uint32_t newStateHeuristicValue = heuristic->EvaluateBoard(newPositions);
            return { newPositions, newStateHeuristicValue };
        };

        virtual Solution Solve(
            const std::vector<olc::vi2d>& figuresPositions,
            const SolverParam& param,
            const std::shared_ptr<Heuristic> heuristic
        ) = 0;
    };
}