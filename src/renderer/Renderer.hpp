#pragma once

#include "Camera.hpp"

#include <glm/glm.hpp>
#include <memory>

class Shader;
class VertexArray;

class Renderer
{
public:
	static void Init();
	static void Shutdown();

	static void OnWindowResize(uint32_t width, uint32_t height);

	static void SceneBegin(Camera& camera);
	static void SceneEnd();
	static void Flush();

	static void ClearColor(const glm::vec4& color);
	static void Clear();

	static void DrawSphere(const glm::vec3& position, float radius, const glm::vec4& color);
	static void DrawIndexed(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, uint32_t count = 0);
	static void SubmitIndexed(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vao, const glm::mat4& transform = glm::mat4(1.0f));

	static void ToggleWireframe();

private:
	static void StartBatch();
	static void NextBatch();

	static glm::mat4 s_ViewProjection;
};