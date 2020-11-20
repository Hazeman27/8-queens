#pragma once
#include "Window.h"


namespace ntf {
	constexpr const char* STRING_SEPARATOR = "----------------------------------";

	class HelpScreen : public Screen
	{
	private:
		Window* window;

	public:
		HelpScreen() : Screen("Help", olc::H, "H"), window(nullptr) {};
		~HelpScreen() { delete window; };

		void DrawStrings(const std::vector<std::string>&& strings) {
			for (size_t i = 0; i < strings.size(); i++) {
				olc::vi2d position{ BASE_GAP_I, static_cast<int>(i) * STRING_HEIGHT_I + BASE_GAP_I + SCREEN_POSITION.y };
				window->DrawString(position, strings.at(i), window->FgColor());
			}
		}

		bool OnCreate(Window* window) override {
			this->window = window;
			return true;
		}

		bool DrawSelf(float elapsedTime) override
		{
			DrawStrings({
				STRING_SEPARATOR,
				"<C> - Change figure",
				"<Y> - Change heuristic",
				"<R> - Randomize",
				"<S> - Change solution type",
				"<T> - Change theme",
				"<Ctrl> + <S> - Solve",
				"<Ctrl> + \"+\" - Increment size",
				"<Ctrl> + \"-\" - Decrement size",
				STRING_SEPARATOR,
				"Hold <Shift> and hover over figure to show its threats.",
				STRING_SEPARATOR,
				"<Right Click> on figure, to show its heuristic, value.",
				STRING_SEPARATOR,
				"Click and drag figure, to change its position",
				STRING_SEPARATOR,
			});

			window->DrawAvailableScreenOptions({ BASE_GAP_I, window->ScreenHeight() - window->screenOptionsApproxSize.y - BASE_GAP_I });
			return true;
		}
	};
}
