#include "SkeletonWarrior.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

SkeletonWarrior::SkeletonWarrior(Game* game, float forwardSpeed)
    : Actor(game)
    , mDyingTimer(0.5f)
    , mIsDying(false)
    , mForwardSpeed(forwardSpeed)
{
    mDrawComponent = new AnimatorComponent(this, 128, 128);
    mDrawComponent->AddAnimation("walk", "../Assets/Sprites/Skeleton_Warrior/Walk.png", 7, 6.0f, true);
    mDrawComponent->AddAnimation("attack", "../Assets/Sprites/Skeleton_Warrior/Attack_1.png", 5, 10.0f, true);
    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Skeleton_Warrior/Dead.png", 4, 8.0f, false);
    mDrawComponent->SetAnimation("walk");

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 3.0f, true);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 24, 56, ColliderLayer::Enemy);
}

void SkeletonWarrior::Kill()
{
    if (mIsDying) return;

    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void SkeletonWarrior::OnUpdate(float deltaTime)
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

        if (!HasGroundAhead()) {
            TurnAround();
        }
    }
}

void SkeletonWarrior::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
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

void SkeletonWarrior::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
}

bool SkeletonWarrior::HasGroundAhead() const
{
    if (!mColliderComponent) return true;

    Vector2 min = mColliderComponent->GetMin();
    Vector2 max = mColliderComponent->GetMax();
    float halfWidth = (max.x - min.x) * 0.5f;
    float dir = (mForwardSpeed >= 0.0f) ? 1.0f : -1.0f;

    Vector2 checkPos;
    checkPos.x = mPosition.x + dir * (halfWidth + 6.0f);
    checkPos.y = max.y + 4.0f;

    for (auto collider : mGame->GetColliders()) {
        if (!collider->IsEnabled() || collider->GetLayer() != ColliderLayer::Blocks) continue;
        Vector2 cMin = collider->GetMin();
        Vector2 cMax = collider->GetMax();
        bool insideX = checkPos.x >= cMin.x && checkPos.x <= cMax.x;
        bool insideY = checkPos.y >= cMin.y && checkPos.y <= cMax.y;
        if (insideX && insideY) {
            return true;
        }
    }

    return false;
}

void SkeletonWarrior::TurnAround()
{
    mForwardSpeed *= -1.0f;
    mScale.x *= -1.0f;
}
