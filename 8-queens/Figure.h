#pragma once
#include "Window.h"


namespace ntf {
    class Figure
    {
    public:
        std::string name;

        std::vector<std::shared_ptr<olc::Sprite>> blackSprites;
        std::vector<std::shared_ptr<olc::Sprite>> whiteSprites;

        Figure(
            const std::string& name,
            const std::vector<std::string>& blackSpritePaths,
            const std::vector<std::string>& whiteSpritePaths,
            const std::shared_ptr<Window> window
        ) : name(name), blackSprites{}, whiteSprites{}
        {
            for (auto& spritePath : blackSpritePaths)
                blackSprites.push_back(std::make_shared<olc::Sprite>(spritePath, window->resourcePack));

            for (auto& spritePath : whiteSpritePaths)
                whiteSprites.push_back(std::make_shared<olc::Sprite>(spritePath, window->resourcePack));
        }

        Figure(
            std::string&& name,
            std::vector<std::string>&& blackSpritePaths,
            std::vector<std::string>&& whiteSpritePaths,
            const std::shared_ptr<Window> window
        ) : name(std::move(name)), blackSprites{}, whiteSprites{}
        {
            for (auto& spritePath : blackSpritePaths)
                blackSprites.push_back(std::make_shared<olc::Sprite>(spritePath, window->resourcePack));

            for (auto& spritePath : whiteSpritePaths)
                whiteSprites.push_back(std::make_shared<olc::Sprite>(spritePath, window->resourcePack));
        }
    };
}