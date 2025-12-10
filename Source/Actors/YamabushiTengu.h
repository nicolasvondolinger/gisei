#pragma once
#include <string>
#include "Actor.h"
#include "BossEnemy.h"

class YamabushiTengu : public Actor, public BossEnemy
{
public:
    explicit YamabushiTengu(class Game* game);
    ~YamabushiTengu() override;

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void Kill() override;
    void ApplyDamage(int amount);

    int GetHealth() const override { return mHealth; }
    int GetMaxHealth() const override { return mMaxHealth; }
    bool IsDying() const override { return mIsDying; }
    const std::string& GetName() const override { return mName; }

private:
    void EnterAttack();
    void EnterLeap();
    void ResetAttackState();

    int mMaxHealth;
    int mHealth;
    bool mIsDying;
    bool mIsAttacking;
    bool mIsLeaping;
    bool mAttackHitApplied;
    float mHurtTimer;
    float mAttackTimer;
    float mAttackDuration;
    float mAttackWindup;
    float mAttackCooldown;
    float mLeapCooldown;
    float mBaseSpeed;
    float mAttackRange;
    std::string mName;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};
