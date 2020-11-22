#pragma once
#include "Heuristic.h"


namespace ntf {
	struct GlobalThreats : public Heuristic
	{
		GlobalThreats() : Heuristic("Global Threats") {}

		HeuristicValue evaluatePosition(const olc::vi2d& position, const std::vector<olc::vi2d>& figuresPositions) override
		{
			uint32_t threatsSum = 0;
			size_t boardSize = figuresPositions.size();

			for (size_t i = 0; i < boardSize; i++) {
				uint32_t threats = 0;
				olc::vi2d posA = i == position.x ? position : figuresPositions[i];

				for (size_t j = i + 1; j < boardSize; j++) {
					olc::vi2d posB = j == position.x ? position : figuresPositions[j];

					if (ChessBoard::FigureAtPosIsThreat(posA, posB))
						threats++;
				}

				threatsSum += threats;
			}

			return { position, threatsSum };
		};

		std::vector<HeuristicValue> evaluateColumn(const olc::vi2d& currentPos, const std::vector<olc::vi2d>& figuresPositions) override
		{
			std::vector<HeuristicValue> values{};

			for (size_t i = 0; i < figuresPositions.size(); i++)
				values.push_back(evaluatePosition({ currentPos.x, static_cast<int>(i) }, figuresPositions));

			return values;
		}

		uint32_t evaluateBoard(const std::vector<olc::vi2d>& figuresPositions) override
		{
			return evaluatePosition(figuresPositions[0], figuresPositions).value;
		}
	};
}