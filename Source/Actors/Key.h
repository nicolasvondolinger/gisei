#pragma once
#include "Actor.h"

class Key : public Actor {
public:
    explicit Key(class Game* game);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

private:
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};
