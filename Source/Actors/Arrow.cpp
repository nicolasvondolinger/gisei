#include "Arrow.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "Ninja.h"

Arrow::Arrow(Game* game, const Vector2& direction)
    : Actor(game)
    , mSpeed(500.0f)
    , mDirection(direction)
    , mLifeTimer(4.0f)
{
    mDrawComponent = new AnimatorComponent(this, 32, 32);
    mDrawComponent->AddAnimation("fly", "../Assets/Sprites/Skeleton_Archer/Arrow.png", 1);
    mDrawComponent->SetAnimation("fly");

    mRigidBodyComponent = new RigidBodyComponent(this, 0.0f, 0.0f, false);
    mRigidBodyComponent->SetVelocity(mDirection * mSpeed);

    mColliderComponent = new AABBColliderComponent(this, 0, 0, 24, 8, ColliderLayer::EnemyProjectile);
}

void Arrow::OnUpdate(float deltaTime)
{
    mLifeTimer -= deltaTime;
    if (mLifeTimer <= 0.0f) {
        mState = ActorState::Destroy;
        return;
    }

    if (mPosition.y > Game::WINDOW_HEIGHT + 100.0f || mPosition.y < -100.0f ||
        mPosition.x < mGame->GetCameraPos().x - 100.0f || 
        mPosition.x > mGame->GetCameraPos().x + Game::WINDOW_WIDTH + 100.0f)
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
            ninja->TakeDamage();
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
            ninja->TakeDamage();
        }
        Kill();
    } else if (other->GetLayer() == ColliderLayer::Blocks) {
        Kill();
    }
}
