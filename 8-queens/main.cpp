#include "ChessBoard.h"
#include "HelpScreen.h"
#include "MenuScreen.h"
#include "Heuristic.h"
#include "Solver.h"


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

ntf::Solution BeamSearchSolver(const olc::vi2d& positions, const ntf::HeuristicFunction heuristicFunction)
{
	return {};
}

ntf::Solution TabooSearchSolver(const olc::vi2d& positions, const ntf::HeuristicFunction heuristicFunction)
{
	return {};
}

int main()
{
	ntf::Screen* board = new ntf::ChessBoard(
		{{ "Local Threats", LocalThreatsHeuristic }, { "Global Threats", GlobalThreatsHeuristic }},
		{{ "Beam Search", BeamSearchSolver, { true, "K param", 100, 5, 8, }}, { "Taboo Search", TabooSearchSolver }}
	);

	ntf::Screen* help = new ntf::HelpScreen();
	ntf::Screen* menu = new ntf::MenuScreen();

	std::vector<ntf::Theme> themes{
		ntf::Theme{ "Dark", "chess", {57, 57, 57}, {255, 255, 255} },
		ntf::Theme{ "Green", "chess_green", {5, 113, 55}, {252, 244, 225} },
		ntf::Theme{ "Pink", "chess_pink", {3, 11, 30}, {250, 142, 200} },
	};

	ntf::Window window({ board, help, menu }, themes, 0);

	if (window.Construct(460, 350, 2, 2))
		window.Start();

	return 0;
}