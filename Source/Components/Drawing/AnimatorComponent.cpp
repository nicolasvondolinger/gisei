#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Texture.h"
#include "../Physics/AABBColliderComponent.h"

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
    mAnimations.clear();
}

void AnimatorComponent::AddAnimation(const std::string& name, const std::string& texturePath, int numFrames, float fps, bool loop)
{
    AnimationData data;
    data.numFrames = numFrames;
    data.frameDuration = 1.0f / fps;
    data.loop = loop;


    data.texture = mOwner->GetGame()->GetRenderer()->GetTexture(texturePath);

    if (!data.texture) {
        SDL_Log("Erro: Nao foi possivel carregar a textura %s para a animacao %s", texturePath.c_str(), name.c_str());
        return;
    }

    mAnimations[name] = data;


    if (mCurrentAnimName.empty()) {
        SetAnimation(name);
    }
}

void AnimatorComponent::SetAnimation(const std::string& name) {
    if (mCurrentAnimName == name) return;

    if (mAnimations.find(name) != mAnimations.end()) {
        mCurrentAnimName = name;
        mAnimTimer = 0.0f;
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

    
    mAnimTimer += deltaTime;

    
    float totalDuration = anim.frameDuration * anim.numFrames;

    if (anim.loop) {
        while (mAnimTimer >= totalDuration) {
            mAnimTimer -= totalDuration;
        }
    } else {
        if (mAnimTimer >= totalDuration) {
            mAnimTimer = totalDuration - 0.001f;
        }
    }
}

void AnimatorComponent::Draw(Renderer* renderer) {
    if (!mIsVisible || mCurrentAnimName.empty()) return;

    
    const AnimationData& anim = mAnimations[mCurrentAnimName];

    if (!anim.texture) return;

    
    auto collider = mOwner->GetComponent<AABBColliderComponent>();
    Vector2 pos = mOwner->GetPosition();
    
    if (collider) {
        float hitboxHeight = collider->GetMax().y - collider->GetMin().y;
        float spriteHeight = static_cast<float>(mHeight);
        pos.y += (hitboxHeight - spriteHeight) / 2.0f;
    }
    
    float rot = mOwner->GetRotation();
    Vector2 size(static_cast<float>(mWidth), static_cast<float>(mHeight));
    Vector3 color(1.0f, 1.0f, 1.0f);
    Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();
    bool flip = (mOwner->GetScale().x < 0.0f);


    int currentFrameIndex = static_cast<int>(mAnimTimer / anim.frameDuration);

    
    if(currentFrameIndex >= anim.numFrames) currentFrameIndex = anim.numFrames - 1;

    
    float texW = static_cast<float>(anim.texture->GetWidth());
    float texH = static_cast<float>(anim.texture->GetHeight());

    
    
    float frameW = texW / static_cast<float>(anim.numFrames);
    float frameH = texH;

    
    
    float u = (currentFrameIndex * frameW) / texW;
    float v = 0.0f; 
    float uw = frameW / texW;
    float vh = 1.0f;

    Vector4 texRect(u, v, uw, vh);

    renderer->DrawTexture(
        pos,
        size,
        rot,
        color,
        anim.texture, 
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

int AnimatorComponent::GetCurrentFrame() const {
    if (mCurrentAnimName.empty()) return 0;
    const AnimationData& anim = mAnimations.at(mCurrentAnimName);
    int frame = static_cast<int>(mAnimTimer / anim.frameDuration);
    if (frame >= anim.numFrames) frame = anim.numFrames - 1;
    return frame;
}