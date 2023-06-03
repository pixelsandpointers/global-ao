#pragma once

#include <glad/gl.h>

template<GLenum TType, GLenum TUsage>
class BufferObject
{
private:
	unsigned int m_ID;

public:
	BufferObject() { glGenBuffers(1, &m_ID); };

	~BufferObject() { glDeleteBuffers(1, &m_ID); };

	void setup(GLsizeiptr size, const void* data)
	{
		glBindBuffer(TType, m_ID);
		glBufferData(TType, size, data, TUsage);
	};

	void bind() const { glBindBuffer(TType, m_ID); };
	void unbind() const { glBindBuffer(TType, 0); };
};

using VertexBuffer = BufferObject<GL_ARRAY_BUFFER, GL_STATIC_DRAW>;
using ElementBuffer = BufferObject<GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW>;
