#pragma once
#include "Actor.h"

class Samurai : public Actor
{
public:
    explicit Samurai(class Game* game, float forwardSpeed = 400.0f, float jumpSpeed = -950.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void Kill() override;

    void StageClear();

private:
    void ManageAnimations();

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDead;

    // Invencibilidade após tomar dano (opcional, já que ele morre de primeira atualmente)
    bool mIsInvincible;
    float mInvincibleTimer;
    const float mInvincibleDuration = 1.5f;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};