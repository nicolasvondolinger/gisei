#pragma once
#include "Actor.h"

class KarasuTengu : public Actor
{
public:
    explicit KarasuTengu(class Game* game);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void Kill() override;
    void ApplyDamage(int amount);

private:
    void EnterAttack();
    void ResetAttackState();

    int mHealth;
    bool mIsDying;
    bool mIsAttacking;
    bool mAttackHitApplied;
    float mHurtTimer;
    float mAttackTimer;
    float mAttackCooldown;
    float mBaseSpeed;
    float mAttackRange;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};
