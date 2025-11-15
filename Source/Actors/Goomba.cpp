//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Goomba.h"

Goomba::Goomba(Game* game, float forwardSpeed, float deathTime)
        : Actor(game)
        , mDyingTimer(deathTime)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
{

        mDrawComponent = new AnimatorComponent(
                this, 
                "../Assets/Sprites/Goomba/Goomba.png",
                "../Assets/Sprites/Goomba/Goomba.json",
                Game::TILE_SIZE,
                Game::TILE_SIZE
        );

        mDrawComponent->AddAnimation("walk", {1, 2});
        mDrawComponent->AddAnimation("dead", {0});
        mDrawComponent->SetAnimation("walk");  
        mDrawComponent->SetAnimFPS(10.0f);    

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

        if(mIsDying){
                mDyingTimer -= deltaTime;
                if(mDyingTimer<= 0.0f) mState = ActorState::Destroy;
                return;
        } 

        if (mPosition.y > Game::WINDOW_HEIGHT + (Game::TILE_SIZE / 2.0f)){
            mState = ActorState::Destroy;
            return;
        }

        if (mRigidBodyComponent->IsEnabled()) {
                
                Vector2 force(mForwardSpeed, 0.0f);
                mRigidBodyComponent->ApplyForce(force);
        }

}

void Goomba::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
        if (mIsDying) return; 
        ColliderLayer otherLayer = other->GetLayer();
        if(otherLayer == ColliderLayer::Player){
                other->GetOwner()->Kill();
        } else {
                mForwardSpeed *= -1;
                Vector2 scale = GetScale();
                scale.x *= -1.0;
                SetScale(scale);
        }
}

void Goomba::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other){


}
