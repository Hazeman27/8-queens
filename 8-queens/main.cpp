#include "ChessBoard.h"
#include "HelpScreen.h"
#include "MenuScreen.h"
#include "BeamSearch.h"
#include "TabuSearch.h"
#include "LocalThreats.h"
#include "GlobalThreats.h"


int main()
{
	std::shared_ptr<ntf::Heuristic> localThreats(std::make_shared<ntf::LocalThreats>());
	std::shared_ptr<ntf::Heuristic> globalThreats(std::make_shared<ntf::GlobalThreats>());

	std::vector<std::shared_ptr<ntf::Heuristic>> heuristics{ localThreats, globalThreats };

	std::shared_ptr<ntf::Solver> beamSearch(std::make_shared<ntf::BeamSearch>(
		ntf::SolverParam{ true, "K param", 2, 1000, 16, 16, }
	));

	std::shared_ptr<ntf::Solver> tabuSearch(std::make_shared<ntf::TabuSearch>(
		ntf::SolverParam{ true, "Tabu list max size", 2, 1000, 16, 16, }
	));

	std::vector<std::shared_ptr<ntf::Solver>> solvers{ beamSearch, tabuSearch };

	std::shared_ptr<ntf::Screen> board(std::make_shared<ntf::ChessBoard>(heuristics, solvers));
	std::shared_ptr<ntf::Screen> help(std::make_shared<ntf::HelpScreen>());
	std::shared_ptr<ntf::Screen> menu(std::make_shared<ntf::MenuScreen>());

	std::shared_ptr<ntf::Theme> dark(std::make_shared<ntf::Theme>(
		"Dark",
		"chess",
		olc::Pixel{ 57, 57, 57 },
		olc::Pixel{ 255, 255, 255 },
		olc::RED
	));

	std::shared_ptr<ntf::Theme> green(std::make_shared<ntf::Theme>(
		"Green",
		"chess_green",
		olc::Pixel{ 5, 113, 55 },
		olc::Pixel{ 252, 244, 225 },
		olc::MAGENTA
	));

	std::shared_ptr<ntf::Theme> pink(std::make_shared<ntf::Theme>(
		"Pink",
		"chess_pink",
		olc::Pixel{ 3, 11, 30 },
		olc::Pixel{ 250, 142, 200 },
		olc::YELLOW
	));

	std::vector<std::shared_ptr<ntf::Screen>> screens{ board, help, menu };
	std::vector<std::shared_ptr<ntf::Theme>> themes{ dark, green, pink };

	std::shared_ptr<ntf::Window> window(std::make_shared<ntf::Window>(screens, themes, 0));

	if (window->Construct(480, 340, 2, 2))
		window->Start();

	return 0;
}