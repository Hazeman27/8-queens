#pragma once

#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

namespace ntf {
	constexpr const char* APP_NAME = "8 Queens";
	constexpr const char* RESOURCE_PACK_NAME = "./8queens_res.dat";
	constexpr const char* RESOURCE_PACK_KEY = "G-r$y37mbS557ke_5?2r1!A";

	constexpr uint32_t DASHED_LINE_PATTERN = 0xF0F0F0F0;

	constexpr uint32_t DEFAULT_SCREEN = 0;
	constexpr uint32_t DEFAULT_THEME = 1;

	constexpr float BASE_GAP_F = 5.0f;
	constexpr float STRING_HEIGHT_F = 12.0f;

	constexpr int BASE_GAP_I = 5;
	constexpr int STRING_HEIGHT_I = 12;

	constexpr int CHAR_WIDTH = 8;

	struct PositionI {
		int x;
		int y;
	};

	constexpr PositionI TITLE_POSITION = { 4, 4 };
	constexpr PositionI SCREEN_POSITION = { 4, 16 };

	struct Window;

	struct Screen {
		std::string name;
		std::string keyStringValue;
		olc::Key key;

		Screen(const std::string& name, const olc::Key& key, const std::string& keyStringValue) :
			name(name),
			key(key),
			keyStringValue(keyStringValue)
		{};

		virtual bool OnCreate(Window* window) = 0;
		virtual bool DrawSelf(float elapsedTime) = 0;
	};

	struct Theme {
		std::string name;
		std::string figuresFolder;
		olc::Pixel bgColor;
		olc::Pixel fgColor;
	};

	struct Window : public olc::PixelGameEngine
	{
		uint32_t currentScreenIndex;
		uint32_t currentThemeIndex;

		std::vector<Screen*> screens;
		std::vector<Theme> themes;
		
		olc::vi2d screenOptionsApproxSize;
		olc::ResourcePack* resourcePack;

		Window(
			const std::vector<Screen*>& screens,
			const std::vector<Theme>& themes,
			uint32_t defaultScreen = DEFAULT_SCREEN,
			uint32_t defaultTheme = DEFAULT_THEME
		) :
			currentScreenIndex(defaultScreen),
			currentThemeIndex(defaultTheme),
			screenOptionsApproxSize({ 0, static_cast<int>(screens.size() * STRING_HEIGHT_I) }),
			screens(screens),
			themes(themes),
			resourcePack(new olc::ResourcePack())
		{
			sAppName = APP_NAME;

			if (!resourcePack->LoadPack(RESOURCE_PACK_NAME, RESOURCE_PACK_KEY)) {
				if (CreateResourcePack() && !resourcePack->LoadPack(RESOURCE_PACK_NAME, RESOURCE_PACK_KEY))
					resourcePack = nullptr;
			}
		}

		~Window()
		{
			for (auto& screen : screens)
				delete screen;
			delete resourcePack;
		}

		bool OnUserCreate() override {
			for (auto& screen : screens) {
				screen->OnCreate(this);
				
				if (screen->name.size() > screenOptionsApproxSize.x)
					screenOptionsApproxSize.x = static_cast<int>(screen->name.size() * 16) + 12;
			}

			return true;
		}

		bool OnUserUpdate(float fElapsedTime) override
		{
			Clear(BgColor());

			for (uint32_t i = 0; i < screens.size(); i++) {
				if (GetKey(screens[i]->key).bPressed)
					currentScreenIndex = i;
			}

			if (GetKey(olc::T).bPressed)
				currentThemeIndex = Window::GetNextArrayIndex(currentThemeIndex, themes.size());

			DrawString({ TITLE_POSITION.x, TITLE_POSITION.y }, CurrentScreen()->name, FgColor());
			return CurrentScreen()->DrawSelf(fElapsedTime);
		}

		olc::vi2d DrawTextBox(
			const olc::vi2d& position,
			const std::vector<std::string>& strings,
			bool includeBorder = true,
			uint32_t borderPattern = DASHED_LINE_PATTERN
		) {
			olc::vi2d size{ 0, static_cast<int>(strings.size() * STRING_HEIGHT_I) + position.y };

			for (auto& string : strings) {
				int width = static_cast<int>(string.size() * CHAR_WIDTH) + CHAR_WIDTH;

				if (width > size.x)
					size.x = width;
			}

			if (includeBorder) {
				olc::vi2d lineOffset1 = { position.x + size.x, position.y };
				olc::vi2d lineOffset2 = { position.x, size.y };
				olc::vi2d lineOffset3 = { lineOffset1.x, lineOffset2.y };

				DrawLine(position, lineOffset1, FgColor(), DASHED_LINE_PATTERN);
				DrawLine(lineOffset2, lineOffset3, FgColor(), DASHED_LINE_PATTERN);
				DrawLine(position, lineOffset2, FgColor(), DASHED_LINE_PATTERN);
				DrawLine(lineOffset1, lineOffset3, FgColor(), DASHED_LINE_PATTERN);
			}

			for (uint32_t i = 0, j = 0; i < strings.size(); i++) {
				if (i == currentScreenIndex)
					continue;

				olc::vi2d pos = { BASE_GAP_I, static_cast<int>(j++ * STRING_HEIGHT_I + BASE_GAP_I * 2) };
				DrawString(pos + position, strings[i], FgColor());
			}

			return size;
		}

		void DrawAvailableScreenOptions(const olc::vi2d& position)
		{
			std::vector<std::string> strings{};

			for (uint32_t i = 0; i < screens.size(); i++)
				strings.push_back("<" + screens[i]->keyStringValue + ">" + ": " + screens[i]->name);

			DrawTextBox(position, strings);
		}

		Screen* CurrentScreen() { return screens.at(currentScreenIndex); }

		bool CreateResourcePack()
		{
			if (resourcePack == nullptr)
				resourcePack = new olc::ResourcePack();

			std::vector<std::vector<std::string>> figuresImages = {
				GetThemeFilesPaths("black_bishop.png"),
				GetThemeFilesPaths("white_bishop.png"),
				GetThemeFilesPaths("black_king.png"),
				GetThemeFilesPaths("white_king.png"),
				GetThemeFilesPaths("black_knight.png"),
				GetThemeFilesPaths("white_knight.png"),
				GetThemeFilesPaths("black_pawn.png"),
				GetThemeFilesPaths("white_pawn.png"),
				GetThemeFilesPaths("black_queen.png"),
				GetThemeFilesPaths("white_queen.png"),
				GetThemeFilesPaths("black_rook.png"),
				GetThemeFilesPaths("white_rook.png"),
			};

			for (auto& figureImages : figuresImages) {
				for (auto& image : figureImages)
					resourcePack->AddFile(image);
			}

			if (!resourcePack->SavePack(RESOURCE_PACK_NAME, RESOURCE_PACK_KEY))
				return false;
			return true;
		}
		
		Theme CurrentTheme() { return themes.at(currentThemeIndex); }

		olc::Pixel BgColor() { return CurrentTheme().bgColor; }
		olc::Pixel FgColor() { return CurrentTheme().fgColor; }
		
		std::string GetThemeFilePath(const std::string& filepath, const Theme& theme)
		{
			return theme.figuresFolder + "/" + filepath;
		}

		std::vector<std::string> GetThemeFilesPaths(const std::string& filepath)
		{
			std::vector<std::string> filepaths{};

			for (auto& theme : themes)
				filepaths.push_back(GetThemeFilePath(filepath, theme));

			return filepaths;
		}

		static bool MouseIsInRectBounds(const olc::vf2d& rectPos, const olc::vi2d& rectSize, int mouseX, int mouseY)
		{
			return (mouseX > rectPos.x && mouseX < rectPos.x + rectSize.x && mouseY > rectPos.y && mouseY < rectPos.y + rectSize.y);
		}

		static uint32_t GetNextArrayIndex(uint32_t currentIndex, size_t arraySize)
		{
			return (static_cast<size_t>(currentIndex) + 1) % arraySize;
		}
	};
}
