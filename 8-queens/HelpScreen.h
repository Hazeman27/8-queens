#pragma once
#include "Window.h"


namespace ntf {
    class HelpScreen : public Screen
    {
    private:
        Window* window;

    public:
        HelpScreen() : Screen("Help", olc::H, "H"), window(nullptr) {};
        ~HelpScreen() { delete window; };

        bool OnCreate(Window* window) override {
            this->window = window;
            return true;
        }

        bool DrawSelf(float elapsedTime) override
        {
            BoundingRect commands = window->DrawTextBox({ SCREEN_POSITION.x, SCREEN_POSITION.y }, {
                "<C> - Change figure",
                "<Y> - Change heuristic",
                "<R> - Randomize",
                "<S> - Change solution type",
                "<T> - Change theme",
                "<G> - Toggle global heuristic mode",
                "<Ctrl> + <S> - Solve",
                "<Ctrl> + <J> - Decremenet solver param",
                "<Ctrl> + <K> - Incremeent solver param",
                "<Ctrl> + \"+\" - Increment size",
                "<Ctrl> + \"-\" - Decrement size",
                });

            window->DrawTextBox(Window::PutBelow(commands), {
                "Hold <Shift> and hover over figure to see its threats.",
                "<Right Click> on figure, to see its heuristic, value.",
                "Click and drag figure, to change its position"
                });

            window->DrawAvailableScreenOptions({ BASE_GAP_I, window->ScreenHeight() - window->screenOptionsApproxSize.y - BASE_GAP_I });
            return true;
        }
    };
}
