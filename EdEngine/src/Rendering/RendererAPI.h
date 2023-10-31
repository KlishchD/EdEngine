#pragma once

#include "Core/Ed.h"
#include "Framebuffer.h"
#include "VertexArray.h"
#include "Core/Assets/Font.h"
#include "Core/Assets/StaticMesh.h"
#include "Core/Components/PointLightComponent.h"
#include "Core/Components/StaticMeshComponent.h"

class RendererAPI {
public:
	void BeginRenderPass(const std::string& name, const std::shared_ptr<BaseFramebuffer>& framebuffer, const std::shared_ptr<Shader>& shader, const glm::mat4& projectionViewMatrix, glm::vec3 viewPosition);
	void BindShader(const std::shared_ptr<Shader>& shared);
	void SetNewCameraInformation(const glm::mat4& projectionViewMatrix, glm::vec3 viewPosition);

	void SubmitLight(const std::shared_ptr<PointLightComponent>& light);
	void SubmitLightMesh(const std::shared_ptr<PointLightComponent>& light, const std::shared_ptr<Texture>& shadowMap);
	
	void SubmitMesh(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform);
	void SubmitSubmesh(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform);

	void SubmitMeshRaw(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform);
	void SubmitSubmeshRaw(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform);

	void SubmitQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	void SubmitIcon(const std::shared_ptr<Texture2D>& texture, const glm::mat4& Transform);

	void EndRenderPass();

	/*
	void SetFramebuffer(const std::shared_ptr<Framebuffer>& framebuffer);
	
	void SubmitScene(const std::shared_ptr<Scene>& scene);

	void SubmitMesh(const std::shared_ptr<StaticMeshComponent>& mesh, const glm::mat4& parentTransformation);
	
	void Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& Transform);
	void Submit(const std::shared_ptr<Shader>& shader, const VertexArray& vertexArray, const glm::mat4& Transform);

	void Submit(const std::string& text, const Font& font, float x, float y, float scale, glm::vec3 color, const std::shared_ptr<Shader>& shader);

	*/

	void SetClearColor(float r, float g, float b, float a);

	void ClearDepthTarget();
	void ClearColorTarget();

	void EnableFaceCulling();
	void DisableFaceCulling();

	void CullFrontFace();
	void CullBackFace();

	void EnableBlending();
	void DisableBlending();
	void SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor);
	
	
	static RendererAPI& GetRendererAPI();
private:
	std::string m_RenderPassName;
	std::shared_ptr<Framebuffer> m_RenderPassFramebuffer;
	std::shared_ptr<Shader> m_Shader;

	int32_t m_LightCount = 0;
	
	glm::mat4 m_ProjectionViewMatrix;
	glm::vec3 m_ViewPosition;
	glm::vec4 m_Up;

	std::shared_ptr<VertexArray> m_QuadVAO;
	std::shared_ptr<VertexBuffer> m_QuadVBO;

	std::shared_ptr<VertexBuffer> m_LightMeshVBO;
	std::shared_ptr<VertexArray> m_LightMeshVAO;

	VertexArray m_TextVAO;

	VertexArray m_IconVertexArray;


	std::shared_ptr<BaseFramebuffer> m_Framebuffer;

		
	RendererAPI();

	//void SubmitComponent(const std::shared_ptr<Component>& component, const glm::mat4& Transform);
};
