#include "EdRendererPrivate.h"
#include "RenderingHelper.h"
#include "Helpers/FilesHelper.h"

ResourceView RenderingHelper::GetWhiteTexture()
{
	static bool isInitialized = false;
    static ResourceView view;

	if (!isInitialized)
	{
		static u32 colors = 0xFFFFFFFF;

		RenderingContext::Get().UploadTextureDeferred(reinterpret_cast<u8*>(&colors), 1, 1, 1, PixelFormat::RGBA8F, view, ResourceState::ShaderRead, "WhiteTexture");

		isInitialized = true;
	}

	return view;
}

bool RenderingHelper::IsLightMeshVisible(const std::vector<glm::vec3>& vertices, const Transform& transform, const Camera& camera)
{
	glm::mat4 projectionViewModelMatrix = camera.GetProjectionView() * transform.GetMatrix();

	glm::vec3 leftBottonCorner(std::numeric_limits<f32>::max());
	glm::vec3 rightTopCorner(std::numeric_limits<f32>::min());

	for (const glm::vec3& point : vertices)
	{
		glm::vec4 transformed = projectionViewModelMatrix * glm::vec4(point, 1.0f);
		transformed /= transformed.w;

		leftBottonCorner.x = glm::min(leftBottonCorner.x, transformed.x);
		leftBottonCorner.y = glm::min(leftBottonCorner.y, transformed.y);
		leftBottonCorner.z = glm::min(leftBottonCorner.z, transformed.z);

		rightTopCorner.x = glm::max(rightTopCorner.x, transformed.x);
		rightTopCorner.y = glm::max(rightTopCorner.y, transformed.y);
		rightTopCorner.z = glm::max(rightTopCorner.z, transformed.z);
	}

	bool xChangesSign = leftBottonCorner.x * rightTopCorner.x < 0;
	bool yChangesSign = leftBottonCorner.y * rightTopCorner.y < 0;
	bool zChangesSign = leftBottonCorner.z * rightTopCorner.z < 0;

	bool xInViewRange = (leftBottonCorner.x >= -1.0f && leftBottonCorner.x <= 1.0f) || (rightTopCorner.x >= -1.0f && rightTopCorner.x <= 1.0f);
	bool yInViewRange = (leftBottonCorner.y >= -1.0f && leftBottonCorner.y <= 1.0f) || (rightTopCorner.y >= -1.0f && rightTopCorner.y <= 1.0f);
	bool zInViewRange = (leftBottonCorner.z >= 0.0f && leftBottonCorner.z <= 1.0f) || (rightTopCorner.z >= 0.0f && rightTopCorner.z <= 1.0f);

	return (xChangesSign && yChangesSign && zChangesSign) || (xInViewRange && yInViewRange && zInViewRange) ||
		(xChangesSign && yChangesSign && zInViewRange) || (yChangesSign && zChangesSign && xInViewRange) || (xChangesSign && zChangesSign && yInViewRange) ||
		(xChangesSign && yInViewRange && zInViewRange) || (yChangesSign && xInViewRange && zInViewRange) || (zChangesSign && yInViewRange && xInViewRange);
}