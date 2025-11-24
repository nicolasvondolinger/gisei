#include "CSV.h"
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Goomba.h"
#include "Actors/Spawner.h"
#include "Actors/Mario.h"

// Includes de UI
#include <algorithm>
#include <vector>
#include <fstream> // Para LoadLevel
#include <sstream> // Para LoadLevel
#include <cstring> // Para memset
#include <stdexcept> // Para stoi
#include "Renderer/Renderer.h" // Incluir Renderer
#include "UI/Screens/UIScreen.h"
#include "UI/Screens/MainMenu.h"


Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mCameraPos(Vector2::Zero)
        ,mMario(nullptr)
        ,mLevelData(nullptr)
        ,mBackgroundTexture(nullptr)
        ,mBackgroundScrollSpeed(0.25f)
        ,mBackgroundMusic(nullptr)
        ,mJumpSound(nullptr)
        ,mJumpSuperSound(nullptr)
        ,mDeadSound(nullptr)
        ,mMushroomSound(nullptr)
        ,mBumpSound(nullptr)
        ,mStageClearSound(nullptr)
{

}

bool Game::Initialize() {
    Random::Init();

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return false;
    }

    // Inicializa SDL_ttf (Necessário para UI)
    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
    }
    // Não carregue a música aqui, deixe SetScene fazer isso

    mJumpSound = Mix_LoadWAV("../Assets/Sounds/Jump.wav");
    mJumpSuperSound = Mix_LoadWAV("../Assets/Sounds/JumpSuper.wav");
    mDeadSound = Mix_LoadWAV("../Assets/Sounds/Dead.wav");
    mMushroomSound = Mix_LoadWAV("../Assets/Sounds/Mushroom.wav");
    mBumpSound = Mix_LoadWAV("../Assets/Sounds/Bump.wav");
    mStageClearSound = Mix_LoadWAV("../Assets/Sounds/StageClear.wav");

    mWindow = SDL_CreateWindow("TP3: Super Mario Bros", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    // Define a cena inicial como o Menu Principal
    SetScene(GameScene::MainMenu);

    mTicksCount = SDL_GetTicks();

    return true;
}

void Game::UnloadScene()
{
    for(auto *actor : mActors) {
        actor->SetState(ActorState::Destroy);
    }
    UpdateActors(0.0f);

    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    if (mLevelData) {
        for (int i = 0; i < LEVEL_HEIGHT; ++i) {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    if (mBackgroundMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(mBackgroundMusic);
        mBackgroundMusic = nullptr;
    }
}

void Game::SetScene(GameScene nextScene)
{
    UnloadScene();
    switch (nextScene) {
        case GameScene::MainMenu:
            mBackgroundTexture = nullptr;
            new MainMenu(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;

        case GameScene::Level1:
            mBackgroundMusic = Mix_LoadMUS("../Assets/Sounds/Music.ogg");
            if (mBackgroundMusic) {
                Mix_PlayMusic(mBackgroundMusic, -1);
            }

            InitializeActors();
            break;
    }
}


void Game::InitializeActors() {
    mLevelData = LoadLevel("../Assets/Levels/level1-1.csv", LEVEL_WIDTH, LEVEL_HEIGHT);

    if(mLevelData == nullptr){
        SDL_Log("Falha ao carregar dados do nível.");
        Quit();
        return;
    }

    BuildLevel(mLevelData, LEVEL_WIDTH, LEVEL_HEIGHT);
}

int **Game::LoadLevel(const std::string& fileName, int width, int height) {

    int** levelData = new int*[height];

    for(int i = 0; i < height; i++){
        levelData[i] = new int[width];
        memset(levelData[i], 0, width * sizeof(int));
    }

    std::ifstream file(fileName);
    if(!file.is_open()){
        SDL_Log("Erro ao abrir arquivo do nível: %s", fileName.c_str());
        return nullptr;
    }

    std::string line; int i = 0;

    while(i < height && getline(file, line)){
        std::stringstream ss(line);
        std::string cell;
        int j = 0;
        while(j < width && getline(ss, cell, ',')){
            try {
                levelData[i][j] = stoi(cell);
            } catch (const std::invalid_argument& e){
                SDL_Log("Dado inválido na célula [%d][%d] em %s", i, j, fileName.c_str());
                levelData[i][j] = 0;
            }
            j++;
        }
        i++;
    }

    file.close();

    return levelData;
}

void Game::BuildLevel(int** levelData, int width, int height) {

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            int id = levelData[i][j];

            if(id == 0) continue;

            Vector2 pos;
            pos.y = (i * TILE_SIZE) + (TILE_SIZE / 2.0f);
            pos.x = (j * TILE_SIZE) + (TILE_SIZE / 2.0f);

            Block* block = nullptr;
            Spawner* spawner = nullptr;

            switch (id) {
                case 17:
                    mMario = new Mario(this);
                    mMario->SetPosition(pos);
                    break;
                case 1:
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockA.png", EBlockType::Ground);
                    break;
                case 2:
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockC.png", EBlockType::Question);
                    break;
                case 28:
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockC.png", EBlockType::EspecialBrick);
                    break;
                case 5:
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockB.png", EBlockType::Brick);
                    break;
                case 3:
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockF.png", EBlockType::Pipe);
                    break;
                case 13:
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockG.png", EBlockType::Pipe);
                    break;
                case 10:
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockH.png", EBlockType::Pipe);
                    break;
                case 7:
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockI.png", EBlockType::Pipe);
                    break;
                case 9:
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockD.png", EBlockType::Ground);
                    break;
                case 11:
                    spawner = new Spawner(this, SPAWN_DISTANCE);
                    break;
                default:
                    break;
            }
            if(block != nullptr) block->SetPosition(pos);
            if(spawner != nullptr) spawner->SetPosition(pos);
        }
    }
}

void Game::RunLoop()
{
    while (mIsRunning)
    {
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        int sleepTime = (1000 / FPS) - (SDL_GetTicks() - mTicksCount);
        if (sleepTime > 0)
        {
            SDL_Delay(sleepTime);
        }
    }
}

void Game::ProcessInput()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                Quit();
                break;
            case SDL_KEYDOWN:
                if (!mUIStack.empty()) {
                    mUIStack.back()->HandleKeyPress(event.key.keysym.sym);
                }
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    bool uiConsomeInput = !mUIStack.empty() && !mUIStack.back()->GetIsTransparent();

    if (!uiConsomeInput)
    {
        for (auto actor : mActors)
        {
            actor->ProcessInput(state);
        }
    }
}

void Game::UpdateGame(float deltaTime) {
    if (mWaitingToQuit){
        bool deathSoundFinished = (mDeathSoundChannel == -1 || Mix_Playing(mDeathSoundChannel) == 0);
        bool stageClearSoundFinished = (mStageClearSoundChannel == -1 || Mix_Playing(mStageClearSoundChannel) == 0);

        if (deathSoundFinished && stageClearSoundFinished) Quit();

        return;
    }

    UpdateActors(deltaTime);

    UpdateCamera();

    for (auto ui : mUIStack) {
        if (ui->GetState() == UIScreen::UIState::Active) {
            ui->Update(deltaTime);
        }
    }

    auto iter = mUIStack.begin();
    while (iter != mUIStack.end()) {
        if ((*iter)->GetState() == UIScreen::UIState::Closing) {
            delete *iter;
            iter = mUIStack.erase(iter);
        } else {
            ++iter;
        }
    }
}

void Game::UpdateActors(float deltaTime)
{
    mUpdatingActors = true;
    for (auto actor : mActors)
    {
        actor->Update(deltaTime);
    }
    mUpdatingActors = false;

    for (auto pending : mPendingActors)
    {
        mActors.emplace_back(pending);
    }
    mPendingActors.clear();

    std::vector<Actor*> deadActors;
    for (auto actor : mActors)
    {
        if (actor->GetState() == ActorState::Destroy)
        {
            deadActors.emplace_back(actor);
        }
    }

    for (auto actor : deadActors)
    {
        delete actor;
    }
}

void Game::UpdateCamera() {

    if(mMario == nullptr) return;

    float targetX = mMario->GetPosition().x - (WINDOW_WIDTH/2.0f);
    float maxCameraX = (LEVEL_WIDTH * TILE_SIZE) - WINDOW_WIDTH;

    float newX = std::max(mCameraPos.x, targetX);

    newX = std::max(0.0f, newX);
    newX = std::min(maxCameraX, newX);

    mCameraPos.x = newX;
    mCameraPos.y = 0.0f;
}

void Game::AddActor(Actor* actor) {
    if (mUpdatingActors) mPendingActors.emplace_back(actor);
    else mActors.emplace_back(actor);
}

void Game::RemoveActor(Actor* actor)
{
    auto iter = std::find(mPendingActors.begin(), mPendingActors.end(), actor);
    if (iter != mPendingActors.end())
    {
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        std::iter_swap(iter, mActors.end() - 1);
        mActors.pop_back();
    }
}

void Game::AddDrawable(class DrawComponent *drawable)
{
    mDrawables.emplace_back(drawable);

    std::sort(mDrawables.begin(), mDrawables.end(),[](DrawComponent* a, DrawComponent* b) {
        return a->GetDrawOrder() < b->GetDrawOrder();
    });
}

void Game::RemoveDrawable(class DrawComponent *drawable)
{
    auto iter = std::find(mDrawables.begin(), mDrawables.end(), drawable);
    if (iter != mDrawables.end())
    {
        mDrawables.erase(iter);
    }
}

void Game::AddCollider(class AABBColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    if (iter != mColliders.end())
    {
        mColliders.erase(iter);
    }
}

void Game::GenerateOutput()
{
    mRenderer->Clear();
    mRenderer->GetBaseShader()->SetActive();
    mRenderer->GetBaseShader()->SetIntegerUniform("uIsUI", 0);

    if(mBackgroundTexture){
        Vector2 parallaxCam = mCameraPos * mBackgroundScrollSpeed;
        float texWidth = static_cast<float>(mBackgroundTexture->GetWidth());
        float texHeight = static_cast<float>(mBackgroundTexture->GetHeight());
        float uOffset = parallaxCam.x / texWidth;
        float uScale = static_cast<float>(WINDOW_WIDTH) / texWidth;
        float vScale = static_cast<float>(WINDOW_HEIGHT) / texHeight;

        Vector4 texRect(uOffset, 0.0f, uScale, vScale);

        Vector2 quadPos = mCameraPos + Vector2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
        Vector2 quadSize(WINDOW_WIDTH, WINDOW_HEIGHT);

        mRenderer->DrawTexture(
            quadPos,
            quadSize,
            0.0f,
            Vector3(1.0f, 1.0f, 1.0f),
            mBackgroundTexture,
            texRect,
            mCameraPos,
            false,
            1.0f
        );
    }

    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);

        if(mIsDebugging)
        {
              for (auto comp : drawable->GetOwner()->GetComponents())
              {
                comp->DebugDraw(mRenderer);
              }
        }
    }

    mRenderer->Draw();

    mRenderer->Present();
}

void Game::Shutdown()
{
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    UnloadScene();

    while (!mActors.empty()) {
        delete mActors.back();
    }

    if (mLevelData) {
        for (int i = 0; i < LEVEL_HEIGHT; ++i) {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
    }

    if (mBackgroundMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(mBackgroundMusic);
        mBackgroundMusic = nullptr;
    }
    if (mJumpSound) Mix_FreeChunk(mJumpSound);
    if (mJumpSuperSound) Mix_FreeChunk(mJumpSuperSound);
    if (mDeadSound) Mix_FreeChunk(mDeadSound);
    if (mMushroomSound) Mix_FreeChunk(mMushroomSound);
    if (mBumpSound) Mix_FreeChunk(mBumpSound);
    if (mStageClearSound) Mix_FreeChunk(mStageClearSound);

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

// Adiciona a função PushUI que estava faltando
void Game::PushUI(UIScreen* screen)
{
    mUIStack.emplace_back(screen);
}

int Game::PlaySound(Mix_Chunk* sound) {
    int channel = -1;
    if (sound) {
        channel = Mix_PlayChannel(-1, sound, 0);
    }

    return channel;
}

Mix_Chunk* Game::GetJumpSound(){
    return mJumpSound;
}

Mix_Chunk* Game::GetJumpSuperSound(){
    return mJumpSuperSound;
}

Mix_Chunk* Game::GetDeadSound(){
    return mDeadSound;
}

Mix_Chunk* Game::GetMushroomSound(){
    return mMushroomSound;
}

Mix_Chunk* Game::GetBumpSound(){
    return mBumpSound;
}

Mix_Chunk* Game::GetStageClearSound(){
    return mStageClearSound;
}