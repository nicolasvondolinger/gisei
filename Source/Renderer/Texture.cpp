#include "Texture.h"

Texture::Texture()
: mTextureID(0)
, mWidth(0)
, mHeight(0)
{
}

Texture::~Texture()
{
}

// CORREÇÃO AQUI: Adicionado 'bool repeat'
bool Texture::Load(const std::string &filePath, bool repeat)
{
    SDL_Surface* surface = IMG_Load(filePath.c_str());

    if (!surface) {
        SDL_Log("Failed to load image %s: %s", filePath.c_str(), IMG_GetError());
        return false;
    }

    // Converte para RGBA para garantir compatibilidade
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    if (!formattedSurface) {
        SDL_Log("Failed to convert surface %s: %s", filePath.c_str(), SDL_GetError());
        SDL_FreeSurface(surface);
        return false;
    }

    mWidth = formattedSurface->w;
    mHeight = formattedSurface->h;

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // Resetar alinhamento para carregamento de imagens padrão
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);

    SDL_FreeSurface(surface);
    SDL_FreeSurface(formattedSurface);

    // Filtros de Pixel Art (GL_NEAREST)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Lógica de Repetição
    if (repeat) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Geralmente só repetimos no X em jogos side-scroller
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    return true;
}

bool Texture::CreateFromSurface(SDL_Surface* surface)
{
    if (!surface) {
        SDL_Log("A superfície (SDL_Surface) está nula em CreateFromSurface");
        return false;
    }

    mWidth = surface->w;
    mHeight = surface->h;

    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
    if (!formattedSurface) {
        SDL_Log("Falha ao converter surface: %s", SDL_GetError());
        return false;
    }

    glGenTextures(1, &mTextureID);
    glBindTexture(GL_TEXTURE_2D, mTextureID);

    // Alinhamento de pixels (Essencial para texto!)
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, formattedSurface->pixels);

    SDL_FreeSurface(formattedSurface);

    // Filtros
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Fontes não repetem
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    return true;
}

void Texture::Unload() {
    glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive(int index) const {
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}