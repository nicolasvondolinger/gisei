#pragma once
#include "Actor.h"
#include <vector>

struct DashParticle {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    float maxLifetime;
    float size;
};

class Samurai : public Actor
{
public:
    explicit Samurai(class Game *game, float accelerationForce = 2500.0f, float jumpSpeed = -650.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;


    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

    void Kill() override;
    void StageClear();
    
    const std::vector<DashParticle>& GetDashParticles() const;

private:
    void ManageAnimations();
    void CreateDashParticles(const Vector2& startPos, const Vector2& endPos);


    float mAccelerationForce;
    float mMaxSpeed;
    float mJumpSpeed;

    bool mIsRunning;
    bool mIsDead;
    bool mIsAttacking;
    bool mIsDashing;
    bool mIsDefending;
    
    float mAttackTimer;
    float mDashTimer;
    float mDashSpeed;
    Vector2 mDashDirection;

    
    bool mIsInvincible;
    float mInvincibleTimer;
    const float mInvincibleDuration = 1.5f;
    
    std::vector<DashParticle> mDashParticles;

    
    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};