#pragma once


#include <string>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>
#include "../Math.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Font.h"


class Shader;
class UIElement;

enum class RendererMode {
    TRIANGLES,
    LINES
};

class Renderer {
public:
    Renderer(SDL_Window *window);

    ~Renderer();

    bool Initialize(float width, float height);

    void Shutdown();

    void UnloadData();


    void DrawRect(const Vector2 &position, const Vector2 &size, float rotation,
                  const Vector3 &color, const Vector2 &cameraPos, RendererMode mode);

    void DrawTexture(const Vector2 &position, const Vector2 &size, float rotation,
                     const Vector3 &color, Texture *texture,
                     const Vector4 &textureRect = Vector4::UnitRect,
                     const Vector2 &cameraPos = Vector2::Zero, bool flip = false,
                     float textureFactor = 1.0f);

    void DrawGeometry(const Vector2 &position, const Vector2 &size, float rotation,
                      const Vector3 &color, const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode);


    void AddUIElement(class UIElement *comp);

    void RemoveUIElement(class UIElement *comp);


    void Clear();

    void Draw();

    void Present();


    class Texture *GetTexture(const std::string &fileName, bool repeat = false);

    class Font *GetFont(const std::string &fileName);

    class Shader *GetBaseShader() const { return mBaseShader; }
    SDL_Window* GetWindow() const { return mWindow; }

private:
    void Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
              const Vector3 &color, Texture *texture = nullptr, const Vector4 &textureRect = Vector4::UnitRect,
              float textureFactor = 1.0f);

    bool LoadShaders();

    void CreateSpriteVerts();


    class Shader *mBaseShader;


    class VertexArray *mSpriteVerts;


    SDL_Window *mWindow;
    SDL_GLContext mContext;


    Matrix4 mOrthoProjection;


    std::unordered_map<std::string, class Texture *> mTextures;
    std::unordered_map<std::string, class Font *> mFonts;


    std::vector<class UIElement *> mUIElements;


    float mScreenWidth;
    float mScreenHeight;
};