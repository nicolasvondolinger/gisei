#include "SkeletonSpearman.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

SkeletonSpearman::SkeletonSpearman(Game* game, float forwardSpeed)
    : Actor(game)
    , mDyingTimer(0.5f)
    , mIsDying(false)
    , mForwardSpeed(forwardSpeed)
{
    mDrawComponent = new AnimatorComponent(this, 96, 96);
    mDrawComponent->AddAnimation("walk", "../Assets/Sprites/Skeleton/Spearman/Walk.png", 7, 6.0f, true);
    mDrawComponent->AddAnimation("attack", "../Assets/Sprites/Skeleton/Spearman/Attack_1.png", 4, 10.0f, true);
    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Skeleton/Spearman/Dead.png", 5, 8.0f, false);
    mDrawComponent->SetAnimation("walk");

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 3.0f, true);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 24, 56, ColliderLayer::Enemy);
}

void SkeletonSpearman::Kill()
{
    if (mIsDying) return;

    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void SkeletonSpearman::OnUpdate(float deltaTime)
{
    if (mIsDying) {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f) {
            mState = ActorState::Destroy;
        }
        return;
    }

    if (mPosition.y > Game::WINDOW_HEIGHT + 100.0f) {
        mState = ActorState::Destroy;
        return;
    }

    if (mRigidBodyComponent->IsEnabled()) {
        Vector2 force(mForwardSpeed, 0.0f);
        mRigidBodyComponent->ApplyForce(force);
    }
}

void SkeletonSpearman::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying) return;

    ColliderLayer otherLayer = other->GetLayer();

    if (otherLayer == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    } else if (otherLayer == ColliderLayer::Blocks || otherLayer == ColliderLayer::Enemy) {
        mForwardSpeed *= -1;
        mScale.x *= -1;
    }
}

void SkeletonSpearman::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
}
