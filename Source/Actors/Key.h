#pragma once
#include "Actor.h"

class Key : public Actor
{
public:
    // Construtor
    Key(class Game* game);

    // Funções de Update e Colisão sobrescritas
    void OnUpdate(float deltaTime) override;
    void OnHorizontalCollision(const float minOverlap, class AABBColliderComponent* other) override;
    void OnVerticalCollision(const float minOverlap, class AABBColliderComponent* other) override;

private:
    // Componentes visuais e físicos usados no .cpp
    class AnimatorComponent* mDrawComponent;
    class AABBColliderComponent* mColliderComponent;
};