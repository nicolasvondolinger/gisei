#include "Samurai.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/DashAfterimageComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Samurai::Samurai(Game *game, const float accelerationForce, const float jumpSpeed)
    : Actor(game)
      , mIsRunning(false)
      , mIsDead(false)
      , mIsAttacking(false)
      , mIsDashing(false)
      , mIsDefending(false)
      , mAttackTimer(0.0f)
      , mDashTimer(0.0f)
      , mDashSpeed(1200.0f)
      , mDashDirection(Vector2::Zero)
      , mIsInvincible(false)
      , mInvincibleTimer(0.0f)
      , mAccelerationForce(accelerationForce)
      , mMaxSpeed(400.0f)
      , mJumpSpeed(jumpSpeed)
{
    mDrawComponent = new AnimatorComponent(this, 128, 128);

    mDrawComponent->AddAnimation("idle", "../Assets/Sprites/Samurai/Idle.png", 6, 10.0f, true);
    mDrawComponent->AddAnimation("run", "../Assets/Sprites/Samurai/Run.png", 8, 10.0f, true);
    mDrawComponent->AddAnimation("jump", "../Assets/Sprites/Samurai/Jump.png", 9, 10.0f, false);
    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Samurai/Dead.png", 6, 10.0f, false);
    mDrawComponent->AddAnimation("attack", "../Assets/Sprites/Samurai/Attack_2.png", 5, 12.0f, false);
    mDrawComponent->AddAnimation("defend", "../Assets/Sprites/Samurai/Protection.png", 3, 8.0f, true);

    mDrawComponent->SetAnimation("idle");

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 6.0f, true);

    mColliderComponent = new AABBColliderComponent (
        this,
        0,
        0,
        128.0f,
        128.0f,
        ColliderLayer::Player
    );
    
    new DashAfterimageComponent(this, 95);
}

void Samurai::OnProcessInput(const uint8_t* state) {
    if (mIsDead || mIsAttacking || mIsDashing) return;

    mIsDefending = state[SDL_SCANCODE_S] && mIsOnGround;
    if (mIsDefending) {
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        return;
    }

    Vector2 force = Vector2::Zero;
    mIsRunning = false;

    if(state[SDL_SCANCODE_D]){
        force.x += mAccelerationForce;
        mIsRunning = true;
    }

    if(state[SDL_SCANCODE_A]){
        force.x -= mAccelerationForce;
        mIsRunning = true;
    }

    if(state[SDL_SCANCODE_SPACE] && mIsOnGround) {
        mGame->PlaySound(mGame->GetJumpSound());
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.y = mJumpSpeed;
        mRigidBodyComponent->SetVelocity(vel);
    }

    if(state[SDL_SCANCODE_J] && mIsOnGround && !mIsAttacking) {
        mIsAttacking = true;
        mAttackTimer = 0.42f;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if(state[SDL_SCANCODE_K] && mIsOnGround && !mIsDashing) {
        mIsDashing = true;
        mDashTimer = 0.15f;
        
        float dashDistance = 200.0f;
        Vector2 startPos = mPosition;
        Vector2 newPos = mPosition + Vector2(mScale.x * dashDistance, 0.0f);
        
        CreateDashParticles(startPos, newPos);
        
        mPosition = newPos;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mIsInvincible = true;
        mInvincibleTimer = 0.3f;
    }

    if (!mIsDashing) {
        mRigidBodyComponent->ApplyForce(force);
    }
}

void Samurai::OnUpdate(float deltaTime) {
    if(mIsDead) return;

    if (mIsAttacking) {
        mAttackTimer -= deltaTime;
        if (mAttackTimer <= 0.0f) {
            mIsAttacking = false;
        }
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (mIsDashing) {
        mDashTimer -= deltaTime;
        if (mDashTimer <= 0.0f) {
            mIsDashing = false;
        }
    }
    
    for(auto it = mDashParticles.begin(); it != mDashParticles.end();) {
        it->lifetime -= deltaTime;
        it->position += it->velocity * deltaTime;
        if(it->lifetime <= 0.0f) {
            it = mDashParticles.erase(it);
        } else {
            ++it;
        }
    }

    float levelEndX = (Game::LEVEL_WIDTH * Game::TILE_SIZE) - (Game::TILE_SIZE / 2.0f);
    if (mPosition.x >= levelEndX + Game::TILE_SIZE) {
        StageClear();
        return;
    }

    if (mPosition.y > Game::WINDOW_HEIGHT + 100.0f){
        Kill();
        return;
    }

    Vector2 velocity = mRigidBodyComponent->GetVelocity();

    if (velocity.x > mMaxSpeed) velocity.x = mMaxSpeed;
    if (velocity.x < -mMaxSpeed) velocity.x = -mMaxSpeed;

    mRigidBodyComponent->SetVelocity(velocity);

    if (std::abs(velocity.y) > 20.0f) {
        mIsOnGround = false;
    }

    if (std::abs(velocity.x) > 1.0f) {
        mScale.x = (velocity.x > 0.0f) ? 1.0f : -1.0f;
    }

    if (mIsInvincible && !mIsDashing) {
        mInvincibleTimer -= deltaTime;
        bool flicker = fmodf(mInvincibleTimer * 10.0f, 1.0f) < 0.5f;
        mDrawComponent->SetVisible(flicker);

        if (mInvincibleTimer <= 0.0f) {
            mIsInvincible = false;
            mDrawComponent->SetVisible(true);
        }
    }

    const float cameraX = GetGame()->GetCameraPos().x;
    if (mPosition.x < cameraX + 25.0f) {
        mPosition.x = cameraX + 25.0f;

        Vector2 v = mRigidBodyComponent->GetVelocity();
        v.x = 0.0f;
        mRigidBodyComponent->SetVelocity(v);
    }

    ManageAnimations();
}

void Samurai::ManageAnimations() {
    if(mIsDead) return;

    if(mIsAttacking) {
        mDrawComponent->SetAnimation("attack");
    }
    else if(mIsDefending) {
        mDrawComponent->SetAnimation("defend");
    }
    else if(!mIsOnGround) {
        mDrawComponent->SetAnimation("jump");
    }
    else if(mIsRunning && std::abs(mRigidBodyComponent->GetVelocity().x) > 10.0f) {
        mDrawComponent->SetAnimation("run");
    }
    else {
        mDrawComponent->SetAnimation("idle");
    }
}

void Samurai::CreateDashParticles(const Vector2& startPos, const Vector2& endPos) {
    int numParticles = 20;
    for(int i = 0; i < numParticles; i++) {
        DashParticle particle;
        float t = static_cast<float>(i) / numParticles;
        particle.position = startPos + (endPos - startPos) * t;
        
        float angle = (rand() % 360) * Math::Pi / 180.0f;
        float speed = 50.0f + (rand() % 100);
        particle.velocity = Vector2(Math::Cos(angle) * speed, Math::Sin(angle) * speed);
        
        particle.maxLifetime = 0.3f + (rand() % 100) / 500.0f;
        particle.lifetime = particle.maxLifetime;
        particle.size = 3.0f + (rand() % 5);
        
        mDashParticles.push_back(particle);
    }
}

const std::vector<DashParticle>& Samurai::GetDashParticles() const {
    return mDashParticles;
}

void Samurai::Kill() {
    if(mIsDead) return;

    if(!mIsInvincible){
        mIsDead = true;
        mDrawComponent->SetAnimation("dead");

        mRigidBodyComponent->SetEnabled(false);
        mColliderComponent->SetEnabled(false);

        Mix_HaltMusic();
        int channel = mGame->PlaySound(mGame->GetDeadSound());
        mGame->SetDeathSoundChannel(channel);
        mGame->SetWaitingToQuit(true);
    }
}

void Samurai::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    if(mIsDead) return;

    if(other->GetLayer() == ColliderLayer::Enemy) {
        if(mIsAttacking || mIsDashing) {
            other->GetOwner()->Kill();
        } else if(!mIsInvincible && !mIsDefending) {
            Kill();
        }
    }
}

void Samurai::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {
    if(mIsDead) return;

    if(other->GetLayer() == ColliderLayer::Enemy){
        float velocityY = mRigidBodyComponent->GetVelocity().y;
        if(velocityY > 0.0f){
            other->GetOwner()->Kill();

            Vector2 vel = mRigidBodyComponent->GetVelocity();
            vel.y = -350.0f;
            mRigidBodyComponent->SetVelocity(vel);
        } else {

            Kill();
        }
    }
    else if(other->GetLayer() == ColliderLayer::Blocks){
        if (mRigidBodyComponent->GetVelocity().y < 0.0f) {
            Vector2 vel = mRigidBodyComponent->GetVelocity();
            vel.y = 0.0f;
            mRigidBodyComponent->SetVelocity(vel);
        }
    }
}

void Samurai::StageClear() {
    mIsDead = true;
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
    mRigidBodyComponent->SetVelocity(Vector2::Zero);

    Mix_HaltMusic();
    int channel = mGame->PlaySound(mGame->GetStageClearSound());
    mGame->SetStageClearSoundChannel(channel);
    mGame->SetWaitingToQuit(true);
}