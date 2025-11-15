//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "Actor.h"

enum class EBlockType
{
    Ground,
    Brick,
    EspecialBrick,
    Question,
    Pipe
};

class Block : public Actor
{
public:
    explicit Block(Game* game, const std::string &texturePath, EBlockType type);

    void OnUpdate(float deltaTime) override;
    void OnVerticalCollision(float minOverlap, class AABBColliderComponent* other) override;
private:
    EBlockType mBlockType;
    bool mIsBumping;
    float mBumpOffset;
    float mBumpSpeed;
    Vector2 mOriginalPosition;
    bool mHasSpawnedItem;
};
