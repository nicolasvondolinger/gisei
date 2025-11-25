#include "Samurai.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Samurai::Samurai(Game *game, const float accelerationForce, const float jumpSpeed)
    : Actor(game)
      , mIsRunning(false)
      , mIsDead(false)
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
    mDrawComponent->AddAnimation("attack1", "../Assets/Sprites/Samurai/Attack_1.png", 4, 10.0f, false);

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
}

void Samurai::OnProcessInput(const uint8_t* state) {
    if (mIsDead) return;

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
    mRigidBodyComponent->ApplyForce(force);
}

void Samurai::OnUpdate(float deltaTime) {
    if(mIsDead) return;

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

    if(!mIsOnGround) {
        mDrawComponent->SetAnimation("jump");
    }
    else if(mIsRunning && std::abs(mRigidBodyComponent->GetVelocity().x) > 10.0f) {
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