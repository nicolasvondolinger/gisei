#include "SkeletonArcher.h"
#include "Arrow.h"
#include "../Game.h"
#include "../Random.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

SkeletonArcher::SkeletonArcher(Game* game, float patrolDistance)
    : Actor(game)
    , mPatrolDistance(patrolDistance)
    , mShootTimer(0.0f)
    , mShootCooldown(3.0f)
    , mArrowSpawned(false)
    , mIsDying(false)
    , mDyingTimer(0.5f)
{
    mDrawComponent = new AnimatorComponent(this, 96, 96);
    mDrawComponent->AddAnimation("idle", "../Assets/Sprites/Skeleton/Archer/Idle.png", 7, 6.0f, true);
    mDrawComponent->AddAnimation("walk", "../Assets/Sprites/Skeleton/Archer/Walk.png", 8, 6.0f, true);
    mDrawComponent->AddAnimation("shot1", "../Assets/Sprites/Skeleton/Archer/Shot_1.png", 15, 12.0f, false);
    mDrawComponent->AddAnimation("shot2", "../Assets/Sprites/Skeleton/Archer/Shot_2.png", 15, 12.0f, false);
    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Skeleton/Archer/Dead.png", 5, 8.0f, false);
    mDrawComponent->SetAnimation("idle");

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 3.0f, true);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 24, 56, ColliderLayer::Enemy);
    
    mPatrolStartX = mPosition.x;
}

void SkeletonArcher::OnUpdate(float deltaTime)
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

    mShootCooldown -= deltaTime;
    
    if (mShootCooldown <= 0.0f && mDrawComponent->GetCurrentAnimation() == "idle") {
        mShootCooldown = 3.0f;
        mShootTimer = 1.1f;
        mArrowSpawned = false;
        mDrawComponent->SetAnimation(Random::GetIntRange(0, 1) == 0 ? "shot1" : "shot2");
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (mShootTimer > 0.0f) {
        mShootTimer -= deltaTime;
        
        int currentFrame = mDrawComponent->GetCurrentFrame();
        if (!mArrowSpawned && currentFrame >= 11) {
            mArrowSpawned = true;
            ShootArrow();
        }
        
        if (mShootTimer <= 0.0f) {
            mDrawComponent->SetAnimation("idle");
        }
    }
}

void SkeletonArcher::ShootArrow()
{
    Vector2 direction(mScale.x, 0.0f);
    Vector2 spawnOffset(mScale.x * 40.0f, 0.0f);
    Arrow* arrow = new Arrow(mGame, direction);
    arrow->SetPosition(mPosition + spawnOffset);
}

void SkeletonArcher::Kill()
{
    if (mIsDying) return;
    
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}
