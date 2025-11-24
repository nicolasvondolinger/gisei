//
// Created by Lucas N. Ferreira on 22/05/25.
//

#include "UIText.h"
#include "../Renderer/Font.h"
#include "../Renderer/Texture.h"
#include "../Renderer/Shader.h"

UIText::UIText(class Game* game, const std::string& text, class Font* font, const Vector2 &offset, float scale, float angle,
               int pointSize, const unsigned wrapLength, int drawOrder)
   :UIImage(game, offset, scale, angle, drawOrder)
   ,mFont(font)
   ,mPointSize(pointSize)
   ,mWrapLength(wrapLength)
   ,mTextColor(Color::White)
   ,mBackgroundColor(0.0f,0.0f,0.0f,1.0f)
   ,mMargin(Vector2(50.0f, 10.f))
{
    SetText(text);
}

UIText::~UIText()
{

}

void UIText::SetText(const std::string &text)
{
    if (mTexture) {
        mTexture->Unload();
        delete mTexture;
        mTexture = nullptr;
    }

    mText = text;
    // MUDANÇA 1: Renderizamos a textura sempre BRANCA Pura (1,1,1)
    // Isso permite que o Shader tinja ela de qualquer cor depois.
    mTexture = mFont->RenderText(mText, Vector3(1.0f, 1.0f, 1.0f), mPointSize, mWrapLength);
}

void UIText::SetTextColor(const Vector3 &color)
{
    mTextColor = color;
}

void UIText::Draw(class Shader* shader)
{
    if (!mTexture || !mIsVisible)
        return;

    // --- 1. Desenhar o Fundo ---
    Matrix4 scaleMat = Matrix4::CreateScale(
        (static_cast<float>(mTexture->GetWidth()) + mMargin.x) * mScale,
        (static_cast<float>(mTexture->GetHeight()) + mMargin.y) * mScale,
        1.0f
    );
    Matrix4 transMat = Matrix4::CreateTranslation(Vector3(mOffset.x, mOffset.y, 0.0f));
    Matrix4 world = scaleMat * transMat;

    shader->SetMatrixUniform("uWorldTransform", world);
    shader->SetFloatUniform("uTextureFactor", 0.0f);
    shader->SetVectorUniform("uBaseColor", mBackgroundColor);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);


    // --- 2. Desenhar o Texto ---
    scaleMat = Matrix4::CreateScale(
        static_cast<float>(mTexture->GetWidth()) * mScale,
        static_cast<float>(mTexture->GetHeight()) * mScale,
        1.0f
    );
    world = scaleMat * transMat;

    shader->SetMatrixUniform("uWorldTransform", world);
    shader->SetFloatUniform("uTextureFactor", 1.0f);
    shader->SetVectorUniform("uBaseColor", Vector4(mTextColor, 1.0f));

    // *** CORREÇÃO IMPORTANTE AQUI ***
    // Dizemos ao shader para usar a textura inteira (0,0 até 1,1)
    // Sem isso, o cálculo de coordenada de textura falha.
    shader->SetVectorUniform("uTexRect", Vector4(0.0f, 0.0f, 1.0f, 1.0f));

    mTexture->SetActive();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}