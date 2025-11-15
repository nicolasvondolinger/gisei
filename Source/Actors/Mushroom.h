#pragma once

#include "Actor.h"
#include "../Game.h"
#include "../Actors/Mario.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"

class Mushroom : public Actor{
    public:
        explicit Mushroom(Game* game, float forwardSpeed);

        void OnUpdate(float deltaTime) override;
        void OnHorizontalCollision(float minOverlap, class AABBColliderComponent* other) override;
        void OnVerticalCollision(float minOverlap, class AABBColliderComponent* other) override;
    private:
        float mForwardSpeed;

        class RigidBodyComponent* mRigidBodyComponent;
        class AABBColliderComponent* mColliderComponent;
        class AnimatorComponent* mDrawComponent;
};