#include "Block.h"
#include "../Game.h"
#include "../Components/Physics/AABBColliderComponent.h"
#include "../Components/Drawing/SpriteComponent.h" 

Block::Block(Game* game, Texture* texture, SDL_Rect srcRect, EBlockType type)
    :Actor(game)
    ,mType(type)
{
    SpriteComponent* sc = new SpriteComponent(this);
    sc->SetTexture(texture);
    sc->SetTextureRect(srcRect); 

    new AABBColliderComponent(this, 0, 0, 32, 32, ColliderLayer::Blocks);
}

void Block::OnUpdate(float deltaTime) {
    // Blocos estáticos geralmente não precisam de update, a menos que sejam animados
}

void Block::OnCollision(Actor* other) {
    // O bloco é passivo, a lógica de dano fica no Ninja (quem colide)
}