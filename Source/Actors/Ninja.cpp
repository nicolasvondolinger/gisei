#include "Ninja.h"
#include "Dart.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Drawing/DashAfterimageComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Ninja::Ninja(Game *game, const float accelerationForce, const float jumpSpeed)
    : Actor(game)
      , mIsRunning(false)
      , mIsDead(false)
      , mIsAttacking(false)
      , mIsDashing(false)
      , mIsDefending(false)
      , mIsShooting(false)
      , mAttackTimer(0.0f)
      , mDashTimer(0.0f)
      , mShootTimer(0.0f)
      , mDartSpawned(false)
      , mDashSpeed(1200.0f)
      , mDashDirection(Vector2::Zero)
      , mIsInvincible(false)
      , mInvincibleTimer(0.0f)
      , mDashCooldown(0.0f)
      , mShootCooldown(0.0f)
      , mAccelerationForce(accelerationForce)
      , mMaxSpeed(400.0f)
      , mJumpSpeed(jumpSpeed)
{
    mDrawComponent = new AnimatorComponent(this, 64, 64);

    mDrawComponent->AddAnimation("idle", "../Assets/Sprites/Ninja/Idle.png", 6, 10.0f, true);
    mDrawComponent->AddAnimation("walk", "../Assets/Sprites/Ninja/Walk.png", 8, 10.0f, true);
    mDrawComponent->AddAnimation("run", "../Assets/Sprites/Ninja/Run.png", 6, 10.0f, true);
    mDrawComponent->AddAnimation("jump", "../Assets/Sprites/Ninja/Jump.png", 8, 10.0f, false);
    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Ninja/Dead.png", 4, 10.0f, false);
    mDrawComponent->AddAnimation("attack1", "../Assets/Sprites/Ninja/Attack_1.png", 6, 12.0f, false);
    mDrawComponent->AddAnimation("attack2", "../Assets/Sprites/Ninja/Attack_2.png", 4, 12.0f, false);
    mDrawComponent->AddAnimation("hurt", "../Assets/Sprites/Ninja/Hurt.png", 2, 10.0f, false);
    mDrawComponent->AddAnimation("shot", "../Assets/Sprites/Ninja/Shot.png", 6, 12.0f, false);
    mDrawComponent->AddAnimation("disguise", "../Assets/Sprites/Ninja/Disguise.png", 9, 10.0f, false);

    mDrawComponent->SetAnimation("idle");

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 6.0f, true);
    


    mColliderComponent = new AABBColliderComponent (
        this,
        0,
        0,
        20.0f,
        48.0f,
        ColliderLayer::Player
    );
    
    new DashAfterimageComponent(this, 95);
}

void Ninja::OnProcessInput(const uint8_t* state) {
    if (mIsDead || mIsAttacking || mIsDashing || mIsShooting) return;

    // 1. Detecta se está defendendo, mas NÃO retorna nem zera a velocidade aqui
    mIsDefending = state[SDL_SCANCODE_S] && mIsOnGround;

    Vector2 force = Vector2::Zero;
    mIsRunning = false;

    // 2. Processa movimentação (agora funciona mesmo se mIsDefending for true)
    if(state[SDL_SCANCODE_D]){
        force.x += mAccelerationForce;
        mIsRunning = true;
    }

    if(state[SDL_SCANCODE_A]){
        force.x -= mAccelerationForce;
        mIsRunning = true;
    }

    // (Opcional) Reduz a força pela metade se estiver defendendo (andar agachado)
    if (mIsDefending) {
        force.x *= 0.5f; 
    }

    // 3. Bloqueia o pulo se estiver defendendo (!mIsDefending)
    if(state[SDL_SCANCODE_SPACE] && mIsOnGround && !mIsDefending) {
        mGame->PlaySound(mGame->GetJumpSound());
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.y = mJumpSpeed;
        mRigidBodyComponent->SetVelocity(vel);
    }

    if(state[SDL_SCANCODE_J] && mIsOnGround && !mIsAttacking) {
        mIsAttacking = true;
        mAttackTimer = 0.33f;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if(state[SDL_SCANCODE_K] && !mIsDashing && mDashCooldown <= 0.0f) {
        mIsDashing = true;
        mDashTimer = 0.3f;
        mDashCooldown = mDashCooldownTime;
        
        float dashDistance = 200.0f;
        Vector2 startPos = mPosition;
        Vector2 endPos = mPosition + Vector2(mScale.x * dashDistance, 0.0f);
        
        CreateDashParticles(startPos, endPos);
        
        mPosition = endPos;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
        mIsInvincible = true;
        mInvincibleTimer = 0.3f;
    }

    if(state[SDL_SCANCODE_L] && !mIsShooting && mShootCooldown <= 0.0f) {
        mIsShooting = true;
        mShootTimer = 0.5f;
        mShootCooldown = mShootCooldownTime;
        mDartSpawned = false;
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
    }

    if (!mIsDashing) {
        mRigidBodyComponent->ApplyForce(force);
    }
}

void Ninja::OnUpdate(float deltaTime) {
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

    if (mIsShooting) {
        mShootTimer -= deltaTime;
        
        int currentFrame = mDrawComponent->GetCurrentFrame();
        if (!mDartSpawned && currentFrame == 3) {
            mDartSpawned = true;
            Vector2 direction(mScale.x, 0.0f);
            Vector2 spawnOffset(mScale.x * 40.0f, 30.0f);
            Dart* dart = new Dart(mGame, direction);
            dart->SetPosition(mPosition + spawnOffset);
        }
        
        if (mShootTimer <= 0.0f) {
            mIsShooting = false;
        }
        mRigidBodyComponent->SetVelocity(Vector2::Zero);
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
    
    if (mDashCooldown > 0.0f) {
        mDashCooldown -= deltaTime;
    }
    
    if (mShootCooldown > 0.0f) {
        mShootCooldown -= deltaTime;
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
        bool flicker = fmodf(mInvincibleTimer * 4.0f, 1.0f) < 0.5f;
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

void Ninja::ManageAnimations() {
    if(mIsDead) return;

    if(mIsShooting) {
        mDrawComponent->SetAnimation("shot");
    }

    else if(mIsDashing) {
        mDrawComponent->SetAnimation("run");
    }
    else if(mIsAttacking) {
        mDrawComponent->SetAnimation("attack2");
    }
    else if(mIsDefending) {
        mDrawComponent->SetAnimation("disguise");
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



void Ninja::Kill() {
    if(mIsDead) return;

    if(!mIsInvincible){
        mIsDead = true;
        mDrawComponent->SetAnimation("dead");

        mRigidBodyComponent->SetEnabled(false);
        mColliderComponent->SetEnabled(false);

        Mix_HaltMusic();
        int channel = mGame->PlaySound(mGame->GetDeadSound());
        mGame->SetDeathSoundChannel(channel);
        mGame->SetScene(GameScene::GameOver);
    }
}

void Ninja::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    if(mIsDead) return;

    if(other->GetLayer() == ColliderLayer::Enemy) {
        if(mIsAttacking || mIsDashing) {
            other->GetOwner()->Kill();
        } else if(!mIsInvincible && !mIsDefending) {
            Kill();
        }
    }
}

void Ninja::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {
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

void Ninja::CreateDashParticles(const Vector2& startPos, const Vector2& endPos) {
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

void Ninja::StageClear() {
    mIsDead = true;
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
    mRigidBodyComponent->SetVelocity(Vector2::Zero);

    Mix_HaltMusic();
    int channel = mGame->PlaySound(mGame->GetStageClearSound());
    mGame->SetStageClearSoundChannel(channel);
    mGame->SetWaitingToQuit(true);
}