#include <SDL2/SDL.h>
#include "Shader.h"
#include <fstream>
#include <sstream>

Shader::Shader()
: mVertexShader(0)
, mFragShader(0)
, mShaderProgram(0)
{
}

Shader::~Shader()
{
}

bool Shader::Load(const std::string& name)
{
    // Compila vertex e pixel shaders
    if (!CompileShader(name + ".vert", GL_VERTEX_SHADER, mVertexShader) ||
       !CompileShader(name + ".frag", GL_FRAGMENT_SHADER, mFragShader))
    {
       return false;
    }

    // Cria o programa de shader
    mShaderProgram = glCreateProgram();
    glAttachShader(mShaderProgram, mVertexShader);
    glAttachShader(mShaderProgram, mFragShader);
    glLinkProgram(mShaderProgram);

    // Verifica se o link foi bem-sucedido
    return IsValidProgram();
}

void Shader::Unload()
{
    // Deleta programa e shaders
    glDeleteProgram(mShaderProgram);
    glDeleteShader(mVertexShader);
    glDeleteShader(mFragShader);

    mShaderProgram = 0;
    mVertexShader = 0;
    mFragShader = 0;
}

void Shader::SetActive() const
{
    // Define este programa como o ativo
    glUseProgram(mShaderProgram);
}

void Shader::SetVectorUniform(const char* name, const Vector2& vector) const
{
    GLint loc = glGetUniformLocation(mShaderProgram, name);
    glUniform2fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetVectorUniform(const char* name, const Vector3& vector) const
{
    GLint loc = glGetUniformLocation(mShaderProgram, name);
    glUniform3fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetVectorUniform(const char* name, const Vector4& vector) const
{
    GLint loc = glGetUniformLocation(mShaderProgram, name);
    glUniform4fv(loc, 1, vector.GetAsFloatPtr());
}

void Shader::SetMatrixUniform(const char* name, const Matrix4& matrix) const
{
    GLuint loc = glGetUniformLocation(mShaderProgram, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, matrix.GetAsFloatPtr());
}

void Shader::SetFloatUniform(const char *name, float value) const
{
    GLuint loc = glGetUniformLocation(mShaderProgram, name);
    glUniform1f(loc, value);
}

// Esta função é da sua v1, usada para setar o uTexture
void Shader::SetIntegerUniform(const char *name, int value) const {
    GLuint loc = glGetUniformLocation(mShaderProgram, name);
    glUniform1i(loc, value);
}

bool Shader::CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader)
{
    // Abre o arquivo
    std::ifstream shaderFile(fileName);
    if (shaderFile.is_open())
    {
       // Lê o conteúdo para uma string
       std::stringstream sstream;
       sstream << shaderFile.rdbuf();
       std::string contents = sstream.str();
       const char* contentsChar = contents.c_str();

       // Cria o shader
       outShader = glCreateShader(shaderType);

       // Define o código-fonte e compila
       glShaderSource(outShader, 1, &(contentsChar), nullptr);
       glCompileShader(outShader);

       if (!IsCompiled(outShader))
       {
          SDL_Log("Falha ao compilar shader %s", fileName.c_str());
          return false;
       }
    }
    else
    {
       SDL_Log("Arquivo de shader não encontrado: %s", fileName.c_str());
       return false;
    }

    return true;
}

bool Shader::IsCompiled(GLuint shader)
{
    GLint status = 0;
    // Checa o status da compilação
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status != GL_TRUE)
    {
       char buffer[512];
       memset(buffer, 0, 512);
       glGetShaderInfoLog(shader, 511, nullptr, buffer);
       SDL_Log("Falha na compilação GLSL:\n%s", buffer);
       return false;
    }

    return true;
}

bool Shader::IsValidProgram() const
{
    GLint status = 0;
    // Checa o status do link
    glGetProgramiv(mShaderProgram, GL_LINK_STATUS, &status);
    if (status != GL_TRUE)
    {
       char buffer[512];
       memset(buffer, 0, 512);
       glGetProgramInfoLog(mShaderProgram, 511, nullptr, buffer);
       SDL_Log("Falha no link GLSL:\n%s", buffer);
       return false;
    }

    return true;
}