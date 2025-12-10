#include "SpriteComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Renderer/Renderer.h"

SpriteComponent::SpriteComponent(Actor* owner, int drawOrder)
    :DrawComponent(owner, drawOrder)
    ,mTexture(nullptr)
    ,mTexWidth(0)
    ,mTexHeight(0)
    ,mColor(1.0f, 1.0f, 1.0f) // Inicializa Branco
{
}

void SpriteComponent::Draw(Renderer* renderer)
{
    if (mTexture)
    {
        renderer->DrawTexture(
            mOwner->GetPosition(),
            mOwner->GetScale() * Vector2(static_cast<float>(mTexWidth), static_cast<float>(mTexHeight)),
            mOwner->GetRotation(),
            mColor, 
            mTexture,
            mTexRect,
            mOwner->GetGame()->GetCameraPos()
        );
    }
}

void SpriteComponent::SetTexture(Texture* texture)
{
    mTexture = texture;
    mTexWidth = texture->GetWidth();
    mTexHeight = texture->GetHeight();
    mTexRect = Vector4(0.0f, 0.0f, 1.0f, 1.0f); 
}

void SpriteComponent::SetTextureRect(const SDL_Rect& rect)
{
    if(mTexture) {
        float invW = 1.0f / mTexture->GetWidth();
        float invH = 1.0f / mTexture->GetHeight();
        
        mTexRect.x = rect.x * invW;
        mTexRect.y = rect.y * invH;
        mTexRect.z = rect.w * invW; 
        mTexRect.w = rect.h * invH; 
        
        mTexWidth = rect.w;
        mTexHeight = rect.h;
    }
}