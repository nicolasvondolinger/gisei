//
// Created by Lucas N. Ferreira on 30/09/23.
//

#include "Spawner.h"
#include "../Game.h"
#include "Mario.h"
#include "Goomba.h"

Spawner::Spawner(Game* game, float spawnDistance)
        :Actor(game)
        ,mSpawnDistance(spawnDistance)
{

}

void Spawner::OnUpdate(float deltaTime) { 
        const Actor* player = mGame->GetPlayer();
        if(player == nullptr) return;
        Vector2 playerPos = player->GetPosition();
        Vector2 spawnPos = mPosition;

        float dist = abs(spawnPos.x - playerPos.x);

        if(dist <= mSpawnDistance){
                Goomba* goomba = new Goomba(mGame, -100.0f);
                goomba->SetPosition(spawnPos);
                mState = ActorState::Destroy;
        }
}