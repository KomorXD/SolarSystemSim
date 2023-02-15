#include "OpenGL.hpp"
#include "Logger.hpp"

#include <fstream>

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
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
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

IndexBuffer::IndexBuffer(const uint32_t* data, uint32_t count)
	: m_Count(count)
{
	GLCall(glGenBuffers(1, &m_ID));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32_t), data, GL_STATIC_DRAW));
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

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	uint32_t offset = 0;

	for (uint32_t i = 0; i < elements.size(); ++i)
	{
		const VertexBufferElement& element = elements[i];

		GLCall(glEnableVertexAttribArray(i));
		GLCall(glVertexAttribPointer(i, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));

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

void Shader::SetUniform4f(const std::string& name, const glm::vec4& vec)
{
	GLCall(glUniform4f(GetUniformLocation(name), vec.r, vec.g, vec.b, vec.a));
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
	/*FILE* file;

	fopen_s(&file, filepath.c_str(), "r");

	if (!file)
	{
		std::cout << "[ERROR] Unable to open file: " << filepath << std::endl;

		return "";
	}

	char line[512];
	std::stringstream ss;

	while (fgets(line, 512, file) != NULL)
		ss << line;

	fclose(file);

	return ss.str();*/

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