#pragma once
#include "Actor.h"

class SkeletonWarrior : public Actor
{
public:
    explicit SkeletonWarrior(class Game* game, float forwardSpeed = -80.0f);

    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void Kill() override;

private:
    bool HasGroundAhead() const;
    void TurnAround();

    bool mIsDying;
    float mForwardSpeed;
    float mDyingTimer;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};
