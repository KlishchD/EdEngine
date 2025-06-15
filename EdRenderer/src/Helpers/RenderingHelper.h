#pragma once

class RenderingHelper
{
public:
	static ResourceView GetWhiteTexture();

	static bool IsLightMeshVisible(const std::vector<glm::vec3>& vertices, const Transform& transform, const Camera& camera);
};
