#include "Spawner.h"
#include "../Game.h"
#include "Ninja.h"
#include "Yokai.h"

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
                Yokai* yokai = new Yokai(mGame, -100.0f);
                yokai->SetPosition(spawnPos);
                mState = ActorState::Destroy;
        }
}