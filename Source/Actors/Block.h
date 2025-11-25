#pragma once
#include "Actor.h"
#include <SDL.h> // Necessário para SDL_Rect

enum class EBlockType
{
    Normal, // Chão solido comum
    Hazard  // Espinhos, lava, etc (Dá dano)
};

class Block : public Actor
{
public:
    // Agora aceita Texture* direto e o recorte (srcRect)
    Block(class Game* game, class Texture* texture, SDL_Rect srcRect, EBlockType type);

    void OnUpdate(float deltaTime) override;

    // Tratamento de colisão simplificado
    void OnCollision(class Actor* other);

    EBlockType GetType() const { return mType; }

private:
    EBlockType mType;
    class AnimatorComponent* mAnimator;
};