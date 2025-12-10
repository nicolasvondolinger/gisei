#pragma once
#include "Actor.h"

class SkeletonWarrior : public Actor
{
public:
    explicit SkeletonWarrior(class Game* game, float forwardSpeed = 100.0f);

    void OnUpdate(float deltaTime) override;
    void Kill() override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void ApplyDamage(int amount);

private:
    bool HasGroundAhead() const;
    void TurnAround();

    float mDyingTimer;
    bool mIsDying;
    float mHurtTimer;
    bool mIsAggro;
    float mBaseSpeed;
    int mHealth;
    float mForwardSpeed;
    
    bool mAttackHitApplied; 

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};