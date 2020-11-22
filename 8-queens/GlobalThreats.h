#pragma once
#include "Heuristic.h"


namespace ntf {
	class GlobalThreats : public Heuristic
	{
	public:
		GlobalThreats() : Heuristic("Global Threats") {}

		HeuristicValue EvaluatePosition(
			const olc::vi2d& position,
			const std::vector<olc::vi2d>& figuresPositions
		) override
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

		uint32_t EvaluateBoard(const std::vector<olc::vi2d>& figuresPositions) override
		{
			return EvaluatePosition(figuresPositions[0], figuresPositions).value;
		}
	};
}