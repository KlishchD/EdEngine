#include "RendererAPI.h"

#include "VertexArray.h"
#include "Core/Scene.h"
#include "Core/Window.h"
#include "Core/Assets/StaticMesh.h"
#include <glm/fwd.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "Utils/GeometryBuilder.h"

void RendererAPI::BeginRenderPass(const std::string& name, const std::shared_ptr<BaseFramebuffer>& framebuffer, const std::shared_ptr<Shader>& shader, const glm::mat4& projectionViewMatrix, glm::vec3 viewPosition)
{
	m_RenderPassName = name;
	m_Framebuffer = framebuffer;
	m_ProjectionViewMatrix = projectionViewMatrix;
	m_ViewPosition = viewPosition;

	m_Framebuffer->Bind();

	BindShader(shader);
}

void RendererAPI::BindShader(const std::shared_ptr<Shader>& shader)
{
	m_Shader = shader;
	
	m_Shader->Bind();
	m_Shader->SetMat4("u_ProjectionViewMatrix", m_ProjectionViewMatrix);
	m_Shader->SetFloat3("u_ViewPosition", m_ViewPosition);
	m_Shader->SetFloat2("u_ScreenSize", m_Framebuffer->GetWidth(), m_Framebuffer->GetHeight());
}

void RendererAPI::SetNewCameraInformation(const glm::mat4& projectionViewMatrix, glm::vec3 viewPosition)
{
	m_ProjectionViewMatrix = projectionViewMatrix;
	m_ViewPosition = viewPosition;

	m_Shader->SetMat4("u_ProjectionViewMatrix", m_ProjectionViewMatrix);
	m_Shader->SetFloat3("u_ViewPosition", m_ViewPosition);
}

void RendererAPI::SubmitLightMesh(const std::shared_ptr<PointLightComponent>& light, const std::shared_ptr<Texture>& shadowMap)
{
	glm::vec3 color = light->GetColor();
	float Imax = std::max(color.x, std::max(color.y, color.z));
	float radius = std::sqrt(light->GetIntensity() * Imax) * 16.0f;

	if (glm::length(m_ViewPosition - light->GetPosition()) * 0.9f > radius)
	{
		glCullFace(GL_BACK);
	}
	else
	{
		glCullFace(GL_FRONT);	
	}

	glm::mat4 transform = glm::scale(light->GetTransform().GetMatrixWithOutScale(), glm::vec3(radius));
	m_Shader->SetMat4("u_ModelMatrix", transform);

	
	m_Shader->SetFloat3("u_PointLight.Position", light->GetPosition());
	m_Shader->SetFloat3("u_PointLight.Color", light->GetColor());
	m_Shader->SetFloat("u_PointLight.Intensity", light->GetIntensity());
	m_Shader->SetFloat("u_PointLight.Radius", radius);
	if (shadowMap)
	{
		shadowMap->Bind(4);
		m_Shader->SetInt("u_PointLight.ShadowMap", 4);
		m_Shader->SetBool("u_PointLight.UseShadowMap", true);
	}
	else
	{
		m_Shader->SetBool("u_PointLight.UseShadowMap", false);
	}
	
	m_LightMeshVAO->Bind();
	glDrawElements(GL_TRIANGLES, m_LightMeshVAO->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr);
}

void RendererAPI::SubmitLight(const std::shared_ptr<PointLightComponent>& light)
{
	std::string front = "u_PointLights[" + std::to_string(m_LightCount) + "]";
	m_Shader->SetFloat3(front + ".Position", light->GetPosition());
	m_Shader->SetFloat3(front + ".Color", light->GetColor());
	m_Shader->SetFloat(front + ".Intensity", light->GetIntensity());

	m_LightCount++;
	m_Shader->SetInt("u_PointLightsCount", m_LightCount);
}

void RendererAPI::SubmitMesh(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform)
{
	for (const std::shared_ptr<StaticSubmesh>& submesh: mesh->GetSubmeshes())
	{
		SubmitSubmesh(submesh, transform);
	}
}

void RendererAPI::SubmitSubmesh(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform)
{
	if (std::shared_ptr<Material> material = submesh->GetMaterial()) {
		submesh->GetVertexArray()->Bind();
		material->SetShaderData(m_Shader);

		m_Shader->SetMat4("u_ModelMatrix", transform.GetMatrix());
		m_Shader->SetMat3("u_NormalMatrix", transform.GetInversedTransposedMatrix());

		glDrawElements(GL_TRIANGLES, submesh->GetVertexArray()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}

void RendererAPI::SubmitMeshRaw(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform)
{
	for (const std::shared_ptr<StaticSubmesh>& submesh: mesh->GetSubmeshes())
	{
		SubmitSubmeshRaw(submesh, transform);
	}
}

void RendererAPI::SubmitSubmeshRaw(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform)
{
	if (std::shared_ptr<Material> material = submesh->GetMaterial()) {
		submesh->GetVertexArray()->Bind();

		m_Shader->SetMat4("u_ModelMatrix", transform.GetMatrix());

		glDrawElements(GL_TRIANGLES, submesh->GetVertexArray()->GetCount(), GL_UNSIGNED_INT, nullptr);
	}
}

void RendererAPI::SubmitQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	float quad[16] = {
		x1, y1,
		x2, y2,
		x3, y3,
		
		x3, y3,
		x4, y4,
		x1, y1
	};

	m_QuadVAO->Bind();
	m_QuadVBO->SetData(quad);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void RendererAPI::EndRenderPass()
{
	m_Shader->Unbind();
	m_Framebuffer->Unbind();
	
	m_RenderPassName.clear();
	m_Framebuffer = nullptr;
	m_Shader = nullptr;
	m_ProjectionViewMatrix = glm::mat4(1);
	m_ViewPosition = glm::vec3(0);
	m_LightCount = 0;
}

void RendererAPI::SetClearColor(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
}

void RendererAPI::ClearDepthTarget()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RendererAPI::ClearColorTarget()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void RendererAPI::EnableFaceCulling()
{
	glEnable(GL_CULL_FACE);
}

void RendererAPI::DisableFaceCulling()
{
	glDisable(GL_CULL_FACE);
}

void RendererAPI::EnableBlending()
{
	glEnable(GL_BLEND);
}

void RendererAPI::DisableBlending()
{
	glDisable(GL_BLEND);
}

void RendererAPI::SetBlendFunction(BlendFactor sourceFactor, BlendFactor destinationFactor)
{
	glBlendFunc(RenderAPIUtils::GetBlendFactorOpenGL(sourceFactor), RenderAPIUtils::GetBlendFactorOpenGL(destinationFactor));
}

/*
void RendererAPI::SetFramebuffer(const std::shared_ptr<Framebuffer>& framebuffer)
{
	if (m_Framebuffer)
	{
		m_Framebuffer->Unbind();
	}

	m_Framebuffer = framebuffer;
	

	if (m_Framebuffer)
	{
		m_Framebuffer->Bind();
	}
}

void RendererAPI::SubmitComponent(const std::shared_ptr<Component>& component, const glm::mat4& Transform)
{
	glm::mat4 componentTransform = glm::mat4(1);
	
	if (component->GetType() == ComponentType::StaticMesh)
	{
		std::shared_ptr<StaticMeshComponent> staticMeshComponent = std::static_pointer_cast<StaticMeshComponent>(component);
		SubmitMesh(staticMeshComponent, Transform);
	}
	
	for (const std::shared_ptr<Component>& child: component->GetChildren())
	{
		SubmitComponent(child, Transform * componentTransform);
	}
}*/

/*
void RendererAPI::SubmitScene(const std::shared_ptr<Scene>& scene)
{
	std::vector<std::shared_ptr<Component>> components = scene->GetAllComponents();
	for (const auto& component: components)
	{
		if (component->GetType() == ComponentType::PointLight)
		{
			AddLight(std::static_pointer_cast<PointLightComponent>(component));
		}
	}
	
	for (const auto& actor: scene->GetActors())
	{
		for (const std::shared_ptr<Component>& component: actor->GetComponents())
		{
			SubmitComponent(component, actor->GetTransform());
		}
	}

#ifdef WITH_EDITOR
	for (const auto& component: components)
	{
		if (component->GetType() == ComponentType::PointLight)
		{
			std::shared_ptr<PointLightComponent> pointLightComponent = std::static_pointer_cast<PointLightComponent>(component);
			SubmitIcon(m_LightIcon, pointLightComponent->GetTransform());			
		}
	}
#endif

}
*/

/*
void RendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& Transform)
{
	Submit(shader, *vertexArray, Transform);
}

void RendererAPI::Submit(const std::shared_ptr<Shader>& shader, const VertexArray& vertexArray, const glm::mat4& Transform)
{
	shader->SetMat4("u_CameraMatrix", m_ProjectionViewMatrix);
	shader->SetMat4("u_Transformation", Transform);
	vertexArray.Bind();
	shader->Bind();
	glDrawArrays(GL_TRIANGLES, 0, vertexArray.GetCount());
}*/

/*
void RendererAPI::Submit(const std::string& text, const Font& font, float x, float y, float scale, glm::vec3 color,
                      const std::shared_ptr<Shader>& shader)
{
	shader->Bind();
	shader->SetMat4("u_CameraMatrix", m_ProjectionViewMatrix);
	shader->SetFloat3("u_Color", color);
	shader->SetInt("u_Texture", 0);

	m_TextVAO.Bind();

	for (const char& c: text)
	{
		const std::shared_ptr<Character>& texture = font.GetCharacterTexture(c);

		float width = texture->GetWidth();
		float height = texture->GetHeight();
		glm::vec2 bearing = texture->GetBearing();

		texture->Bind();

		float xPos = x + bearing.x * scale;
		float yPos = y - (height - bearing.y) * scale;
			
		float data[20] = {
			xPos,                 yPos,                  0.0f, 0.0f, 1.0f,
			xPos,                 yPos + height * scale, 0.0f, 0.0f, 0.0f,
			xPos + width * scale, yPos + height * scale, 0.0f, 1.0f, 0.0f,
			xPos + width * scale, yPos,                  0.0f, 1.0f, 1.0f
		};

		m_TextVAO.GetBuffer(0)->SetData(data);
			
		x += ((int32_t) texture->GetAdvance().x >> 6) * scale;
			
		glDrawArrays(GL_QUADS, 0, 4);
	}
}
*/

void RendererAPI::SubmitIcon(const std::shared_ptr<Texture2D>& texture, const glm::mat4& transform)
{
	m_IconVertexArray.Bind();
	
	m_Shader->SetMat4("u_ModelTransform", transform);
	m_Shader->SetInt("u_IconTexture", 0);
	texture->Bind(0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}

RendererAPI& RendererAPI::GetRendererAPI() {
	static RendererAPI s_RendererAPI;
	return s_RendererAPI;
}

RendererAPI::RendererAPI()
{
	m_TextVAO.AddBuffer(
		nullptr,
		20 * sizeof(float),
		{
			{ "vertex", 1, ShaderDataType::Float3 },
			{ "textureCoordinates", 1, ShaderDataType::Float2 }
		},
		0
	);

	float square[5 * 6] = {
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	};

	//TODO: Maybe use xy of position as texture coordinates ?)


	m_IconVertexArray.AddBuffer(
		square,
		5 * 6 * sizeof(float),
		{
			{ "position", 1, ShaderDataType::Float3 },
			{ "textureCoordinates", 1, ShaderDataType::Float2 }		
		},
		0
	);

	m_QuadVAO = std::make_shared<VertexArray>();
	m_QuadVBO = std::make_shared<VertexBuffer>(nullptr, 16 * sizeof(float));
	m_QuadVBO->SetLayout({ VertexBufferLayoutElement { "position", 1, ShaderDataType::Float2 }});
	m_QuadVAO->AddBuffer(m_QuadVBO, 0);

	auto [vertices, indices] = GeometryBuilder::MakeSphere(1, 50, 50); 

	m_LightMeshVAO = std::make_shared<VertexArray>();
	m_LightMeshVBO = std::make_shared<VertexBuffer>(vertices.data(), sizeof(float) * vertices.size());
	m_LightMeshVBO->SetLayout({ VertexBufferLayoutElement { "position", 1, ShaderDataType::Float3 } });
	m_LightMeshVAO->AddBuffer(m_LightMeshVBO, 0);
	m_LightMeshVAO->SetIndexBuffer(indices.data(), sizeof(int32_t) * indices.size());
}
