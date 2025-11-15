//
// Created by Lucas N. Ferreira on 28/09/23.
//

#include "AnimatorComponent.h"
#include "../../Actors/Actor.h"
#include "../../Game.h"
#include "../../Json.h"
#include "../../Renderer/Texture.h"


AnimatorComponent::AnimatorComponent(class Actor* owner, const std::string &texPath, const std::string &dataPath,
                                     int width, int height, int drawOrder)
        :DrawComponent(owner,  drawOrder)
        ,mAnimTimer(0.0f)
        ,mIsPaused(false)
        ,mWidth(width)
        ,mHeight(height)
        ,mTextureFactor(1.0f)
{
    mSpriteTexture = mOwner->GetGame()->GetRenderer()->GetTexture(texPath);
    
    if (!dataPath.empty())  {
        if (!LoadSpriteSheetData(dataPath)) {
            SDL_Log("Falha ao carregar dados da sprite sheet: %s", dataPath.c_str());
        }
    }
}

AnimatorComponent::~AnimatorComponent()
{
    mAnimations.clear();
    mSpriteSheetData.clear();
}

bool AnimatorComponent::LoadSpriteSheetData(const std::string& dataPath)
{
    // Load sprite sheet data and return false if it fails
    std::ifstream spriteSheetFile(dataPath);

    if (!spriteSheetFile.is_open()) {
        SDL_Log("Failed to open sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    nlohmann::json spriteSheetData = nlohmann::json::parse(spriteSheetFile);

    if (spriteSheetData.is_null()) {
        SDL_Log("Failed to parse sprite sheet data file: %s", dataPath.c_str());
        return false;
    }

    auto textureWidth = static_cast<float>(spriteSheetData["meta"]["size"]["w"].get<int>());
    auto textureHeight = static_cast<float>(spriteSheetData["meta"]["size"]["h"].get<int>());

    for(const auto& frame : spriteSheetData["frames"]) {

        int x = frame["frame"]["x"].get<int>();
        int y = frame["frame"]["y"].get<int>();
        int w = frame["frame"]["w"].get<int>();
        int h = frame["frame"]["h"].get<int>();

        mSpriteSheetData.emplace_back(static_cast<float>(x)/textureWidth, static_cast<float>(y)/textureHeight,
                                      static_cast<float>(w)/textureWidth, static_cast<float>(h)/textureHeight);
    }

    return true;
}

void AnimatorComponent::Draw(Renderer* renderer) {

    if(!mIsVisible || !mSpriteTexture) return;

    Vector2 pos = mOwner->GetPosition();
    float rot = mOwner->GetRotation();
    Vector2 size(static_cast<float>(mWidth), static_cast<float>(mHeight));
    Vector3 color(1.0f, 1.0f, 1.0f);

    Vector2 cameraPos = mOwner->GetGame()->GetCameraPos();

    bool flip = (mOwner->GetScale().x < 0.0f);

    Vector4 texRect;

    auto animIter = mAnimations.find(mAnimName);

    if(animIter != mAnimations.end() && !mSpriteSheetData.empty()){

        const vector<int> & frames = animIter->second;

        int currentFrameIndex = static_cast<int>(mAnimTimer);

        int spriteNum = frames[currentFrameIndex];

        texRect = mSpriteSheetData[spriteNum];
    } else if(!mSpriteSheetData.empty()){
        texRect = mSpriteSheetData[0];
    } else texRect = Vector4::UnitRect;
    
    renderer->DrawTexture(
        pos,          // Posição
        size,         // Tamanho
        rot,          // Rotação
        color,        // Cor (tint)
        mSpriteTexture, // Textura
        texRect,      // Retângulo UV (calculado acima)
        cameraPos,    // Posição da Câmera
        flip,         // Espelhamento (calculado acima)
        mTextureFactor 
    );

}

void AnimatorComponent::Update(float deltaTime) {

    if(mIsPaused || mAnimations.empty()) return;

    auto animIter = mAnimations.find(mAnimName);
    if(animIter == mAnimations.end()) return;

    const vector<int> & frames = animIter->second;

    if(frames.empty()) return;

    mAnimTimer += mAnimFPS * deltaTime;

    float numFrames = static_cast<float>(frames.size());

    while(mAnimTimer >= numFrames) mAnimTimer -= numFrames;
}

void AnimatorComponent::SetAnimation(const string& name) {
    if(mAnimName == name) return;
    mAnimName = name;
    Update(0.0f);
}

void AnimatorComponent::AddAnimation(const string& name, const vector<int>& spriteNums) {
    mAnimations.emplace(name, spriteNums);
}

void AnimatorComponent::SetSize(int w, int h){
    mWidth = w;
    mHeight = h;
}