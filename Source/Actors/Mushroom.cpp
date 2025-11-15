#include "Mushroom.h"

Mushroom::Mushroom(Game* game, float forwardSpeed)
    : Actor(game),
    mForwardSpeed(forwardSpeed)
{
    mDrawComponent = new AnimatorComponent(
        this, 
        "../Assets/Sprites/Collectables/Mushroom.png",
        "",
        Game::TILE_SIZE,
        Game::TILE_SIZE
    );

    mDrawComponent->AddAnimation("walk", {0});
    mDrawComponent->SetAnimation("walk");
    mDrawComponent->SetAnimFPS(10.0f);

    mRigidBodyComponent = new RigidBodyComponent(
        this,
        1.0f,
        0.0f,
        true
    );

    mColliderComponent = new AABBColliderComponent(
        this,
        0,
        0,
        Game::TILE_SIZE,
        Game::TILE_SIZE,
        ColliderLayer::Mushroom
    );

}


void Mushroom::OnUpdate(float deltaTime){
    if(mRigidBodyComponent->IsEnabled()){
        Vector2 force(mForwardSpeed, 0.0f);
        mRigidBodyComponent->ApplyForce(force);
    }

    if(mPosition.y> Game::WINDOW_HEIGHT + Game::TILE_SIZE){
        mState = ActorState::Destroy;
    }

    float levelWidth = Game::LEVEL_WIDTH * Game::TILE_SIZE;
    if(mPosition.x < 0.0f || mPosition.x > levelWidth){
        mState = ActorState::Destroy;
    }
}

void Mushroom::OnHorizontalCollision(float minOverlap, AABBColliderComponent* other) {
    if (other->GetLayer() == ColliderLayer::Player){
        Mario* mario = static_cast<Mario*>(other->GetOwner());
        mario->Grow(); 
        mState = ActorState::Destroy;
    } else mForwardSpeed *= -1.0f;
}

void Mushroom::OnVerticalCollision(float minOverlap, AABBColliderComponent* other) {
    if (other->GetLayer() == ColliderLayer::Player) {        
        Mario* mario = static_cast<Mario*>(other->GetOwner());
        mario->Grow();
        mState = ActorState::Destroy;
    }
}