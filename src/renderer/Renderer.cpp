#include "Renderer.hpp"
#include "../OpenGL.hpp"
#include "../Logger.hpp"
#include "../Timer.hpp"

#include <glm/gtc/matrix_transform.hpp>

static void OpenGLMessageCallback(
	unsigned source,
	unsigned type,
	unsigned id,
	unsigned severity,
	int length,
	const char* message,
	const void* userParam
)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH:		 LOG_CRITICAL(message); return;
	case GL_DEBUG_SEVERITY_MEDIUM:		 LOG_ERROR(message);	return;
	case GL_DEBUG_SEVERITY_LOW:			 LOG_WARN(message);		return;
	case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO(message);		return;
	}
}

glm::mat4 Renderer::s_ViewProjection = glm::mat4(1.0f);

struct SphereVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec4 Color;
};

struct QuadVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
};

struct LineVertex
{
	glm::vec3 Position;
	glm::vec4 Color;
};

struct BasicVertex
{
	glm::vec4 Position;
	glm::vec3 Normal;
};

struct RendererData
{
	static constexpr uint32_t MaxQuads		   = 20000;
	static constexpr uint32_t MaxVertices	   = MaxQuads * 4;
	static constexpr uint32_t MaxIndices	   = MaxQuads * 6;

	static constexpr uint32_t IndicesPerSphere = 5952;

	std::shared_ptr<VertexArray>  QuadVertexArray;
	std::shared_ptr<VertexBuffer> QuadVertexBuffer;
	std::shared_ptr<Shader>		  QuadShader;

	std::shared_ptr<VertexArray>  SphereVertexArray;
	std::shared_ptr<VertexBuffer> SphereVertexBuffer;
	std::shared_ptr<Shader>		  SphereShader;

	uint32_t QuadIndexCount = 0;
	QuadVertex* QuadBufferBase = nullptr;
	QuadVertex* QuadBufferPtr = nullptr;

	uint32_t SphereIndexCount	   = 0;
	SphereVertex* SphereBufferBase = nullptr;
	SphereVertex* SphereBufferPtr  = nullptr;

	std::array<glm::vec4, 4> QuadVertices;
	std::vector<BasicVertex> SphereVertices;
};

static RendererData s_Data{};

std::vector<BasicVertex> GenerateUVSphereVertices(int32_t sectorCount, int32_t stackCount)
{
	constexpr float PI = 3.14159265358979323846f;
	constexpr float radius = 1.0f;

	std::vector<BasicVertex> vertices{};

	float lengthInv = 1.0f / radius;
	float sectorStep = 2.0f * PI / sectorCount;
	float stackStep = PI / stackCount;

	for (int32_t i = 0; i <= stackCount; ++i)
	{
		float stackAngle = PI / 2.0f - i * stackStep;

		float xy = radius * std::cosf(stackAngle);
		float z = radius * std::sinf(stackAngle);

		for (int32_t j = 0; j <= sectorCount; ++j)
		{
			float sectorAngle = j * sectorStep;

			float x = xy * std::cosf(sectorAngle);
			float y = xy * std::sinf(sectorAngle);

			float nx = x * lengthInv;
			float ny = y * lengthInv;
			float nz = z * lengthInv;

			BasicVertex v;

			v.Position = glm::vec4(x, y, z, 1.0f);
			v.Normal   = glm::vec3(nx, ny, nz);

			vertices.push_back(v);
		}
	}

	return vertices;
}

std::vector<uint32_t> GenerateUVSphereIndices(int32_t sectorCount, int32_t stackCount, int32_t maxIndices)
{
	int32_t offset = 0;

	std::vector<uint32_t> indices{};
	int32_t k1{};
	int32_t k2{};

	indices.reserve(maxIndices);

	while (offset + s_Data.IndicesPerSphere < maxIndices)
	{
		for (int32_t i = 0; i < stackCount; ++i)
		{
			k1 = i * (sectorCount + 1) + offset;
			k2 = k1 + sectorCount + 1;

			for (int32_t j = 0; j < sectorCount; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				if (i != (stackCount - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}
			}
		}
		
		offset += s_Data.IndicesPerSphere;
	}

	return indices;
}

void Renderer::Init()
{
	FUNC_PROFILE();

#ifdef CONF_DEBUG
	GLCall(glEnable(GL_DEBUG_OUTPUT));
	GLCall(glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS));
	GLCall(glDebugMessageCallback(OpenGLMessageCallback, nullptr));
	GLCall(glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE));
#endif

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthFunc(GL_LESS));
	
	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glEnable(GL_LINE_SMOOTH));

	{
		SCOPE_PROFILE("Quad data init");

		s_Data.QuadVertexArray = std::make_shared<VertexArray>();
		s_Data.QuadVertexBuffer = std::make_shared<VertexBuffer>(nullptr, s_Data.MaxVertices * sizeof(QuadVertex));

		VertexBufferLayout layout;

		layout.Push<float>(3);
		layout.Push<float>(4);

		std::vector<uint32_t> quadIndices(s_Data.MaxIndices);

		uint32_t offset = 0;

		for (uint32_t i = 0; i < s_Data.MaxIndices; i += 6)
		{
			quadIndices[i + 0] = offset + 0;
			quadIndices[i + 1] = offset + 1;
			quadIndices[i + 2] = offset + 2;

			quadIndices[i + 3] = offset + 2;
			quadIndices[i + 4] = offset + 3;
			quadIndices[i + 5] = offset + 0;

			offset += 4;
		}

		std::unique_ptr<IndexBuffer> ibo = std::make_unique<IndexBuffer>(quadIndices.data(), quadIndices.size());

		s_Data.QuadVertexArray->AddBuffer(*s_Data.QuadVertexBuffer, ibo, layout);

		s_Data.QuadBufferBase = new QuadVertex[s_Data.MaxVertices];
		s_Data.QuadShader = std::make_shared<Shader>("res/shaders/Quad.vert", "res/shaders/Quad.frag");

		s_Data.QuadVertices[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertices[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertices[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertices[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	{
		SCOPE_PROFILE("Sphere data init");

		s_Data.SphereVertexArray = std::make_shared<VertexArray>();
		s_Data.SphereVertexBuffer = std::make_shared<VertexBuffer>(nullptr, s_Data.MaxVertices * sizeof(SphereVertex));

		VertexBufferLayout layout;

		layout.Push<float>(3);
		layout.Push<float>(3);
		layout.Push<float>(4);

		std::vector<uint32_t> sphereIndices = GenerateUVSphereIndices(32, 32, s_Data.MaxIndices);

		std::unique_ptr<IndexBuffer> ibo = std::make_unique<IndexBuffer>(sphereIndices.data(), sphereIndices.size());

		s_Data.SphereVertexArray->AddBuffer(*s_Data.SphereVertexBuffer, ibo, layout);

		s_Data.SphereBufferBase = new SphereVertex[s_Data.MaxVertices];
		s_Data.SphereShader = std::make_shared<Shader>("res/shaders/Sphere.vert", "res/shaders/Sphere.frag");

		s_Data.SphereVertices = GenerateUVSphereVertices(32, 32);
	}
}

void Renderer::Shutdown()
{
	delete[] s_Data.QuadBufferBase;
	delete[] s_Data.SphereBufferBase;
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	GLCall(glViewport(0, 0, width, height));
}

void Renderer::SceneBegin(Camera& camera)
{
	s_ViewProjection = camera.GetViewProjection();

	StartBatch();
}

void Renderer::SceneEnd()
{
	Flush();
}

void Renderer::Flush()
{
	if (s_Data.QuadIndexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.QuadBufferPtr - (uint8_t*)s_Data.QuadBufferBase);

		s_Data.QuadVertexBuffer->SetData(s_Data.QuadBufferBase, dataSize);

		GLCall(glDisable(GL_CULL_FACE));
		DrawIndexed(s_Data.QuadShader, s_Data.QuadVertexArray, s_Data.QuadIndexCount);
		GLCall(glEnable(GL_CULL_FACE));
	}

	if (s_Data.SphereIndexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.SphereBufferPtr - (uint8_t*)s_Data.SphereBufferBase);

		s_Data.SphereVertexBuffer->SetData(s_Data.SphereBufferBase, dataSize);

		DrawIndexed(s_Data.SphereShader, s_Data.SphereVertexArray, s_Data.SphereIndexCount);
	}
}

void Renderer::ClearColor(const glm::vec4& color)
{
	GLCall(glClearColor(color.r, color.g, color.b, color.a));
}

void Renderer::Clear()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void Renderer::DrawQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color)
{
	if (s_Data.QuadIndexCount + 6 > s_Data.MaxIndices)
	{
		NextBatch();
	}

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), size);

	for (size_t i = 0; i < s_Data.QuadVertices.size(); ++i)
	{
		s_Data.QuadBufferPtr->Position = transform * s_Data.QuadVertices[i];
		s_Data.QuadBufferPtr->Color = color;

		++s_Data.QuadBufferPtr;
	}

	s_Data.QuadIndexCount += 6;
}

void Renderer::DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color)
{
	if (s_Data.SphereIndexCount + s_Data.IndicesPerSphere >= s_Data.MaxIndices)
	{
		NextBatch();
	}

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::scale(glm::mat4(1.0f), glm::vec3(radius));

	for (size_t i = 0; i < s_Data.SphereVertices.size(); ++i)
	{
		s_Data.SphereBufferPtr->Position = transform * s_Data.SphereVertices[i].Position;
		s_Data.SphereBufferPtr->Normal = s_Data.SphereVertices[i].Normal;
		s_Data.SphereBufferPtr->Color = color;

		++s_Data.SphereBufferPtr;
	}

	s_Data.SphereIndexCount += s_Data.IndicesPerSphere;
}

void Renderer::DrawIndexed(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, uint32_t count)
{
	uint32_t indices = count ? count : vao->GetIndexBuffer()->GetCount();

	shader->Bind();
	shader->SetUniformMat4("u_ViewProjection", s_ViewProjection);

	vao->Bind();

	GLCall(glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, nullptr));
}

void Renderer::SubmitIndexed(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetUniformMat4("u_ViewProjection", s_ViewProjection);
	shader->SetUniformMat4("u_Transform", transform);

	vao->Bind();

	GLCall(glDrawElements(GL_TRIANGLES, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::ToggleWireframe()
{
	GLint mode[2]{};

	GLCall(glGetIntegerv(GL_POLYGON_MODE, mode));
	mode[0] = (mode[0] == GL_LINE ? GL_FILL : GL_LINE);
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, mode[0]));
}

void Renderer::StartBatch()
{
	s_Data.QuadIndexCount = 0;
	s_Data.QuadBufferPtr = s_Data.QuadBufferBase;

	s_Data.SphereIndexCount = 0;
	s_Data.SphereBufferPtr = s_Data.SphereBufferBase;
}

void Renderer::NextBatch()
{
	Flush();
	StartBatch();
}