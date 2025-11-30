#pragma once
#include "Actor.h"

class SkeletonArcher : public Actor
{
public:
    explicit SkeletonArcher(class Game* game, float patrolDistance = 200.0f);

    void OnUpdate(float deltaTime) override;
    void Kill() override;
    void ApplyDamage(int amount);

private:
    void ShootArrow();
    
    int mHealth;
    float mHurtTimer;
    float mPatrolDistance;
    float mPatrolStartX;
    float mShootTimer;
    float mShootCooldown;
    bool mArrowSpawned;
    bool mIsDying;
    float mDyingTimer;
    
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};
