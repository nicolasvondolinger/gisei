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
#include "Actors/SkeletonWarrior.h"
#include "Actors/SkeletonSpearman.h"
#include "Actors/SkeletonArcher.h"
#include "Actors/KarasuTengu.h"
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
#include "UI/Screens/IntroCrawl.h"
#include "UI/Screens/HUD.h"
#include "Components/Drawing/AnimatorComponent.h"

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
        , mMapPrefix("level1")
        ,mBackgroundTexture(nullptr)
        ,mBackgroundScrollSpeed(0.25f)
        ,mBackgroundMusic(nullptr)
        ,mJumpSound(nullptr)
        ,mJumpSuperSound(nullptr)
        ,mDeadSound(nullptr)
        ,mSpiritOrbSound(nullptr)
        ,mBumpSound(nullptr)
        ,mStageClearSound(nullptr)
        ,mAudio(nullptr)
        ,mHUD(nullptr)
        ,mUIHoverSound(nullptr)
        ,mUIConfirmSound(nullptr)
        ,mUIBackSound(nullptr)
        ,mIsHitStop(false)
        ,mHitStopTimer(0.0f)
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
    mUIHoverSound = Mix_LoadWAV("../Assets/Sounds/10_UI_Menu_SFX/001_Hover_01.wav");
    mUIConfirmSound = Mix_LoadWAV("../Assets/Sounds/10_UI_Menu_SFX/013_Confirm_03.wav");
    mUIBackSound = Mix_LoadWAV("../Assets/Sounds/10_UI_Menu_SFX/029_Decline_09.wav");

    mWindow = SDL_CreateWindow("Gisei: The Path of the Shattered Blade", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    if (!mWindow) {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return false;
    }

    mRenderer = new Renderer(mWindow);
    mRenderer->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT);

    mLanguage.Load("en", "../Assets/Lang");

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
    // Pause gameplay updates during the intro so we can render a static preview of the level
    mIsPaused = (scene == GameScene::Intro);
    mCurrentScene = scene;
    // Reset any pending quit state from previous scenes
    mWaitingToQuit = false;
    mDeathSoundChannel = -1;
    mStageClearSoundChannel = -1;
    UnloadScene();

    float zoomFactor = 2.0f; 

    switch (scene) {
        case GameScene::Intro:
            mBackgroundTexture = nullptr;
            InitializeActors();
            new IntroCrawl(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;
        case GameScene::MainMenu:
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

            mHUD = new HUD(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;

        case GameScene::GameOver:
            mRenderer->SetView(WINDOW_WIDTH, WINDOW_HEIGHT);

            mBackgroundTexture = nullptr;
            new GameOver(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;
    }
}

void Game::InitializeActors() {
    new ParallaxBackground(this);
    mCameraPos = Vector2::Zero;
    mExitTiles.clear();
    std::string basePath = "../Assets/Levels/" + mMapPrefix;
    Vector2 spawnPos(200.0f, 640.0f);

    struct Layer {
        std::string csvPath;
        std::string texturePath;
        int columns;
    };

    Layer layers[] = {
        {basePath + "_Blocos.csv", "../Assets/Levels/Tileset.png", 17},
        {basePath + "_Objetos.csv", "../Assets/Levels/Objects.png", 40},
        {basePath + "_Entradas.csv", "../Assets/Levels/cave_entrance.png", 6}
    };

    for(const auto& layer : layers) {
        int width, height;
        int** data = LoadLevel(layer.csvPath, width, height);
        if(data == nullptr){
            SDL_Log("Falha ao carregar layer: %s", layer.csvPath.c_str());
            continue;
        }

        if (layer.csvPath.find("_Entradas") != std::string::npos) {
            static const std::unordered_set<int> exitIds{0,1,2,6,7,8,12,13,14,18,19,20};
            for (int i = 0; i < height; ++i) {
                for (int j = 0; j < width; ++j) {
                    if (exitIds.count(data[i][j])) {
                        Vector2 pos((j + 0.5f) * TILE_SIZE, (i + 0.5f) * TILE_SIZE);
                        mExitTiles.push_back(pos);
                    }
                }
            }
        }

        BuildLevel(data, width, height, layer.texturePath, layer.columns);

        for(int i = 0; i < height; i++) {
            delete[] data[i];
        }
        delete[] data;
    }

    mNinja = new Ninja(this);
    // Spawn fixo no canto esquerdo (64, 640) para todos os mapas, inclusive o boss
    mNinja->SetPosition(spawnPos);
    UpdateCamera();

    // Spawn boss only on boss map
    if (mMapPrefix == "boss1") {
        Vector2 bossPos(3200.0f, 640.0f);
        auto boss = new KarasuTengu(this);
        boss->SetPosition(bossPos);
    }

    const std::string enemiesPath = basePath + "_Inimigos.csv";
    std::ifstream enemiesFile(enemiesPath);
    if (enemiesFile.good()) {
        int enemiesW = 0, enemiesH = 0;
        int** enemies = LoadLevel(enemiesPath, enemiesW, enemiesH);
        if (enemies) {
            for (int i = 0; i < enemiesH; ++i) {
                for (int j = 0; j < enemiesW; ++j) {
                    int id = enemies[i][j];
                    if (id < 0) continue;

                    Vector2 pos((j + 0.5f) * TILE_SIZE, (i + 0.5f) * TILE_SIZE);
                    switch (id) {
                        case 0: { // warrior
                            auto e = new SkeletonWarrior(this, -90.0f);
                            e->SetPosition(pos);
                            e->SetScale(Vector2(-1.0f, 1.0f));
                            break;
                        }
                        case 1: { // spearman
                            auto e = new SkeletonSpearman(this, 90.0f);
                            e->SetPosition(pos);
                            break;
                        }
                        case 2: { // archer
                            auto e = new SkeletonArcher(this, 180.0f);
                            e->SetPosition(pos);
                            e->SetScale(Vector2(-1.0f, 1.0f));
                            break;
                        }
                        default:
                            break;
                    }
                }
            }

            for (int i = 0; i < enemiesH; ++i) {
                delete[] enemies[i];
            }
            delete[] enemies;
        }
    }
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
    std::set<int> thornIds = { 285, 325 };

    Texture* tilesetTexture = mRenderer->GetTexture(tilesetPath);
    const int TILE_SIZE_PX = 32;

    bool isCollisionLayer = (tilesetPath.find("Tileset.png") != std::string::npos);

    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            int id = levelData[i][j];
            if(id == -1) continue;

            Vector2 pos;
            pos.y = (i * TILE_SIZE) + (TILE_SIZE / 2.0f);
            pos.x = (j * TILE_SIZE) + (TILE_SIZE / 2.0f);

            if(pos.y < 96.0f && isCollisionLayer) continue;

            if (thornIds.count(id)) {
                std::string thornPath;
                if (id == 285) thornPath = "../Assets/Sprites/Blocks/thorns_top.png";
                else if (id == 325) thornPath = "../Assets/Sprites/Blocks/thorns_bottom.png";
                
                Texture* thornTex = mRenderer->GetTexture(thornPath);
                
                SDL_Rect thornRect;
                thornRect.x = 0; thornRect.y = 0;
                thornRect.w = thornTex->GetWidth();
                thornRect.h = thornTex->GetHeight();

                Block* block = new Block(this, thornTex, thornRect, EBlockType::Thorns);
                block->SetPosition(pos);
                
                continue;
            }

            SDL_Rect srcRect;
            srcRect.w = TILE_SIZE_PX;
            srcRect.h = TILE_SIZE_PX;
            srcRect.x = (id % columns) * TILE_SIZE_PX;
            srcRect.y = (id / columns) * TILE_SIZE_PX;

            Block* block = new Block(this, tilesetTexture, srcRect, EBlockType::Normal);
            block->SetPosition(pos);

            if(!isCollisionLayer) {
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

    if (mPendingLanguageReload) {
        RebuildUIForLanguage();
        mPendingLanguageReload = false;
    }
    
    if (mIsHitStop) {
        mHitStopTimer -= deltaTime;
        if (mHitStopTimer <= 0.0f) {
            mIsHitStop = false;
        } else return;
    }

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

    if (mCurrentScene == GameScene::Level1 && !mIsPaused && mFadeState == FadeState::None) {
        CheckExitReached();
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

    float zoomFactor = 2.0f; 

    float logicalWidth = WINDOW_WIDTH / zoomFactor;
    float logicalHeight = WINDOW_HEIGHT / zoomFactor;

    float targetX = mNinja->GetPosition().x - (logicalWidth / 2.0f);
    float maxCameraX = (LEVEL_WIDTH * TILE_SIZE) - logicalWidth;

    mCameraPos.x = std::clamp(targetX, 0.0f, maxCameraX);

    float targetY = mNinja->GetPosition().y - (logicalHeight / 2.0f);

    float levelHeight = (float)WINDOW_HEIGHT; 
    float maxCameraY = levelHeight - logicalHeight;

    mCameraPos.y = std::clamp(targetY, 0.0f, maxCameraY);
}

void Game::CheckExitReached()
{
    if (!mNinja || mExitTiles.empty()) return;

    Vector2 pos = mNinja->GetPosition();
    float halfTile = TILE_SIZE * 0.5f;

    for (const auto& tilePos : mExitTiles) {
        if (Math::Abs(pos.x - tilePos.x) <= halfTile &&
            Math::Abs(pos.y - tilePos.y) <= halfTile) {
            mMapPrefix = "boss1";
            SetScene(GameScene::Level1);
            return;
        }
    }
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

    float zoomFactor = 2.0f; 

    if (mCurrentScene == GameScene::Level1) {
        mRenderer->SetView(WINDOW_WIDTH / zoomFactor, WINDOW_HEIGHT / zoomFactor);
    } else {
        mRenderer->SetView(WINDOW_WIDTH, WINDOW_HEIGHT);
    }

    mRenderer->GetBaseShader()->SetActive();
    mRenderer->GetBaseShader()->SetIntegerUniform("uIsUI", 0);

    if(mBackgroundTexture){
         Vector2 quadPos = mCameraPos + Vector2(WINDOW_WIDTH / (2.0f * zoomFactor), WINDOW_HEIGHT / (2.0f * zoomFactor));
         Vector2 quadSize(WINDOW_WIDTH / zoomFactor, WINDOW_HEIGHT / zoomFactor);
         Vector2 parallaxCam = mCameraPos * mBackgroundScrollSpeed;
         float texWidth = static_cast<float>(mBackgroundTexture->GetWidth());
         float texHeight = static_cast<float>(mBackgroundTexture->GetHeight());
         float uOffset = parallaxCam.x / texWidth;
         float uScale = static_cast<float>(WINDOW_WIDTH / zoomFactor) / texWidth;
         float vScale = static_cast<float>(WINDOW_HEIGHT / zoomFactor) / texHeight;
         Vector4 texRect(uOffset, 0.0f, uScale, vScale);
         mRenderer->DrawTexture(quadPos, quadSize, 0.0f, Vector3::One, mBackgroundTexture, texRect, mCameraPos);
    }

    for (auto actor : mActors) {
        auto parallax = actor->GetComponent<ParallaxComponent>();
        if (parallax && parallax->IsEnabled()) parallax->Draw(mRenderer);
    }

    for (auto drawable : mDrawables) {
        drawable->Draw(mRenderer);
        if(mIsDebugging) {
             for (auto comp : drawable->GetOwner()->GetComponents()) comp->DebugDraw(mRenderer);
        }
    }
    
    for (auto actor : mActors) {
        auto smoke = actor->GetComponent<DashSmokeComponent>();
        if (smoke && smoke->IsEnabled()) smoke->Draw(mRenderer);
    }

    
    if (mIsHitStop) {
        mRenderer->DrawHitStopOverlay(0.5f);
        
        if (mNinja) {
            mRenderer->GetBaseShader()->SetActive();
            mRenderer->GetBaseShader()->SetVectorUniform("uBaseColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            mRenderer->GetBaseShader()->SetIntegerUniform("uIsUI", 0);

            mNinja->GetDrawComponent()->Draw(mRenderer);
        }
    }
    
    mRenderer->SetView(WINDOW_WIDTH, WINDOW_HEIGHT);
    mRenderer->Draw(); 
    
    DrawFade();
    
    mRenderer->Present();
}

void Game::RebuildUIForLanguage() {
    for (auto ui : mUIStack) {
        delete ui;
    }
    mUIStack.clear();

    switch (mCurrentScene) {
        case GameScene::Intro:
            new IntroCrawl(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;
        case GameScene::MainMenu:
            new MainMenu(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;
        case GameScene::Level1:
            if (mIsPaused) {
                new PauseMenu(this, "../Assets/Fonts/Alkhemikal.ttf");
            }
            break;
        case GameScene::GameOver:
            new GameOver(this, "../Assets/Fonts/Alkhemikal.ttf");
            break;
    }
}

void Game::OnLanguageChanged() {
    mPendingLanguageReload = true;
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
    if (mUIHoverSound) Mix_FreeChunk(mUIHoverSound);
    if (mUIConfirmSound) Mix_FreeChunk(mUIConfirmSound);
    if (mUIBackSound) Mix_FreeChunk(mUIBackSound);

    mRenderer->Shutdown();
    delete mRenderer;
    mRenderer = nullptr;

    mLanguage = LanguageManager();

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

void Game::StartHitStop(float duration) {
    mIsHitStop = true;
    mHitStopTimer = duration;
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
