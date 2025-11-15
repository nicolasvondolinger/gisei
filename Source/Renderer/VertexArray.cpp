#include "VertexArray.h"
#include <GL/glew.h>

VertexArray::VertexArray(const float* verts, unsigned int numVerts, const unsigned int* indices,
						 unsigned int numIndices)
: mNumVerts(numVerts)
, mNumIndices(numIndices)
, mVertexBuffer(0)
, mIndexBuffer(0)
, mVertexArray(0)
{
    glGenVertexArrays(1, &mVertexArray);
    glBindVertexArray(mVertexArray);

    glGenBuffers(1, &mVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    // Assumindo que cada vértice tem 5 floats (x, y, z, u, v)
    glBufferData(GL_ARRAY_BUFFER,               // Buffer de vértices
                 numVerts * 5 * sizeof(float), // Tamanho total dos dados (4 vértices * 5 floats/vértice)
                 verts,                        // Ponteiro para os dados
                 GL_STATIC_DRAW);              // Como os dados serão usados

    glGenBuffers(1, &mIndexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // Buffer de índices
                 numIndices * sizeof(unsigned int), // Tamanho total dos dados (6 índices)
                 indices,                           // Ponteiro para os índices
                 GL_STATIC_DRAW);                 // Como os dados serão usados

    GLsizei stride = 5 * sizeof(float);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,                 // location = 0 (corresponde ao 'layout(location = 0)' no shader)
        3,                 // Número de componentes (x, y, z)
        GL_FLOAT,          // Tipo dos dados
        GL_FALSE,          // Não normalizar
        stride,            // Stride: O tamanho total de um vértice (5 floats)
        (void*)0           // Offset: Onde este atributo começa (no byte 0)
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,                 // location = 1 (corresponde ao 'layout(location = 1)' no shader)
        2,                 // Número de componentes (u, v)
        GL_FLOAT,          // Tipo dos dados
        GL_FALSE,          // Não normalizar
        stride,            // Stride: O tamanho total de um vértice (5 floats)
        (void*)(3 * sizeof(float)) // Offset: Onde este atributo começa (após os 3 floats da posição)
    );
}

VertexArray::~VertexArray()
{
	glDeleteBuffers(1, &mVertexBuffer);
	glDeleteBuffers(1, &mIndexBuffer);
	glDeleteVertexArrays(1, &mVertexArray);
}

void VertexArray::SetActive() const
{
	glBindVertexArray(mVertexArray);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBuffer);
}
