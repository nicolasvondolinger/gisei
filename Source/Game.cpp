#include "CSV.h"
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Drawing/ParallaxComponent.h"
#include "Components/Drawing/DashSmokeComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Random.h"
#include "Actors/Actor.h"
#include "Actors/Block.h"
#include "Actors/Spawner.h"
#include "Actors/Ninja.h"
#include "Actors/ParallaxBackground.h"
#include <algorithm>
#include <vector>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include "Renderer/Renderer.h"
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
        , mNinja(nullptr)
        , mLevelData(nullptr)
        , mLevelDataWidth(0)
        , mLevelDataHeight(0)
        ,mBackgroundTexture(nullptr)
        ,mBackgroundScrollSpeed(0.25f)
        ,mBackgroundMusic(nullptr)
        ,mJumpSound(nullptr)
        ,mJumpSuperSound(nullptr)
        ,mDeadSound(nullptr)
        ,mSpiritOrbSound(nullptr)
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


    if (TTF_Init() != 0)
    {
        SDL_Log("Failed to initialize SDL_ttf");
        return false;
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        SDL_Log("SDL_mixer could not initialize! SDL_mixer Error: %s", Mix_GetError());
    }


    mJumpSound = Mix_LoadWAV("../Assets/Sounds/Jump.wav");
    mJumpSuperSound = Mix_LoadWAV("../Assets/Sounds/JumpSuper.wav");
    mDeadSound = Mix_LoadWAV("../Assets/Sounds/Dead.wav");
    mSpiritOrbSound = Mix_LoadWAV("../Assets/Sounds/SpiritOrb.wav");
    mBumpSound = Mix_LoadWAV("../Assets/Sounds/Bump.wav");
    mStageClearSound = Mix_LoadWAV("../Assets/Sounds/StageClear.wav");

    mWindow = SDL_CreateWindow("Gisei: The Path of the Shattered Blade", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);


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
        for (int i = 0; i < mLevelDataHeight; ++i) {
            delete[] mLevelData[i];
        }
        delete[] mLevelData;
        mLevelData = nullptr;
        mLevelDataWidth = 0;
        mLevelDataHeight = 0;
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
    new ParallaxBackground(this);
    
    const int LEVEL_TILES_WIDTH = 60;
    const int LEVEL_TILES_HEIGHT = 34;
    
    mLevelData = LoadLevel("../Assets/Levels/level1.csv", LEVEL_TILES_WIDTH, LEVEL_TILES_HEIGHT);
    if(mLevelData == nullptr){
        SDL_Log("Falha ao carregar dados do nível.");
        Quit();
        return;
    }
    mLevelDataWidth = LEVEL_TILES_WIDTH;
    mLevelDataHeight = LEVEL_TILES_HEIGHT;
    BuildLevel(mLevelData, LEVEL_TILES_WIDTH, LEVEL_TILES_HEIGHT);
    
    mNinja = new Ninja(this);
    mNinja->SetPosition(Vector2(100.0f, 500.0f));
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

    // 1. Defina os IDs que dão dano (Espinhos)
    std::set<int> hazardIds = { 182, 183, 184, 185, 199, 200, 201, 202 };

    // 2. Carregue o Tileset Completo (Atlas)
    // Certifique-se que o caminho está correto
    Texture* tilesetTexture = mRenderer->GetTexture("../Assets/Levels/Tileset.png");

    // 3. Configurações da Imagem do Tileset
    const int TILESET_COLUMNS = 17; // <--- MUDE ISSO para o numero real de colunas da sua imagem png
    const int TILE_SIZE_PX = 32;    // Tamanho do tile na imagem

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            int id = levelData[i][j];

            if(id == -1) continue; // Empty space

            // Calcula a posição no mundo
            Vector2 pos;
            pos.y = (i * TILE_SIZE) + (TILE_SIZE / 2.0f);
            pos.x = (j * TILE_SIZE) + (TILE_SIZE / 2.0f);

            // --- Lógica do Bloco com Atlas ---

            // Use ID directly as index
            int index = id;

            // Calcula o recorte (srcRect)
            SDL_Rect srcRect;
            srcRect.w = TILE_SIZE_PX;
            srcRect.h = TILE_SIZE_PX;
            srcRect.x = (index % TILESET_COLUMNS) * TILE_SIZE_PX;
            srcRect.y = (index / TILESET_COLUMNS) * TILE_SIZE_PX;

            // Define o tipo baseado na lista de perigos
            EBlockType type = EBlockType::Normal;
            if (hazardIds.count(id)) {
                type = EBlockType::Hazard;
            }

            // Cria o bloco
            Block* block = new Block(this, tilesetTexture, srcRect, type);
            block->SetPosition(pos);
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
                if (!mUIStack.empty() && mUIStack.back()->GetState() == UIScreen::UIState::Active) {
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
    if(mNinja == nullptr) return;


    float targetX = mNinja->GetPosition().x - (WINDOW_WIDTH/2.0f);
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

    for (auto actor : mActors)
    {
        auto parallax = actor->GetComponent<ParallaxComponent>();
        if (parallax && parallax->IsEnabled()) {
            parallax->Draw(mRenderer);
        }
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
    
    for (auto actor : mActors)
    {
        auto smoke = actor->GetComponent<DashSmokeComponent>();
        if (smoke && smoke->IsEnabled()) {
            smoke->Draw(mRenderer);
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

    if (mLevelData && mLevelDataHeight > 0) {
        for (int i = 0; i < mLevelDataHeight; ++i) {
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
    if (mSpiritOrbSound) Mix_FreeChunk(mSpiritOrbSound);
    if (mBumpSound) Mix_FreeChunk(mBumpSound);
    if (mStageClearSound) Mix_FreeChunk(mStageClearSound);

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}


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

Mix_Chunk* Game::GetSpiritOrbSound(){
    return mSpiritOrbSound;
}

Mix_Chunk* Game::GetBumpSound(){
    return mBumpSound;
}

Mix_Chunk* Game::GetStageClearSound(){
    return mStageClearSound;
}