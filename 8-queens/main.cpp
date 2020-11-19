#include "ChessBoard.h"

std::vector<HeuristicValue> LocalThreatsHeuristic(int trgIndex, const std::vector<olc::vi2d>& positions)
{
	std::vector<HeuristicValue> values{};

	int boardSize = positions.size();
	olc::vi2d targetPos = positions[trgIndex];

	auto getThreatsAtPos = [&](olc::vi2d position) {
		return HeuristicValue{ position, static_cast<int>(ChessBoard::GetThreatsIndicesForPos(position, positions).size()) };
	};

	for (int i = 0; i < boardSize; i++)
		values.push_back(getThreatsAtPos({ targetPos.x, i }));

	return values;
}

std::vector<HeuristicValue> GlobalThreatsHeuristic(int trgIndex, const std::vector<olc::vi2d>& positions)
{
	std::vector<HeuristicValue> values{};

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

		return HeuristicValue{ position, threatsSum };
	};

	for (int i = 0; i < boardSize; i++)
		values.push_back(getThreatsAtPos({ trgPos.x, i }));

	return values;
}


int main()
{
	ChessBoard board(8, {
		std::make_pair("Local Threats", LocalThreatsHeuristic),
		std::make_pair("Global Threats", GlobalThreatsHeuristic),
	});

	if (board.Construct(430, 300, 2, 2))
		board.Start();

	return 0;
}