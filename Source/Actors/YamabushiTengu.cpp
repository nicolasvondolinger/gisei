#include "YamabushiTengu.h"
#include "Ninja.h"
#include "BossEnemy.h"
#include "../Game.h"
#include "../Math.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../UI/Screens/YouWin.h"

YamabushiTengu::YamabushiTengu(Game* game)
    : Actor(game)
    , mMaxHealth(14)
    , mHealth(mMaxHealth)
    , mIsDying(false)
    , mIsAttacking(false)
    , mIsLeaping(false)
    , mAttackHitApplied(false)
    , mHurtTimer(0.0f)
    , mAttackTimer(0.0f)
    , mAttackDuration(0.0f)
    , mAttackWindup(0.0f)
    , mAttackCooldown(1.5f)
    , mLeapCooldown(3.0f)
    , mBaseSpeed(150.0f)
    , mAttackRange(120.0f)
    , mName("Yamabushi Tengu")
{
    mDrawComponent = new AnimatorComponent(this, 128, 128);
    mDrawComponent->AddAnimation("idle", "../Assets/Sprites/Yamabushi_tengu/Idle.png", 6, 6.0f, true);
    mDrawComponent->AddAnimation("idle2", "../Assets/Sprites/Yamabushi_tengu/Idle_2.png", 5, 6.0f, true);
    mDrawComponent->AddAnimation("walk", "../Assets/Sprites/Yamabushi_tengu/Walk.png", 8, 7.0f, true);
    mDrawComponent->AddAnimation("run", "../Assets/Sprites/Yamabushi_tengu/Run.png", 8, 8.0f, true);
    mDrawComponent->AddAnimation("jump", "../Assets/Sprites/Yamabushi_tengu/Jump.png", 15, 12.0f, false);
    mDrawComponent->AddAnimation("attack1", "../Assets/Sprites/Yamabushi_tengu/Attack_1.png", 3, 12.0f, false);
    mDrawComponent->AddAnimation("attack2", "../Assets/Sprites/Yamabushi_tengu/Attack_2.png", 6, 12.0f, false);
    mDrawComponent->AddAnimation("attack3", "../Assets/Sprites/Yamabushi_tengu/Attack_3.png", 4, 10.0f, false);
    mDrawComponent->AddAnimation("hurt", "../Assets/Sprites/Yamabushi_tengu/Hurt.png", 3, 12.0f, false);
    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Yamabushi_tengu/Dead.png", 6, 8.0f, false);
    mDrawComponent->SetAnimation("idle");

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 3.0f, true);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, 42, 66, ColliderLayer::Enemy);

    mGame->RegisterBoss(this);
}

YamabushiTengu::~YamabushiTengu()
{
    if (mGame) {
        mGame->ClearBoss(this);
    }
}

void YamabushiTengu::OnUpdate(float deltaTime)
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
        if (mHurtTimer <= 0.0f && !mIsAttacking && !mIsLeaping) {
            mDrawComponent->SetAnimation("idle");
        }
        return;
    }

    if (mAttackCooldown > 0.0f) mAttackCooldown -= deltaTime;
    if (mLeapCooldown > 0.0f) mLeapCooldown -= deltaTime;

    if (mIsAttacking || mIsLeaping) {
        mAttackTimer -= deltaTime;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);

        if (!mAttackHitApplied && mAttackTimer <= mAttackDuration - mAttackWindup) {
            if (auto ninja = mGame->GetPlayer()) {
                Vector2 pos = mPosition;
                Vector2 target = ninja->GetPosition();
                if (Math::Abs(pos.x - target.x) <= mAttackRange &&
                    Math::Abs(pos.y - target.y) <= 90.0f) {
                    mAttackHitApplied = true;
                    ninja->TakeDamage(pos);
                }
            }
        }

        if (mAttackTimer <= 0.0f) {
            ResetAttackState();
        }
        return;
    }

    Vector2 velocity = mRigidBodyComponent->GetVelocity();
    velocity.x = 0.0f;

    if (auto ninja = mGame->GetPlayer()) {
        Vector2 target = ninja->GetPosition();
        float dx = target.x - mPosition.x;
        float dy = target.y - mPosition.y;
        float dir = (dx >= 0.0f) ? 1.0f : -1.0f;
        mScale.x = dir;

        bool canLeap = (Math::Abs(dx) > mAttackRange * 1.2f) && (Math::Abs(dy) < 120.0f) && mLeapCooldown <= 0.0f;
        if (canLeap) {
            EnterLeap();
            velocity = Vector2(dir * mBaseSpeed * 1.2f, -260.0f);
            mRigidBodyComponent->SetVelocity(velocity);
        } else if (Math::Abs(dx) > mAttackRange || Math::Abs(dy) > 100.0f) {
            velocity.x = dir * mBaseSpeed;
            mDrawComponent->SetAnimation("run");
        } else if (mAttackCooldown <= 0.0f) {
            EnterAttack();
            velocity.x = 0.0f;
        } else {
            mDrawComponent->SetAnimation("idle2");
        }
    } else {
        mDrawComponent->SetAnimation("idle");
    }

    mRigidBodyComponent->SetVelocity(velocity);
}

void YamabushiTengu::EnterAttack()
{
    mIsAttacking = true;
    mIsLeaping = false;
    mAttackHitApplied = false;
    mAttackDuration = 0.7f;
    mAttackWindup = 0.25f;
    mAttackTimer = mAttackDuration;
    mAttackCooldown = 1.6f;

    static int attackIndex = 0;
    const char* attackNames[] = { "attack1", "attack2", "attack3" };
    mDrawComponent->SetAnimation(attackNames[attackIndex % 3]);
    attackIndex++;
}

void YamabushiTengu::EnterLeap()
{
    mIsLeaping = true;
    mIsAttacking = false;
    mAttackHitApplied = false;
    mAttackDuration = 0.9f;
    mAttackWindup = 0.2f;
    mAttackTimer = mAttackDuration;
    mAttackCooldown = 0.8f;
    mLeapCooldown = 3.0f;
    mDrawComponent->SetAnimation("jump");
}

void YamabushiTengu::ResetAttackState()
{
    mIsAttacking = false;
    mIsLeaping = false;
    mAttackHitApplied = false;
    mAttackTimer = 0.0f;
    mDrawComponent->SetAnimation("idle");
}

void YamabushiTengu::Kill()
{
    if (mIsDying) return;
    mIsDying = true;
    mDrawComponent->SetAnimation("dead");
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);

    new YouWin(mGame, "../Assets/Fonts/Alkhemikal.ttf");
}

void YamabushiTengu::ApplyDamage(int amount)
{
    if (mIsDying) return;
    mHealth -= amount;
    mAttackHitApplied = false;
    mIsAttacking = false;
    mIsLeaping = false;
    mAttackTimer = 0.0f;
    mDrawComponent->SetAnimation("hurt");
    mHurtTimer = 0.35f;

    if (mHealth <= 0) {
        Kill();
    }
}

void YamabushiTengu::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying) return;

    if (other->GetLayer() == ColliderLayer::Player) {
        if (auto ninja = dynamic_cast<Ninja*>(other->GetOwner())) {
            ninja->TakeDamage(mPosition);
        }
    } else if (other->GetLayer() == ColliderLayer::Blocks) {
        mScale.x *= -1.0f;
        mBaseSpeed *= -1.0f;
    }
}

void YamabushiTengu::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    if (mIsDying) return;

    if (other->GetLayer() == ColliderLayer::Player) {
        if (auto ninja = dynamic_cast<Ninja*>(other->GetOwner())) {
            ninja->TakeDamage(mPosition);
        }
    }
}
