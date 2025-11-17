#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <string>
#include <unordered_map>
#include "Math.h"

// Declarações antecipadas (Forward declarations)
class Actor;
class Renderer;
class Mario;
class DrawComponent;
class AABBColliderComponent;
class UIScreen;
class Texture;
class Font;

// Enum para as cenas do jogo
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

    // Funções de gerenciamento de atores
    void AddActor(Actor* actor);
    void RemoveActor(Actor* actor);

    // Funções de gerenciamento de componentes
    void AddDrawable(class DrawComponent *drawable);
    void RemoveDrawable(class DrawComponent *drawable);
    void AddCollider(class AABBColliderComponent* collider);
    void RemoveCollider(class AABBColliderComponent* collider);

    // Gerenciamento de Cenas e UI
    void PushUI(UIScreen* screen);
    void SetScene(GameScene nextScene);
    void UnloadScene();

    // Getters
    Renderer* GetRenderer() { return mRenderer; }
    const std::vector<class AABBColliderComponent*>& GetColliders() const { return mColliders; }
    Vector2 GetCameraPos() const { return mCameraPos; }
    Mario* GetPlayer() const { return mMario; }

    // Funções de Som
    int PlaySound(Mix_Chunk* sound);
    Mix_Chunk* GetJumpSound();
    Mix_Chunk* GetJumpSuperSound();
    Mix_Chunk* GetDeadSound();
    Mix_Chunk* GetMushroomSound();
    Mix_Chunk* GetBumpSound();
    Mix_Chunk* GetStageClearSound();

    // Setters para estado do jogo
    void SetWaitingToQuit(bool status) { mWaitingToQuit = status; }
    void SetDeathSoundChannel(int channel) { mDeathSoundChannel = channel; }
    void SetStageClearSoundChannel(int channel) { mStageClearSoundChannel = channel; }

    // Outras funções
    void Quit() { mIsRunning = false; }

    // Constantes Estáticas do Jogo
    static constexpr float WINDOW_WIDTH = 1024.0f;
    static constexpr float WINDOW_HEIGHT = 768.0f;
    static constexpr float LEVEL_WIDTH = 3400.0f;
    static constexpr float LEVEL_HEIGHT = 768.0f;
    static constexpr float TILE_SIZE = 32.0f;
    static constexpr float SPAWN_DISTANCE = 500.0f;
    static constexpr int FPS = 60;

private:
    void ProcessInput();
    void UpdateGame(float deltaTime);
    void GenerateOutput();
    void UpdateActors(float deltaTime);
    void UpdateCamera();

    // Funções de carregamento de nível
    void InitializeActors();
    int **LoadLevel(const std::string& fileName, int width, int height);
    void BuildLevel(int** levelData, int width, int height);

    // Variáveis de estado do jogo
    SDL_Window* mWindow;
    Renderer* mRenderer;
    Uint32 mTicksCount;
    bool mIsRunning;
    bool mIsDebugging;
    bool mUpdatingActors;
    bool mWaitingToQuit = false;

    // Listas de gerenciamento
    std::vector<Actor*> mActors;
    std::vector<Actor*> mPendingActors;
    std::vector<DrawComponent*> mDrawables;
    std::vector<AABBColliderComponent*> mColliders;
    std::vector<UIScreen*> mUIStack;

    // Variáveis específicas do jogo 2D (Mario)
    Vector2 mCameraPos;
    Mario* mMario;
    int** mLevelData;

    // Texturas
    class Texture* mBackgroundTexture;
    float mBackgroundScrollSpeed;

    // Sons e Música
    Mix_Music* mBackgroundMusic;
    Mix_Chunk* mJumpSound;
    Mix_Chunk* mJumpSuperSound;
    Mix_Chunk* mDeadSound;
    Mix_Chunk* mMushroomSound;
    Mix_Chunk* mBumpSound;
    Mix_Chunk* mStageClearSound;
    
    int mDeathSoundChannel = -1;
    int mStageClearSoundChannel = -1;
};