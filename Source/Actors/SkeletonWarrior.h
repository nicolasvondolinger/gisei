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
    bool mIsDying;
    float mForwardSpeed;
    float mDyingTimer;
    float mChaseDelay;
    bool mIsChasing;

    class RigidBodyComponent* mRigidBodyComponent;
    class AABBColliderComponent* mColliderComponent;
    class AnimatorComponent* mDrawComponent;
};
