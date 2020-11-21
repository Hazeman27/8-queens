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

    struct Heuristic {
        std::string name;
        std::vector<olc::vi2d>* figuresPositions;

        Heuristic(const std::string& name) : name(name), figuresPositions(nullptr) {}
        ~Heuristic() { delete figuresPositions; }

        virtual HeuristicValue evaluatePosition(const olc::vi2d& position) = 0;
        virtual std::vector<HeuristicValue> evaluateColumn(uint32_t targetCol) = 0;
    };
}