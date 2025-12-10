#include "KarasuTengu.h"
#include "Ninja.h"
#include "Key.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

KarasuTengu::KarasuTengu(Game* game)
    : Actor(game)
    , mMaxHealth(10)
    , mHealth(mMaxHealth)
    , mIsDying(false)
    , mIsAttacking(false)
    , mAttackHitApplied(false)
    , mHurtTimer(0.0f)
    , mAttackTimer(0.0f)
    , mAttackCooldown(1.2f)
    , mBaseSpeed(140.0f)
    , mAttackRange(110.0f)
    , mName("Karasu Tengu")
{
    mDrawComponent = new AnimatorComponent(this, 128, 128);
    mDrawComponent->AddAnimation("idle", "../Assets/Sprites/Karasu_tengu/Idle.png", 6, 6.0f, true);
    mDrawComponent->AddAnimation("walk", "../Assets/Sprites/Karasu_tengu/Walk.png", 8, 7.0f, true);
    mDrawComponent->AddAnimation("run", "../Assets/Sprites/Karasu_tengu/Run.png", 8, 8.0f, true);
    mDrawComponent->AddAnimation("attack1", "../Assets/Sprites/Karasu_tengu/Attack_1.png", 6, 12.0f, false);
    mDrawComponent->AddAnimation("attack2", "../Assets/Sprites/Karasu_tengu/Attack_2.png", 4, 12.0f, false);
    mDrawComponent->AddAnimation("attack3", "../Assets/Sprites/Karasu_tengu/Attack_3.png", 3, 10.0f, false);
    mDrawComponent->AddAnimation("hurt", "../Assets/Sprites/Karasu_tengu/Hurt.png", 3, 12.0f, false);
    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Karasu_tengu/Dead.png", 6, 8.0f, false);
    mDrawComponent->SetAnimation("idle");

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 3.0f, true);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 40, 64, ColliderLayer::Enemy);

    mGame->RegisterBoss(this);
}

KarasuTengu::~KarasuTengu()
{
    if (mGame) {
        mGame->ClearBoss(this);
    }
}

void KarasuTengu::OnUpdate(float deltaTime)
{
    if (mIsDying) {
        if (mDrawComponent->GetCurrentAnimation() != "dead") {
            mDrawComponent->SetAnimation("dead");
        }
        return;
    }

    if (mHurtTimer > 0.0f) {
        mHurtTimer -= deltaTime;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        if (mHurtTimer <= 0.0f && !mIsAttacking) {
            mDrawComponent->SetAnimation("idle");
        }
        return;
    }

    if (mAttackCooldown > 0.0f) mAttackCooldown -= deltaTime;

    if (mIsAttacking) {
        mAttackTimer -= deltaTime;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);

        // Aplica o dano UMA VEZ durante o ataque
        if (!mAttackHitApplied) {
            if (auto ninja = mGame->GetPlayer()) {
                Vector2 pos = mPosition;
                Vector2 target = ninja->GetPosition();
                // Verifica hitbox simples do ataque
                if (Math::Abs(pos.x - target.x) <= mAttackRange &&
                    Math::Abs(pos.y - target.y) <= 80.0f) {
                    
                    mAttackHitApplied = true;
                    
                    // --- CORREÇÃO AQUI ---
                    // Passamos a posição do Tengu (mPosition) como origem do dano
                    // para calcular o knockback corretamente.
                    ninja->TakeDamage(mPosition); 
                }
            }
        }

        if (mAttackTimer <= 0.0f) {
            ResetAttackState();
        }
        return;
    }

    // Comportamento padrão: perseguir jogador
    Vector2 velocity = mRigidBodyComponent->GetVelocity();
    velocity.x = 0.0f;

    if (auto ninja = mGame->GetPlayer()) {
        Vector2 target = ninja->GetPosition();
        float dx = target.x - mPosition.x;
        float dy = target.y - mPosition.y;
        float dir = (dx >= 0.0f) ? 1.0f : -1.0f;
        mScale.x = dir;

        if (Math::Abs(dx) > mAttackRange || Math::Abs(dy) > 100.0f) {
            velocity.x = dir * mBaseSpeed;
            mDrawComponent->SetAnimation("run");
        } else if (mAttackCooldown <= 0.0f) {
            EnterAttack();
            velocity.x = 0.0f;
        } else {
            mDrawComponent->SetAnimation("idle");
        }
    } else {
        mDrawComponent->SetAnimation("idle");
    }

    mRigidBodyComponent->SetVelocity(velocity);
}

void KarasuTengu::EnterAttack()
{
    mIsAttacking = true;
    mAttackHitApplied = false;
    mAttackTimer = 0.6f;
    mAttackCooldown = 1.5f;

    static int attackIndex = 0;
    const char* attackNames[] = { "attack1", "attack2", "attack3" };
    mDrawComponent->SetAnimation(attackNames[attackIndex % 3]);
    attackIndex++;
}

void KarasuTengu::ResetAttackState()
{
    mIsAttacking = false;
    mAttackHitApplied = false;
    mAttackTimer = 0.0f;
    mDrawComponent->SetAnimation("idle");
}

void KarasuTengu::Kill()
{
    if (mIsDying) return;
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    // Drop de chave ao morrer
    Key* key = new Key(mGame);
    key->SetPosition(mPosition + Vector2(0.0f, -20.0f));
}

void KarasuTengu::ApplyDamage(int amount)
{
    if (mIsDying) return;
    mHealth -= amount;
    mAttackHitApplied = false;
    mIsAttacking = false;
    mAttackTimer = 0.0f;
    mDrawComponent->SetAnimation("hurt");
    mHurtTimer = 0.35f;

    if (mHealth <= 0) {
        Kill();
    }
}

void KarasuTengu::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying) return;

    if (other->GetLayer() == ColliderLayer::Blocks) {
        // bate e volta na parede
        mScale.x *= -1.0f;
        mBaseSpeed *= -1.0f;
    }
}

void KarasuTengu::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying) return;

}
