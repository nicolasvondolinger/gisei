#pragma once
#include <GL/glew.h>
#include <bits/stdc++.h>
#include "../Math.h"

using namespace std;

class Shader
{
public:
	Shader();
	~Shader();

	
	
	bool Load(const std::string& name);
	void Unload();

    
	void SetActive() const;

    
    void SetVectorUniform (const char* name, const Vector2& vector) const;
	void SetVectorUniform (const char* name, const Vector3& vector) const;
    void SetVectorUniform (const char* name, const Vector4& vector) const;
	void SetMatrixUniform(const char* name, const Matrix4& matrix) const;
    void SetFloatUniform(const char* name, float value) const;
    void SetIntegerUniform(const char *name, int value) const;

private:
	
	bool CompileShader(const std::string& fileName, GLenum shaderType, GLuint& outShader);

	
	bool IsCompiled(GLuint shader);
	
	bool IsValidProgram() const;

	
	GLuint mVertexShader;
	GLuint mFragShader;
	GLuint mShaderProgram;
};
