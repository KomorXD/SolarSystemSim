#pragma once

#include <unordered_map>
#include <string>
#include <optional>
#include <glad/glad.h>

#define __FILENAME__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
#define ASSERT(x) if(!(x)) __debugbreak()

#ifdef CONF_DEBUG
#define GLCall(f) GLClearErrors();\
	f;\
	ASSERT(GLCheckForErrors(#f, __FILENAME__, __LINE__))
#else
#define GLCall(f) f
#endif

void GLClearErrors();

bool GLCheckForErrors(const char* func, const char* filename, int32_t line);

class VertexBuffer
{
public:
	VertexBuffer(const void* data, uint64_t size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

private:
	uint32_t m_ID = 0;
};

class IndexBuffer
{
public:
	IndexBuffer(const uint32_t* data, uint32_t count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline uint32_t GetCount() const { return m_Count; }

private:
	uint32_t m_ID = 0;
	uint32_t m_Count = 0;
};

struct VertexBufferElement
{
	uint32_t type = 0;
	uint32_t count = 0;
	uint8_t  normalized = 0;

	static uint32_t GetSizeOfType(uint32_t type);
};

class VertexBufferLayout
{
public:
	template<typename T>
	void Push(uint32_t count)
	{
		// static_assert(false);
	}

	template<>
	void Push<float>(uint32_t count)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * sizeof(GLfloat);
	}

	template<>
	void Push<uint32_t>(uint32_t count)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * sizeof(GLuint);
	}

	template<>
	void Push<uint8_t>(uint32_t count)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE });
		m_Stride += count * sizeof(GLbyte);
	}

	inline const std::vector<VertexBufferElement>& GetElements() const { return m_Elements; }
	inline const uint32_t GetStride() const { return m_Stride; }

private:
	std::vector<VertexBufferElement> m_Elements;
	uint32_t m_Stride = 0;
};

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	void Bind() const;
	void Unbind() const;

private:
	uint32_t m_ID = 0;
};

class Shader
{
public:
	Shader(const std::string& vs, const std::string& fs);
	Shader(const std::string& vs, const std::string& gs, const std::string& fs);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform1i(const std::string& name, int32_t val);
	void SetUniform1f(const std::string& name, float val);

	bool ReloadShader();

private:
	std::optional<std::string> ShaderParse(const std::string& filepath);
	uint32_t ShaderCompile(uint32_t type, const std::string& sourceCode);
	uint32_t ShaderCreate(const std::string& vs, const std::string& fs);
	uint32_t ShaderCreate(const std::string& vs, const std::string& gs, const std::string& fs);

	int32_t GetUniformLocation(const std::string& name);

	std::string m_VertexShaderPath{};
	std::string m_FragmentShaderPath{};
	std::string m_GeometryShaderPath{};

	uint32_t m_ID = 0;
	std::unordered_map<std::string, int32_t> m_UniformLocations;
};
