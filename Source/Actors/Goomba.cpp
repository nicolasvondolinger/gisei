//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Goomba.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Goomba::Goomba(Game* game, float forwardSpeed, float deathTime)
        : Actor(game)
        , mDyingTimer(deathTime)
        , mIsDying(false)
        , mForwardSpeed(forwardSpeed)
{
        // 1. Configura o Animator (Tamanho 32x32 padrão do Goomba)
        mDrawComponent = new AnimatorComponent(
                this,
                Game::TILE_SIZE, // Width
                Game::TILE_SIZE  // Height
        );

        // 2. Adiciona as animações com arquivos separados
        // Certifique-se de que esses arquivos existam na pasta!

        // Walk: 2 frames de animação
        mDrawComponent->AddAnimation("walk", "../Assets/Sprites/Goomba/Walk.png", 2);

        // Dead: 1 frame (achatado)
        mDrawComponent->AddAnimation("dead", "../Assets/Sprites/Goomba/Dead.png", 1);

        mDrawComponent->SetAnimation("walk");
        mDrawComponent->SetAnimFPS(4.0f); // Goomba anda devagar, animação mais lenta

        // 3. Física
        mRigidBodyComponent = new RigidBodyComponent(
                this,
                1.0f,
                5.0f,
                true // Enabled
        );

        // 4. Colisão
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

        // Desativa colisão e gravidade para ele ficar achatado no chão por um tempo
        mRigidBodyComponent->SetEnabled(false);
        mColliderComponent->SetEnabled(false);
}

void Goomba::OnUpdate(float deltaTime) {

        if(mIsDying){
                mDyingTimer -= deltaTime;
                if(mDyingTimer <= 0.0f) {
                    mState = ActorState::Destroy;
                }
                return;
        }

        // Se cair do mundo, morre instantaneamente
        if (mPosition.y > Game::WINDOW_HEIGHT + (Game::TILE_SIZE / 2.0f)){
            mState = ActorState::Destroy;
            return;
        }

        // Movimento simples (Patrulha)
        if (mRigidBodyComponent->IsEnabled()) {
                Vector2 force(mForwardSpeed, 0.0f);
                mRigidBodyComponent->ApplyForce(force);
        }
}

void Goomba::OnHorizontalCollision(const float minOverlap, AABBColliderComponent* other) {
        if (mIsDying) return;

        ColliderLayer otherLayer = other->GetLayer();

        if(otherLayer == ColliderLayer::Player){
                // Se encostar no Player de lado, mata o Player
                other->GetOwner()->Kill();
        }
        else if (otherLayer == ColliderLayer::Blocks || otherLayer == ColliderLayer::Enemy) {
                // Se bater na parede ou outro inimigo, vira para o outro lado
                mForwardSpeed *= -1;

                // Espelha o sprite se necessário (opcional para Goomba pois é simétrico, mas boa prática)
                /*
                Vector2 scale = GetScale();
                scale.x *= -1.0f;
                SetScale(scale);
                */
        }
}

void Goomba::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other){
    // Geralmente Goomba não faz nada ao colidir verticalmente.
    // Quem trata o "pisão" é o Player na função OnVerticalCollision dele.
}