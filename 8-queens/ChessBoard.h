#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Vector2.h"
#include <vector>
#include <random>

constexpr auto STRING_SEPARATOR = "--------------";;

constexpr int8_t MAX_BOARD_SIZE = 18;
constexpr int8_t MIN_BOARD_SIZE = 6;

enum class TileColor { BLACK, WHITE };

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
	char currentChessFigureIndex;
	int size;
	
	olc::vf2d boardPosition;
	olc::vf2d boardSize;
	olc::vf2d tileSize;

	std::default_random_engine randomGenerator;

	olc::Pixel tileColorBlack;
	olc::Pixel tileColorWhite;

	std::vector<olc::vi2d> queensPositions;
	std::array<Figure*, 6> figures;

public:
	ChessBoard(int size) :
		currentChessFigureIndex(4),
		size(size),
		boardPosition({ 0.0f, 0.0f }),
		boardSize({ 0.0f, 0.0f }),
		tileSize({ 0.0f, 0.0f }),
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

	bool MouseIsInRectBounds(const olc::vf2d& rectPos, const olc::vi2d& rectSize)
	{
		return GetMouseX() > rectPos.x && GetMouseX() < rectSize.x && GetMouseY() > rectPos.y && GetMouseY() < rectSize.y;
	}

	void SetBoardMeasures()
	{
		int screenMinDimension = std::min(ScreenWidth(), ScreenHeight());

		float boardWidth = static_cast<float>(screenMinDimension) * 0.7f;
		float tileWidth = std::floorf(boardWidth / size);

		boardPosition = { (ScreenWidth() - boardWidth) / 2.0f, (ScreenHeight() - boardWidth) / 2.0f };
		boardSize = { boardWidth, boardWidth };
		tileSize = { tileWidth, tileWidth };
	}

	void RandomizePositions()
	{
		std::uniform_int_distribution<int> uniformDistribution(0, size - 1);
		queensPositions.clear();

		for (int i = 0; i < size; i++)
			queensPositions.push_back({ i, uniformDistribution(randomGenerator) });
	}

	void DrawBoard()
	{
		DrawRect(boardPosition, boardSize, tileColorWhite);

		for (int i = 0; i < size; i++) {
			for (int j = 0; j < size; j++) {
				olc::Pixel color = GetTileColor(j, i) == TileColor::WHITE ? tileColorWhite : tileColorBlack;
				FillRect(GetTilePosition(j, i), tileSize, color);
			}
		}
	}

	void DrawQueens()
	{
		Figure* figure = CurrentFigure();
		olc::vi2d currentFigureSpriteSize = { figure->blackSprite->width, figure->blackSprite->height };

		for (auto& position : queensPositions) {
			olc::vf2d pos = GetTilePosition(position) + (tileSize - currentFigureSpriteSize) / 2.0f;

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

	void DrawStrings(const std::vector<std::string>& strings) {
		float topOffset = boardPosition.y;

		for (size_t i = 0; i < strings.size(); i++) {
			olc::vf2d position = { 5.0f, i * 12.0f + topOffset };
			DrawString(position, strings.at(i), tileColorWhite);
		}
	}

	void HighlightThreats()
	{
		size_t targetIndex = -1;
		std::vector<size_t> threatIndices;

		for (size_t i = 0; i < size; i++) {
			olc::vf2d position = GetTilePosition(queensPositions[i]);

			if (MouseIsInRectBounds(position, tileSize)) {
				targetIndex = i;
				threatIndices = GetThreatsIndices(targetIndex, queensPositions);
				break;
			}
		}

		if (targetIndex < 0)
			return;

		olc::vi2d targetPosition = {
			static_cast<int>(tileSize.x * targetIndex * 1.5f),
			static_cast<int>(tileSize.y * targetIndex * 1.5f)
		};

		for (auto& index : threatIndices) {
			DrawLine(
				targetPosition,
				{ static_cast<int>(tileSize.x * index * 1.5f), static_cast<int>(tileSize.y * index * 1.5f) },
				olc::RED,
				0xF0F0F0F0
			);
		}
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
		DrawQueens();

		DrawStrings({
			"Current figure: " + CurrentFigure()->name,
			"Search type: Taboo",
			STRING_SEPARATOR,
			"C - Change figure",
			"R - Randomize positions",
			"Ctrl + \"+\" - Inc. size",
			"Ctrl + \"-\" - Dec. size",
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

		if (GetKey(olc::SHIFT).bHeld && MouseIsInRectBounds(boardPosition, boardSize))
			HighlightThreats();

		if (GetKey(olc::R).bPressed)
			RandomizePositions();

		if (GetKey(olc::C).bPressed)
			ChangeFigure();

		return true;
	}

	static std::vector<size_t> GetThreatsIndices(const size_t targetIndex, const std::vector<olc::vi2d>& positions)
	{
		std::vector<size_t> threats;
		size_t positionsCount = positions.size();

		auto& curr = positions.at(targetIndex);

		for (size_t j = targetIndex; j < positionsCount; j++) {
			auto& pos = positions[j];

			if (pos.y == curr.y || std::abs(curr.x - pos.x) == std::abs(curr.y - pos.y))
				threats.push_back(j);
		}

		return threats;
	}
};

