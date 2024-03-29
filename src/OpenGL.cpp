#include "OpenGL.hpp"
#include "Logger.hpp"
#include "Timer.hpp"

#include <fstream>
#include <stb_image/stb_image.h>

void GLClearErrors()
{
	while (glGetError() != GL_NO_ERROR);
}

bool GLCheckForErrors(const char* func, const char* filename, int32_t line)
{
	if (GLenum error = glGetError())
	{
		LOG_WARN("[OpenGL error {}]: {} at {} while calling {}", error, filename, line, func);

		return false;
	}

	return true;
}

VertexBuffer::VertexBuffer(const void* data, uint64_t size)
{
	GLCall(glGenBuffers(1, &m_ID));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
	if (m_ID != 0)
	{
		Unbind();
		GLCall(glDeleteBuffers(1, &m_ID));
	}
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::SetData(const void* data, uint32_t size, uint32_t offset)
{
	Bind();
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, (GLintptr)offset, size, data));
}

IndexBuffer::IndexBuffer(const uint32_t* data, uint32_t count)
	: m_Count(count)
{
	GLCall(glGenBuffers(1, &m_ID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_DYNAMIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
	if (m_ID != 0)
	{
		Unbind();
		GLCall(glDeleteBuffers(1, &m_ID));
	}
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

uint32_t VertexBufferElement::GetSizeOfType(uint32_t type)
{
	switch (type)
	{
	case GL_FLOAT:			return sizeof(GLfloat);
	case GL_UNSIGNED_INT:	return sizeof(GLuint);
	case GL_UNSIGNED_BYTE:	return sizeof(GLbyte);
	}

	return 0;
}

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_ID));
	GLCall(glBindVertexArray(m_ID));
}

VertexArray::~VertexArray()
{
	if (m_ID != 0)
	{
		Unbind();
		GLCall(glDeleteVertexArrays(1, &m_ID));
	}
}

void VertexArray::AddBuffers(const std::shared_ptr<VertexBuffer>& vbo, std::unique_ptr<IndexBuffer>& ibo, const VertexBufferLayout& layout, uint32_t attribOffset)
{
	Bind();
	vbo->Bind();
	ibo->Bind();

	const auto& elements = layout.GetElements();
	uint32_t offset = 0;

	for (uint32_t i = attribOffset; i < elements.size() + attribOffset; ++i)
	{
		const VertexBufferElement& element = elements[i - attribOffset];

		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}

	m_IBO = std::move(ibo);
}

void VertexArray::AddVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo, const VertexBufferLayout& layout, uint32_t attribOffset)
{
	Bind();
	vbo->Bind();

	const auto& elements = layout.GetElements();
	uint32_t offset = 0;

	for (uint32_t i = attribOffset; i < elements.size() + attribOffset; ++i)
	{
		const VertexBufferElement& element = elements[i - attribOffset];

		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::AddInstancedVertexBuffer(const std::shared_ptr<VertexBuffer>& vbo, const VertexBufferLayout& layout, uint32_t attribOffset)
{
	Bind();
	vbo->Bind();

	const auto& elements = layout.GetElements();
	uint32_t offset = 0;

	for (uint32_t i = attribOffset; i < elements.size() + attribOffset; ++i)
	{
		const VertexBufferElement& element = elements[i - attribOffset];

		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));
		GLCall(glVertexAttribDivisor(i, 1));

		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_ID));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}

Shader::Shader(const std::string& vs, const std::string& fs)
	: m_VertexShaderPath(vs), m_FragmentShaderPath(fs), m_ID(0)
{
	std::optional<std::string> vertexSource = ShaderParse(vs);

	if (!vertexSource.has_value())
	{
		LOG_ERROR("Failed to open vertex shader file: {}", vs);

		return;
	}

	std::optional<std::string> fragmentSource = ShaderParse(fs);

	if (!fragmentSource.has_value())
	{
		LOG_ERROR("Failed to open fragment shader file: {}", fs);

		return;
	}

	m_ID = ShaderCreate(vertexSource.value(), fragmentSource.value());
}

Shader::Shader(const std::string& vs, const std::string& gs, const std::string& fs)
	: m_VertexShaderPath(vs), m_GeometryShaderPath(gs), m_FragmentShaderPath(fs), m_ID(0)
{
	std::optional<std::string> vertexSource = ShaderParse(vs);

	if (!vertexSource.has_value())
	{
		LOG_ERROR("Failed to open vertex shader file: {}", vs);

		return;
	}

	std::optional<std::string> geometrySource = ShaderParse(gs);

	if (!geometrySource.has_value())
	{
		LOG_ERROR("Failed to open geometry shader file: {}", gs);

		return;
	}

	std::optional<std::string> fragmentSource = ShaderParse(fs);

	if (!fragmentSource.has_value())
	{
		LOG_ERROR("Failed to open fragment shader file: {}", fs);

		return;
	}

	m_ID = ShaderCreate(vertexSource.value(), geometrySource.value(), fragmentSource.value());
}

Shader::~Shader()
{
	if (m_ID != 0)
	{
		Unbind();
		GLCall(glDeleteProgram(m_ID));
	}
}

void Shader::Bind() const
{
	GLCall(glUseProgram(m_ID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int32_t val)
{
	GLCall(glUniform1i(GetUniformLocation(name), val));
}

void Shader::SetUniform1f(const std::string& name, float val)
{
	GLCall(glUniform1f(GetUniformLocation(name), val));
}

void Shader::SetUniform3f(const std::string& name, const glm::vec3& vec)
{
	GLCall(glUniform3f(GetUniformLocation(name), vec.r, vec.g, vec.b));
}

void Shader::SetUniform4f(const std::string& name, const glm::vec4& vec)
{
	GLCall(glUniform4f(GetUniformLocation(name), vec.r, vec.g, vec.b, vec.a));
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& vec)
{
	GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, &vec[0][0]));
}

bool Shader::ReloadShader()
{
	Unbind();
	GLCall(glDeleteProgram(m_ID));

	std::optional<std::string> vertexSource = ShaderParse(m_VertexShaderPath);

	if (!vertexSource.has_value())
	{
		LOG_ERROR("Failed to open vertex shader file: {}", m_VertexShaderPath);

		return false;
	}

	std::optional<std::string> fragmentSource = ShaderParse(m_FragmentShaderPath);

	if (!fragmentSource.has_value())
	{
		LOG_ERROR("Failed to open fragment shader file: {}", m_FragmentShaderPath);

		return false;
	}

	if (m_GeometryShaderPath.empty())
	{
		m_ID = ShaderCreate(vertexSource.value(), fragmentSource.value());

		return m_ID != 0;
	}

	std::optional<std::string> geometrySource = ShaderParse(m_GeometryShaderPath);

	if (!geometrySource.has_value())
	{
		LOG_ERROR("Failed to open geometry shader file: {}", m_GeometryShaderPath);

		return false;
	}

	m_ID = ShaderCreate(vertexSource.value(), geometrySource.value(), fragmentSource.value());

	return m_ID != 0;
}

std::optional<std::string> Shader::ShaderParse(const std::string& filepath)
{
	std::ifstream shaderSourceFile(filepath);

	if (!shaderSourceFile.good())
	{
		return {};
	}

	std::string line{};
	std::stringstream ss{};

	while (std::getline(shaderSourceFile, line))
	{
		ss << line;
		ss << '\n';
	}

	return ss.str();
}

uint32_t Shader::ShaderCompile(uint32_t type, const std::string& sourceCode)
{
	GLCall(uint32_t id = glCreateShader(type));
	const char* src = sourceCode.c_str();
	int result = 0;

	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	if (result == GL_FALSE)
	{
		int len = 0;

		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &len));

		char* message = (char*)_malloca(len * sizeof(char));

		GLCall(glGetShaderInfoLog(id, len, &len, message));

		LOG_ERROR("Failed to compile shader: {}", message);

		GLCall(glDeleteShader(id));

		return 0;
	}

	return id;
}

uint32_t Shader::ShaderCreate(const std::string& vs, const std::string& fs)
{
	GLCall(uint32_t program = glCreateProgram());
	uint32_t vsID = ShaderCompile(GL_VERTEX_SHADER, vs);
	uint32_t fsID = ShaderCompile(GL_FRAGMENT_SHADER, fs);

	GLCall(glAttachShader(program, vsID));
	GLCall(glAttachShader(program, fsID));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	int success = 0;

	GLCall(glGetProgramiv(program, GL_LINK_STATUS, &success));

	if (success == GL_FALSE)
	{
		int len = 0;

		GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len));

		char* message = (char*)_malloca(len * sizeof(char));

		GLCall(glGetProgramInfoLog(program, len, &len, message));

		LOG_ERROR("Failed to link shaders: {}", message);

		GLCall(glDeleteProgram(program));

		return 0;
	}

	GLCall(glDeleteShader(vsID));
	GLCall(glDeleteShader(fsID));

	return program;
}

uint32_t Shader::ShaderCreate(const std::string& vs, const std::string& gs, const std::string& fs)
{
	GLCall(uint32_t program = glCreateProgram());
	uint32_t vsID = ShaderCompile(GL_VERTEX_SHADER, vs);
	uint32_t gsID = ShaderCompile(GL_GEOMETRY_SHADER, gs);
	uint32_t fsID = ShaderCompile(GL_FRAGMENT_SHADER, fs);

	GLCall(glAttachShader(program, vsID));
	GLCall(glAttachShader(program, gsID));
	GLCall(glAttachShader(program, fsID));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	int success = 0;

	GLCall(glGetProgramiv(program, GL_LINK_STATUS, &success));

	if (success == GL_FALSE)
	{
		int len = 0;

		GLCall(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len));

		char* message = (char*)_malloca(len * sizeof(char));

		GLCall(glGetProgramInfoLog(program, len, &len, message));

		LOG_ERROR("Failed to link shaders: {}", message);

		GLCall(glDeleteProgram(program));

		return 0;
	}

	GLCall(glDeleteShader(vsID));
	GLCall(glDeleteShader(gsID));
	GLCall(glDeleteShader(fsID));

	return program;
}

int32_t Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocations.contains(name))
	{
		return m_UniformLocations[name];
	}

	GLCall(int32_t location = glGetUniformLocation(m_ID, name.c_str()));

	if (location == -1)
	{
		LOG_WARN("Uniform {} does not exist or is not in use", name);

		return -1;
	}

	m_UniformLocations[name] = location;

	return location;
}

Framebuffer::Framebuffer()
{
	GLCall(glGenFramebuffers(1, &m_ID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
}

Framebuffer::~Framebuffer()
{
	if (m_ID != 0)
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glDeleteFramebuffers(1, &m_ID));
	}

	if (m_TextureID != 0)
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glDeleteTextures(1, &m_TextureID));
	}

	if (m_RenderbufferID != 0)
	{
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
		GLCall(glDeleteRenderbuffers(1, &m_RenderbufferID));
	}
}

void Framebuffer::AttachTexture(uint32_t width, uint32_t height)
{
	GLCall(glGenTextures(1, &m_TextureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureID, 0));

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Framebuffer::AttachRenderBuffer(uint32_t width, uint32_t height)
{
	GLCall(glGenRenderbuffers(1, &m_RenderbufferID));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID));

	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID));

	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

void Framebuffer::ResizeTexture(uint32_t width, uint32_t height)
{
	GLCall(glDeleteTextures(1, &m_TextureID));
	AttachTexture(width, height);
}

void Framebuffer::ResizeRenderBuffer(uint32_t width, uint32_t height)
{
	GLCall(glDeleteRenderbuffers(1, &m_RenderbufferID));
	AttachRenderBuffer(width, height);
}

glm::uvec4 Framebuffer::GetPixelAt(const glm::vec2& coords)
{
	glm::vec<4, uint8_t> pixel{};

	BindBuffer();
	BindTexture();
	BindRenderBuffer();

	GLCall(glReadPixels((GLint)coords.x, (GLint)coords.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel[0]));

	return pixel;
}

void Framebuffer::BindBuffer() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
}

void Framebuffer::BindTexture(uint32_t slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID));
}

void Framebuffer::BindRenderBuffer() const
{
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID));
}

void Framebuffer::UnbindBuffer() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::UnbindTexture() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Framebuffer::UnbindRenderBuffer() const
{
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

bool Framebuffer::IsComplete() const
{
	GLCall(bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return complete;
}

MultisampledFramebuffer::MultisampledFramebuffer(int32_t samples)
	: m_Samples(samples)
{
	GLCall(glGenFramebuffers(1, &m_ID));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
}

MultisampledFramebuffer::~MultisampledFramebuffer()
{
	if (m_ID != 0)
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		GLCall(glDeleteFramebuffers(1, &m_ID));
	}

	if (m_TextureID != 0)
	{
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glDeleteTextures(1, &m_TextureID));
	}

	if (m_RenderbufferID != 0)
	{
		GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
		GLCall(glDeleteRenderbuffers(1, &m_RenderbufferID));
	}
}

void MultisampledFramebuffer::AttachTexture(uint32_t width, uint32_t height)
{
	GLCall(glGenTextures(1, &m_TextureID));
	GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_TextureID));

	GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_Samples, GL_RGB, width, height, GL_TRUE));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, m_TextureID, 0));

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void MultisampledFramebuffer::AttachRenderBuffer(uint32_t width, uint32_t height)
{
	GLCall(glGenRenderbuffers(1, &m_RenderbufferID));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID));
				 
	GLCall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, m_Samples, GL_DEPTH24_STENCIL8, width, height));
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RenderbufferID));
				 
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

void MultisampledFramebuffer::ResizeTexture(uint32_t width, uint32_t height)
{
	GLCall(glDeleteTextures(1, &m_TextureID));
	AttachTexture(width, height);
}

void MultisampledFramebuffer::ResizeRenderBuffer(uint32_t width, uint32_t height)
{
	GLCall(glDeleteRenderbuffers(1, &m_RenderbufferID));
	AttachRenderBuffer(width, height);
}

void MultisampledFramebuffer::BlitBuffers(uint32_t width, uint32_t height, uint32_t targetFramebufferID)
{
	GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_ID));
	GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetFramebufferID));
	GLCall(glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST));
}

glm::uvec4 MultisampledFramebuffer::GetPixelAt(const glm::vec2& coords)
{
	glm::vec<4, uint8_t> pixel{};

	BindBuffer();
	BindTexture();
	BindRenderBuffer();

	GLCall(glReadPixels((GLint)coords.x, (GLint)coords.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel[0]));

	return pixel;
}

void MultisampledFramebuffer::BindBuffer() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_ID));
}

void MultisampledFramebuffer::BindTexture(uint32_t slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, m_TextureID));
}

void MultisampledFramebuffer::BindRenderBuffer() const
{
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RenderbufferID));
}

void MultisampledFramebuffer::UnbindBuffer() const
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void MultisampledFramebuffer::UnbindTexture() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
}

void MultisampledFramebuffer::UnbindRenderBuffer() const
{
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

bool MultisampledFramebuffer::IsComplete() const
{
	GLCall(bool complete = glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

	return complete;
}

Cubemap::Cubemap(const std::array<std::string, 6>& texFaces)
{
	FUNC_PROFILE();

	GLCall(glGenTextures(1, &m_ID));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID));

	stbi_set_flip_vertically_on_load(0);

	for (size_t i = 0; i < texFaces.size(); ++i)
	{
		m_LocalBuffer = stbi_load(texFaces[i].c_str(), &m_Width, &m_Height, &m_BPP, 0);

		if (m_LocalBuffer)
		{
			GLCall(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + (uint32_t)i, 0, GL_RGBA8,
				m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
			stbi_image_free(m_LocalBuffer);
		}
		else
		{
			LOG_ERROR("Failed to load texture cubemap: {}", texFaces[i]);
			stbi_image_free(m_LocalBuffer);
		}
	}

	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE));
}

Cubemap::~Cubemap()
{
	if (m_ID != 0)
	{
		Unbind();
		GLCall(glDeleteTextures(1, &m_ID));
	}
}

void Cubemap::Bind(uint32_t slot)
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_ID));
}

void Cubemap::Unbind()
{
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, 0));
}

Texture::Texture(const std::string& filepath)
	: m_ID(0), m_LocalBuffer(nullptr), m_Width(0), m_Height(0), m_BPP(0), m_Path(filepath)
{
	stbi_set_flip_vertically_on_load(0);
	m_LocalBuffer = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	GLCall(glGenTextures(1, &m_ID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer);
	}
}

Texture::Texture(uint32_t width, uint32_t height)
	: m_ID(0), m_Width(width), m_Height(height), m_BPP(4)
{
	std::vector<uint8_t> def;
	def.resize(static_cast<size_t>(m_Width) * m_Height * m_BPP);
	std::fill(def.begin(), def.end(), 255);

	GLCall(glGenTextures(1, &m_ID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ID));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, def.data()));
	GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::~Texture()
{
	if (m_ID != 0)
	{
		GLCall(glDeleteTextures(1, &m_ID));
	}
}

void Texture::Bind(uint32_t slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture::SetSubtexture(const uint8_t* data, const glm::ivec2& offset, const glm::ivec2& size)
{
	Bind();
	GLCall(glTexSubImage2D(GL_TEXTURE_2D, 0, offset.x, offset.y, size.x, size.y, GL_RGBA, GL_UNSIGNED_BYTE, data));
}

std::vector<uint8_t> Texture::GetTextureData()
{
	std::vector<uint8_t> pixels;
	pixels.resize(static_cast<size_t>(m_Width) * m_Height * m_BPP);
	
	Bind();
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data()));

	return pixels;
}
