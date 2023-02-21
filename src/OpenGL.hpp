#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <string>
#include <optional>
#include <memory>

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

	void SetData(const void* data, uint32_t size, uint32_t offset = 0);

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
	void Push(uint32_t count, bool instanced = false)
	{
		// static_assert(false);
	}

	template<>
	void Push<float>(uint32_t count, bool instanced)
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += count * sizeof(GLfloat);
	}

	template<>
	void Push<uint32_t>(uint32_t count, bool instanced)
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += count * sizeof(GLuint);
	}

	template<>
	void Push<uint8_t>(uint32_t count, bool instanced)
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE });
		m_Stride += count * sizeof(GLbyte);
	}

	void Clear()
	{
		m_Elements.clear();
		m_Stride = 0;
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

	void AddBuffers(const std::shared_ptr<VertexBuffer>& vbo, std::unique_ptr<IndexBuffer>& ibo, const VertexBufferLayout& layout, uint32_t attribOffset = 0);
	void AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo, const VertexBufferLayout& layout, uint32_t attribOffset = 0);
	void AddInstancedVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo, const VertexBufferLayout& layout, uint32_t attribOffset = 0);

	void Bind() const;
	void Unbind() const;

	inline const std::unique_ptr<IndexBuffer>& GetIndexBuffer() const { return m_IBO; }

private:
	uint32_t m_ID = 0;
	std::unique_ptr<IndexBuffer> m_IBO;
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
	void SetUniform4f(const std::string& name, const glm::vec4& vec);
	void SetUniformMat4(const std::string& name, const glm::mat4& vec);

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

class Framebuffer
{
public:
	Framebuffer();
	~Framebuffer();

	void AttachTexture(uint32_t width, uint32_t height);
	void AttachRenderBuffer(uint32_t width, uint32_t height);

	void ResizeTexture(uint32_t width, uint32_t height);
	void ResizeRenderBuffer(uint32_t width, uint32_t height);

	void BindBuffer()					const;
	void BindTexture(uint32_t slot = 0)	const;
	void BindRenderBuffer()				const;
	void UnbindBuffer()					const;
	void UnbindTexture()				const;
	void UnbindRenderBuffer()			const;

	inline uint32_t GetTextureID() const { return m_TextureID; }

	bool IsComplete() const;

private:
	uint32_t m_ID = 0;
	uint32_t m_TextureID = 0;
	uint32_t m_RenderbufferID = 0;
};

class Cubemap
{
public:
	Cubemap(const std::array<std::string, 6>& texFaces);
	~Cubemap();

	void Bind(uint32_t slot = 0);
	void Unbind();

private:
	uint32_t m_ID		   = 0;
	uint8_t* m_LocalBuffer = nullptr;
	int32_t m_Width		   = 0;
	int32_t m_Height	   = 0;
	int32_t m_BPP		   = 0;
};