//
// Created by Lucas N. Ferreira on 03/08/23.
//

#include "Block.h"
#include "Mushroom.h"
#include "../Game.h"
#include "../Components/Drawing/AnimatorComponent.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Physics/RigidBodyComponent.h"

const float BUMP_GRAVITY = 3000.0f;
const float BUMP_START_SPEED = -350.0f;

Block::Block(Game* game, const string &texturePath, EBlockType type)
        :Actor(game), 
        mBlockType(type),
        mIsBumping(false),
        mBumpOffset(0.0f),
        mBumpSpeed(0.0f),
        mOriginalPosition(Vector2::Zero),
        mHasSpawnedItem(false)
{
        new AnimatorComponent(
                this,
                texturePath,
                "",
                Game::TILE_SIZE,
                Game::TILE_SIZE
        );

        new AABBColliderComponent(
                this, 
                0,
                0,
                Game::TILE_SIZE,
                Game::TILE_SIZE,
                ColliderLayer::Blocks,
                true
        );
}

void Block::OnUpdate(float deltaTime)
{
    
        if (mOriginalPosition.x == 0 && mOriginalPosition.y == 0)
                mOriginalPosition = mPosition;

        if (!mIsBumping) return;
    
        mBumpOffset += mBumpSpeed * deltaTime;
        mBumpSpeed += BUMP_GRAVITY * deltaTime;

        if (mBumpOffset >= 0.0f) {
                mIsBumping = false;
                mBumpOffset = 0.0f;
                SetPosition(mOriginalPosition); 
        } else  SetPosition(mOriginalPosition + Vector2(0.0f, mBumpOffset));
        
}

void Block::OnVerticalCollision(float minOverlap, AABBColliderComponent* other) {
        if (mIsBumping || other->GetLayer() != ColliderLayer::Player) return;
    
    
        bool playerIsBelow = other->GetOwner()->GetPosition().y > mPosition.y;
        if (playerIsBelow) {
                mIsBumping = true;
                mBumpSpeed = BUMP_START_SPEED;
                if(mBlockType != EBlockType::EspecialBrick){
                        mGame->PlaySound(mGame->GetBumpSound());
                }else if(mBlockType == EBlockType::EspecialBrick && !mHasSpawnedItem){
                        mHasSpawnedItem = true;
                        Mushroom* mushroom = new Mushroom(mGame, 100.0f);        
                        mGame->PlaySound(mGame->GetMushroomSound());
                        Vector2 temp = mOriginalPosition;
                        temp.y -= Game::TILE_SIZE;
                        mushroom->SetPosition(temp);
                }
        }
}
