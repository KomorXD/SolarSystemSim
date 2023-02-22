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
glm::mat4 Renderer::s_Projection = glm::mat4(1.0f);
glm::mat4 Renderer::s_View = glm::mat4(1.0f);

struct SphereVertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
};

struct SphereInstance
{
	glm::mat4 Transform;
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

struct RendererData
{
	static constexpr uint32_t MaxQuads	  = 5000;
	static constexpr uint32_t MaxVertices = MaxQuads * 4;
	static constexpr uint32_t MaxIndices  = MaxQuads * 6;

	std::shared_ptr<VertexArray>  QuadVertexArray;
	std::shared_ptr<VertexBuffer> QuadVertexBuffer;
	std::shared_ptr<Shader>		  QuadShader;

	std::shared_ptr<VertexArray>  LineVertexArray;
	std::shared_ptr<VertexBuffer> LineVertexBuffer;
	std::shared_ptr<Shader>		  LineShader;

	std::shared_ptr<VertexArray>  SphereVertexArray;
	std::shared_ptr<VertexBuffer> SphereVertexBuffer;
	std::shared_ptr<VertexBuffer> SphereTransformsVertexBuffer;
	std::shared_ptr<Shader>		  SphereShader;

	std::shared_ptr<VertexArray>  SkyboxVertexArray;
	std::shared_ptr<VertexBuffer> SkyboxVertexBuffer;
	std::shared_ptr<Shader>		  SkyboxShader;

	uint32_t	QuadIndexCount = 0;
	QuadVertex* QuadBufferBase = nullptr;
	QuadVertex* QuadBufferPtr  = nullptr;

	uint32_t	LineVertexCount = 0;
	LineVertex* LineBufferBase  = nullptr;
	LineVertex* LineBufferPtr   = nullptr;

	uint32_t		SpheresInstanceCount		= 0;
	SphereInstance* SpheresTransformsBufferBase = nullptr;
	SphereInstance* SpheresTransformsBufferPtr  = nullptr;
	uint32_t		SpheresBufferOffset		    = 0;

	std::array<glm::vec4, 4> QuadVertices;

	float LineWidth = 2.0f;

	std::array<glm::vec4, 36> SkyboxVertices;
};

static RendererData s_Data{};

static std::vector<SphereVertex> GenerateUVSphereData(int32_t sectorCount, int32_t stackCount)
{
	constexpr float PI = 3.14159265358979323846f;
	constexpr float radius = 1.0f;

	std::vector<SphereVertex> vertices;

	vertices.reserve((stackCount + 1) * (sectorCount + 1));

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

			SphereVertex v{};

			v.Position = glm::vec3(x, y, z);
			v.Normal   = glm::vec3(nx, ny, nz);

			vertices.push_back(v);
		}
	}

	return vertices;
}

static std::vector<uint32_t> GenerateUVSphereIndices(int32_t sectorCount, int32_t stackCount)
{
	std::vector<uint32_t> indices{};
	int32_t k1{};
	int32_t k2{};

	for (int32_t i = 0; i < stackCount; ++i)
	{
		k1 = i * (sectorCount + 1);
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

	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glDepthFunc(GL_LESS));

	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	GLCall(glEnable(GL_STENCIL_TEST));
	GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));
	
	GLCall(glEnable(GL_CULL_FACE));
	GLCall(glEnable(GL_LINE_SMOOTH));

	{
		SCOPE_PROFILE("Quad data init");

		s_Data.QuadVertexArray = std::make_shared<VertexArray>();
		s_Data.QuadVertexBuffer = std::make_shared<VertexBuffer>(nullptr, s_Data.MaxVertices * sizeof(QuadVertex));

		VertexBufferLayout layout;

		layout.Push<float>(3); // Position
		layout.Push<float>(4); // Color

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

		std::unique_ptr<IndexBuffer> ibo = std::make_unique<IndexBuffer>(quadIndices.data(), (uint32_t)quadIndices.size());

		s_Data.QuadVertexArray->AddBuffers(s_Data.QuadVertexBuffer, ibo, layout);

		s_Data.QuadBufferBase = new QuadVertex[s_Data.MaxVertices];
		s_Data.QuadShader = std::make_shared<Shader>("res/shaders/Quad.vert", "res/shaders/Quad.frag");

		s_Data.QuadVertices[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertices[1] = {  0.5f, -0.5f, 0.0f, 1.0f };
		s_Data.QuadVertices[2] = {  0.5f,  0.5f, 0.0f, 1.0f };
		s_Data.QuadVertices[3] = { -0.5f,  0.5f, 0.0f, 1.0f };
	}

	{
		SCOPE_PROFILE("Line data init");

		s_Data.LineVertexArray = std::make_shared<VertexArray>();
		s_Data.LineVertexBuffer = std::make_shared<VertexBuffer>(nullptr, s_Data.MaxVertices * sizeof(LineVertex));

		VertexBufferLayout layout;

		layout.Push<float>(3); // Position
		layout.Push<float>(4); // Color

		s_Data.LineVertexArray->AddVertexBuffer(s_Data.LineVertexBuffer, layout);
		s_Data.LineBufferBase = new LineVertex[s_Data.MaxVertices];
		s_Data.LineShader = std::make_shared<Shader>("res/shaders/Line.vert", "res/shaders/Line.frag");
	}

	{
		SCOPE_PROFILE("Sphere data init");

		std::vector<SphereVertex> sphereVertices = GenerateUVSphereData(32, 32);

		s_Data.SphereVertexArray = std::make_shared<VertexArray>();
		s_Data.SphereVertexBuffer = std::make_shared<VertexBuffer>(nullptr, s_Data.MaxVertices * sizeof(SphereVertex));
		s_Data.SphereVertexBuffer->SetData(sphereVertices.data(), (uint32_t)(sphereVertices.size() * sizeof(SphereVertex)));
		s_Data.SpheresBufferOffset = (uint32_t)(sphereVertices.size() * sizeof(SphereVertex));

		VertexBufferLayout layout;

		layout.Push<float>(3); // Position
		layout.Push<float>(3); // Normal
		
		std::vector<uint32_t> sphereIndices = GenerateUVSphereIndices(32, 32);
		std::unique_ptr<IndexBuffer> ibo = std::make_unique<IndexBuffer>(sphereIndices.data(), (uint32_t)sphereIndices.size());

		s_Data.SphereVertexArray->AddBuffers(s_Data.SphereVertexBuffer, ibo, layout);

		layout.Clear();
		layout.Push<float>(4); // Transform
		layout.Push<float>(4); // Transform
		layout.Push<float>(4); // Transform
		layout.Push<float>(4); // Transform
		layout.Push<float>(4); // Color

		s_Data.SphereTransformsVertexBuffer = std::make_shared<VertexBuffer>(nullptr, s_Data.MaxVertices * sizeof(glm::mat4));
		s_Data.SphereVertexArray->AddInstancedVertexBuffer(s_Data.SphereTransformsVertexBuffer, layout, 2);

		s_Data.SpheresTransformsBufferBase = new SphereInstance[254];
		s_Data.SphereShader = std::make_shared<Shader>("res/shaders/Sphere.vert", "res/shaders/Sphere.frag");
		
		s_Data.SphereVertexArray->Unbind();
	}

	{
		SCOPE_PROFILE("Skybox data init");

		s_Data.SkyboxVertexArray = std::make_shared<VertexArray>();

		float skyboxVertices[] = {
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		s_Data.SkyboxVertexBuffer = std::make_shared<VertexBuffer>(skyboxVertices, sizeof(skyboxVertices));

		VertexBufferLayout layout;

		layout.Push<float>(3); // Position

		s_Data.SkyboxVertexArray->AddVertexBuffer(s_Data.SkyboxVertexBuffer, layout);
		s_Data.SkyboxShader = std::make_shared<Shader>("res/shaders/Skybox.vert", "res/shaders/Skybox.frag");
	}
}

void Renderer::Shutdown()
{
	delete[] s_Data.QuadBufferBase;
	delete[] s_Data.LineBufferBase;
	delete[] s_Data.SpheresTransformsBufferBase;
}

void Renderer::OnWindowResize(uint32_t width, uint32_t height)
{
	GLCall(glViewport(0, 0, width, height));
}

void Renderer::SceneBegin(Camera& camera)
{
	s_ViewProjection = camera.GetViewProjection();
	s_Projection = camera.GetProjection();
	s_View = camera.GetViewMatrix();

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

	if (s_Data.LineVertexCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.LineBufferPtr - (uint8_t*)s_Data.LineBufferBase);

		s_Data.LineVertexBuffer->SetData(s_Data.LineBufferBase, dataSize);

		GLCall(glDisable(GL_CULL_FACE));
		DrawLines(s_Data.LineShader, s_Data.LineVertexArray, s_Data.LineVertexCount);
		GLCall(glEnable(GL_CULL_FACE));
	}

	if (s_Data.SpheresInstanceCount)
	{
		uint32_t dataSize = (uint32_t)((uint8_t*)s_Data.SpheresTransformsBufferPtr - (uint8_t*)s_Data.SpheresTransformsBufferBase);

		s_Data.SphereTransformsVertexBuffer->SetData(s_Data.SpheresTransformsBufferBase, dataSize);

		DrawIndexedInstanced(s_Data.SphereShader, s_Data.SphereVertexArray, s_Data.SpheresInstanceCount);
	}
}

void Renderer::ClearColor(const glm::vec4& color)
{
	GLCall(glClearColor(color.r, color.g, color.b, color.a));
}

void Renderer::Clear()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
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

void Renderer::DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color)
{
	if (s_Data.LineVertexCount + 2 >= s_Data.MaxVertices)
	{
		NextBatch();
	}

	s_Data.LineBufferPtr->Position = start;
	s_Data.LineBufferPtr->Color = color;
	++s_Data.LineBufferPtr;

	s_Data.LineBufferPtr->Position = end;
	s_Data.LineBufferPtr->Color = color;
	++s_Data.LineBufferPtr;

	s_Data.LineVertexCount += 2;
}

void Renderer::SubmitSphereInstanced(const glm::mat4& transform, const glm::vec4& color)
{
	if (s_Data.SpheresInstanceCount >= 10000)
	{
		NextBatch();
	}

	s_Data.SpheresTransformsBufferPtr->Transform = transform;
	s_Data.SpheresTransformsBufferPtr->Color = color;
	++s_Data.SpheresTransformsBufferPtr;
	++s_Data.SpheresInstanceCount;
}

void Renderer::DrawIndexedInstanced(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, uint32_t instances)
{
	shader->Bind();
	shader->SetUniformMat4("u_ViewProjection", s_ViewProjection);

	vao->Bind();

	GLCall(glDrawElementsInstanced(GL_TRIANGLES, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr, instances));
}

void Renderer::DrawIndexed(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, uint32_t count)
{
	uint32_t indices = count ? count : vao->GetIndexBuffer()->GetCount();

	shader->Bind();
	shader->SetUniformMat4("u_ViewProjection", s_ViewProjection);

	vao->Bind();

	GLCall(glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawLines(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, uint32_t vertexCount)
{
	shader->Bind();
	shader->SetUniformMat4("u_ViewProjection", s_ViewProjection);

	vao->Bind();

	GLCall(glDrawArrays(GL_LINES, 0, vertexCount));
}

void Renderer::SubmitIndexed(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, const glm::mat4& transform)
{
	shader->Bind();
	shader->SetUniformMat4("u_ViewProjection", s_ViewProjection);
	shader->SetUniformMat4("u_Transform", transform);

	vao->Bind();

	GLCall(glDrawElements(GL_TRIANGLES, vao->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawSkybox(const std::shared_ptr<Cubemap>& cubemap)
{
	cubemap->Bind();

	s_Data.SkyboxShader->Bind();
	s_Data.SkyboxShader->SetUniformMat4("u_Projection", s_Projection);
	s_Data.SkyboxShader->SetUniformMat4("u_View", glm::mat4(glm::mat3(s_View)));
	s_Data.SkyboxShader->SetUniform1i("u_SkyboxTex", 0);

	s_Data.SkyboxVertexArray->Bind();

	GLCall(glDepthFunc(GL_LEQUAL));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	GLCall(glDepthFunc(GL_LESS));
}

glm::uvec4 Renderer::GetPixelAt(const glm::vec2& coords)
{
	glm::vec<4, uint8_t> pixel{};

	GLCall(glReadPixels((GLint)coords.x, (GLint)coords.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &pixel[0]));

	return pixel;
}

glm::vec3 Renderer::ScreenToWorldCoords(const glm::vec2& screenCoords, float depth)
{
	struct
	{
		int X;
		int Y;
		int Width;
		int Height;
	} viewport{};

	GLCall(glGetIntegerv(GL_VIEWPORT, (int*)&viewport));

	glm::mat4 viewProjInverse = glm::inverse(s_ViewProjection);
	glm::vec4 mouseCoords(
		(screenCoords.x - viewport.Width / 2.0f) / (viewport.Width / 2.0f),
		(screenCoords.y - viewport.Height / 2.0f) / (viewport.Height / 2.0f),
		depth,
		1.0f
	);
	glm::vec4 worldCoords = viewProjInverse * mouseCoords;

	worldCoords /= worldCoords.w;

	return glm::vec3(worldCoords);
}

void Renderer::EnableDepth()
{
	GLCall(glEnable(GL_DEPTH_TEST));
}

void Renderer::DisableDepth()
{
	GLCall(glDisable(GL_DEPTH_TEST));
}

void Renderer::BeginStencil()
{
	GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
	GLCall(glStencilMask(0xFF));
}

void Renderer::EndStencil()
{
	GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
	GLCall(glStencilMask(0x00));
}

void Renderer::SetSphereLightning(bool flag)
{
	s_Data.SphereShader->Bind();
	s_Data.SphereShader->SetUniform1i("u_Lightning", (int32_t)flag);
}

void Renderer::ToggleWireframe()
{
	GLint mode[2]{};

	GLCall(glGetIntegerv(GL_POLYGON_MODE, mode));
	mode[0] = (mode[0] == GL_LINE ? GL_FILL : GL_LINE);
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, mode[0]));
}

void Renderer::SetLineWidth(float width)
{
	GLCall(glLineWidth(width));
}

void Renderer::StartBatch()
{
	s_Data.QuadIndexCount = 0;
	s_Data.QuadBufferPtr = s_Data.QuadBufferBase;

	s_Data.LineVertexCount = 0;
	s_Data.LineBufferPtr = s_Data.LineBufferBase;

	s_Data.SpheresInstanceCount = 0;
	s_Data.SpheresTransformsBufferPtr = s_Data.SpheresTransformsBufferBase;
}

void Renderer::NextBatch()
{
	Flush();
	StartBatch();
}