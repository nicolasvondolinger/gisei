#pragma once

#include "DrawComponent.h"
#include <string>
#include <vector>
#include <unordered_map>

class AABBColliderComponent;


struct AnimationData {
    class Texture *texture;
    int numFrames;
    float frameDuration;
    bool loop;
};

class AnimatorComponent : public DrawComponent {
public:
    
    AnimatorComponent(class Actor* owner, int width, int height, int drawOrder = 100);
    ~AnimatorComponent() override;

    void Draw(Renderer* renderer) override;
    void Update(float deltaTime) override;

    
    void SetAnimation(const std::string& name);

    
    
    void AddAnimation(const std::string& name, const std::string& texturePath, int numFrames, float fps = 10.0f, bool loop = true);

    
    void SetIsPaused(bool pause) { mIsPaused = pause; }

    
    void SetSize(int w, int h);


    void SetAnimFPS(float fps);
    
    int GetCurrentFrame() const;
    const std::string& GetCurrentAnimation() const { return mCurrentAnimName; }

private:
    
    std::unordered_map<std::string, AnimationData> mAnimations;

    
    std::string mCurrentAnimName;


    float mAnimTimer;


    bool mIsPaused;

    
    int mWidth;
    int mHeight;

    float mTextureFactor;
};