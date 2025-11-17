//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Mario.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/ParticleSystemComponent.h"

Mario::Mario(Game* game, const float forwardSpeed, const float jumpSpeed)
        : Actor(game)
        , mIsRunning(false)
        , mIsDead(false)
        , mIsBig(false)
        , mIsInvincible(false)
        , mInvincibleTimer(0.0f)
        , mForwardSpeed(forwardSpeed)
        , mJumpSpeed(jumpSpeed)
{

    mDrawComponent = new AnimatorComponent(
        this,
        "../Assets/Sprites/Mario/texture.png",
        "../Assets/Sprites/Mario/texture.json",
        Game::TILE_SIZE,
        Game::TILE_SIZE
    );

    // Animações Pequenas
    mDrawComponent->AddAnimation("idle", {10});
    mDrawComponent->AddAnimation("run",  {16, 11, 12});
    mDrawComponent->AddAnimation("jump", {2});
    mDrawComponent->AddAnimation("dead", {14});

    // Animações Grandes
    mDrawComponent->AddAnimation("big_idle", {0});
    mDrawComponent->AddAnimation("big_run",  {9, 8, 6});
    mDrawComponent->AddAnimation("big_jump", {3});

    mDrawComponent->SetAnimation("idle");
    mDrawComponent->SetAnimFPS(10.0f);

    mRigidBodyComponent = new RigidBodyComponent(this, 1.0, 8.0f, true);

    mColliderComponent = new AABBColliderComponent (
        this, 
        0, 
        0,
        Game::TILE_SIZE,
        Game::TILE_SIZE,
        ColliderLayer::Player
    );

}

void Mario::OnProcessInput(const uint8_t* state) {

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
        if(mIsBig) mGame->PlaySound(mGame->GetJumpSuperSound());
        else mGame->PlaySound(mGame->GetJumpSound());
        Vector2 vel = mRigidBodyComponent->GetVelocity();
        vel.y = mJumpSpeed; 
        mRigidBodyComponent->SetVelocity(vel);        
        // mIsOnGround = false;
    }

    mRigidBodyComponent->ApplyForce(force);
}

void Mario::OnUpdate(float deltaTime) {

    if(mIsDead) return;

    float levelEndX = (Game::LEVEL_WIDTH * Game::TILE_SIZE) - (Game::TILE_SIZE / 2.0f);
    if (mPosition.x >= levelEndX + Game::TILE_SIZE) {
        StageClear();
        return; 
    }

    if(mRigidBodyComponent->GetVelocity().y != 0.0f){
        mIsOnGround = false;
    }

    if (mPosition.y > Game::WINDOW_HEIGHT + (Game::TILE_SIZE / 2.0f)){
        Kill();
    }

    if (mIsInvincible) {
        mInvincibleTimer-= deltaTime;
        bool flicker = fmodf(mInvincibleTimer * 10.0f, 1.0f) < 0.5f;
        mDrawComponent->SetEnabled(flicker);

        if (mInvincibleTimer <= 0.0f) {
            mIsInvincible = false;
            mDrawComponent->SetEnabled(true); // Garante que ele fique visível no final
        }
    }

    const float cameraX = GetGame()->GetCameraPos().x;

    float marioLeftEdge = mPosition.x - (Game::TILE_SIZE / 2.0f);

    if (marioLeftEdge < cameraX) {
        mPosition.x = cameraX + (Game::TILE_SIZE / 2.0f);
    }
    
    ManageAnimations();
}

void Mario::ManageAnimations() {

    if(mIsDead) return;

    if(mIsBig){
        if(mIsRunning && mIsOnGround){
            mDrawComponent->SetAnimation("big_run");
        } else if(!mIsRunning && mIsOnGround){
            mDrawComponent->SetAnimation("big_idle");
        } else if(!mIsOnGround) {
            mDrawComponent->SetAnimation("big_jump");
        }
    } else {
        if(mIsRunning && mIsOnGround){
            mDrawComponent->SetAnimation("run");
        } else if(!mIsRunning && mIsOnGround){
            mDrawComponent->SetAnimation("idle");
        } else if(!mIsOnGround) {
            mDrawComponent->SetAnimation("jump");
        }
    }

}

void Mario::Kill() {
    if(mIsDead) return;
    else if(mIsBig){
        Shrink();
        return;
    } else if(!mIsInvincible){
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

void Mario::Grow(){
    if(mIsDead || mIsBig) return;

    mIsBig = true;

    mColliderComponent->SetSize(Game::TILE_SIZE, Game::TILE_SIZE*2);
    mDrawComponent->SetSize(Game::TILE_SIZE, Game::TILE_SIZE*2);
    mPosition.y -= (Game::TILE_SIZE/2.0f);
}

void Mario::Shrink(){
    if(mIsDead || !mIsBig) return;

    mIsBig = false;
    mIsInvincible = true; mInvincibleTimer = mInvincibleDuration;
    mColliderComponent->SetSize(Game::TILE_SIZE, Game::TILE_SIZE);
    mDrawComponent->SetSize(Game::TILE_SIZE, Game::TILE_SIZE);
    mPosition.y += (Game::TILE_SIZE/2.0f);
}

void Mario::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
    if(mIsDead) return;

    if(other->GetLayer() == ColliderLayer::Enemy && !mIsInvincible) Kill();
}

void Mario::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other) {

    if(mIsDead) return;

    if(other->GetLayer() == ColliderLayer::Enemy){
        float marioVelocityY = mRigidBodyComponent->GetVelocity().y;

        if(marioVelocityY > 0.0f){
            other->GetOwner()->Kill();

            Vector2 vel = mRigidBodyComponent->GetVelocity();
            vel.y = mJumpSpeed * 0.5f;
            mRigidBodyComponent->SetVelocity(vel);
        } else Kill();
    } else if(other->GetLayer() == ColliderLayer::Blocks){
        if (mRigidBodyComponent->GetVelocity().y < 0.0f) {
            Vector2 vel = mRigidBodyComponent->GetVelocity();
            vel.y = 0.0f;
            mRigidBodyComponent->SetVelocity(vel);
        }
    }

}


void Mario::StageClear() {
    mIsDead = true;
    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
    mRigidBodyComponent->SetVelocity(Vector2::Zero); 

    Mix_HaltMusic();

    int channel = mGame->PlaySound(mGame->GetStageClearSound());
    mGame->SetStageClearSoundChannel(channel);
    mGame->SetWaitingToQuit(true);
}
