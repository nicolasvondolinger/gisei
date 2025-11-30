#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Font.h"
#include "../UI/UIElement.h"
#include <SDL2/SDL.h>

Renderer::Renderer(SDL_Window *window)
    : mBaseShader(nullptr), mContext(nullptr), mWindow(window), mSpriteVerts(nullptr)
    , mOrthoProjection(Matrix4::Identity), mScreenWidth(1024.0f), mScreenHeight(768.0f) {}

Renderer::~Renderer() {
    delete mSpriteVerts; mSpriteVerts = nullptr;
}

bool Renderer::Initialize(float width, float height) {
    mScreenWidth = width; mScreenHeight = height;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetSwapInterval(1);

    mContext = SDL_GL_CreateContext(mWindow);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return false;
    if (!LoadShaders()) return false;

    CreateSpriteVerts();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    UpdateViewportToWindow();
    return true;
}

void Renderer::AddUIElement(UIElement *comp) {
    mUIElements.emplace_back(comp);
    std::sort(mUIElements.begin(), mUIElements.end(), [](UIElement *a, UIElement *b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Renderer::RemoveUIElement(UIElement *comp) {
    auto iter = std::find(mUIElements.begin(), mUIElements.end(), comp);
    if (iter != mUIElements.end()) mUIElements.erase(iter);
}

void Renderer::UnloadData() {
    for (auto i: mTextures) { i.second->Unload(); delete i.second; }
    mTextures.clear();
    for (auto i: mFonts) { i.second->Unload(); delete i.second; }
    mFonts.clear();
}

void Renderer::Shutdown() {
    UnloadData();
    delete mSpriteVerts; mSpriteVerts = nullptr;
    mBaseShader->Unload(); delete mBaseShader;
    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

void Renderer::UpdateViewportToWindow() {
    int w = 0, h = 0;
    SDL_GL_GetDrawableSize(mWindow, &w, &h);
    if (w > 0 && h > 0) glViewport(0, 0, w, h);
}

void Renderer::Clear() { glClear(GL_COLOR_BUFFER_BIT); }

void Renderer::Draw() {
    glEnable(GL_BLEND); glDisable(GL_DEPTH_TEST);
    mBaseShader->SetActive(); mSpriteVerts->SetActive();
    mBaseShader->SetIntegerUniform("uIsUI", 1);
    for (auto ui: mUIElements) ui->Draw(mBaseShader);
}

void Renderer::Present() { SDL_GL_SwapWindow(mWindow); }

void Renderer::Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
                    const Vector3 &color, Texture *texture, const Vector4 &textureRect, float textureFactor) {
    mBaseShader->SetMatrixUniform("uWorldTransform", modelMatrix);
    mBaseShader->SetVectorUniform("uBaseColor", Vector4(color, 1.0f));
    mBaseShader->SetVectorUniform("uTexRect", textureRect);
    mBaseShader->SetVectorUniform("uCameraPos", cameraPos);
    mBaseShader->SetFloatUniform("uWhiteAura", 0.0f);

    if (vertices) vertices->SetActive(); else mSpriteVerts->SetActive();

    if (texture) {
        texture->SetActive();
        mBaseShader->SetFloatUniform("uTextureFactor", textureFactor);
    } else {
        mBaseShader->SetFloatUniform("uTextureFactor", 0.0f);
    }

    if (mode == RendererMode::LINES) glDrawElements(GL_LINE_LOOP, vertices ? vertices->GetNumIndices() : 6, GL_UNSIGNED_INT, nullptr);
    else glDrawElements(GL_TRIANGLES, vertices ? vertices->GetNumIndices() : 6, GL_UNSIGNED_INT, nullptr);
}

void Renderer::DrawRect(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                        const Vector2 &cameraPos, RendererMode mode) {
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));
    Draw(mode, model, cameraPos, nullptr, color);
}

void Renderer::DrawTexture(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                           Texture *texture, const Vector4 &textureRect, const Vector2 &cameraPos, bool flip,
                           float textureFactor) {
    float flipFactor = flip ? -1.0f : 1.0f;
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x * flipFactor, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));
    Draw(RendererMode::TRIANGLES, model, cameraPos, nullptr, color, texture, textureRect, textureFactor);
}

void Renderer::DrawGeometry(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                            const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode) {
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));
    Draw(mode, model, cameraPos, vertexArray, color);
}

bool Renderer::LoadShaders() {
    mBaseShader = new Shader();
    if (!mBaseShader->Load("../Shaders/Base")) return false;
    mBaseShader->SetActive();
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, mScreenWidth, mScreenHeight, 0.0f, -1.0f, 1.0f);
    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
    mBaseShader->SetIntegerUniform("uTexture", 0);
    mBaseShader->SetFloatUniform("uWhiteAura", 0.0f);
    return true;
}

void Renderer::CreateSpriteVerts() {
    float vertices[] = { -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f };
    unsigned int indices[] = { 0, 1, 2, 2, 3, 0 };
    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

Texture *Renderer::GetTexture(const std::string &fileName, bool repeat) {
    Texture *tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end()) tex = iter->second;
    else {
        tex = new Texture();
        if (tex->Load(fileName, repeat)) mTextures.emplace(fileName, tex);
        else { delete tex; tex = nullptr; }
    }
    return tex;
}

Font *Renderer::GetFont(const std::string &fileName) {
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end()) return iter->second;
    else {
        Font *font = new Font();
        if (font->Load(fileName)) mFonts.emplace(fileName, font);
        else { font->Unload(); delete font; font = nullptr; }
        return font;
    }
}

void Renderer::DrawFade(float alpha) {
    if (alpha <= 0.0f) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mBaseShader->SetActive();
    mSpriteVerts->SetActive();
    mBaseShader->SetIntegerUniform("uIsUI", 1);

    Matrix4 scaleMat = Matrix4::CreateScale(Vector3(mScreenWidth, mScreenHeight, 1.0f));
    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mScreenWidth / 2.0f, mScreenHeight / 2.0f, 0.0f));
    Matrix4 world = scaleMat * transMat;

    mBaseShader->SetMatrixUniform("uWorldTransform", world);
    mBaseShader->SetFloatUniform("uTextureFactor", 0.0f);
    mBaseShader->SetVectorUniform("uBaseColor", Vector4(0.0f, 0.0f, 0.0f, alpha));
    mBaseShader->SetVectorUniform("uCameraPos", Vector2::Zero);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    mBaseShader->SetIntegerUniform("uIsUI", 0);
}

void Renderer::DrawDarkOverlay(float alpha) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mBaseShader->SetActive();
    mSpriteVerts->SetActive();
    
    mBaseShader->SetIntegerUniform("uIsUI", 1);

    Matrix4 scaleMat = Matrix4::CreateScale(Vector3(mScreenWidth, mScreenHeight, 1.0f));
    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mScreenWidth / 2.0f, mScreenHeight / 2.0f, 0.0f));
    Matrix4 world = scaleMat * transMat;

    mBaseShader->SetMatrixUniform("uWorldTransform", world);
    mBaseShader->SetFloatUniform("uTextureFactor", 0.0f);
    mBaseShader->SetVectorUniform("uBaseColor", Vector4(0.0f, 0.0f, 0.0f, alpha));
    mBaseShader->SetVectorUniform("uCameraPos", Vector2::Zero);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    
    mBaseShader->SetIntegerUniform("uIsUI", 0);
}

void Renderer::SetView(float width, float height) {
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    mBaseShader->SetActive();
    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection);
}

void Renderer::DrawHitStopOverlay(float alpha) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    mBaseShader->SetActive();
    mSpriteVerts->SetActive();

    // 1. ATIVA MODO UI (Ignora Câmera e Zoom, cobre 100% da janela)
    mBaseShader->SetIntegerUniform("uIsUI", 1);

    // 2. Cria matriz para cobrir a tela inteira (1024x768)
    Matrix4 scaleMat = Matrix4::CreateScale(Vector3(mScreenWidth, mScreenHeight, 1.0f));
    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mScreenWidth / 2.0f, mScreenHeight / 2.0f, 0.0f));
    Matrix4 world = scaleMat * transMat;

    mBaseShader->SetMatrixUniform("uWorldTransform", world);
    
    // 3. Define cor preta com transparência
    mBaseShader->SetFloatUniform("uTextureFactor", 0.0f);
    mBaseShader->SetVectorUniform("uBaseColor", Vector4(0.0f, 0.0f, 0.0f, alpha));
    mBaseShader->SetVectorUniform("uCameraPos", Vector2::Zero);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    
    // 4. DESATIVA MODO UI (CRUCIAL para o Ninja ser desenhado depois)
    mBaseShader->SetIntegerUniform("uIsUI", 0);
}