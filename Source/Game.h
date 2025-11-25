#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "Math.h"


class Actor;
class Renderer;
class Ninja;
class DrawComponent;
class AABBColliderComponent;
class UIScreen;
class Texture;
class Font;

enum class GameScene
{
    MainMenu,
    Level1
};

class Game
{
public:
    Game();
    bool Initialize();
    void RunLoop();
    void Shutdown();

    void AddActor(Actor* actor);
    void RemoveActor(Actor* actor);

    void AddDrawable(class DrawComponent *drawable);
    void RemoveDrawable(class DrawComponent *drawable);
    void AddCollider(class AABBColliderComponent* collider);
    void RemoveCollider(class AABBColliderComponent* collider);

    void PushUI(UIScreen* screen);
    std::vector<UIScreen*>& GetUIStack() { return mUIStack; }
    void SetScene(GameScene nextScene);
    void UnloadScene();

    Renderer* GetRenderer() { return mRenderer; }
    const std::vector<class AABBColliderComponent*>& GetColliders() const { return mColliders; }
    Vector2 GetCameraPos() const { return mCameraPos; }


    Ninja* GetPlayer() const { return mNinja; }

    int PlaySound(Mix_Chunk* sound);
    Mix_Chunk* GetJumpSound();
    Mix_Chunk* GetJumpSuperSound();
    Mix_Chunk* GetDeadSound();
    Mix_Chunk* GetSpiritOrbSound();
    Mix_Chunk* GetBumpSound();
    Mix_Chunk* GetStageClearSound();

    void SetWaitingToQuit(bool status) { mWaitingToQuit = status; }
    void SetDeathSoundChannel(int channel) { mDeathSoundChannel = channel; }
    void SetStageClearSoundChannel(int channel) { mStageClearSoundChannel = channel; }

    void Quit() { mIsRunning = false; }

    static constexpr float WINDOW_WIDTH = 1920.0f;
    static constexpr float WINDOW_HEIGHT = 1080.0f;
    static constexpr float LEVEL_WIDTH = 1920.0f;
    static constexpr float LEVEL_HEIGHT = 1056.0f;
    static constexpr float TILE_SIZE = 32.0f;
    static constexpr float SPAWN_DISTANCE = 500.0f;
    static constexpr int FPS = 60;

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void GenerateOutput();
    void UpdateActors(float deltaTime);
    void UpdateCamera();

    void InitializeActors();
    int **LoadLevel(const std::string& fileName, int width, int height);
    void BuildLevel(int** levelData, int width, int height);

    SDL_Window* mWindow;
    Renderer* mRenderer;
    Uint32 mTicksCount;
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;
    bool mWaitingToQuit = false;

    std::vector<Actor*> mActors;
    std::vector<Actor*> mPendingActors;
    std::vector<DrawComponent*> mDrawables;
    std::vector<AABBColliderComponent*> mColliders;
    std::vector<UIScreen*> mUIStack;

    Vector2 mCameraPos;


    Ninja* mNinja;

    int** mLevelData;
    int mLevelDataWidth;
    int mLevelDataHeight;
    class Texture* mBackgroundTexture;
    float mBackgroundScrollSpeed;

    Mix_Music* mBackgroundMusic;
    Mix_Chunk* mJumpSound;
    Mix_Chunk* mJumpSuperSound;
    Mix_Chunk* mDeadSound;
    Mix_Chunk* mSpiritOrbSound;
    Mix_Chunk* mBumpSound;
    Mix_Chunk* mStageClearSound;
    
    int mDeathSoundChannel = -1;
    int mStageClearSoundChannel = -1;
};