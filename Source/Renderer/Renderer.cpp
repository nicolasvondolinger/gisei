#include <GL/glew.h>
#include "Renderer.h"
#include "Shader.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Font.h"
#include "../UI/UIElement.h" // A única dependência de "desenho" que o Renderer gerencia

// Construtor
Renderer::Renderer(SDL_Window *window)
: mBaseShader(nullptr) // Corrigido de mSpriteShader para mBaseShader
, mContext(nullptr)
, mWindow(window)
, mSpriteVerts(nullptr)
, mOrthoProjection(Matrix4::Identity)
, mScreenWidth(1024.0f)
, mScreenHeight(768.0f)
{
    // mUIElements e mTextures/mFonts são inicializados por padrão
}

// Destrutor
Renderer::~Renderer()
{
    delete mSpriteVerts;
    mSpriteVerts = nullptr;
}

// Initialize
bool Renderer::Initialize(float width, float height)
{
    mScreenWidth = width;
    mScreenHeight = height;

    // Configura atributos OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetSwapInterval(1); // V-Sync

    mContext = SDL_GL_CreateContext(mWindow);

    // Inicializa GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        SDL_Log("Falha ao inicializar GLEW.");
        return false;
    }

    // Carrega o shader base
    if (!LoadShaders())
    {
        SDL_Log("Falha ao carregar shaders.");
        return false;
    }

    // Cria vértices do quad
    CreateSpriteVerts();

    // Define a cor de limpeza
    glClearColor(0.419f, 0.549f, 1.0f, 1.0f); // Azul claro

    // Habilita alpha blending (Essencial para 2D)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Desabilita teste de profundidade (Bom para 2D)
    glDisable(GL_DEPTH_TEST);

    return true;
}

// --- Gerenciamento de UI ---

void Renderer::AddUIElement(UIElement *comp)
{
    mUIElements.emplace_back(comp);
    std::sort(mUIElements.begin(), mUIElements.end(),[](UIElement* a, UIElement* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Renderer::RemoveUIElement(UIElement *comp)
{
    auto iter = std::find(mUIElements.begin(), mUIElements.end(), comp);
    if(iter != mUIElements.end())
    {
        mUIElements.erase(iter);
    }
}

// --- Funções de Limpeza ---

void Renderer::UnloadData()
{
    // Destrói texturas
    for (auto i : mTextures)
    {
        i.second->Unload();
        delete i.second;
    }
    mTextures.clear();

    // Destrói fontes
    for (auto i : mFonts)
    {
        i.second->Unload();
        delete i.second;
    }
    mFonts.clear();
}

void Renderer::Shutdown()
{
    UnloadData();

    delete mSpriteVerts;
    mSpriteVerts = nullptr;

    mBaseShader->Unload(); // Corrigido
    delete mBaseShader;    // Corrigido

    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
}

// --- Loop de Desenho ---

void Renderer::Clear()
{
    // Limpa apenas o buffer de cor
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw()
{
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);

    mBaseShader->SetActive();
    mSpriteVerts->SetActive();

    // *** ADICIONE ISTO: ***
    // Diz ao shader para IGNORAR a câmera (1 = true)
    mBaseShader->SetIntegerUniform("uIsUI", 1);

    // Desenha Elementos de UI
    for (auto ui : mUIElements)
    {
        ui->Draw(mBaseShader);
    }
}
void Renderer::Present()
{
    // Troca os buffers
    SDL_GL_SwapWindow(mWindow);
}

// --- Funções de Desenho Direto ---
// Usadas pelo Game.cpp (background) e DrawComponent (atores)

void Renderer::Draw(RendererMode mode, const Matrix4 &modelMatrix, const Vector2 &cameraPos, VertexArray *vertices,
                    const Vector3 &color, Texture *texture, const Vector4 &textureRect, float textureFactor)
{
    mBaseShader->SetMatrixUniform("uWorldTransform", modelMatrix);
    // *** CORREÇÃO: Envia um vec4 para o uBaseColor ***
    mBaseShader->SetVectorUniform("uBaseColor", Vector4(color, 1.0f));
    mBaseShader->SetVectorUniform("uTexRect", textureRect);        // Corrigido
    mBaseShader->SetVectorUniform("uCameraPos", cameraPos);        // Corrigido

    if(vertices)
    {
        vertices->SetActive();
    }
    else
    {
        mSpriteVerts->SetActive();
    }

    if(texture)
    {
        texture->SetActive();
        mBaseShader->SetFloatUniform("uTextureFactor", textureFactor); // Corrigido
    }
    else {
        mBaseShader->SetFloatUniform("uTextureFactor", 0.0f); // Corrigido
    }

    if (mode == RendererMode::LINES)
    {
        glDrawElements(GL_LINE_LOOP, vertices ? vertices->GetNumIndices() : 6, GL_UNSIGNED_INT, nullptr);
    }
    else // TRIANGLES
    {
        glDrawElements(GL_TRIANGLES, vertices ? vertices->GetNumIndices() : 6, GL_UNSIGNED_INT, nullptr);
    }
}

void Renderer::DrawRect(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                        const Vector2 &cameraPos, RendererMode mode)
{
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, nullptr, color);
}

void Renderer::DrawTexture(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                           Texture *texture, const Vector4 &textureRect, const Vector2 &cameraPos, bool flip,
                           float textureFactor)
{
    float flipFactor = flip ? -1.0f : 1.0f;

    Matrix4 model = Matrix4::CreateScale(Vector3(size.x * flipFactor, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(RendererMode::TRIANGLES, model, cameraPos, nullptr, color, texture, textureRect, textureFactor);
}

void Renderer::DrawGeometry(const Vector2 &position, const Vector2 &size, float rotation, const Vector3 &color,
                            const Vector2 &cameraPos, VertexArray *vertexArray, RendererMode mode)
{
    Matrix4 model = Matrix4::CreateScale(Vector3(size.x, size.y, 1.0f)) *
                    Matrix4::CreateRotationZ(rotation) *
                    Matrix4::CreateTranslation(Vector3(position.x, position.y, 0.0f));

    Draw(mode, model, cameraPos, vertexArray, color);
}

// --- Carregadores ---

bool Renderer::LoadShaders()
{
    mBaseShader = new Shader(); // Corrigido
    if (!mBaseShader->Load("../Shaders/Base")) // Corrigido
    {
        return false;
    }

    mBaseShader->SetActive(); // Corrigido

    // Define a matriz de projeção ortográfica
    mOrthoProjection = Matrix4::CreateOrtho(0.0f, mScreenWidth, mScreenHeight, 0.0f, -1.0f, 1.0f);
    mBaseShader->SetMatrixUniform("uOrthoProj", mOrthoProjection); // Corrigido
    mBaseShader->SetIntegerUniform("uTexture", 0);                 // Corrigido

    return true;
}

void Renderer::CreateSpriteVerts()
{
    float vertices[] = {
        // Posição (x, y, z)      // UV (u, v)
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, // 0: Inf-Esq
        -0.5f,  0.5f, 0.0f,     0.0f, 1.0f, // 1: Sup-Esq
         0.5f,  0.5f, 0.0f,     1.0f, 1.0f, // 2: Sup-Dir
         0.5f, -0.5f, 0.0f,     1.0f, 0.0f  // 3: Inf-Dir
    };

    unsigned int indices[] = {
        0, 1, 2,  // Triângulo 1
        2, 3, 0   // Triângulo 2
    };

    mSpriteVerts = new VertexArray(vertices, 4, indices, 6);
}

Texture* Renderer::GetTexture(const std::string& fileName, bool repeat)
{
    Texture* tex = nullptr;
    auto iter = mTextures.find(fileName);
    if (iter != mTextures.end()) {
        tex = iter->second;
    } else {
        tex = new Texture();
        if (tex->Load(fileName, repeat)) {
            mTextures.emplace(fileName, tex);
        } else {
            delete tex;
            tex = nullptr;
        }
    }
    return tex;
}

Font* Renderer::GetFont(const std::string& fileName)
{
    auto iter = mFonts.find(fileName);
    if (iter != mFonts.end())
    {
        return iter->second;
    }
    else
    {
        Font* font = new Font();
        if (font->Load(fileName))
        {
            mFonts.emplace(fileName, font);
        }
        else
        {
            font->Unload();
            delete font;
            font = nullptr;
        }
        return font;
    }
}