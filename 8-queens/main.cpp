#include "ChessBoard.h"
#include "HelpScreen.h"
#include "MenuScreen.h"
#include "Heuristic.h"
#include "Solver.h"


struct LocalThreats : public ntf::Heuristic
{
	LocalThreats() : Heuristic("Local Threats") {}

	ntf::HeuristicValue evaluatePosition(const olc::vi2d& position) override
	{
		return ntf::HeuristicValue{
			position,
			static_cast<uint32_t>(ntf::ChessBoard::GetThreatsIndicesForPos(position, *figuresPositions).size())
		};
	};

	std::vector<ntf::HeuristicValue> evaluateColumn(uint32_t targetCol) override
	{
		std::vector<ntf::HeuristicValue> values{};
		olc::vi2d targetPos = figuresPositions->at(targetCol);

		for (size_t i = 0; i < figuresPositions->size(); i++)
			values.push_back(evaluatePosition({ targetPos.x, static_cast<int>(i) }));

		return values;
	}
};

struct GlobalThreats : public ntf::Heuristic
{
	GlobalThreats() : Heuristic("Global Threats") {}

	ntf::HeuristicValue evaluatePosition(const olc::vi2d& position) override
	{
		uint32_t threatsSum = 0;
		size_t boardSize = figuresPositions->size();

		for (size_t i = 0; i < boardSize; i++) {
			uint32_t threats = 0;
			olc::vi2d posA = i == position.x ? position : figuresPositions->at(i);

			for (size_t j = i + 1; j < boardSize; j++) {
				olc::vi2d posB = j == position.x ? position : figuresPositions->at(j);

				if (ntf::ChessBoard::FigureAtPosIsThreat(posA, posB))
					threats++;
			}

			threatsSum += threats;
		}

		return ntf::HeuristicValue{ position, threatsSum };
	};

	std::vector<ntf::HeuristicValue> evaluateColumn(uint32_t targetCol) override
	{
		std::vector<ntf::HeuristicValue> values{};

		size_t boardSize = figuresPositions->size();
		olc::vi2d trgPos = figuresPositions->at(targetCol);

		for (size_t i = 0; i < boardSize; i++)
			values.push_back(evaluatePosition({ trgPos.x, static_cast<int>(i) }));

		return values;
	}
};

ntf::Solution BeamSearch(
	const std::vector<olc::vi2d>& positions,
	const ntf::SolverParam& param,
	ntf::Heuristic* heuristic
) {
	using State = std::pair<std::vector<olc::vi2d>, uint32_t>;
	using Clock = std::chrono::system_clock;
	using Distribution = std::uniform_int_distribution<size_t>;

	std::default_random_engine randomEngine;
	auto seedEngine = [&]() { randomEngine.seed(static_cast<uint32_t>(Clock::now().time_since_epoch().count())); };

	int k = param.value;
	size_t size = positions.size();

	std::vector<State> states{};

	for (size_t i = 0; i < size; i++) {
		uint32_t heuristicValue = heuristic->evaluatePosition(positions[i]).value;
		states.push_back({ positions, heuristicValue });
	}

	for (int i = 0; i < k; i++) {
		seedEngine();
	}

	return {};
}

ntf::Solution TabooSearch(
	const std::vector<olc::vi2d>& positions,
	const ntf::SolverParam& param,
	ntf::Heuristic* heuristic
) {
	return {};
}

int main()
{
	ntf::Heuristic* localThreats(new LocalThreats);
	ntf::Heuristic* globalThreats(new GlobalThreats);

	ntf::Screen* board = new ntf::ChessBoard(
		{ localThreats, globalThreats },
		{{ "Beam Search", BeamSearch, { true, "K param", 100, 3, 8, }}, { "Taboo Search", TabooSearch }}
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