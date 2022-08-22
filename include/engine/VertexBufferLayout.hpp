#ifndef _VERTEXBUFFERLAYOUT_H
#define _VERTEXBUFFERLAYOUT_H

#include <vector>

#include <glad/glad.h>

#include "Renderer.hpp"

enum LayoutType
{
	LT_FLOAT,
	LT_UINT,
	LT_UCHAR
};

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type)
	{
		switch (type)
		{
		case GL_FLOAT:
			return 4;
		case GL_UNSIGNED_INT:
			return 4;
		case GL_UNSIGNED_BYTE:
			return 1;
		}

		return 0;
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;
	unsigned int m_Stride;

public:
	VertexBufferLayout()
			: m_Stride(0) {}

	void Push(LayoutType type, unsigned int count)
	{
		unsigned int glType;
		unsigned char normalized;

		switch (type)
		{
		case LayoutType::LT_FLOAT:
			glType = GL_FLOAT;
			normalized = GL_FALSE;
			break;

		case LayoutType::LT_UINT:
			glType = GL_UNSIGNED_INT;
			normalized = GL_FALSE;
			break;

		case LayoutType::LT_UCHAR:
			glType = GL_UNSIGNED_BYTE;
			normalized = GL_TRUE;
			break;
		}

		VertexBufferElement element = {glType, count, normalized};
		m_Elements.push_back(element);
		m_Stride += count * VertexBufferElement::GetSizeOfType(glType);
	}

	inline const std::vector<VertexBufferElement>
	GetElements() const
	{
		return m_Elements;
	}
	inline unsigned int GetStride() const { return m_Stride; }
};

#endif // _VERTEXBUFFERLAYOUT_H
