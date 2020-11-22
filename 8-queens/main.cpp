#include "ChessBoard.h"
#include "HelpScreen.h"
#include "MenuScreen.h"
#include "BeamSearch.h"
#include "LocalThreats.h"
#include "GlobalThreats.h"


int main()
{
	ntf::Heuristic* localThreats(new ntf::LocalThreats);
	ntf::Heuristic* globalThreats(new ntf::GlobalThreats);

	ntf::Solver* beamSearch(new ntf::BeamSearch({ true, "K param", 56, 3, 8, }));

	ntf::Screen* board = new ntf::ChessBoard(
		{ localThreats, globalThreats },
		{ beamSearch }
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