#pragma once

#include "Camera.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <string>

class Shader;
class Cubemap;
class VertexArray;

struct Viewport
{
	int32_t StartX = 0;
	int32_t StartY = 0;
	int32_t Width  = 0;
	int32_t Height = 0;
};

class Renderer
{
public:
	static void Init();
	static void Shutdown();

	static void ReloadShaders();

	static void OnWindowResize(const Viewport& newViewport);

	static void SceneBegin(Camera& camera);
	static void SceneEnd();
	static void Flush();

	static void ClearColor(const glm::vec4& color);
	static void Clear();

	static void DrawQuad(const glm::vec3& position, const glm::vec3& size, const glm::vec4& color);
	static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec4& color);
	static void LoadLineUniform3f(const std::string& name, const glm::vec3& val);

	static void SubmitSphereInstanced(const glm::mat4& transform, const glm::vec4& color = { 0.2f, 0.2f, 0.2f, 1.0f });
	static void DrawIndexedInstanced(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, uint32_t instances);

	static void DrawIndexed(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, uint32_t count = 0);
	static void DrawLines(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, uint32_t vertexCount);
	static void SubmitIndexed(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, const glm::mat4& transform = glm::mat4(1.0f));
	static void DrawSkybox(const std::shared_ptr<Cubemap>& cubemap);

	static Viewport GetViewport();

	static glm::vec3 ScreenToWorldCoords(const glm::vec2& screenCoords, float depth);
	static glm::vec3 WorldToScreenCoords(const glm::vec3& worldCoords);

	static void EnableDepth();
	static void DisableDepth();

	static void SetFrontCull();
	static void SetBackCull();

	static void SetSphereLightning(bool flag);

	static void ToggleWireframe();
	static void SetLineWidth(float width);

private:
	static void StartBatch();
	static void NextBatch();

	static glm::mat4 s_ViewProjection;
	static glm::mat4 s_Projection;
	static glm::mat4 s_View;
};