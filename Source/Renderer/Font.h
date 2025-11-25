#pragma once
#include <string>
#include <unordered_map>
#include <SDL_ttf.h>
#include "../Math.h"

class Font {
public:
    Font();

    ~Font();


    bool Load(const std::string &fileName);

    void Unload();


    class Texture *RenderText(const std::string &text, const Vector3 &color = Color::White,
                              int pointSize = 30, unsigned wrapLength = 900);

private:
    std::unordered_map<int, TTF_Font *> mFontData;
};