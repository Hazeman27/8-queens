#pragma once
#include "Window.h"

namespace ntf {
    struct Figure {
        std::string name;

        std::vector<olc::Sprite*> blackSprites;
        std::vector<olc::Sprite*> whiteSprites;

        Figure(
            const std::string& name,
            const std::vector<std::string>& blackSpritePaths,
            const std::vector<std::string>& whiteSpritePaths,
            const Window* window
        ) : name(name), blackSprites{}, whiteSprites{}
        {
            for (auto& spritePath : blackSpritePaths)
                blackSprites.push_back(new olc::Sprite(spritePath, window->resourcePack));

            for (auto& spritePath : whiteSpritePaths)
                whiteSprites.push_back(new olc::Sprite(spritePath, window->resourcePack));
        }

        ~Figure()
        {
            for (auto& sprite : blackSprites)
                delete sprite;

            for (auto& sprite : whiteSprites)
                delete sprite;
        }
    };
}