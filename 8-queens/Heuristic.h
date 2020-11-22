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
    
    class Heuristic : public std::enable_shared_from_this<Heuristic>
    {
    public:
        std::string name;

        Heuristic(const std::string& name) : name(name) {}
        Heuristic(std::string&& name) : name(std::move(name)) {}

        virtual HeuristicValue EvaluatePosition(
            const olc::vi2d& position,
            const std::vector<olc::vi2d>& figuresPositions
        ) = 0;

        virtual uint32_t EvaluateBoard(const std::vector<olc::vi2d>& figuresPositions) = 0;

        HeuristicValue GetColumnMinValue(
            const olc::vi2d& currentPos,
            const std::vector<olc::vi2d>& figuresPositions
        ) {
            if (figuresPositions.size() < 0)
                return {};

            HeuristicValue minValue = EvaluatePosition({ currentPos.x, 0 }, figuresPositions);

            for (size_t i = 1; i < figuresPositions.size(); i++) {
                auto value = EvaluatePosition({ currentPos.x, static_cast<int>(i) }, figuresPositions);

                if (value < minValue)
                    minValue = value;
            }

            return minValue;
        }

        std::vector<HeuristicValue> EvaluateColumn(
            const olc::vi2d& currentPos,
            const std::vector<olc::vi2d>& figuresPositions
        ) {
            std::vector<HeuristicValue> values{};

            for (size_t i = 0; i < figuresPositions.size(); i++)
                values.push_back(EvaluatePosition({ currentPos.x, static_cast<int>(i) }, figuresPositions));

            return values;
        }
    };
}