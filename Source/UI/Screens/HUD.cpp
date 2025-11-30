#include "HUD.h"
#include "../../Game.h"
#include "../../Actors/Ninja.h"

HUD::HUD(class Game* game, const std::string& fontName)
    :UIScreen(game, fontName)
{
    mIsTransparent = true; 

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
}

void HUD::UpdateHUD(Ninja* ninja) {
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
}