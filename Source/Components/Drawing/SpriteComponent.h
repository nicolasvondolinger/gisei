#pragma once
#include "DrawComponent.h"
#include "../../Renderer/Texture.h"

class SpriteComponent : public DrawComponent
{
public:
    SpriteComponent(class Actor* owner, int drawOrder = 100);

    void Draw(class Renderer* renderer) override;
    void SetTexture(class Texture* texture);
    void SetColor(const Vector3& color) { mColor = color; }
    
    // Permite definir qual pedaço da imagem desenhar (Recorte)
    void SetTextureRect(const SDL_Rect& rect);

protected:
    class Texture* mTexture;
    Vector4 mTexRect; // x, y, w, h (em coordenadas normalizadas ou pixels, depende do shader)
    Vector3 mColor;
    int mTexWidth;
    int mTexHeight;
};