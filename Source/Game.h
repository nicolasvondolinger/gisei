#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "Math.h"
#include "Renderer/Renderer.h"
#include "AudioSystem.h"
#include "LanguageManager.h"


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
    Intro,
    MainMenu,
    Level1, 
    GameOver
};

enum class FadeState
{
    None,
    FadingOut, 
    FadingIn   
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

    AudioSystem* GetAudio() { return mAudio; }

    void SetScene(GameScene scene);
    void LoadScene(GameScene scene);
    void UnloadScene();
    void DrawFade();
    void OnLanguageChanged();

    Renderer* GetRenderer() { return mRenderer; }
    LanguageManager& GetLanguage() { return mLanguage; }
    const LanguageManager& GetLanguage() const { return mLanguage; }
    const std::vector<class AABBColliderComponent*>& GetColliders() const { return mColliders; }
    Vector2 GetCameraPos() const { return mCameraPos; }
    bool IsPaused() const { return mIsPaused; }
    GameScene GetCurrentScene() const { return mCurrentScene; }


    Ninja* GetPlayer() const { return mNinja; }

    void StartHitStop(float duration);

    int PlaySound(Mix_Chunk* sound);
    Mix_Chunk* GetJumpSound();
    Mix_Chunk* GetJumpSuperSound();
    Mix_Chunk* GetDeadSound();
    Mix_Chunk* GetSpiritOrbSound();
    Mix_Chunk* GetBumpSound();
    Mix_Chunk* GetStageClearSound();
    Mix_Chunk* GetUIHoverSound() { return mUIHoverSound; }
    Mix_Chunk* GetUIConfirmSound() { return mUIConfirmSound; }
    Mix_Chunk* GetUIBackSound() { return mUIBackSound; }
    class HUD* GetHUD() { return mHUD; }

    void SetWaitingToQuit(bool status) { mWaitingToQuit = status; }
    void SetDeathSoundChannel(int channel) { mDeathSoundChannel = channel; }
    void SetStageClearSoundChannel(int channel) { mStageClearSoundChannel = channel; }
    void PauseGame();
    void ResumeGame();

    void Quit() { mIsRunning = false; }

    static constexpr float WINDOW_WIDTH = 1920.0f;
    static constexpr float WINDOW_HEIGHT = 1080.0f;
    static constexpr float LEVEL_WIDTH = 3840.0f;
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
    void CheckExitReached();
    class HUD* mHUD;

    void InitializeActors();
    int **LoadLevel(const std::string& fileName, int& width, int& height);
    void BuildLevel(int** levelData, int width, int height, const std::string& tilesetPath, int columns);
    void RebuildUIForLanguage();

    SDL_Window* mWindow;
    Renderer* mRenderer;
    Uint32 mTicksCount;
    bool mIsRunning;
    bool mIsPaused;
    bool mIsDebugging;
    bool mUpdatingActors;
    bool mWaitingToQuit = false;
    GameScene mCurrentScene;
    bool mPendingLanguageReload = false;

    std::vector<Actor*> mActors;
    std::vector<Actor*> mPendingActors;
    std::vector<DrawComponent*> mDrawables;
    std::vector<AABBColliderComponent*> mColliders;
    std::vector<UIScreen*> mUIStack;

    Vector2 mCameraPos;
    std::vector<Vector2> mExitTiles;
    std::string mMapPrefix;

    FadeState mFadeState;
    float mFadeAlpha;
    GameScene mNextScene;

    // Audio system
    AudioSystem* mAudio;
    LanguageManager mLanguage;

    Ninja* mNinja;

    bool mIsHitStop;
    float mHitStopTimer;

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
    Mix_Chunk* mUIHoverSound;
    Mix_Chunk* mUIConfirmSound;
    Mix_Chunk* mUIBackSound;
    
    int mDeathSoundChannel = -1;
    int mStageClearSoundChannel = -1;
};
