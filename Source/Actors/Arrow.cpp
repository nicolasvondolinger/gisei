#include "Arrow.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Drawing/DrawComponent.h"
#include "../Renderer/Renderer.h"
#include "Ninja.h"
#include <SDL2/SDL.h>

// --- COMPONENTE DE AURA ---
class AuraComponent : public DrawComponent {
public:
    AuraComponent(Actor* owner, int order) : DrawComponent(owner, order) {}
    
    void Draw(Renderer* renderer) override {
        Vector2 camPos = mOwner->GetGame()->GetCameraPos();
        
        // Offset Y negativo para subir o círculo
        Vector2 pos = mOwner->GetPosition() + Vector2(0.0f, -10.0f);
        
        // Desenha o círculo (Raio 20px)
        renderer->DrawSolidCircle(pos, 20.0f, mColor, camPos);
    }

    void SetColor(const Vector3& color) { mColor = color; }

private:
    Vector3 mColor{1.0f, 1.0f, 1.0f};
};
// --------------------------

Arrow::Arrow(Game* game, const Vector2& direction)
    : Actor(game)
    , mSpeed(500.0f)
    , mDirection(direction)
    , mLifeTimer(4.0f)
    , mAuraSprite(nullptr)
{
    // 1. AURA
    mAura = new AuraComponent(this, 90);
    mAura->SetColor(Vector3(1.0f, 1.0f, 1.0f)); 

    // 2. FLECHA
    mDrawComponent = new AnimatorComponent(this, 32, 32);
    mDrawComponent->AddAnimation("fly", "../Assets/Sprites/Skeleton_Archer/Arrow.png", 1);
    mDrawComponent->SetAnimation("fly");

    mRigidBodyComponent = new RigidBodyComponent(this, 0.0f, 0.0f, false);
    mRigidBodyComponent->SetVelocity(mDirection * mSpeed);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, 24, 8, ColliderLayer::EnemyProjectile);
    
    if (direction.x != 0 || direction.y != 0) {
        SetRotation(Math::Atan2(-direction.y, direction.x));
    }
}

void Arrow::OnUpdate(float deltaTime)
{
    mLifeTimer -= deltaTime;
    
    float pulse = 0.85f + 0.15f * Math::Sin(SDL_GetTicks() / 100.0f);
    
    if (mAura) {
        mAura->SetColor(Vector3(pulse, pulse, pulse));
    }

    if (mLifeTimer <= 0.0f) {
        mState = ActorState::Destroy;
        return;
    }

    float camX = mGame->GetCameraPos().x;
    if (mPosition.y > Game::WINDOW_HEIGHT + 100.0f || mPosition.y < -100.0f ||
        mPosition.x < camX - 100.0f || 
        mPosition.x > camX + Game::WINDOW_WIDTH + 100.0f)
    {
        mState = ActorState::Destroy;
    }
}

void Arrow::Kill()
{
    mState = ActorState::Destroy;
}

void Arrow::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        if (auto ninja = dynamic_cast<Ninja*>(other->GetOwner())) {
            // CORREÇÃO: Passar mPosition como fonte do dano
            ninja->TakeDamage(mPosition);
        }
        Kill();
    } else if (other->GetLayer() == ColliderLayer::Blocks) {
        Kill();
    }
}

void Arrow::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        if (auto ninja = dynamic_cast<Ninja*>(other->GetOwner())) {
            // CORREÇÃO: Passar mPosition como fonte do dano
            ninja->TakeDamage(mPosition);
        }
        Kill();
    } else if (other->GetLayer() == ColliderLayer::Blocks) {
        Kill();
    }
}