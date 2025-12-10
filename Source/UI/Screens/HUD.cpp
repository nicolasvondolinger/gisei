#include "HUD.h"
#include "../../Game.h"
#include "../../Actors/Ninja.h"
#include "../../Actors/BossEnemy.h"
#include <algorithm>

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    mIsTransparent = true; 

    mBossBarCenterX = Game::WINDOW_WIDTH / 2.0f;
    mBossBarLeft = mBossBarCenterX - (mBossBarWidth / 2.0f);

    // --- POSICIONAMENTO ---
    // Como o retângulo é desenhado pelo centro, a posição X não é a borda esquerda.
    // X = MargemEsquerda + MetadeDaLargura
    
    float healthCenterX = mLeftMargin + (mHealthMaxWidth / 2.0f);
    float energyCenterX = mLeftMargin + (mEnergyMaxWidth / 2.0f);

    // --- BARRA DE VIDA (Vermelha) ---
    // Fundo
    mHealthBarBG = AddRect(Vector2(healthCenterX, 30.0f), Vector2(mHealthMaxWidth, mBarHeight));
    mHealthBarBG->SetColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f)); 

    // Frente
    mHealthBarFG = AddRect(Vector2(healthCenterX, 30.0f), Vector2(mHealthMaxWidth, mBarHeight));
    mHealthBarFG->SetColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f)); 

    // --- BARRA DE ENERGIA (Azul) ---
    // Fundo (Note que usamos energyCenterX, que é diferente da vida pois a barra é menor)
    mEnergyBarBG = AddRect(Vector2(energyCenterX, 60.0f), Vector2(mEnergyMaxWidth, mBarHeight));
    mEnergyBarBG->SetColor(Vector4(0.1f, 0.1f, 0.1f, 1.0f)); 

    // Frente
    mEnergyBarFG = AddRect(Vector2(energyCenterX, 60.0f), Vector2(mEnergyMaxWidth, mBarHeight));
    mEnergyBarFG->SetColor(Vector4(0.0f, 0.5f, 1.0f, 1.0f)); 

    // --- BARRA DO BOSS (Centro da tela) ---
    mBossBarBG = AddRect(Vector2(mBossBarCenterX, mBossBarY), Vector2(mBossBarWidth, mBossBarHeight));
    mBossBarBG->SetColor(Vector4(0.08f, 0.08f, 0.08f, 0.9f));
    mBossBarFG = AddRect(Vector2(mBossBarCenterX, mBossBarY), Vector2(mBossBarWidth, mBossBarHeight));
    mBossBarFG->SetColor(Vector4(0.75f, 0.12f, 0.12f, 1.0f));

    mBossNameText = AddText("", Vector2(mBossBarCenterX, mBossNameY), 1.0f, 0.0f, 32);
    mBossNameText->SetTextColor(Vector3(0.95f, 0.86f, 0.70f));
    mBossNameText->SetBackgroundColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));

    // Oculta a barra do boss por padrão até existir um.
    mBossBarBG->SetIsVisible(false);
    mBossBarFG->SetIsVisible(false);
    mBossNameText->SetIsVisible(false);
}

void HUD::UpdateHUD(Ninja* ninja, BossEnemy* boss) {
    if (!ninja) return;

    // ---------------- VIDA ----------------
    float healthPercent = static_cast<float>(ninja->GetHealth()) / ninja->GetMaxHealth();
    if (healthPercent < 0.0f) healthPercent = 0.0f;
    
    float currentHealthWidth = mHealthMaxWidth * healthPercent;
    
    // Atualiza Tamanho
    mHealthBarFG->SetSize(Vector2(currentHealthWidth, mBarHeight));
    
    // TRUQUE DE ANCORAGEM: Recalcula a posição para manter a borda esquerda fixa
    // NovoCentro = MargemFixa + (NovaLargura / 2)
    float newHealthX = mLeftMargin + (currentHealthWidth / 2.0f);
    mHealthBarFG->SetPosition(Vector2(newHealthX, 30.0f));


    // ---------------- ENERGIA ----------------
    float cooldown = ninja->GetDashCooldown();
    float maxTime = ninja->GetDashCooldownTime();
    
    float energyPercent = 1.0f - (cooldown / maxTime);
    if (energyPercent > 1.0f) energyPercent = 1.0f;
    if (energyPercent < 0.0f) energyPercent = 0.0f;

    float currentEnergyWidth = mEnergyMaxWidth * energyPercent;

    // Atualiza Tamanho
    mEnergyBarFG->SetSize(Vector2(currentEnergyWidth, mBarHeight));

    // TRUQUE DE ANCORAGEM
    float newEnergyX = mLeftMargin + (currentEnergyWidth / 2.0f);
    mEnergyBarFG->SetPosition(Vector2(newEnergyX, 60.0f));


    // ---------------- BOSS ----------------
    bool bossActive = boss != nullptr;
    if (!bossActive) {
        mBossBarBG->SetIsVisible(false);
        mBossBarFG->SetIsVisible(false);
        mBossNameText->SetIsVisible(false);
        return;
    }

    mBossBarBG->SetIsVisible(true);
    mBossBarFG->SetIsVisible(true);
    mBossNameText->SetIsVisible(true);
    const std::string& bossName = boss->GetName();
    if (mCurrentBossName != bossName) {
        mCurrentBossName = bossName;
        mBossNameText->SetText(mCurrentBossName);
    }

    float bossPercent = static_cast<float>(boss->GetHealth()) / static_cast<float>(boss->GetMaxHealth());
    bossPercent = std::clamp(bossPercent, 0.0f, 1.0f);
    float currentBossWidth = mBossBarWidth * bossPercent;
    mBossBarFG->SetSize(Vector2(currentBossWidth, mBossBarHeight));
    float bossX = mBossBarLeft + (currentBossWidth / 2.0f);
    mBossBarFG->SetPosition(Vector2(bossX, mBossBarY));
}
