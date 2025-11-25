#include "Goomba.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Goomba::Goomba(Game *game, float forwardSpeed, float deathTime)
    : Actor(game)
      , mDyingTimer(deathTime)
      , mIsDying(false)
      , mForwardSpeed(forwardSpeed) {
    mDrawComponent = new AnimatorComponent(
        this,
        Game::TILE_SIZE,
        Game::TILE_SIZE
    );


    mDrawComponent->AddAnimation("walk", "../Assets/Sprites/Goomba/Walk.png", 2);


    mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Goomba/Dead.png", 1);

    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(4.0f);


    mRigidBodyComponent = new RigidBodyComponent(
        this,
        1.0f,
        5.0f,
        true
    );


    mColliderComponent = new AABBColliderComponent(
        this,
        0,
        0,
        Game::TILE_SIZE,
        Game::TILE_SIZE,
        ColliderLayer::Enemy
    );
}

void Goomba::Kill() {
    if (mIsDying) return;

    mIsDying = true;
    mDrawComponent->SetAnimation("dead");


    mRigidBodyComponent->SetEnabled(false);
    mColliderComponent->SetEnabled(false);
}

void Goomba::OnUpdate(float deltaTime) {
    if (mIsDying) {
        mDyingTimer -= deltaTime;
        if (mDyingTimer <= 0.0f) {
            mState = ActorState::Destroy;
        }
        return;
    }


    if (mPosition.y > Game::WINDOW_HEIGHT + (Game::TILE_SIZE / 2.0f)) {
        mState = ActorState::Destroy;
        return;
    }


    if (mRigidBodyComponent->IsEnabled()) {
        Vector2 force(mForwardSpeed, 0.0f);
        mRigidBodyComponent->ApplyForce(force);
    }
}

void Goomba::OnHorizontalCollision(const float minOverlap, AABBColliderComponent *other) {
    if (mIsDying) return;

    ColliderLayer otherLayer = other->GetLayer();

    if (otherLayer == ColliderLayer::Player) {
        other->GetOwner()->Kill();
    } else if (otherLayer == ColliderLayer::Blocks || otherLayer == ColliderLayer::Enemy) {
        mForwardSpeed *= -1;
    }
}

void Goomba::OnVerticalCollision(const float minOverlap, AABBColliderComponent *other) {
}
