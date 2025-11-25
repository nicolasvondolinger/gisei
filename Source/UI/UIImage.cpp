#include "UIImage.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"
#include "../Renderer/Renderer.h"
#include "../Actors/Actor.h"
#include "../Game.h"


UIImage::UIImage(class Game *game, const Vector2 &offset, const float scale, const float angle, int drawOrder)
    : UIElement(game, offset, scale, angle, drawOrder)
      , mTexture(nullptr) {
}

UIImage::UIImage(class Game *game, const std::string &imagePath, const Vector2 &offset, const float scale,
                 const float angle, int drawOrder)
    : UIElement(game, offset, scale, angle, drawOrder) {
    mTexture = GetGame()->GetRenderer()->GetTexture(imagePath);
}

UIImage::~UIImage() {
}

void UIImage::Draw(class Shader *shader) {
    if (!mTexture || !mIsVisible)
        return;


    Matrix4 scaleMat = Matrix4::CreateScale(static_cast<float>(mTexture->GetWidth()) * mScale,
                                            static_cast<float>(mTexture->GetHeight()) * mScale, 1.0f);

    Matrix4 rotMat = Matrix4::CreateRotationZ(mAngle);


    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mOffset.x, mOffset.y, 0.0f));


    Matrix4 world = scaleMat * rotMat * transMat;
    shader->SetMatrixUniform("uWorldTransform", world);


    shader->SetVectorUniform("uBaseColor", Vector4(1.0f, 1.0f, 1.0f, 1.0f));


    shader->SetFloatUniform("uTextureFactor", 1.0f);

    mTexture->SetActive();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}