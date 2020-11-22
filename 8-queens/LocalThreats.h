#pragma once
#include "Heuristic.h"


namespace ntf {
	struct LocalThreats : public Heuristic
	{
		LocalThreats() : Heuristic("Local Threats") {}

		HeuristicValue evaluatePosition(const olc::vi2d& position, const std::vector<olc::vi2d>& figuresPositions) override
		{
			return {
				position,
				static_cast<uint32_t>(ChessBoard::GetThreatsIndicesForPos(position, figuresPositions).size())
			};
		}

		std::vector<HeuristicValue> evaluateColumn(const olc::vi2d& currentPos, const std::vector<olc::vi2d>& figuresPositions) override
		{
			std::vector<HeuristicValue> values{};

			for (size_t i = 0; i < figuresPositions.size(); i++)
				values.push_back(evaluatePosition({ currentPos.x, static_cast<int>(i) }, figuresPositions));

			return values;
		}

		uint32_t evaluateBoard(const std::vector<olc::vi2d>& figuresPositions) override
		{
			uint32_t result = 0;

			for (uint32_t i = 0; i < figuresPositions.size(); i++)
				result += evaluatePosition(figuresPositions[i], figuresPositions).value;

			return result;
		}
	};
}