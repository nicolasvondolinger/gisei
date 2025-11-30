#include "Dart.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Dart::Dart(Game* game, const Vector2& direction)
    : Actor(game)
    , mSpeed(600.0f)
    , mDirection(direction)
{
    mDrawComponent = new AnimatorComponent(this, 12, 12);
    mDrawComponent->AddAnimation("fly", "../Assets/Sprites/Ninja/Dart.png", 1);
    mDrawComponent->SetAnimation("fly");

    mRigidBodyComponent = new RigidBodyComponent(this, 0.0f, 0.0f, false);
    mRigidBodyComponent->SetVelocity(mDirection * mSpeed);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, 12, 12, ColliderLayer::Player);
}

void Dart::OnUpdate(float deltaTime)
{
    if (mPosition.y > Game::WINDOW_HEIGHT + 100.0f || mPosition.y < -100.0f ||
        mPosition.x < mGame->GetCameraPos().x - 100.0f || 
        mPosition.x > mGame->GetCameraPos().x + Game::WINDOW_WIDTH + 100.0f)
    {
        mState = ActorState::Destroy;
    }
}

void Dart::Kill()
{
    mState = ActorState::Destroy;
}

void Dart::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        other->GetOwner()->Kill();
        Kill();
    }
    else if (other->GetLayer() == ColliderLayer::Blocks)
    {
        // Só colide com blocos sólidos (que têm collider ativado)
        if (other->IsEnabled())
        {
            Kill();
        }
    }
}

void Dart::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Enemy)
    {
        other->GetOwner()->Kill();
        Kill();
    }
    else if (other->GetLayer() == ColliderLayer::Blocks)
    {
        // Só colide com blocos sólidos (que têm collider ativado)
        if (other->IsEnabled())
        {
            Kill();
        }
    }
}
