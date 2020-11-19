#include "ChessBoard.h"

HeuristicResult LocalThreatsHeuristic(int trgIndex, const std::vector<olc::vi2d>& positions)
{
	HeuristicMapping mapping;

	int boardSize = positions.size();
	olc::vi2d targetPos = positions[trgIndex];

	auto getThreatsAtPos = [&](olc::vi2d position) {
		return std::make_pair(position, ChessBoard::GetThreatsIndicesForPos(position, positions).size());
	};

	for (int i = 0; i < boardSize; i++) {
		if (i == targetPos.y)
			continue;
		mapping.push_back(getThreatsAtPos({ targetPos.x, i }));
	}

	mapping.push_back(getThreatsAtPos(targetPos));

	return { mapping, mapping.back().second };
}

HeuristicResult GlobalThreatsHeuristic(int trgIndex, const std::vector<olc::vi2d>& positions)
{
	HeuristicMapping mapping;

	int boardSize = positions.size();
	olc::vi2d trgPos = positions[trgIndex];

	auto getThreatsAtPos = [&](olc::vi2d position) {
		int threatsSum = 0;

		for (int i = 0; i < boardSize; i++) {
			int threats = 0;
			olc::vi2d posA = i == trgIndex ? position : positions[i];

			for (int j = i + 1; j < boardSize; j++) {
				olc::vi2d posB = j == trgIndex ? position : positions[j];

				if (ChessBoard::FigureAtPosIsThreat(posA, posB))
					threats++;
			}

			threatsSum += threats;
		}

		return std::make_pair(position, threatsSum);
	};

	for (int i = 0; i < boardSize; i++) {
		if (i == trgPos.y)
			continue;
		mapping.push_back(getThreatsAtPos({ trgPos.x, i }));
	}

	mapping.push_back(getThreatsAtPos(trgPos));

	return { mapping, mapping.back().second };
}


int main()
{
	std::vector<HeuristicFunctionEntry> heuristicFunctions{
		std::make_pair("Local Threats", LocalThreatsHeuristic),
		std::make_pair("Global Threats", GlobalThreatsHeuristic),
	};

	ChessBoard board(8, heuristicFunctions);

	if (board.Construct(430, 300, 2, 2))
		board.Start();

	return 0;
}