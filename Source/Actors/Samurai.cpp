#include "Samurai.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Samurai::Samurai(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mIsInvincible(false)
        , mInvincibleTimer(0.0f)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
{
    mDrawComponent = new AnimatorComponent(this, 128, 128);
    mDrawComponent->AddAnimation("idle",  "../Assets/Sprites/Samurai/Idle.png", 6);
    mDrawComponent->AddAnimation("run",  "../Assets/Sprites/Samurai/Run.png", 8);
    mDrawComponent->AddAnimation("jump", "../Assets/Sprites/Samurai/Jump.png", 9); // Ajuste se o pulo for rápido demais
    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Samurai/Dead.png", 6);
    mDrawComponent->AddAnimation("attack1", "../Assets/Sprites/Samurai/Attack_1.png", 4);
    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(6.0f);
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0, 1.0f, true); // Friction 4.0 para parar de escorregar rápido
    mColliderComponent = new AABBColliderComponent (
        this,
        0,
        0,
        65.0f,
        128.0f,
        ColliderLayer::Player
    );
}

void Samurai::OnProcessInput(const uint8_t* state) {
    if (mIsDead) return;

    Vector2 force = Vector2::Zero;
    mIsRunning = false;

    if(state[SDL_SCANCODE_D]){
        force.x = mForwardSpeed;
        mScale.x = 1.0f;
        mIsRunning = true;
    }

    if(state[SDL_SCANCODE_A]){
        force.x = -mForwardSpeed;
        mScale.x = -1.0f;
        mIsRunning = true;
    }

    if(state[SDL_SCANCODE_SPACE] && mIsOnGround) {
        mGame->PlaySound(mGame->GetJumpSound());

        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.y = mJumpSpeed;
        mRigidBodyComponent->SetVelocity(vel);
    }

    mRigidBodyComponent->ApplyForce(force);
}

void Samurai::OnUpdate(float deltaTime) {
    if(mIsDead) return;

    float levelEndX = (Game::LEVEL_WIDTH * Game::TILE_SIZE) - (Game::TILE_SIZE / 2.0f);
    if (mPosition.x >= levelEndX + Game::TILE_SIZE) {
        StageClear();
        return;
    }

    if(mRigidBodyComponent->GetVelocity().y != 0.0f){
        mIsOnGround = false;
    }

    if (mPosition.y > Game::WINDOW_HEIGHT + 100.0f){
        Kill();
    }

    if (mIsInvincible) {
        mInvincibleTimer -= deltaTime;
        bool flicker = fmodf(mInvincibleTimer * 10.0f, 1.0f) < 0.5f;
        mDrawComponent->SetVisible(flicker);

        if (mInvincibleTimer <= 0.0f) {
            mIsInvincible = false;
            mDrawComponent->SetVisible(true);
        }
    }

    const float cameraX = GetGame()->GetCameraPos().x;

    if (mPosition.x < cameraX + 20.0f) {
        mPosition.x = cameraX + 20.0f;

        // Zera velocidade X se bater na "parede" da câmera
        Vector2 v = mRigidBodyComponent->GetVelocity();
        v.x = 0.0f;
        mRigidBodyComponent->SetVelocity(v);
    }

    ManageAnimations();
}

void Samurai::ManageAnimations() {
    if(mIsDead) return;

    if(!mIsOnGround) {
        mDrawComponent->SetAnimation("jump");
    }
    else if(mIsRunning){
        mDrawComponent->SetAnimation("run");
    }
    else {
        mDrawComponent->SetAnimation("idle");
    }
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
    if(other->GetLayer() == ColliderLayer::Enemy && !mIsInvincible) {
        Kill();
    }
}

void Samurai::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {
    if(mIsDead) return;

    if(other->GetLayer() == ColliderLayer::Enemy){
        float velocityY = mRigidBodyComponent->GetVelocity().y;

        if(velocityY > 0.0f){
            other->GetOwner()->Kill();

            Vector2 vel = mRigidBodyComponent->GetVelocity();
            vel.y = -400.0f;
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