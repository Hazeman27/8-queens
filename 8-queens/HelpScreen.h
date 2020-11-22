#pragma once
#include "Window.h"


namespace ntf {
    class HelpScreen : public Screen
    {
    public:
        HelpScreen() : Screen("Help", olc::H, "H") {};

        bool OnCreate(const std::shared_ptr<Window> window) override {
            this->window = window;
            return true;
        }

        bool DrawSelf(float elapsedTime) override
        {
            BoundingRect commands = window->DrawTextBox({ SCREEN_POSITION.x, SCREEN_POSITION.y }, {
                "<Arrow Up> - Next heuristic",
                "<Arrow Down> - Prev. heuristic",
                "<Arrow Right> - Next solution type",
                "<Arrow Left> - Prev. solution type",
                "<C> - Change figure",
                "<R> - Randomize",
                "<T> - Change theme",
                "<G> - Toggle global heuristic mode",
                "<S> - Solve",
                "<Ctrl> + <J> - Decrease solver param",
                "<Ctrl> + <K> - Increase solver param",
                "<Ctrl> + <R> - Reset solver param",
                "<Ctrl> + \"+\" - Increase board size",
                "<Ctrl> + \"-\" - Decrease board size",
            });

            window->DrawTextBox(Window::PutBelow(commands), {
                "Hold <Shift> and hover over figure to see its threats.",
                "<Right Click> on figure, to see its heuristic, value.",
                "Click and drag figure, to change its position."
            });

            window->DrawAvailableScreenOptions({
                BASE_GAP_I,
                window->ScreenHeight() - window->screenOptionsApproxSize.y - BASE_GAP_I
            });

            return true;
        }
    };
}
