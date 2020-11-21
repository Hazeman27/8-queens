#pragma once
#include "Window.h"


namespace ntf {
    class MenuScreen : public Screen
    {
    private:
        Window* window;

    public:
        MenuScreen() : Screen("Menu", olc::ESCAPE, "ESC"), window(nullptr) {};
        ~MenuScreen() { delete window; };

        bool OnCreate(Window* window) override {
            this->window = window;
            return true;
        }

        bool DrawSelf(float elapsedTime) override
        {
            for (uint32_t i = 0; i < window->screens.size(); i++) {
                olc::vi2d position = {
                    window->ScreenWidth() / 4,
                    window->ScreenHeight() / 2 + static_cast<int>(i) * STRING_HEIGHT_I
                };

                window->DrawString(
                    position,
                    "Press <" + window->screens[i]->keyStringValue + ">" + " to open " + window->screens[i]->name,
                    window->FgColor()
                );
            }

            return true;
        }
    };
}
