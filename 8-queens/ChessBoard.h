#pragma once

#include "Window.h"
#include "Figure.h"
#include "Heuristic.h"
#include "Solver.h"
#include <random>


namespace ntf {
	constexpr int16_t INVALID_FIGURE = -1;

	constexpr uint8_t DEFAULT_BOARD_SIZE = 8U;
	constexpr int8_t MAX_BOARD_SIZE = 16;
	constexpr int8_t MIN_BOARD_SIZE = 4;
	
	constexpr uint32_t DEFAULT_FIGURE_INDEX = 4U;
	constexpr uint32_t FIGURES_COUNT = 6U;

	enum class TileColor { BLACK, WHITE };
	enum class BoardSide { TOP, RIGHT, BOTTOM, LEFT };

	class ChessBoard : public Screen
	{
	private:
		int16_t currentHeuristicResultFigureIndex;
		int16_t selectedFigureIndex;
		uint8_t currentFigureIndex;
		uint8_t currentHeuristicIndex;
		uint8_t currentSolverIndex;
		uint8_t size;

		Solution* currentSolution;

		olc::vf2d boardPosition;
		olc::vf2d boardSize;
		olc::vf2d tileSize;

		std::vector<Heuristic> heuristics;
		std::vector<Solver> solvers;

		std::vector<olc::vi2d> figuresPositions;
		std::array<Figure*, FIGURES_COUNT> figures;

		Window* window;
		std::default_random_engine randomGenerator;

	public:
		ChessBoard(const std::vector<Heuristic>& heuristics, const std::vector<Solver>& solvers) :
			Screen("Puzzle", olc::P, "P"),
			currentHeuristicResultFigureIndex(INVALID_FIGURE),
			selectedFigureIndex(INVALID_FIGURE),
			currentFigureIndex(DEFAULT_FIGURE_INDEX),
			currentHeuristicIndex(0),
			currentSolverIndex(0),
			size(DEFAULT_BOARD_SIZE),
			currentSolution(nullptr),
			boardPosition{ 0.0f, 0.0f },
			boardSize{ 0.0f, 0.0f },
			tileSize{ 0.0f, 0.0f },
			heuristics(heuristics),
			solvers(solvers),
			figuresPositions{},
			figures{},
			window(nullptr)
		{}

		~ChessBoard()
		{
			for (auto& figure : figures)
				delete figure;
			delete window;
		}

	private:
		void DeselectFigure()
		{
			if (selectedFigureIndex == INVALID_FIGURE)
				return;

			int mouseX = window->GetMouseX();
			int mouseY = window->GetMouseY();

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

			window->DrawRect(boardPosition + posOffset, boardSize + sizeOffset, window->FgColor());

			for (int i = 0; i < size; i++) {
				for (int j = 0; j < size; j++) {
					olc::Pixel fillColor{};
					olc::Pixel borderColor(window->FgColor());

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

			olc::vi2d currentFigureSpriteSize{
				figure->blackSprites.at(window->currentThemeIndex)->width,
				figure->blackSprites.at(window->currentThemeIndex)->height
			};

			for (int i = 0; i < size; i++) {
				auto& position = figuresPositions[i];
				olc::vf2d pos{};

				if (i == selectedFigureIndex)
					pos = window->GetMousePos() - currentFigureSpriteSize / 2;
				else
					pos = GetTilePosition(position) + (tileSize - currentFigureSpriteSize) / 2.0f;

				olc::Sprite* sprite = GetTileColorType(position.x, position.y) == TileColor::WHITE
					? figure->blackSprites.at(window->currentThemeIndex)
					: figure->whiteSprites.at(window->currentThemeIndex);

				window->SetPixelMode(olc::Pixel::MASK);
				window->DrawSprite(pos, sprite);
				window->SetPixelMode(olc::Pixel::NORMAL);
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

				window->DrawString(fgPos, std::to_string(value), GetTileColor(position, true));
			};

			auto& trgRes = results.at(figuresPositions[currentHeuristicResultFigureIndex].y);

			olc::vf2d tilePos = GetTilePosition(trgRes.position);
			olc::vf2d bgPos = { tilePos.x, tilePos.y + (tileSize.y / 2.0f) };
			olc::vi2d bgSize = { static_cast<int>(tileSize.x), static_cast<int>(tileSize.y / 2) };

			window->FillRect(bgPos, bgSize, GetTileColor(trgRes.position));
			window->DrawRect(GetTilePosition(trgRes.position), tileSize, olc::BLUE);

			for (auto& result : results)
				drawResult(result);
		}

		void DrawSolution()
		{
			if (currentSolution == nullptr)
				return;

			auto& [steps, duration, generatedStatesCount] = *currentSolution;

			for (auto& [index, position] : steps) {
				auto origPos = GetTilePositionI(figuresPositions.at(index));
				auto trgPos = GetTilePositionI(position);

				window->DrawLine(origPos, trgPos, olc::BLUE, DASHED_LINE_PATTERN);
				window->FillCircle(trgPos, 2, olc::BLUE);
			}
		}

		void DrawStrings(const std::vector<std::string>&& strings, BoardSide side = BoardSide::LEFT) {
			for (size_t i = 0; i < strings.size(); i++) {
				olc::vf2d position{};

				if (side == BoardSide::TOP)
					position = { boardPosition.x, i * STRING_HEIGHT_F + BASE_GAP_F };

				else if (side == BoardSide::RIGHT)
					position = { boardPosition.x + boardSize.x + BASE_GAP_F, boardPosition.y + i * STRING_HEIGHT_F };

				else if (side == BoardSide::BOTTOM)
					position = { boardPosition.x, boardPosition.y + boardSize.y + i * STRING_HEIGHT_F + BASE_GAP_F };

				else if (side == BoardSide::LEFT)
					position = { BASE_GAP_F, boardPosition.y + i * STRING_HEIGHT_F };

				window->DrawString(position, strings.at(i), window->FgColor());
			}
		}

		void DrawTile(const uint32_t col, const uint32_t row, const olc::Pixel& fillColor, const olc::Pixel& borderColor)
		{
			window->FillRect(GetTilePosition(col, row), tileSize, fillColor);
			window->DrawRect(GetTilePosition(col, row), tileSize, borderColor);
		}

		void DrawTile(const olc::vi2d& position, const olc::Pixel& fillColor, const olc::Pixel& borderColor)
		{
			window->FillRect(GetTilePosition(position), tileSize, fillColor);
			window->DrawRect(GetTilePosition(position), tileSize, borderColor);
		}

		int GetMouseTargetFigureIndex()
		{
			int mouseX = window->GetMouseX();
			int mouseY = window->GetMouseY();

			int deltaX = mouseX - static_cast<int>(boardPosition.x);
			int index = deltaX / static_cast<int>(tileSize.x);

			if (index < 0 || index >= figuresPositions.size())
				return INVALID_FIGURE;

			auto position = GetTilePosition(figuresPositions.at(index));

			if (Window::MouseIsInRectBounds(position, tileSize, mouseX, mouseY))
				return index;

			return INVALID_FIGURE;
		}

		olc::Pixel GetTileColor(const uint32_t col, const uint32_t row, bool inverse = false)
		{
			if (inverse)
				return (col + row) % 2 == 0 ? window->BgColor() : window->FgColor();
			return (col + row) % 2 == 0 ? window->FgColor() : window->BgColor();
		}

		olc::Pixel GetTileColor(const olc::vi2d& position, bool inverse = false)
		{
			return GetTileColor(position.x, position.y, inverse);
		}

		olc::Pixel GetTileColor(const TileColor color, bool inverse = false)
		{
			if (inverse)
				return color == TileColor::WHITE ? window->BgColor() : window->FgColor();
			return color == TileColor::WHITE ? window->FgColor() : window->BgColor();
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
			return olc::vf2d{ static_cast<float>(col), static_cast<float>(row) } * tileSize + boardPosition;
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

				window->FillCircle(threatPosition, 2, olc::RED);
				window->DrawLine(trgPos, threatPosition, olc::RED, DASHED_LINE_PATTERN);
			}
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
			float boardWidth = window->ScreenWidth() / 2.2f + BASE_GAP_I;
			float tileWidth = std::floorf(boardWidth / size);

			boardPosition = { window->ScreenWidth() / 2.0f, (window->ScreenHeight() - boardWidth) / 2.0f };
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
			if (currentFigureIndex == INVALID_FIGURE)
				return nullptr;
			return figures[currentFigureIndex];
		}

		Heuristic CurrentHeuristic()
		{
			return heuristics.at(currentHeuristicIndex);
		}

		Solver CurrentSolver()
		{
			return solvers.at(currentSolverIndex);
		}

		bool OnCreate(Window *window) override
		{
			this->window = window;

			figures = {
				new Figure("Bishop", window->GetThemeFilesPaths("black_bishop.png"), window->GetThemeFilesPaths("white_bishop.png"), window),
				new Figure("King", window->GetThemeFilesPaths("black_king.png"), window->GetThemeFilesPaths("white_king.png"), window),
				new Figure("Knight", window->GetThemeFilesPaths("black_knight.png"), window->GetThemeFilesPaths("white_knight.png"), window),
				new Figure("Pawn", window->GetThemeFilesPaths("black_pawn.png"), window->GetThemeFilesPaths("white_pawn.png"), window),
				new Figure("Queen", window->GetThemeFilesPaths("black_queen.png"), window->GetThemeFilesPaths("white_queen.png"), window),
				new Figure("Rook", window->GetThemeFilesPaths("black_rook.png"), window->GetThemeFilesPaths("white_rook.png"), window),
			};

			SetBoardMeasures();
			RandomizePositions();
			return true;
		}

		bool DrawSelf(float fElapsedTime) override
		{
			DrawBoard();
			DrawFigures();
			DrawHeuristicResult();
			
			olc::vi2d infoBoxSize = window->DrawTextBox(
				{ BASE_GAP_I, static_cast<int>(boardPosition.y) },
				{
					"Size: " + size,
					"Theme: " + window->CurrentTheme().name,
					"Figure: " + CurrentFigure()->name,
					"Heuristic: " + CurrentHeuristic().name,
					"Solution: " + CurrentSolver().name,
				}
			);

			window->DrawAvailableScreenOptions({ BASE_GAP_I, infoBoxSize.y + BASE_GAP_I });

			DrawStrings({ std::to_string(size) + " " + CurrentFigure()->name + "s puzzle" }, BoardSide::TOP);

			if (window->GetKey(olc::CTRL).bHeld && window->GetKey(olc::MINUS).bPressed && size > MIN_BOARD_SIZE) {
				size--;
				SetBoardMeasures();
				RandomizePositions();
			}

			if (window->GetKey(olc::CTRL).bHeld && window->GetKey(olc::EQUALS).bPressed && size < MAX_BOARD_SIZE) {
				size++;
				SetBoardMeasures();
				RandomizePositions();
			}

			if (window->GetKey(olc::SHIFT).bHeld && Window::MouseIsInRectBounds(boardPosition, boardSize, window->GetMouseX(), window->GetMouseY()))
				HighlightThreats();

			if (window->GetKey(olc::C).bPressed)
				currentFigureIndex = Window::GetNextArrayIndex(currentFigureIndex, figures.size());

			if (window->GetKey(olc::Y).bPressed)
				currentHeuristicIndex = Window::GetNextArrayIndex(currentHeuristicIndex, heuristics.size());

			if (window->GetKey(olc::R).bPressed)
				RandomizePositions();

			if (window->GetKey(olc::S).bPressed)
				currentSolverIndex = Window::GetNextArrayIndex(currentSolverIndex, solvers.size());

			if (selectedFigureIndex == INVALID_FIGURE && window->GetMouse(0).bHeld)
				SelectFigure();

			if (selectedFigureIndex != INVALID_FIGURE && !window->GetMouse(0).bHeld)
				DeselectFigure();

			if (window->GetMouse(1).bPressed)
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
