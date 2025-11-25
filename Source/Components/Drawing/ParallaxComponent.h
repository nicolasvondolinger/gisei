#pragma once
#include "../Component.h"
#include "../../Math.h"
#include <vector>
#include <string>

struct ParallaxLayer {
    class Texture* texture;
    float scrollSpeed;
    float offsetY;
};

class ParallaxComponent : public Component {
public:
    ParallaxComponent(class Actor* owner, int updateOrder = 10);
    ~ParallaxComponent();

    void AddLayer(const std::string& texturePath, float scrollSpeed, float offsetY = 0.0f);
    void Draw(class Renderer* renderer);

private:
    std::vector<ParallaxLayer> mLayers;
};
