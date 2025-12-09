#pragma once
#include <string>
#include "Actor.h"

class KarasuTengu : public Actor
{
public:
    explicit KarasuTengu(class Game* game);
    ~KarasuTengu() override;

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void Kill() override;
    void ApplyDamage(int amount);

    int GetHealth() const { return mHealth; }
    int GetMaxHealth() const { return mMaxHealth; }
    bool IsDying() const { return mIsDying; }
    const std::string& GetName() const { return mName; }

private:
    void EnterAttack();
    void ResetAttackState();

    int mMaxHealth;
    int mHealth;
    bool mIsDying;
    bool mIsAttacking;
    bool mAttackHitApplied;
    float mHurtTimer;
    float mAttackTimer;
    float mAttackCooldown;
    float mBaseSpeed;
    float mAttackRange;
    std::string mName;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};
