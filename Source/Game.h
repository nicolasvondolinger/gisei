// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#pragma once
#include <SDL.h>
#include <SDL_mixer.h>
#include <bits/stdc++.h>
#include "Renderer/Renderer.h"

using namespace std;

class Game
{
public:
    Game();

    bool Initialize();
    void RunLoop();
    void Shutdown();
    void Quit() { mIsRunning = false; }

    // Actor functions
    void InitializeActors();
    void UpdateActors(float deltaTime);
    void AddActor(class Actor* actor);
    void RemoveActor(class Actor* actor);

    // Renderer
    class Renderer* GetRenderer() { return mRenderer; }

    static const int WINDOW_WIDTH   = 640;
    static const int WINDOW_HEIGHT  = 448;
    static const int LEVEL_WIDTH    = 215;
    static const int LEVEL_HEIGHT   = 15;
    static const int TILE_SIZE      = 32;
    static const int SPAWN_DISTANCE = 700;
    static const int FPS = 60;

    // Draw functions
    void AddDrawable(class DrawComponent* drawable);
    void RemoveDrawable(class DrawComponent* drawable);
    std::vector<class DrawComponent*>& GetDrawables() { return mDrawables; }

    // Collider functions
    void AddCollider(class AABBColliderComponent* collider);
    void RemoveCollider(class AABBColliderComponent* collider);
    std::vector<class AABBColliderComponent*>& GetColliders() { return mColliders; }

    // Camera functions
    Vector2& GetCameraPos() { return mCameraPos; };
    void SetCameraPos(const Vector2& position) { mCameraPos = position; };

    // Game specific
    const class Mario* GetPlayer() { return mMario; }

    int mDeathSoundChannel = -1; 
    int mStageClearSoundChannel = -1;
    bool mWaitingToQuit = false;

    Mix_Chunk* GetJumpSound();
    Mix_Chunk* GetJumpSuperSound();
    Mix_Chunk* GetDeadSound();
    Mix_Chunk* GetMushroomSound();
    Mix_Chunk* GetBumpSound();
    Mix_Chunk* GetStageClearSound();
    int PlaySound(Mix_Chunk* sound);

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void UpdateCamera();
    void GenerateOutput();

    // Level loading
    int **LoadLevel(const std::string& fileName, int width, int height);
    void BuildLevel(int** levelData, int width, int height);

    // All the actors in the game
    std::vector<class Actor*> mActors;
    std::vector<class Actor*> mPendingActors;

    // Camera
    Vector2 mCameraPos;

    // All the draw components
    std::vector<class DrawComponent*> mDrawables;

    // All the collision components
    std::vector<class AABBColliderComponent*> mColliders;

    // SDL stuff
    SDL_Window* mWindow;
    class Renderer* mRenderer;

    Mix_Music* mBackgroundMusic;
    Mix_Chunk* mJumpSound;
    Mix_Chunk* mJumpSuperSound;
    Mix_Chunk* mDeadSound;
    Mix_Chunk* mMushroomSound;
    Mix_Chunk* mBumpSound;
    Mix_Chunk* mStageClearSound;

    // Track elapsed time since game start
    Uint32 mTicksCount;

    // Track if we're updating actors right now
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;

    // Game-specific
    class Mario *mMario;
    int **mLevelData;
    
    class Texture* mBackgroundTexture;
    float mBackgroundScrollSpeed;
};
