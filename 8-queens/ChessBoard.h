#pragma once

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Heuristic.h"
#include <vector>
#include <random>

namespace ntf {
	constexpr int16_t INVALID_FIGURE = -1;
	constexpr const char* STRING_SEPARATOR = "--------------";

	constexpr float STRING_GAP = 5.0f;
	constexpr float STRING_HEIGHT = 12.0f;

	constexpr int8_t MAX_BOARD_SIZE = 16;
	constexpr int8_t MIN_BOARD_SIZE = 4;

	enum class TileColor { BLACK, WHITE };
	enum class BoardSide { TOP, BOTTOM, LEFT };


	struct Figure {
		std::string name;

		olc::Sprite* blackSprite;
		olc::Sprite* whiteSprite;

		Figure(const std::string& name, const std::string& blackSpritePath, const std::string& whiteSpritePath) :
			name(name),
			blackSprite(new olc::Sprite(blackSpritePath)),
			whiteSprite(new olc::Sprite(whiteSpritePath))
		{}

		Figure(std::string&& name, std::string&& blackSpritePath, std::string&& whiteSpritePath) :
			name(std::move(name)),
			blackSprite(new olc::Sprite(std::move(blackSpritePath))),
			whiteSprite(new olc::Sprite(std::move(whiteSpritePath)))
		{}

		~Figure()
		{
			delete blackSprite;
			delete whiteSprite;
		}
	};

	class ChessBoard : public olc::PixelGameEngine
	{
	private:
		int16_t currentHeuristicResultFigureIndex;
		int16_t selectedFigureIndex;
		uint8_t currentChessFigureIndex;
		uint8_t currentHeuristicIndex;
		uint8_t size;

		olc::Pixel tileColorBlack;
		olc::Pixel tileColorWhite;

		olc::vf2d boardPosition;
		olc::vf2d boardSize;
		olc::vf2d tileSize;

		std::vector<Heuristic> heuristics;

		std::vector<olc::vi2d> figuresPositions;
		std::array<Figure*, 6> figures;

		std::default_random_engine randomGenerator;

	public:
		ChessBoard(const uint8_t size, const std::vector<Heuristic>& heuristicFunctions) :
			currentHeuristicResultFigureIndex(INVALID_FIGURE),
			selectedFigureIndex(INVALID_FIGURE),
			currentChessFigureIndex(4),
			currentHeuristicIndex(0),
			size(size),
			tileColorBlack(5, 113, 55),
			tileColorWhite(252, 244, 225),
			boardPosition{ 0.0f, 0.0f },
			boardSize{ 0.0f, 0.0f },
			tileSize{ 0.0f, 0.0f },
			heuristics(heuristicFunctions),
			figuresPositions{},
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
		void ChangeFigure()
		{
			currentChessFigureIndex = static_cast<uint8_t>(currentChessFigureIndex + 1) % static_cast<uint8_t>(figures.size());
		}

		void ChangeHeuristicFunction()
		{
			currentHeuristicIndex = static_cast<uint8_t>(currentHeuristicIndex + 1) % static_cast<uint8_t>(heuristics.size());
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

		void DrawBoard()
		{
			olc::vf2d posOffset{ -1.0f, -1.0f };
			olc::vf2d sizeOffset{ 1.0f, 1.0f };

			DrawRect(boardPosition + posOffset, boardSize + sizeOffset, tileColorWhite);

			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					olc::Pixel fillColor{};
					olc::Pixel borderColor(tileColorWhite);

					if (selectedFigureIndex != INVALID_FIGURE && j != figuresPositions.at(selectedFigureIndex).x)
						fillColor = olc::Pixel(3, 100, 48);
					else
						fillColor = GetTileColor(j, i);

					DrawTile(j, i, fillColor, borderColor);
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
					pos = GetMousePos() - currentFigureSpriteSize / 2;
				else
					pos = GetTilePosition(position) + (tileSize - currentFigureSpriteSize) / 2.0f;

				olc::Sprite* sprite = GetTileColorType(position.x, position.y) == TileColor::WHITE
					? figure->blackSprite : figure->whiteSprite;

				SetPixelMode(olc::Pixel::MASK);
				DrawSprite(pos, sprite);
				SetPixelMode(olc::Pixel::NORMAL);
			}
		}

		void DrawHeuristicResult()
		{
			if (currentHeuristicResultFigureIndex == INVALID_FIGURE)
				return;

			auto& [_, heuristicFunction] = heuristics.at(currentHeuristicIndex);

			auto results = heuristicFunction(currentHeuristicResultFigureIndex, figuresPositions);

			auto drawResult = [&](HeuristicValue result) {
				auto& [position, value] = result;

				olc::vf2d tilePos = GetTilePosition(position);
				olc::vf2d fgPos = { tilePos.x + 3.0f, tilePos.y + (tileSize.y / 2.0f) + 3.0f };

				DrawString(fgPos, std::to_string(value), GetTileColor(position, true));
			};

			auto& trgRes = results.at(figuresPositions[currentHeuristicResultFigureIndex].y);

			olc::vf2d tilePos = GetTilePosition(trgRes.position);
			olc::vf2d bgPos = { tilePos.x, tilePos.y + (tileSize.y / 2.0f) };
			olc::vi2d bgSize = { static_cast<int>(tileSize.x), static_cast<int>(tileSize.y / 2) };

			FillRect(bgPos, bgSize, GetTileColor(trgRes.position));
			DrawRect(GetTilePosition(trgRes.position), tileSize, olc::BLUE);

			for (auto& result : results)
				drawResult(result);
		}

		void DrawStrings(const std::vector<std::string>&& strings, BoardSide side = BoardSide::LEFT) {
			for (size_t i = 0; i < strings.size(); i++) {
				olc::vf2d position{};

				if (side == BoardSide::LEFT)
					position = { STRING_GAP, i * STRING_HEIGHT + boardPosition.y };

				else if (side == BoardSide::TOP)
					position = { STRING_GAP, i * STRING_HEIGHT + STRING_GAP };

				else if (side == BoardSide::BOTTOM)
					position = { STRING_GAP, i * STRING_HEIGHT + STRING_GAP + boardPosition.y + boardSize.y };

				DrawString(position, strings.at(i), tileColorWhite);
			}
		}

		void DrawTile(const uint32_t col, const uint32_t row, const olc::Pixel& fillColor, const olc::Pixel& borderColor)
		{
			FillRect(GetTilePosition(col, row), tileSize, fillColor);
			DrawRect(GetTilePosition(col, row), tileSize, borderColor);
		}

		void DrawTile(const olc::vi2d& position, const olc::Pixel& fillColor, const olc::Pixel& borderColor)
		{
			FillRect(GetTilePosition(position), tileSize, fillColor);
			DrawRect(GetTilePosition(position), tileSize, borderColor);
		}

		int GetMouseTargetFigureIndex()
		{
			int mouseX = GetMouseX();
			int mouseY = GetMouseY();

			int deltaX = mouseX - static_cast<int>(boardPosition.x);
			int index = deltaX / static_cast<int>(tileSize.x);

			if (index < 0 || index >= figuresPositions.size())
				return INVALID_FIGURE;

			auto position = GetTilePosition(figuresPositions.at(index));

			if (MouseIsInRectBounds(position, tileSize, mouseX, mouseY))
				return index;

			return INVALID_FIGURE;
		}

		olc::Pixel GetTileColor(const uint32_t col, const uint32_t row, bool inverse = false)
		{
			if (inverse)
				return (col + row) % 2 == 0 ? tileColorBlack : tileColorWhite;
			return (col + row) % 2 == 0 ? tileColorWhite : tileColorBlack;
		}

		olc::Pixel GetTileColor(const olc::vi2d& position, bool inverse = false)
		{
			return GetTileColor(position.x, position.y, inverse);
		}

		olc::Pixel GetTileColor(const TileColor color, bool inverse = false)
		{
			if (inverse)
				return color == TileColor::WHITE ? tileColorBlack : tileColorWhite;
			return color == TileColor::WHITE ? tileColorWhite : tileColorBlack;
		}

		TileColor GetTileColorType(const uint32_t col, const uint32_t row)
		{
			return (col + row) % 2 == 0 ? TileColor::WHITE : TileColor::BLACK;
		}

		TileColor GetTileColorType(const olc::vi2d& position)
		{
			return GetTileColorType(position.x, position.y);
		}

		olc::vf2d GetTilePosition(const uint32_t col, const uint32_t row)
		{
			return olc::vf2d{ static_cast<float>(col), static_cast<float>(row) } *tileSize + boardPosition;
		}

		olc::vf2d GetTilePosition(const olc::vi2d& position)
		{
			return position * tileSize + boardPosition;
		}

		olc::vi2d GetTilePositionI(const uint32_t col, const uint32_t row)
		{
			return {
				static_cast<int>(tileSize.x * col + boardPosition.x),
				static_cast<int>(tileSize.y * row + boardPosition.y)
			};
		}

		olc::vi2d GetTilePositionI(const olc::vi2d& position)
		{
			return GetTilePositionI(position.x, position.y);
		}

		void HighlightThreats()
		{
			int trgIndex = GetMouseTargetFigureIndex();

			if (trgIndex == INVALID_FIGURE)
				return;

			std::vector<uint32_t> threatIndices(GetThreatsIndices(trgIndex, figuresPositions));

			if (threatIndices.size() == 0)
				return;

			olc::vi2d trgPos = GetTilePositionI(figuresPositions.at(trgIndex)) + tileSize / 2.0f;

			for (auto& index : threatIndices) {
				auto threatPosition = GetTilePositionI(figuresPositions.at(index)) + tileSize / 2.0f;

				FillCircle(threatPosition, 2, olc::RED);
				DrawLine(trgPos, threatPosition, olc::RED, 0xF0F0F0F0);
			}
		}

		bool MouseIsInRectBounds(const olc::vf2d& rectPos, const olc::vi2d& rectSize, int mouseX, int mouseY)
		{
			return (
				mouseX > rectPos.x &&
				mouseX < rectPos.x + rectSize.x &&
				mouseY > rectPos.y &&
				mouseY < rectPos.y + rectSize.y
				);
		}

		void SelectFigure()
		{
			selectedFigureIndex = GetMouseTargetFigureIndex();
		}

		void SetCurrentHeuristicResultFigureIndex()
		{
			int index = GetMouseTargetFigureIndex();

			if (index == currentHeuristicResultFigureIndex)
				currentHeuristicResultFigureIndex = INVALID_FIGURE;
			else
				currentHeuristicResultFigureIndex = index;
		}

		void SetBoardMeasures()
		{
			float boardWidth = ScreenWidth() / 2.1f;
			float tileWidth = std::floorf(boardWidth / size);

			boardPosition = { ScreenWidth() / 2.0f, (ScreenHeight() - boardWidth) / 2.0f };
			boardSize = { tileWidth * size, tileWidth * size };
			tileSize = { tileWidth, tileWidth };

			currentHeuristicResultFigureIndex = INVALID_FIGURE;
			selectedFigureIndex = INVALID_FIGURE;
		}

		void RandomizePositions()
		{
			std::uniform_int_distribution<int> uniformDistribution(0, size - 1);
			figuresPositions.clear();

			for (int i = 0; i < size; i++)
				figuresPositions.push_back({ i, uniformDistribution(randomGenerator) });
		}

	public:
		Figure* CurrentFigure()
		{
			return figures[currentChessFigureIndex];
		}

		Heuristic CurrentHeuristicFunction()
		{
			return heuristics.at(currentHeuristicIndex);
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
			DrawHeuristicResult();

			DrawStrings({
				std::to_string(size) + " " + CurrentFigure()->name + "s puzzle",
				}, BoardSide::TOP);

			DrawStrings({
				"Size: " + std::to_string(size),
				"Figure: " + CurrentFigure()->name,
				"Heuristic: " + CurrentHeuristicFunction().name,
				"Search type: Taboo",
				STRING_SEPARATOR,
				"<C> - Change figure",
				"<H> - Change heuristic",
				"<R> - Randomize",
				"<Ctrl> + \"+\" - Inc. size",
				"<Ctrl> + \"-\" - Dec. size",
				STRING_SEPARATOR,
				"Hold <Shift> and hover",
				"over figure to show its",
				"threats.",
				STRING_SEPARATOR,
				"Click and drag figure",
				"to change its position",
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

			if (GetKey(olc::C).bPressed)
				ChangeFigure();

			if (GetKey(olc::H).bPressed)
				ChangeHeuristicFunction();

			if (GetKey(olc::R).bPressed)
				RandomizePositions();

			if (selectedFigureIndex == INVALID_FIGURE && GetMouse(0).bHeld)
				SelectFigure();

			if (selectedFigureIndex != INVALID_FIGURE && !GetMouse(0).bHeld)
				DeselectFigure();

			if (GetMouse(1).bPressed)
				SetCurrentHeuristicResultFigureIndex();

			return true;
		}

		static bool FigureAtPosIsThreat(const olc::vi2d& posA, const olc::vi2d& posB)
		{
			if (posA == posB)
				return false;
			return posA.y == posB.y || std::abs(posA.x - posB.x) == std::abs(posA.y - posB.y);
		}

		static std::vector<uint32_t> GetThreatsIndicesForPos(const olc::vi2d& trgPos, const std::vector<olc::vi2d>& positions)
		{
			std::vector<uint32_t> threats{};

			for (uint32_t i = 0; i < positions.size(); i++) {
				if (FigureAtPosIsThreat(trgPos, positions[i]))
					threats.push_back(i);
			}

			return threats;
		}

		static std::vector<uint32_t> GetThreatsIndices(const uint32_t trgIndex, const std::vector<olc::vi2d>& positions)
		{
			return GetThreatsIndicesForPos(positions.at(trgIndex), positions);
		}
	};
}
