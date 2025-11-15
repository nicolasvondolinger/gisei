// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

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

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
    } else {
        mBackgroundMusic = Mix_LoadMUS("../Assets/Sounds/Music.ogg");
        if (mBackgroundMusic == nullptr) {
            SDL_Log("Failed to load background music! SDL_mixer Error: %s", Mix_GetError());
        } else {
            if (Mix_PlayMusic(mBackgroundMusic, -1) == -1) {
                SDL_Log("Failed to play background music! SDL_mixer Error: %s", Mix_GetError());
            }
        }
    }

    mJumpSound = Mix_LoadWAV("../Assets/Sounds/Jump.wav");
    if (mJumpSound == nullptr) {
        SDL_Log("Failed to load jump sound effect! SDL_mixer Error: %s", Mix_GetError());
    }

    mJumpSuperSound = Mix_LoadWAV("../Assets/Sounds/JumpSuper.wav");
    if (mJumpSuperSound == nullptr) {
        SDL_Log("Failed to load jump super sound effect! SDL_mixer Error: %s", Mix_GetError());
    }

    mDeadSound = Mix_LoadWAV("../Assets/Sounds/Dead.wav");
    if (mDeadSound == nullptr) {
        SDL_Log("Failed to load dead sound effect! SDL_mixer Error: %s", Mix_GetError());
    }

    mMushroomSound = Mix_LoadWAV("../Assets/Sounds/Mushroom.wav");
    if (mMushroomSound == nullptr) {
        SDL_Log("Failed to load mushroom sound effect! SDL_mixer Error: %s", Mix_GetError());
    }

    mBumpSound = Mix_LoadWAV("../Assets/Sounds/Bump.wav");
    if (mBumpSound == nullptr) {
        SDL_Log("Failed to load bump sound effect! SDL_mixer Error: %s", Mix_GetError());
    }

    mStageClearSound = Mix_LoadWAV("../Assets/Sounds/StageClear.wav");
    if (mStageClearSound == nullptr) {
        SDL_Log("Failed to load stage clear sound effect! SDL_mixer Error: %s", Mix_GetError());
    }

    mWindow = SDL_CreateWindow("TP3: Super Mario Bros", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);
    mBackgroundTexture = mRenderer->GetTexture("../Assets/Sprites/Background.png", true);

    // Init all game actors
    InitializeActors();

    mTicksCount = SDL_GetTicks();

    return true;
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
    
    ifstream file(fileName);
    if(!file.is_open()){
        SDL_Log("Erro ao abrir arquivo do nível: %s", fileName.c_str());
        return nullptr;
    }

    string line; int i = 0;

    while(i < height && getline(file, line)){
        stringstream ss(line);
        string cell;
        int j = 0;
        while(j < width && getline(ss, cell, ',')){
            try {
                levelData[i][j] = stoi(cell);
            } catch (const invalid_argument& e){
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
                case 3: // Topo Esquerdo
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockF.png", EBlockType::Pipe);
                    break;
                case 13: // Topo Direito
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockG.png", EBlockType::Pipe);
                    break;
                case 10: // Baixo Esquerdo
                    block = new Block(this, "../Assets/Sprites/Blocks/BlockH.png", EBlockType::Pipe);
                    break;
                case 7: // Baixo Direito
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
        // Calculate delta time in seconds
        float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
        if (deltaTime > 0.05f)
        {
            deltaTime = 0.05f;
        }

        mTicksCount = SDL_GetTicks();

        ProcessInput();
        UpdateGame(deltaTime);
        GenerateOutput();

        // Sleep to maintain frame rate
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
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    for (auto actor : mActors)
    {
        actor->ProcessInput(state);
    }
}

void Game::UpdateGame(float deltaTime) {
    if (mWaitingToQuit){
        bool deathSoundFinished = (mDeathSoundChannel == -1 || Mix_Playing(mDeathSoundChannel) == 0);
        bool stageClearSoundFinished = (mStageClearSoundChannel == -1 || Mix_Playing(mStageClearSoundChannel) == 0);

        if (deathSoundFinished && stageClearSoundFinished) Quit();

        return;
    }
    
    // Update all actors and pending actors
    UpdateActors(deltaTime);

    // Update camera position
    UpdateCamera();
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

    float newX = max(mCameraPos.x, targetX);

    newX = max(0.0f, newX);
    newX = min(maxCameraX, newX);

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
        // Swap to end of vector and pop off (avoid erase copies)
        std::iter_swap(iter, mPendingActors.end() - 1);
        mPendingActors.pop_back();
    }

    iter = std::find(mActors.begin(), mActors.end(), actor);
    if (iter != mActors.end())
    {
        // Swap to end of vector and pop off (avoid erase copies)
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
    mDrawables.erase(iter);
}

void Game::AddCollider(class AABBColliderComponent* collider)
{
    mColliders.emplace_back(collider);
}

void Game::RemoveCollider(AABBColliderComponent* collider)
{
    auto iter = std::find(mColliders.begin(), mColliders.end(), collider);
    mColliders.erase(iter);
}

void Game::GenerateOutput()
{
    // Clear back buffer
    mRenderer->Clear();

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
            quadPos,          // Posição (centro da tela)
            quadSize,         // Tamanho (tamanho da tela)
            0.0f,             // Rotação
            Vector3(1.0f, 1.0f, 1.0f), // Cor
            mBackgroundTexture,
            texRect,          // Nossas UVs calculadas
            mCameraPos,       // A CÂMERA REAL (para a Posição)
            false,
            1.0f
        );
    }

    for (auto drawable : mDrawables)
    {
        drawable->Draw(mRenderer);

        if(mIsDebugging)
        {
           // Call draw for actor components
              for (auto comp : drawable->GetOwner()->GetComponents())
              {
                comp->DebugDraw(mRenderer);
              }
        }
    }

    // Swap front buffer and back buffer
    mRenderer->Present();
}

void Game::Shutdown()
{
    while (!mActors.empty()) {
        delete mActors.back();
    }

    // Delete level data
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

    if (mJumpSound) {
        Mix_FreeChunk(mJumpSound);
        mJumpSound = nullptr;
    }

    if(mJumpSuperSound){
        Mix_FreeChunk(mJumpSuperSound);
        mJumpSuperSound = nullptr; 
    }

    if(mDeadSound){
        Mix_FreeChunk(mDeadSound);
        mDeadSound = nullptr;
    }

    if(mMushroomSound){
        Mix_FreeChunk(mMushroomSound);
        mMushroomSound = nullptr;
    }

    if(mBumpSound){
        Mix_FreeChunk(mBumpSound);
        mBumpSound = nullptr;
    }

    if(mStageClearSound){
        Mix_FreeChunk(mStageClearSound);
        mStageClearSound = nullptr;
    }

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
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