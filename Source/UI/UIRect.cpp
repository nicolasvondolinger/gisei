#include "UIRect.h"

UIRect::UIRect(class Game *game, const Vector2 &offset, const Vector2 &size, const float scale, float angle,
               int drawOrder)
    : UIElement(game, offset, scale, angle, drawOrder)
      , mSize(size)
      , mColor(Vector4(0.0f, 0.0f, 0.0f, 1.0f)) {
}

UIRect::~UIRect() {
}

void UIRect::Draw(class Shader *shader) {
    if (!mIsVisible) return;

    Matrix4 scaleMat = Matrix4::CreateScale(mSize.x * mScale, mSize.y * mScale, 1.0f);

    Matrix4 rotMat = Matrix4::CreateRotationZ(mAngle);


    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mOffset.x, mOffset.y, 0.0f));


    Matrix4 world = scaleMat * rotMat * transMat;
    shader->SetMatrixUniform("uWorldTransform", world);


    shader->SetFloatUniform("uTextureFactor", 0.0f);
    shader->SetVectorUniform("uBaseColor", mColor);


    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
