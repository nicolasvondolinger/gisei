// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "UIScreen.h"
#include "../../Game.h"
#include "../../Renderer/Shader.h"
#include "../../Renderer/Renderer.h"
#include "../../Renderer/Font.h"

UIScreen::UIScreen(Game* game, const std::string& fontName)
	:mGame(game)
	,mPos(0.f, 0.f)
	,mSize(0.f, 0.f)
	,mState(UIState::Active)
    ,mSelectedButtonIndex(-1)
    , mIsTransparent(false)
{
    mGame->PushUI(this);

    Renderer* renderer = mGame->GetRenderer();
    mFont = renderer->GetFont(fontName);

    if (!mFont)
    {
        SDL_Log("Falha ao carregar fonte da UI: %s", fontName.c_str());
    }
}

UIScreen::~UIScreen()
{
    for (auto t : mTexts) {
        delete t;
    }
    mTexts.clear();

    for (auto b : mButtons) {
        delete b;
    }
	mButtons.clear();

    for (auto img : mImages) {
        delete img;
    }
    mImages.clear();

    for (auto rect : mRects) {
        delete rect;
    }
    mRects.clear();
}

void UIScreen::Update(float deltaTime)
{
	
}

void UIScreen::HandleKeyPress(int key)
{

}

void UIScreen::Close()
{
	mState = UIState::Closing;
}

UIText* UIScreen::AddText(const std::string& name, const Vector2& offset, float scale, float angle, const int pointSize, const int unsigned wrapLength, int drawOrder)
{
    UIText* uiText = new UIText(mGame, name, mFont, offset, scale, angle, pointSize, wrapLength, drawOrder);
    mTexts.push_back(uiText);
    return uiText;
}

UIButton* UIScreen::AddButton(const std::string& name, std::function<void()> onClick, const Vector2& offset, float scale, float angle, const int pointSize, const int unsigned wrapLength, int drawOrder)
{
    UIButton* uiButton = new UIButton(mGame, onClick, name, mFont, offset, scale, angle, pointSize, wrapLength, drawOrder);
    mButtons.push_back(uiButton);
    if (mButtons.size() == 1)
    {
        mSelectedButtonIndex = 0;
        uiButton->SetHighlighted(true);
    }
    return uiButton;
}

UIImage* UIScreen::AddImage(const std::string& imagePath, const Vector2& offset, float scale, float angle, int drawOrder)
{
    UIImage* uiImage = new UIImage(mGame, imagePath, offset, scale, angle, drawOrder);
    mImages.push_back(uiImage);
    return uiImage;
}

UIRect *UIScreen::AddRect(const Vector2 &offset, const Vector2 &size, float scale, float angle, int drawOrder)
{
    UIRect* uiRect = new UIRect(mGame, offset, size, scale, angle, drawOrder);
    mRects.push_back(uiRect);
    return uiRect;
}