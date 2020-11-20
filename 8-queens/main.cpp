#include "ChessBoard.h"
#include "Heuristic.h"


std::vector<ntf::HeuristicValue> LocalThreatsHeuristic(uint32_t trgIndex, const std::vector<olc::vi2d>& positions)
{
	std::vector<ntf::HeuristicValue> values{};
	olc::vi2d targetPos = positions[trgIndex];

	auto getThreatsAtPos = [&](olc::vi2d position) {
		return ntf::HeuristicValue{
			position,
			static_cast<uint32_t>(ntf::ChessBoard::GetThreatsIndicesForPos(position, positions).size())
		};
	};

	for (size_t i = 0; i < positions.size(); i++)
		values.push_back(getThreatsAtPos({ targetPos.x, static_cast<int>(i) }));

	return values;
}

std::vector<ntf::HeuristicValue> GlobalThreatsHeuristic(uint32_t trgIndex, const std::vector<olc::vi2d>& positions)
{
	std::vector<ntf::HeuristicValue> values{};

	size_t boardSize = positions.size();
	olc::vi2d trgPos = positions[trgIndex];

	auto getThreatsAtPos = [&](olc::vi2d position) {
		uint32_t threatsSum = 0;

		for (size_t i = 0; i < boardSize; i++) {
			uint32_t threats = 0;
			olc::vi2d posA = i == trgIndex ? position : positions[i];

			for (size_t j = i + 1; j < boardSize; j++) {
				olc::vi2d posB = j == trgIndex ? position : positions[j];

				if (ntf::ChessBoard::FigureAtPosIsThreat(posA, posB))
					threats++;
			}

			threatsSum += threats;
		}

		return ntf::HeuristicValue{ position, threatsSum };
	};

	for (size_t i = 0; i < boardSize; i++)
		values.push_back(getThreatsAtPos({ trgPos.x, static_cast<int>(i) }));

	return values;
}


int main()
{
	ntf::ChessBoard board(8, {
		{ "Local Threats", LocalThreatsHeuristic },
		{ "Global Threats", GlobalThreatsHeuristic },
	});

	if (board.Construct(430, 300, 2, 2))
		board.Start();

	return 0;
}