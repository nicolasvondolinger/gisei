#pragma once

// Includes necessários (substituindo o bits/stdc++.h)
#include <string>
#include <vector>
#include <unordered_map>
#include <SDL2/SDL.h>
#include "../Math.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Font.h" // Adicionado do 3D

// Declarações antecipadas
class Shader;
class UIElement;

enum class RendererMode
{
    TRIANGLES,
    LINES
};

class Renderer
{
public:
    Renderer(SDL_Window* window);
    ~Renderer();

    bool Initialize(float width, float height);
    void Shutdown();
    void UnloadData(); // Adicionado do 3D

    // --- Funções de Desenho Direto (do seu 2D original) ---
    // Usadas pelo Game.cpp e DrawComponent
    void DrawRect(const Vector2 &position, const Vector2 &size,  float rotation,
                  const Vector3 &color, const Vector2 &cameraPos, RendererMode mode);

    void DrawTexture(const Vector2 &position, const Vector2 &size,  float rotation,
                     const Vector3 &color, Texture *texture,
                     const Vector4 &textureRect = Vector4::UnitRect, // <-- CORRIGIDO AQUI
                     const Vector2 &cameraPos = Vector2::Zero, bool flip = false, // <-- E AQUI
                     float textureFactor = 1.0f);

    void DrawGeometry(const Vector2 &position, const Vector2 &size,  float rotation,
                      const Vector3 &color, const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode);

    // --- Gerenciamento de UI (Adicionado do 3D) ---
    void AddUIElement(class UIElement *comp);
    void RemoveUIElement(class UIElement *comp);

    // --- Funções do Loop Principal ---
    void Clear();
    void Draw(); // Agora desenha a UI (adicionado do 3D)
    void Present();

    // --- Getters ---
    class Texture* GetTexture(const std::string& fileName, bool repeat = false); // Versão 2D (com repeat)
    class Font* GetFont(const std::string& fileName); // Adicionado do 3D
    class Shader* GetBaseShader() const { return mBaseShader; }

private:
    // Helper de desenho (do 2D original)
    void Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
              const Vector3 &color,  Texture *texture = nullptr, const Vector4 &textureRect = Vector4::UnitRect, float textureFactor = 1.0f);

    bool LoadShaders();
    void CreateSpriteVerts();

    // Shader base (do 2D original)
    class Shader* mBaseShader;

    // Vértices de sprite (do 2D original)
    class VertexArray *mSpriteVerts;

    // Janela e Contexto
    SDL_Window* mWindow;
    SDL_GLContext mContext;

    // Projeção (do 2D original)
    Matrix4 mOrthoProjection;

    // Mapas de assets
    std::unordered_map<std::string, class Texture*> mTextures;
    std::unordered_map<std::string, class Font*> mFonts; // Adicionado do 3D

    // Lista de UI (Adicionado do 3D)
    std::vector<class UIElement*> mUIElements;

    // Dimensões da tela (Adicionado do 3D)
    float mScreenWidth;
    float mScreenHeight;
};