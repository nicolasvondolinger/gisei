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

enum class ActionState {
    None,           // Parado/Andando normal
    CrouchGoingDown,// Baixando (agacha.png)
    CrouchHolding,  // Segurando embaixo (Congelado no final de agacha.png)
    CrouchGoingUp,  // Subindo (sobe.png)
    ShieldStart,    // Entrando na defesa (inicio_defesa.png)
    ShieldHolding,  // Segurando defesa (Congelado no final de inicio_defesa.png)
    ShieldEnd       // Saindo da defesa (fim_defesa.png)
};

class Ninja : public Actor
{
public:
    explicit Ninja(class Game *game, float accelerationForce = 2500.0f, float jumpSpeed = -750.0f);

    void OnProcessInput(const Uint8* keyState) override;
    void OnUpdate(float deltaTime) override;


    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

    void Kill() override;
    int GetHealth() const { return mHealth; }
    int GetMaxHealth() const { return mMaxHealth; }
    float GetDashCooldown() const { return mDashCooldown; }
    float GetDashCooldownTime() const { return mDashCooldownTime; }

    class AnimatorComponent* GetDrawComponent() { return mDrawComponent; }

    void TakeDamage(bool fromThorns = false);
    void StageClear();
    
    bool IsDashing() const { return mIsDashing; }
    const std::vector<DashParticle>& GetDashParticles() const { return mDashParticles; }

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
    bool mIsShooting;

    ActionState mActionState;

    class Actor* mShieldEffectActor;
    class AnimatorComponent* mShieldEffectAnim;
    
    float mAttackTimer;
    float mDashTimer;
    float mShootTimer;
    bool mDartSpawned;
    float mDashSpeed;
    Vector2 mDashDirection;
    Vector2 mDashStartPos;
    Vector2 mDashEndPos;

    int mHealth;
    int mMaxHealth;
    
    bool mIsInvincible;
    float mInvincibleTimer;
    const float mInvincibleDuration = 1.5f;
    
    float mDashCooldown;
    float mShootCooldown;
    const float mDashCooldownTime = 1.0f;
    const float mShootCooldownTime = 0.5f;
    const float mDashDuration = 0.3f;
    
    std::vector<DashParticle> mDashParticles;

    
    class RigidBodyComponent* mRigidBodyComponent;
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;

};