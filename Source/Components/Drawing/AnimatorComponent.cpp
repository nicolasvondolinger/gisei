//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Texture.h" // Necessário para pegar Width/Height da textura

AnimatorComponent::AnimatorComponent(class Actor* owner, int width, int height, int drawOrder)
        :DrawComponent(owner, drawOrder)
        ,mAnimTimer(0.0f)
        ,mIsPaused(false)
        ,mWidth(width)
        ,mHeight(height)
        ,mTextureFactor(1.0f)
{
}

AnimatorComponent::~AnimatorComponent()
{
    // As texturas são gerenciadas pelo Renderer (Cache), então não deletamos elas aqui
    mAnimations.clear();
}

void AnimatorComponent::AddAnimation(const std::string& name, const std::string& texturePath, int numFrames, float fps, bool loop)
{
    AnimationData data;
    data.numFrames = numFrames;
    data.frameDuration = 1.0f / fps;
    data.loop = loop;

    // Carrega a textura através do Renderer do jogo
    data.texture = mOwner->GetGame()->GetRenderer()->GetTexture(texturePath);

    if (!data.texture) {
        SDL_Log("Erro: Nao foi possivel carregar a textura %s para a animacao %s", texturePath.c_str(), name.c_str());
        return;
    }

    mAnimations[name] = data;

    // Se for a primeira animação adicionada, define como a padrão
    if (mCurrentAnimName.empty()) {
        SetAnimation(name);
    }
}

void AnimatorComponent::SetAnimation(const std::string& name) {
    if (mCurrentAnimName == name) return;

    if (mAnimations.find(name) != mAnimations.end()) {
        mCurrentAnimName = name;
        mAnimTimer = 0.0f; // Reseta o timer ao trocar de animação
    } else {
        SDL_Log("Aviso: Tentou setar animacao inexistente %s", name.c_str());
    }
}

void AnimatorComponent::SetAnimFPS(float fps) {
    if (!mCurrentAnimName.empty()) {
        mAnimations[mCurrentAnimName].frameDuration = 1.0f / fps;
    }
}

void AnimatorComponent::Update(float deltaTime) {
    if (mIsPaused || mCurrentAnimName.empty()) return;

    AnimationData& anim = mAnimations[mCurrentAnimName];

    // Incrementa o tempo
    mAnimTimer += deltaTime;

    // Verifica loop ou fim da animação
    float totalDuration = anim.frameDuration * anim.numFrames;

    if (anim.loop) {
        while (mAnimTimer >= totalDuration) {
            mAnimTimer -= totalDuration;
        }
    } else {
        if (mAnimTimer >= totalDuration) {
            mAnimTimer = totalDuration - 0.001f; // Trava no último frame
        }
    }
}

void AnimatorComponent::Draw(Renderer* renderer) {
    if (!mIsVisible || mCurrentAnimName.empty()) return;

    // Pega os dados da animação atual
    const AnimationData& anim = mAnimations[mCurrentAnimName];

    if (!anim.texture) return;

    // Cálculos para desenhar
    Vector2 pos = mOwner->GetPosition();
    float rot = mOwner->GetRotation();
    Vector2 size(static_cast<float>(mWidth), static_cast<float>(mHeight));
    Vector3 color(1.0f, 1.0f, 1.0f);
    Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();
    bool flip = (mOwner->GetScale().x < 0.0f);

    // --- CÁLCULO DO FRAME (UV) ---

    // Descobre qual índice do frame estamos baseados no tempo
    int currentFrameIndex = static_cast<int>(mAnimTimer / anim.frameDuration);

    // Proteção de índice
    if(currentFrameIndex >= anim.numFrames) currentFrameIndex = anim.numFrames - 1;

    // Dimensões da textura total
    float texW = static_cast<float>(anim.texture->GetWidth());
    float texH = static_cast<float>(anim.texture->GetHeight());

    // Assumindo que a sprite sheet é uma linha horizontal (strip)
    // Largura de 1 frame = LarguraTotal / NumeroDeFrames
    float frameW = texW / static_cast<float>(anim.numFrames);
    float frameH = texH;

    // Calcula coordenadas UV (0.0 a 1.0)
    // u = x / width, v = y / height
    float u = (currentFrameIndex * frameW) / texW;
    float v = 0.0f; // Sempre 0 se for uma única linha
    float uw = frameW / texW; // Largura proporcional de um frame (ex: 1/8)
    float vh = 1.0f;          // Altura total

    Vector4 texRect(u, v, uw, vh);

    renderer->DrawTexture(
        pos,
        size,
        rot,
        color,
        anim.texture, // Usa a textura específica da animação atual
        texRect,
        cameraPos,
        flip,
        mTextureFactor
    );
}

void AnimatorComponent::SetSize(int w, int h){
    mWidth = w;
    mHeight = h;
}