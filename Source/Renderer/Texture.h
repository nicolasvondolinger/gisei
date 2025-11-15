#pragma once
#include <bits/stdc++.h>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;

class Texture
{
public:
	Texture();
	~Texture();

	bool Load(const string& fileName, bool repeat = false);
	void Unload();

	void SetActive(int index = 0) const;

    static GLenum SDLFormatToGL(SDL_PixelFormat* fmt);

	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }

	unsigned int GetTextureID() const { return mTextureID; }

private:
	unsigned int mTextureID;
	int mWidth;
	int mHeight;
};
