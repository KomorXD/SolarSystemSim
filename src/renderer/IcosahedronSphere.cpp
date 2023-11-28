#include "IcosahedronSphere.hpp"

static std::vector<glm::vec3> GenerateIcosahedronVertices()
{
	float phi = (1.0f + sqrt(5.0f)) / 2.0f;
	float inv_phi = 1.0f / phi;

	std::vector<glm::vec3> vertices;

	vertices.push_back(glm::vec3(-1.0f, phi, 0.0f));
	vertices.push_back(glm::vec3(1.0f, phi, 0.0f));
	vertices.push_back(glm::vec3(-1.0f, -phi, 0.0f));
	vertices.push_back(glm::vec3(1.0f, -phi, 0.0f));

	vertices.push_back(glm::vec3(0.0f, -1.0f, phi));
	vertices.push_back(glm::vec3(0.0f, 1.0f, phi));
	vertices.push_back(glm::vec3(0.0f, -1.0f, -phi));
	vertices.push_back(glm::vec3(0.0f, 1.0f, -phi));

	vertices.push_back(glm::vec3(phi, 0.0f, -1.0f));
	vertices.push_back(glm::vec3(phi, 0.0f, 1.0f));
	vertices.push_back(glm::vec3(-phi, 0.0f, -1.0f));
	vertices.push_back(glm::vec3(-phi, 0.0f, 1.0f));

	for (glm::vec3& vertex : vertices)
	{
		vertex = glm::normalize(vertex);
	}

	return vertices;
}

static std::vector<glm::vec3> s_IcosahedronVertices = GenerateIcosahedronVertices();
static std::vector<uint32_t> s_IcosahedronIndices =
{
	 0, 11, 5, 0, 5,  1,  0,  1,  7,  0,  7, 10, 0, 10, 11,
	 1, 5,  9, 5, 11, 4,  11, 10, 2,  10, 7, 6,  7, 1,  8,
	 3, 9,  4, 3, 4,  2,  3,  2,  6,  3,  6, 8,  3, 8,  9,
	 4, 9,  5, 2, 4,  11, 6,  2,  10, 8,  6, 7,  9, 8,  1
};

static std::pair<glm::vec3, glm::vec3> CalcualteTangentBasis(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	glm::vec2 uv1(0.0f, 1.0f);
	glm::vec2 uv2(0.0f, 0.0f);
	glm::vec2 uv3(1.0f, 0.0f);
	glm::vec2 deltaUV1 = uv2 - uv1;
	glm::vec2 deltaUV2 = uv3 - uv1;
	glm::vec3 edge1 = b - a;
	glm::vec3 edge2 = c - a;

	float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
	glm::vec3 tangent(
		f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x),
		f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y),
		f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z)
	);
	glm::vec3 bitangent(
		f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x),
		f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y),
		f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z)
	);

	return { glm::normalize(tangent), glm::normalize(bitangent) };
}

static void Subdivide(IcosahedronSphereData& target, const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, int32_t depth)
{
	if (depth == 0)
	{
		target.Indices.push_back(target.Vertices.size() + 0);
		target.Indices.push_back(target.Vertices.size() + 1);
		target.Indices.push_back(target.Vertices.size() + 2);

		glm::vec3 normA = glm::normalize(a);
		glm::vec3 normB = glm::normalize(b);
		glm::vec3 normC = glm::normalize(c);

		auto[tangent, bitangent] = CalcualteTangentBasis(normA, normB, normC);
		target.Vertices.push_back({ normA, tangent, bitangent });
		target.Vertices.push_back({ normB, tangent, bitangent });
		target.Vertices.push_back({ normC, tangent, bitangent });

		return;
	}

	glm::vec3 ab = glm::normalize(a + b);
	glm::vec3 bc = glm::normalize(b + c);
	glm::vec3 ca = glm::normalize(c + a);

	Subdivide(target, a, ab, ca, depth - 1);
	Subdivide(target, ab, b, bc, depth - 1);
	Subdivide(target, ca, bc, c, depth - 1);
	Subdivide(target, ab, bc, ca, depth - 1);
}

IcosahedronSphereData GenerateIcosahedronSphere(int32_t depth)
{
	IcosahedronSphereData data;

	for (size_t i = 0; i < s_IcosahedronIndices.size(); i += 3)
	{
		glm::vec3 a = s_IcosahedronVertices[s_IcosahedronIndices[i + 0]];
		glm::vec3 b = s_IcosahedronVertices[s_IcosahedronIndices[i + 1]];
		glm::vec3 c = s_IcosahedronVertices[s_IcosahedronIndices[i + 2]];

		Subdivide(data, a, b, c, depth);
	}

	return data;
}