//
// Created by Lucas N. Ferreira on 28/09/23.
//

#pragma once

#include "DrawComponent.h"
#include <string>
#include <vector>
#include <unordered_map>

// Estrutura para guardar dados de uma animação específica
struct AnimationData {
    class Texture* texture; // A imagem desta animação
    int numFrames;          // Quantos quadros ela tem
    float frameDuration;    // Duração de cada quadro (1/FPS)
    bool loop;              // Se deve repetir
};

class AnimatorComponent : public DrawComponent {
public:
    // Construtor simplificado (não pede mais dataPath)
    AnimatorComponent(class Actor* owner, int width, int height, int drawOrder = 100);
    ~AnimatorComponent() override;

    void Draw(Renderer* renderer) override;
    void Update(float deltaTime) override;

    // Define a animação atual pelo nome
    void SetAnimation(const std::string& name);

    // Adiciona uma animação carregando o arquivo de imagem
    // ex: AddAnimation("run", "../Assets/Sprites/Samurai/Run.png", 8);
    void AddAnimation(const std::string& name, const std::string& texturePath, int numFrames, float fps = 10.0f, bool loop = true);

    // Pausar/Despausar
    void SetIsPaused(bool pause) { mIsPaused = pause; }

    // Tamanho do desenho na tela
    void SetSize(int w, int h);

    // Define FPS da animação ATUAL
    void SetAnimFPS(float fps);

private:
    // Mapa de animações: Nome -> Dados
    std::unordered_map<std::string, AnimationData> mAnimations;

    // Nome da animação atual
    std::string mCurrentAnimName;

    // Tempo decorrido na animação atual
    float mAnimTimer;

    // Se está pausado
    bool mIsPaused;

    // Tamanho na tela (destino)
    int mWidth;
    int mHeight;

    float mTextureFactor;
};