#pragma once
#include "Heuristic.h"


namespace ntf {
	struct LocalThreats : public Heuristic
	{
		LocalThreats() : Heuristic("Local Threats") {}

		HeuristicValue EvaluatePosition(const olc::vi2d& position, const std::vector<olc::vi2d>& figuresPositions) override
		{
			return {
				position,
				static_cast<uint32_t>(ChessBoard::GetThreatsIndicesForPos(position, figuresPositions).size())
			};
		}

		uint32_t EvaluateBoard(const std::vector<olc::vi2d>& figuresPositions) override
		{
			uint32_t result = 0;

			for (uint32_t i = 0; i < figuresPositions.size(); i++)
				result += EvaluatePosition(figuresPositions[i], figuresPositions).value;

			return result;
		}
	};
}