#pragma once
#include "olcPixelGameEngine.h"
#include <queue>


namespace ntf {

    struct HeuristicValue {
        olc::vi2d position;
        uint32_t  value;

        bool operator == (const HeuristicValue& other) const {
            return position == other.position && value == other.value;
        }

        bool operator < (const HeuristicValue& other) const {
            return value < other.value;
        }

        bool operator > (const HeuristicValue& other) const {
            return value > other.value;
        }
    };
    
    using HeuristicValuesQueue = std::priority_queue<HeuristicValue, std::vector<HeuristicValue>, std::greater<HeuristicValue>>;

    struct Heuristic {
        std::string name;

        Heuristic(const std::string& name) : name(name) {}

        virtual HeuristicValue evaluatePosition(const olc::vi2d& position, const std::vector<olc::vi2d>& figuresPositions) = 0;
        virtual std::vector<HeuristicValue> evaluateColumn(const olc::vi2d& currentPos, const std::vector<olc::vi2d>& figuresPositions) = 0;
        virtual uint32_t evaluateBoard(const std::vector<olc::vi2d>& figuresPositions) = 0;
    };
}