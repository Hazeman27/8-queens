#include "ChessBoard.h"
#include "HelpScreen.h"
#include "MenuScreen.h"
#include "BeamSearch.h"
#include "TabuSearch.h"
#include "LocalThreats.h"
#include "GlobalThreats.h"


int main()
{
	std::shared_ptr<ntf::Heuristic> localThreats(new ntf::LocalThreats());
	std::shared_ptr<ntf::Heuristic> globalThreats(new ntf::GlobalThreats());

	std::shared_ptr<ntf::Solver> beamSearch(new ntf::BeamSearch({ true, "K param", 2, 1000, 16, 16, }));
	std::shared_ptr<ntf::Solver> tabuSearch(new ntf::TabuSearch({ true, "Tabu list max size", 2, 1000, 16, 16, }));

	std::shared_ptr<ntf::Screen> board(new ntf::ChessBoard({ globalThreats, localThreats, }, { beamSearch, tabuSearch }));
	std::shared_ptr<ntf::Screen> help(new ntf::HelpScreen());
	std::shared_ptr<ntf::Screen> menu(new ntf::MenuScreen());

	std::shared_ptr<ntf::Theme> dark(new ntf::Theme("Dark", "chess", { 57, 57, 57 }, { 255, 255, 255 }, olc::RED));
	std::shared_ptr<ntf::Theme> green(new ntf::Theme("Green", "chess_green", { 5, 113, 55 }, { 252, 244, 225 }, olc::MAGENTA));
	std::shared_ptr<ntf::Theme> pink(new ntf::Theme("Pink", "chess_pink", { 3, 11, 30 }, { 250, 142, 200 }, olc::YELLOW));

	std::shared_ptr<ntf::Window> window(new ntf::Window({ board, help, menu }, { dark, green, pink }, 0));

	if (window->Construct(460, 350, 2, 2))
		window->Start();

	return 0;
}