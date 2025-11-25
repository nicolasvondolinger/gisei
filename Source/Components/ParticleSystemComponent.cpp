#include "../Game.h"
#include "ParticleSystemComponent.h"
#include "Physics/AABBColliderComponent.h"
#include "Physics/RigidBodyComponent.h"
#include "Drawing/RectComponent.h"

Particle::Particle(class Game *game, int width, int height)
    : Actor(game)
      , mDrawComponent(nullptr)
      , mRigidBodyComponent(nullptr)
      , mColliderComponent(nullptr)
      , mIsDead(true)
      , mLifeTime(1.0f) {
    mDrawComponent = new RectComponent(this, width, height, RendererMode::TRIANGLES);
    mRigidBodyComponent = new RigidBodyComponent(this, 1.0f, 0.0f, true);
    mColliderComponent = new AABBColliderComponent(this, 0, 0, width, height, ColliderLayer::Blocks);

    SetState(ActorState::Paused);
    mDrawComponent->SetVisible(false);
}

void Particle::Kill() {
    mIsDead = true;
    SetState(ActorState::Paused);
    mDrawComponent->SetVisible(false);
    mColliderComponent->SetEnabled(false);


    mRigidBodyComponent->SetVelocity(Vector2::Zero);
}

void Particle::Awake(const Vector2 &position, float rotation, float lifetime) {
    mLifeTime = lifetime;

    mIsDead = false;
    SetState(ActorState::Active);
    mDrawComponent->SetVisible(true);
    mColliderComponent->SetEnabled(true);

    SetPosition(position);
    SetRotation(rotation);
}

void Particle::OnUpdate(float deltaTime) {
    mLifeTime -= deltaTime;
    if (mLifeTime <= 0) {
        Kill();
        return;
    }
}

ParticleSystemComponent::ParticleSystemComponent(class Actor *owner, int particleW, int particleH, int poolSize,
                                                 int updateOrder)
    : Component(owner, updateOrder) {
    for (int i = 0; i < poolSize; i++) {
        auto *p = new Particle(owner->GetGame(), particleW, particleH);
        mParticles.push_back(p);
    }
}

void ParticleSystemComponent::EmitParticle(float lifetime, float speed, const Vector2 &offsetPosition) {
    for (auto p: mParticles) {
        if (p->IsDead()) {
            Vector2 spawnPos = mOwner->GetPosition() + offsetPosition * mOwner->GetScale().x;
            p->Awake(spawnPos, mOwner->GetRotation(), lifetime);


            Vector2 direction = mOwner->GetScale();
            p->GetComponent<RigidBodyComponent>()->ApplyForce(direction * speed);


            break;
        }
    }
}