#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Vector2.h"
#include <vector>
#include <random>

constexpr int INVALID_FIGURE = -1;
constexpr const char* STRING_SEPARATOR = "--------------";

constexpr int8_t MAX_BOARD_SIZE = 18;
constexpr int8_t MIN_BOARD_SIZE = 6;

enum class TileColor { BLACK, WHITE };
enum class BoardSide { TOP, RIGHT, BOTTOM, LEFT };

struct Figure {
	std::string name;

	olc::Sprite* blackSprite;
	olc::Sprite* whiteSprite;

	Figure(const std::string& name, const std::string& blackSpritePath, const std::string& whiteSpritePath) :
		name(name),
		blackSprite(new olc::Sprite(blackSpritePath)),
		whiteSprite(new olc::Sprite(whiteSpritePath))
	{}

	~Figure()
	{
		delete blackSprite;
		delete whiteSprite;
	}
};

class ChessBoard : public olc::PixelGameEngine
{
	int8_t currentChessFigureIndex;
	int size;
	int selectedFigureIndex;
	
	olc::vf2d boardPosition;
	olc::vf2d boardSize;
	olc::vf2d tileSize;

	std::default_random_engine randomGenerator;

	olc::Pixel tileColorBlack;
	olc::Pixel tileColorWhite;

	std::vector<olc::vi2d> figuresPositions;
	std::array<Figure*, 6> figures;

public:
	ChessBoard(int32_t size) :
		currentChessFigureIndex(4),
		size(size),
		selectedFigureIndex(INVALID_FIGURE),
		boardPosition{ 0.0f, 0.0f },
		boardSize{ 0.0f, 0.0f },
		tileSize{ 0.0f, 0.0f },
		tileColorBlack(5, 113, 55),
		tileColorWhite(252, 244, 225),
		figures{
			new Figure("Bishop", "chess_green/black_bishop.png", "chess_green/white_bishop.png"),
			new Figure("King", "chess_green/black_king.png", "chess_green/white_king.png"),
			new Figure("Knight", "chess_green/black_knight.png", "chess_green/white_knight.png"),
			new Figure("Pawn", "chess_green/black_pawn.png", "chess_green/white_pawn.png"),
			new Figure("Queen", "chess_green/black_queen.png", "chess_green/white_queen.png"),
			new Figure("Rook", "chess_green/black_rook.png", "chess_green/white_rook.png"),
		}
	{
		sAppName = "8 Queens";
	}

	~ChessBoard()
	{
		for (auto& figure : figures)
			delete figure;
	}

private:
	TileColor GetTileColor(const size_t col, const size_t row)
	{
		return (col + row) % 2 == 0 ? TileColor::WHITE : TileColor::BLACK;
	}

	olc::vf2d GetTilePosition(const size_t col, const size_t row)
	{
		olc::vf2d position = { static_cast<float>(col), static_cast<float>(row) };
		return position * tileSize + boardPosition;
	}

	olc::vf2d GetTilePosition(const olc::vi2d& position)
	{
		return position * tileSize + boardPosition;
	}

	olc::vi2d GetTilePositionI(const size_t col, const size_t row)
	{
		return {
			static_cast<int>(tileSize.x * col + boardPosition.x),
			static_cast<int>(tileSize.y * row + boardPosition.y)
		};
	}

	bool MouseIsInRectBounds(const olc::vf2d& rectPos, const olc::vi2d& rectSize, int mouseX, int mouseY)
	{
		return mouseX > rectPos.x && mouseX < rectPos.x + rectSize.x && mouseY > rectPos.y && mouseY < rectPos.y + rectSize.y;
	}

	int GetMouseTargetFigureIndex()
	{
		int mouseX = GetMouseX();
		int mouseY = GetMouseY();

		int deltaX = mouseX - static_cast<int>(boardPosition.x);
		int index = deltaX / static_cast<int>(tileSize.x);

		if (MouseIsInRectBounds(GetTilePosition(figuresPositions[index]), tileSize, mouseX, mouseY))
			return index;

		return INVALID_FIGURE;
	}

	void SetBoardMeasures()
	{
		int screenMinDimension = std::min(ScreenWidth(), ScreenHeight());

		float boardWidth = static_cast<float>(screenMinDimension) * 0.7f;
		float tileWidth = std::floorf(boardWidth / size);

		boardPosition = { (ScreenWidth() - boardWidth) / 2.0f, (ScreenHeight() - boardWidth) / 2.0f };
		boardSize = { tileWidth * size, tileWidth * size };
		tileSize = { tileWidth, tileWidth };
	}

	void RandomizePositions()
	{
		std::uniform_int_distribution<int> uniformDistribution(0, size - 1);
		figuresPositions.clear();

		for (int i = 0; i < size; i++)
			figuresPositions.push_back({ i, uniformDistribution(randomGenerator) });
	}

	void DrawBoard()
	{
		olc::vf2d posOffset{ -1.0f, -1.0f };
		olc::vf2d sizeOffset{ 1.0f, 1.0f };

		DrawRect(boardPosition + posOffset, boardSize + sizeOffset, tileColorWhite);

		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				olc::Pixel fillColor{};
				olc::Pixel borderColor(tileColorWhite);

				if (selectedFigureIndex != INVALID_FIGURE && j != figuresPositions[selectedFigureIndex].x)
					fillColor = olc::Pixel(3, 100, 48);

				else
					fillColor = GetTileColor(j, i) == TileColor::WHITE ? tileColorWhite : tileColorBlack;

				FillRect(GetTilePosition(j, i), tileSize, fillColor);
				DrawRect(GetTilePosition(j, i), tileSize, borderColor);
			}
		}
	}

	void DrawFigures()
	{
		Figure* figure = CurrentFigure();
		olc::vi2d currentFigureSpriteSize{ figure->blackSprite->width, figure->blackSprite->height };

		for (int i = 0; i < size; i++) {
			auto& position = figuresPositions[i];
			olc::vf2d pos{};

			if (i == selectedFigureIndex)
				pos = GetMousePos() - (currentFigureSpriteSize / 2.0f);
			else
				pos = GetTilePosition(position) + (tileSize - currentFigureSpriteSize) / 2.0f;

			olc::Sprite* sprite = GetTileColor(position.x, position.y) == TileColor::WHITE
				? figure->blackSprite : figure->whiteSprite;

			SetPixelMode(olc::Pixel::MASK);
			DrawSprite(pos, sprite);
			SetPixelMode(olc::Pixel::NORMAL);
		}
	}

	void ChangeFigure()
	{
		currentChessFigureIndex = (currentChessFigureIndex + 1) % figures.size();
	}

	void DrawStrings(const std::vector<std::string>& strings, BoardSide side = BoardSide::LEFT) {
		for (size_t i = 0; i < strings.size(); i++) {
			olc::vf2d position{};

			if (side == BoardSide::LEFT)
				position = { 5.0f, i * 12.0f + boardPosition.y };

			else if (side == BoardSide::RIGHT)
				position = { 5.0f + boardPosition.x + boardSize.x, i * 12.0f + boardPosition.y };

			else if (side == BoardSide::TOP)
				position = { boardPosition.x, i * 12.0f + 5.0f };

			else
				position = { boardPosition.x, i * 12.0f + 5.0f + boardPosition.y + boardSize.y };

			DrawString(position, strings.at(i), tileColorWhite);
		}
	}

	void HighlightThreats()
	{
		int targetIndex = GetMouseTargetFigureIndex();

		if (targetIndex == INVALID_FIGURE)
			return;

		std::vector<int> threatIndices(GetThreatsIndices(targetIndex, figuresPositions));

		if (threatIndices.size() == 0)
			return;

		olc::vi2d targetPosition = GetTilePositionI(
			figuresPositions[targetIndex].x,
			figuresPositions[targetIndex].y
		) + tileSize / 2.0f;

		for (auto& index : threatIndices) {
			DrawLine(
				targetPosition,
				GetTilePositionI(figuresPositions[index].x, figuresPositions[index].y) + tileSize / 2.0f,
				olc::RED,
				0xF0F0F0F0
			);
		}
	}

	void SelectFigure()
	{
		selectedFigureIndex = GetMouseTargetFigureIndex();
	}

	void DeselectFigure()
	{
		if (selectedFigureIndex == INVALID_FIGURE)
			return;

		int mouseX = GetMouseX();
		int mouseY = GetMouseY();

		int col = (mouseX - static_cast<int>(boardPosition.x)) / static_cast<int>(tileSize.x);
		int row = (mouseY - static_cast<int>(boardPosition.y)) / static_cast<int>(tileSize.y);

		if (col == selectedFigureIndex)
			figuresPositions[selectedFigureIndex] = { col, row };

		selectedFigureIndex = INVALID_FIGURE;
	}

public:
	Figure* CurrentFigure()
	{
		return figures[currentChessFigureIndex];
	}

	bool OnUserCreate() override
	{
		SetBoardMeasures();
		RandomizePositions();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(tileColorBlack);

		DrawBoard();
		DrawFigures();

		DrawStrings({
			"Size: " + std::to_string(size),
			"Figure: " + CurrentFigure()->name,
			"Search type: Taboo",
			STRING_SEPARATOR,
			"<C> - Change figure",
			"<R> - Randomize",
			"<Ctrl>+\"+\" - Inc. size",
			"<Ctrl>+\"-\" - Dec. size",
			STRING_SEPARATOR,
			"Hold <Shift> and hover",
			"over figure to show its",
			"threats.",
			STRING_SEPARATOR,
			"You can click and drag",
			"figures.",
			STRING_SEPARATOR,
		});

		if (GetKey(olc::CTRL).bHeld && GetKey(olc::MINUS).bPressed && size > MIN_BOARD_SIZE) {
			size--;
			SetBoardMeasures();
			RandomizePositions();
		}

		if (GetKey(olc::CTRL).bHeld && GetKey(olc::EQUALS).bPressed && size < MAX_BOARD_SIZE) {
			size++;
			SetBoardMeasures();
			RandomizePositions();
		}

		if (GetKey(olc::SHIFT).bHeld && MouseIsInRectBounds(boardPosition, boardSize, GetMouseX(), GetMouseY()))
			HighlightThreats();

		if (GetKey(olc::R).bPressed)
			RandomizePositions();

		if (GetKey(olc::C).bPressed)
			ChangeFigure();

		if (selectedFigureIndex == INVALID_FIGURE && GetMouse(0).bHeld)
			SelectFigure();

		if (selectedFigureIndex != INVALID_FIGURE && GetMouse(0).bReleased)
			DeselectFigure();

		return true;
	}

	static std::vector<int> GetThreatsIndices(const int targetIndex, const std::vector<olc::vi2d>& positions)
	{
		std::vector<int> threats{};

		auto& current = positions.at(targetIndex);

		for (size_t j = targetIndex + 1; j < positions.size(); j++) {
			auto& pos = positions[j];

			if (pos.y == current.y || std::abs(current.x - pos.x) == std::abs(current.y - pos.y))
				threats.push_back(j);
		}

		return threats;
	}
};

