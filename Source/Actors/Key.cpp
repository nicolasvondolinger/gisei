#include "Key.h"
#include "Ninja.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

Key::Key(Game* game)
    : Actor(game)
{
    mDrawComponent = new AnimatorComponent(this, 22, 20);
    mDrawComponent->AddAnimation("idle", "../Assets/Levels/Key.png", 1);
    mDrawComponent->SetAnimation("idle");

    mColliderComponent = new AABBColliderComponent(this, 0, 0, 20, 28, ColliderLayer::Mushroom);
}

void Key::OnUpdate(float /*deltaTime*/)
{
    // A chave fica parada; destruição é feita ao coletar.
}

void Key::OnHorizontalCollision(const float /*minOverlap*/, AABBColliderComponent* other)
{
    if (other->GetLayer() == ColliderLayer::Player) {
        if (auto ninja = dynamic_cast<Ninja*>(other->GetOwner())) {
            (void)ninja;
            mState = ActorState::Destroy;
        }
    }
}

void Key::OnVerticalCollision(const float minOverlap, AABBColliderComponent* other)
{
    OnHorizontalCollision(minOverlap, other);
}
