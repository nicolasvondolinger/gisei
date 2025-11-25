#include "Block.h"
#include "../Game.h"
#include "../Components/Drawing/DrawComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Renderer/Texture.h"

class BlockDrawComponent : public DrawComponent {
public:
    BlockDrawComponent(Actor* owner, Texture* tex, SDL_Rect src, int w, int h)
        : DrawComponent(owner, 100), mTexture(tex), mSrcRect(src), mWidth(w), mHeight(h) {}
    
    void Draw(Renderer* renderer) override {
        if (!mIsVisible || !mTexture) return;
        
        Vector2 pos = mOwner->GetPosition();
        Vector2 size(static_cast<float>(mWidth), static_cast<float>(mHeight));
        
        float u = static_cast<float>(mSrcRect.x) / mTexture->GetWidth();
        float v = static_cast<float>(mSrcRect.y) / mTexture->GetHeight();
        float uw = static_cast<float>(mSrcRect.w) / mTexture->GetWidth();
        float vh = static_cast<float>(mSrcRect.h) / mTexture->GetHeight();
        
        Vector4 texRect(u, v, uw, vh);
        
        renderer->DrawTexture(
            pos, size, 0.0f, mColor, mTexture, texRect,
            mOwner->GetGame()->GetCameraPos(), false, 1.0f
        );
    }
    
private:
    Texture* mTexture;
    SDL_Rect mSrcRect;
    int mWidth, mHeight;
};

Block::Block(Game* game, Texture* texture, SDL_Rect srcRect, EBlockType type)
    :Actor(game), mType(type)
{
    new BlockDrawComponent(this, texture, srcRect, srcRect.w, srcRect.h);
    
    new AABBColliderComponent(
        this, 0, 0, 32, 32,
        ColliderLayer::Blocks, true
    );
}

void Block::OnUpdate(float deltaTime)
{
        // Não precisamos de update lógico se o bloco é estático
}

void Block::OnCollision(Actor* other)
{
        // Se for um bloco de perigo (Espinho) e colidir com o Player
        if (mType == EBlockType::Hazard)
        {
                // Verifique se o outro ator é o Ninja/Player
                // Aqui estou assumindo que você tem como checar o tipo ou tag
                // Exemplo genérico:
                /*
                Ninja* ninja = dynamic_cast<Ninja*>(other);
                if (ninja) {
                    ninja->TakeDamage(); // Sua função de dano
                }
                */
                SDL_Log("Colisao com Espinho! Dano deve ser aplicado.");
        }
}