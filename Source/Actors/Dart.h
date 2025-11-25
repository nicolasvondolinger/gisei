#pragma once
#include "Actor.h"

class Dart : public Actor
{
public:
    explicit Dart(class Game* game, const Vector2& direction);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

    void Kill() override;

private:
    float mSpeed;
    Vector2 mDirection;
    
    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};
