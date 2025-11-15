//
// Created by Lucas N. Ferreira on 08/09/23.
//

#pragma once
#include "Actor.h"

class Mario : public Actor
{
public:
    explicit Mario(Game* game, float forwardSpeed = 1500.0f, float jumpSpeed = -750.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;

    void OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) override;

    void Kill() override;
    void Grow();
    void Shrink();
    void StageClear();

private:
    void ManageAnimations();

    float mForwardSpeed;
    float mJumpSpeed;
    bool mIsRunning;
    bool mIsDead;
    bool mIsBig;
    bool mIsInvincible;
    float mInvincibleTimer;
    const float mInvincibleDuration = 1.5f;

    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
    class ParticleSystemComponent* mFireBalls;

    float mFireBallCooldown = 1.0f;
};