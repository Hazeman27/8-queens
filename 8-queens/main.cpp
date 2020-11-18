#include "ChessBoard.h"

int main()
{
	ChessBoard board(8);

	if (board.Construct(638, 352, 2, 2))
		board.Start();
}