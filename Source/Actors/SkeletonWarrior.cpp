#include "SkeletonWarrior.h"
#include "Ninja.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Math.h"

SkeletonWarrior::SkeletonWarrior(Game* game, float forwardSpeed)
    : Actor(game)
    , mDyingTimer(0.5f)
    , mIsDying(false)
    , mForwardSpeed(forwardSpeed)
    , mChaseDelay(1.0f)
    , mIsChasing(false)
{
    mDrawComponent = new AnimatorComponent(this, 96, 96);
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
        // Detecta precipício à frente
        Vector2 checkPos = mPosition;
        checkPos.x += (mScale.x > 0 ? 32.0f : -32.0f);
        checkPos.y += 32.0f;
        
        bool hasGroundAhead = false;
        for (auto collider : mGame->GetColliders()) {
            if (collider->GetLayer() == ColliderLayer::Blocks && collider->IsEnabled()) {
                Vector2 min = collider->GetMin();
                Vector2 max = collider->GetMax();
                if (checkPos.x >= min.x && checkPos.x <= max.x &&
                    checkPos.y >= min.y && checkPos.y <= max.y) {
                    hasGroundAhead = true;
                    break;
                }
            }
        }
        
        if (!hasGroundAhead && mIsOnGround) {
            mForwardSpeed *= -1;
            mScale.x *= -1;
            mIsChasing = false;
        }
        
        if (mChaseDelay > 0.0f) {
            mChaseDelay -= deltaTime;
        } else {
            auto player = mGame->GetPlayer();
            if (player) {
                float dx = player->GetPosition().x - mPosition.x;
                float distance = Math::Abs(dx);
                
                if (distance < 500.0f) {
                    float targetDir = (dx > 0) ? 1.0f : -1.0f;
                    
                    // Verifica precipício na direção do jogador antes de perseguir
                    Vector2 chaseCheckPos = mPosition;
                    chaseCheckPos.x += targetDir * 32.0f;
                    chaseCheckPos.y += 32.0f;
                    
                    bool canChase = false;
                    for (auto collider : mGame->GetColliders()) {
                        if (collider->GetLayer() == ColliderLayer::Blocks && collider->IsEnabled()) {
                            Vector2 min = collider->GetMin();
                            Vector2 max = collider->GetMax();
                            if (chaseCheckPos.x >= min.x && chaseCheckPos.x <= max.x &&
                                chaseCheckPos.y >= min.y && chaseCheckPos.y <= max.y) {
                                canChase = true;
                                break;
                            }
                        }
                    }
                    
                    if (canChase) {
                        mForwardSpeed = targetDir * 140.0f;
                        mScale.x = targetDir;
                        mIsChasing = true;
                        mDrawComponent->SetAnimation("attack");
                    } else {
                        mIsChasing = false;
                        mDrawComponent->SetAnimation("walk");
                    }
                } else {
                    mIsChasing = false;
                    mDrawComponent->SetAnimation("walk");
                }
            }
        }
        Vector2 force(mForwardSpeed, 0.0f);
        mRigidBodyComponent->ApplyForce(force);
    }
}

void SkeletonWarrior::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying) return;

    ColliderLayer otherLayer = other->GetLayer();

    if (otherLayer == ColliderLayer::Player) {
        auto ninja = dynamic_cast<Ninja*>(other->GetOwner());
        if (ninja && !ninja->IsDashing()) {
            other->GetOwner()->Kill();
        }
    } else if (otherLayer == ColliderLayer::Blocks || otherLayer == ColliderLayer::Enemy) {
        mForwardSpeed *= -1;
        mScale.x *= -1;
    }
}

void SkeletonWarrior::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
}
