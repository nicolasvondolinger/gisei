#include "CSV.h"
#include "Game.h"
#include "Components/Drawing/DrawComponent.h"
#include "Components/Drawing/ParallaxComponent.h"
#include "Components/Drawing/DashSmokeComponent.h"
#include "Components/Physics/RigidBodyComponent.h"
#include "Components/Physics/AABBColliderComponent.h"
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
#include "UI/Screens/PauseMenu.h"
#include "UI/Screens/GameOver.h"
#include "UI/Screens/HUD.h"

Game::Game()
        :mWindow(nullptr)
        ,mRenderer(nullptr)
        ,mTicksCount(0)
        ,mIsRunning(true)
        ,mIsPaused(false)
        ,mIsDebugging(false)
        ,mUpdatingActors(false)
        ,mCameraPos(Vector2::Zero)
        , mNinja(nullptr)
        , mCurrentScene(GameScene::MainMenu)
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
        ,mHUD(nullptr)
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


    mJumpSound = Mix_LoadWAV("../Assets/Sounds/12_Player_Movement_SFX/30_Jump_03.wav");
    mJumpSuperSound = Mix_LoadWAV("../Assets/Sounds/12_Player_Movement_SFX/52_Dive_02.wav");
    mDeadSound = Mix_LoadWAV("../Assets/Sounds/10_Battle_SFX/69_Enemy_death_01.wav");
    mSpiritOrbSound = Mix_LoadWAV("../Assets/Sounds/8_Buffs_Heals_SFX/02_Heal_02.wav");
    mBumpSound = Mix_LoadWAV("../Assets/Sounds/10_Battle_SFX/15_Impact_flesh_02.wav");
    mStageClearSound = Mix_LoadWAV("../Assets/Sounds/10_UI_Menu_SFX/013_Confirm_03.wav");

    mWindow = SDL_CreateWindow("Gisei: The Path of the Shattered Blade", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mFadeAlpha = 0.0f;
    mFadeState = FadeState::None;

    LoadScene(GameScene::MainMenu);
    
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
    mHUD = nullptr;

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
    mNextScene = nextScene;
    mFadeState = FadeState::FadingOut;
}

void Game::LoadScene(GameScene scene)
{
    mIsPaused = false;
    mCurrentScene = scene;
    UnloadScene();

    // Defina o fator de zoom que você quer (2.0f = 2x mais perto)
    float zoomFactor = 2.0f; 

    switch (scene) {
        case GameScene::MainMenu:
            // MENU: Resolução original (Sem zoom)
            mRenderer->SetView(WINDOW_WIDTH, WINDOW_HEIGHT);
            
            mBackgroundTexture = nullptr;
            new MainMenu(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;

        case GameScene::Level1:
            mRenderer->SetView(WINDOW_WIDTH / zoomFactor, WINDOW_HEIGHT / zoomFactor);

            mBackgroundMusic = Mix_LoadMUS("../Assets/Sounds/Troubadeck 20 Long Lonely Road.ogg");
            if (mBackgroundMusic) {
                Mix_PlayMusic(mBackgroundMusic, -1);
            }
            InitializeActors();
            
            // ADICIONE ISTO:
            mHUD = new HUD(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;

        case GameScene::GameOver:
            // GAME OVER: Resolução original (Sem zoom)
            mRenderer->SetView(WINDOW_WIDTH, WINDOW_HEIGHT);

            mBackgroundTexture = nullptr;
            new GameOver(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;
    }
}

void Game::InitializeActors() {
    new ParallaxBackground(this);
    
    struct Layer {
        std::string csvPath;
        std::string texturePath;
        int columns;
    };
    
    Layer layers[] = {
        {"../Assets/Levels/level1-test_Tile Layer 1.csv", "../Assets/Levels/Tileset.png", 17},
        {"../Assets/Levels/level1-test_Tile Layer 2.csv", "../Assets/Levels/Objects.png", 40},
        {"../Assets/Levels/level1-test_Tile Layer 3.csv", "../Assets/Levels/cave_entrance.png", 6}
    };
    
    for(const auto& layer : layers) {
        int width, height;
        int** data = LoadLevel(layer.csvPath, width, height);
        if(data == nullptr){
            SDL_Log("Falha ao carregar layer: %s", layer.csvPath.c_str());
            continue;
        }
        BuildLevel(data, width, height, layer.texturePath, layer.columns);
        
        for(int i = 0; i < height; i++) {
            delete[] data[i];
        }
        delete[] data;
    }
    
    mNinja = new Ninja(this);
    mNinja->SetPosition(Vector2(64.0f, 640.0f));
}

int **Game::LoadLevel(const std::string& fileName, int& width, int& height) {
    std::ifstream file(fileName);
    if(!file.is_open()){
        SDL_Log("Erro ao abrir arquivo do nível: %s", fileName.c_str());
        return nullptr;
    }

    std::vector<std::vector<int>> tempData;
    std::string line;
    width = 0;

    while(getline(file, line)){
        std::vector<int> row;
        std::stringstream ss(line);
        std::string cell;
        while(getline(ss, cell, ',')){
            try {
                row.push_back(stoi(cell));
            } catch (const std::invalid_argument& e){
                row.push_back(-1);
            }
        }
        if(row.size() > width) width = row.size();
        tempData.push_back(row);
    }
    file.close();

    height = tempData.size();
    int** levelData = new int*[height];
    for(int i = 0; i < height; i++){
        levelData[i] = new int[width];
        for(int j = 0; j < width; j++){
            levelData[i][j] = (j < tempData[i].size()) ? tempData[i][j] : -1;
        }
    }

    return levelData;
}

void Game::BuildLevel(int** levelData, int width, int height, const std::string& tilesetPath, int columns) {
    std::set<int> hazardIds = { 182, 183, 184, 185, 199, 200, 201, 202 };
    Texture* tilesetTexture = mRenderer->GetTexture(tilesetPath);
    const int TILE_SIZE_PX = 32;
    bool hasCollision = (tilesetPath.find("Tileset.png") != std::string::npos);

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            int id = levelData[i][j];
            if(id == -1) continue;

            Vector2 pos;
            pos.y = (i * TILE_SIZE) + (TILE_SIZE / 2.0f);
            pos.x = (j * TILE_SIZE) + (TILE_SIZE / 2.0f);

            if(pos.y < 96.0f && hasCollision) continue;

            SDL_Rect srcRect;
            srcRect.w = TILE_SIZE_PX;
            srcRect.h = TILE_SIZE_PX;
            srcRect.x = (id % columns) * TILE_SIZE_PX;
            srcRect.y = (id / columns) * TILE_SIZE_PX;

            EBlockType type = hazardIds.count(id) ? EBlockType::Hazard : EBlockType::Normal;
            Block* block = new Block(this, tilesetTexture, srcRect, type);
            block->SetPosition(pos);
            
            if(!hasCollision) {
                auto collider = block->GetComponent<AABBColliderComponent>();
                if(collider) collider->SetEnabled(false);
            }
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

                if (event.key.keysym.sym == SDLK_ESCAPE && mCurrentScene == GameScene::Level1 && !mIsPaused) {
                    PauseGame();
                }
                break;
        }
    }

    const Uint8* state = SDL_GetKeyboardState(nullptr);

    bool uiConsomeInput = !mUIStack.empty() && !mUIStack.back()->GetIsTransparent();

    if (!uiConsomeInput && !mIsPaused)
    {
        for (auto actor : mActors)
        {
            actor->ProcessInput(state);
        }
    }
}

void Game::UpdateGame(float deltaTime) {

    if (mFadeState == FadeState::FadingOut) {
        mFadeAlpha += 2.5f * deltaTime;
        if (mFadeAlpha >= 1.0f) {
            mFadeAlpha = 1.0f;
            LoadScene(mNextScene); 
            mFadeState = FadeState::FadingIn;
        }
    } else if (mFadeState == FadeState::FadingIn) {
        mFadeAlpha -= 2.5f * deltaTime;
        if (mFadeAlpha <= 0.0f) {
            mFadeAlpha = 0.0f;
            mFadeState = FadeState::None;
        }
    }

    if (mWaitingToQuit){
        bool deathSoundFinished = (mDeathSoundChannel == -1 || Mix_Playing(mDeathSoundChannel) == 0);
        bool stageClearSoundFinished = (mStageClearSoundChannel == -1 || Mix_Playing(mStageClearSoundChannel) == 0);

        if (deathSoundFinished && stageClearSoundFinished) Quit();

        return;
    }

    if (!mIsPaused) {
        UpdateActors(deltaTime);
        UpdateCamera();

        if (mCurrentScene == GameScene::Level1 && mNinja && mHUD) {
            mHUD->UpdateHUD(mNinja);
        }
    }

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

    // --- Configuração do Zoom ---
    // Mantenha igual ao valor usado no LoadScene
    float zoomFactor = 2.0f; 
    
    // Calcula o tamanho real da "lente" da câmera
    float logicalWidth = WINDOW_WIDTH / zoomFactor;
    float logicalHeight = WINDOW_HEIGHT / zoomFactor;

    // --- EIXO X (Horizontal) ---
    float targetX = mNinja->GetPosition().x - (logicalWidth / 2.0f);
    float maxCameraX = (LEVEL_WIDTH * TILE_SIZE) - logicalWidth;
    
    // Usa std::clamp para manter dentro dos limites (requer <algorithm>)
    // Se não tiver <algorithm>, use std::max e std::min como antes
    mCameraPos.x = std::clamp(targetX, 0.0f, maxCameraX);

    // --- EIXO Y (Vertical) ---
    // Centraliza o Ninja verticalmente
    float targetY = mNinja->GetPosition().y - (logicalHeight / 2.0f);

    // Define o limite vertical da câmera
    // Assumindo que a altura da fase é igual à altura original da janela (768px)
    float levelHeight = (float)WINDOW_HEIGHT; 
    float maxCameraY = levelHeight - logicalHeight;

    // Trava a câmera para não mostrar além do teto ou abaixo do chão
    mCameraPos.y = std::clamp(targetY, 0.0f, maxCameraY);
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

void Game::DrawFade() {
    if (mFadeState != FadeState::None) {
        mRenderer->DrawFade(mFadeAlpha);
    }
}

void Game::GenerateOutput()
{
    mRenderer->Clear();

    // ---------------------------------------------------------
    // FASE 1: DESENHO DO MUNDO (COM ZOOM)
    // ---------------------------------------------------------
    float zoomFactor = 2.0f; 

    if (mCurrentScene == GameScene::Level1) {
        // Aplica o zoom para a fase
        mRenderer->SetView(WINDOW_WIDTH / zoomFactor, WINDOW_HEIGHT / zoomFactor);
    } else {
        // Menu usa resolução normal
        mRenderer->SetView(WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    // Ativa shader para objetos do jogo (uIsUI = 0)
    mRenderer->GetBaseShader()->SetActive();
    mRenderer->GetBaseShader()->SetIntegerUniform("uIsUI", 0);

    // 1. Background (Se existir)
    if(mBackgroundTexture){
         Vector2 quadPos = mCameraPos + Vector2(WINDOW_WIDTH / (2.0f * zoomFactor), WINDOW_HEIGHT / (2.0f * zoomFactor));
         Vector2 quadSize(WINDOW_WIDTH / zoomFactor, WINDOW_HEIGHT / zoomFactor);
         
         // Lógica do Parallax simples para o fundo
         Vector2 parallaxCam = mCameraPos * mBackgroundScrollSpeed;
         float texWidth = static_cast<float>(mBackgroundTexture->GetWidth());
         float texHeight = static_cast<float>(mBackgroundTexture->GetHeight());
         float uOffset = parallaxCam.x / texWidth;
         float uScale = static_cast<float>(WINDOW_WIDTH / zoomFactor) / texWidth;
         float vScale = static_cast<float>(WINDOW_HEIGHT / zoomFactor) / texHeight;
         Vector4 texRect(uOffset, 0.0f, uScale, vScale);

         mRenderer->DrawTexture(quadPos, quadSize, 0.0f, Vector3::One, mBackgroundTexture, texRect, mCameraPos);
    }

    // 2. Atores Especiais (Parallax)
    for (auto actor : mActors) {
        auto parallax = actor->GetComponent<ParallaxComponent>();
        if (parallax && parallax->IsEnabled()) {
            parallax->Draw(mRenderer);
        }
    }

    // 3. Objetos do Jogo (Inimigos, Ninja, Blocos)
    for (auto drawable : mDrawables) {
        drawable->Draw(mRenderer);
        if(mIsDebugging) {
             for (auto comp : drawable->GetOwner()->GetComponents()) {
                comp->DebugDraw(mRenderer);
             }
        }
    }
    
    // 4. Partículas/Fumaça
    for (auto actor : mActors) {
        auto smoke = actor->GetComponent<DashSmokeComponent>();
        if (smoke && smoke->IsEnabled()) {
            smoke->Draw(mRenderer);
        }
    }
    
    // ---------------------------------------------------------
    // FASE 2: DESENHO DA UI (SEM ZOOM)
    // ---------------------------------------------------------
    
    // Reseta a visão para o tamanho original da janela (1024x768)
    mRenderer->SetView(WINDOW_WIDTH, WINDOW_HEIGHT);

    // CORREÇÃO: Apenas chamamos o Draw do renderer. 
    // Ele já sabe desenhar todos os botões/textos ativos.
    mRenderer->Draw(); 
    
    // Desenha o Fade por cima de tudo (também sem zoom)
    DrawFade();
    
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

void Game::PauseGame() {
    if (mIsPaused) return;
    mIsPaused = true;
    new PauseMenu(this, "../Assets/Fonts/Alkhemikal.ttf");
}

void Game::ResumeGame() {
    mIsPaused = false;
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
