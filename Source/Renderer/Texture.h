#pragma once
#include <string>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Texture {
public:
    Texture();

    ~Texture();


    bool Load(const std::string &filePath, bool repeat = false);

    bool CreateFromSurface(SDL_Surface *surface);

    void Unload();

    void SetActive(int index = 0) const;

    int GetWidth() const { return mWidth; }
    int GetHeight() const { return mHeight; }

private:
    GLuint mTextureID;
    int mWidth;
    int mHeight;
};