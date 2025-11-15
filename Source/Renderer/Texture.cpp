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

bool Texture::Load(const string &filePath, bool repeat) {
    SDL_Surface* surface = IMG_Load(filePath.c_str());

    if(!surface){
        cout << "Erro ao carregar arquivo de textura " << filePath.c_str() << " : " << IMG_GetError() << endl;
        return false;
    }

    mWidth = surface->w;
    mHeight = surface->h;

    GLenum format;
    if (surface->format->BytesPerPixel == 4){
        format = GL_RGBA;
    } else if (surface->format->BytesPerPixel == 3){
        format = GL_RGB;
    } else{
        SDL_Log("Formato de pixel desconhecido (nem RGB nem RGBA) para: %s", filePath.c_str());
        SDL_FreeSurface(surface);
        return false;
    }

    glGenTextures(1, &mTextureID);

    glBindTexture(GL_TEXTURE_2D, mTextureID);

    glTexImage2D(
        GL_TEXTURE_2D,       // Tipo de textura
        0,                   // Nível de Mipmap (0 é o base)
        format,              // Formato interno (como a GPU armazena)
        mWidth,              // Largura
        mHeight,             // Altura
        0,                   // Borda (deve ser 0)
        format,              // Formato dos dados de origem (da SDL_Surface)
        GL_UNSIGNED_BYTE,    // Tipo dos dados de origem
        surface->pixels      // Ponteiro para os dados da imagem na CPU
    );

    SDL_FreeSurface(surface);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (repeat){
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    return true;
}

void Texture::Unload() {
	glDeleteTextures(1, &mTextureID);
}

void Texture::SetActive(int index) const {
    glActiveTexture(GL_TEXTURE0 + index);
    
    glBindTexture(GL_TEXTURE_2D, mTextureID);
}

GLenum Texture::SDLFormatToGL(SDL_PixelFormat* fmt) {
    if (fmt->BytesPerPixel == 4) {
        return GL_RGBA;
    }
    if (fmt->BytesPerPixel == 3) {
        return GL_RGB;
    }
    return GL_RGB; // Padrão
}
