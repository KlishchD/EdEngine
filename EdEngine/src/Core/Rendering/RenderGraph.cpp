#include "RenderGraph.h"
#include "Utils/RenderingHelper.h"
#include "Passes/RenderPass.h"

void RenderGraph::AddPass(std::shared_ptr<BaseRenderPass> pass)
{
	if (pass->GetType() == RenderPassType::MultiPass)
	{
		std::static_pointer_cast<BaseMultiPassRenderPass>(pass)->CreatePasses();
	}

	m_Passes.push_back(pass);
}

void RenderGraph::Build()
{
	for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
	{
		ProcessDeclarations(pass);

		if (pass->GetBaseParameters().Type == RenderPassType::MultiPass)
		{
			std::shared_ptr<BaseMultiPassRenderPass> castedPass = std::static_pointer_cast<BaseMultiPassRenderPass>(pass);
			for (const std::shared_ptr<BaseRenderPass>& innerPass : castedPass->GetRenderPasses())
			{
				ProcessDeclarations(innerPass);
			}
		}
	}

	for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
	{
		ProcessReferences(pass);

		if (pass->GetBaseParameters().Type == RenderPassType::MultiPass)
		{
			std::shared_ptr<BaseMultiPassRenderPass> castedPass = std::static_pointer_cast<BaseMultiPassRenderPass>(pass);
			for (const std::shared_ptr<BaseRenderPass>& innerPass : castedPass->GetRenderPasses())
			{
				ProcessReferences(innerPass);
			}
		}
	}

	for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
	{
		pass->Initialize(shared_from_this());

		if (pass->GetBaseParameters().Type == RenderPassType::MultiPass)
		{
			std::shared_ptr<BaseMultiPassRenderPass> castedPass = std::static_pointer_cast<BaseMultiPassRenderPass>(pass);
			for (const std::shared_ptr<BaseRenderPass>& innerPass : castedPass->GetRenderPasses())
			{
				innerPass->Initialize(shared_from_this());
			}
		}
	}
}

void RenderGraph::InitializePasses()
{
	for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
	{
		SetInitializedRenderPass(pass);
		pass->Initialize(shared_from_this());
	}
}

void RenderGraph::Update(float deltaSeconds)
{
	for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
	{
		pass->PreUpdate();

		BeginPass(pass->GetBaseParameters());

		pass->Update(deltaSeconds);

		EndPass(pass->GetBaseParameters());
	}
}

void RenderGraph::SetCanOverrideInitializedPass(bool state)
{
	m_bCanOverrideInitializedPass = state;
}

void RenderGraph::SetInitializedRenderPass(std::shared_ptr<BaseRenderPass> pass)
{
	if (m_bCanOverrideInitializedPass)
	{
		m_InitializedRenderPass = pass;
	}
}

std::shared_ptr<Renderer> RenderGraph::GetRenderer() const
{
	return m_Renderer;
}

std::shared_ptr<RenderingContext> RenderGraph::GetContext() const
{
	return m_Context;
}

std::shared_ptr<Resource>& RenderGraph::GetResource(const std::string& name) const
{
	return *m_Resources.at(name);
}

void RenderGraph::BeginPass(const RenderPassParameters& inParameters)
{
	switch (inParameters.Type)
	{
		case RenderPassType::Base:
		{
			const BaseRenderPassParameters& parameters = static_cast<const BaseRenderPassParameters&>(inParameters);

			m_Context->SetShader(parameters.Shader);

			m_Context->SetFramebuffer(parameters.DrawFramebuffer);

			if (parameters.bUseBlending)
			{
				m_Context->EnableBlending(parameters.SourceFactor, parameters.DestinationFactor);
			}
			else
			{
				m_Context->DisableBlending();
			}

			if (parameters.bUseDepthTesting)
			{
				m_Context->EnableDethTest(parameters.DepthFunction);
			}
			else
			{
				m_Context->DisableDethTest();
			}

			if (parameters.bClearColors)
			{
				m_Context->ClearColorTarget();
			}

			if (parameters.bClearDepth)
			{
				m_Context->ClearDepthTarget();
			}

			if (parameters.bEnableFaceCulling)
			{
				m_Context->EnableFaceCulling(parameters.FaceToCull);
			}
			else
			{
				m_Context->DisableFaceCulling();
			}
		} break;
		case RenderPassType::Compute:
		{
			const ComputeRenderPassParameters& parameters = static_cast<const ComputeRenderPassParameters&>(inParameters);
			m_Context->SetShader(parameters.Shader);
		} break;
		case RenderPassType::MultiPass:
			break;
		default:
			ED_ASSERT(0, "Unsupported render pass");
	}
}

void RenderGraph::EndPass(const RenderPassParameters& inParameters)
{
	if (inParameters.Type == RenderPassType::Base)
	{
		m_Context->SetDefaultFramebuffer();
	}
}

void RenderGraph::ProcessDeclarations(std::shared_ptr<BaseRenderPass> pass)
{
	RenderPassParameters& parameters = pass->GetBaseParameters();

	if (parameters.Type == RenderPassType::Base)
	{
		BaseRenderPassParameters& castedParameters = static_cast<BaseRenderPassParameters&>(parameters);

		std::shared_ptr<Framebuffer>& framebuffer = castedParameters.DrawFramebuffer;

		FramebufferSpecification specification;
		specification.Name = "DrawFramebuffer";
		framebuffer = RenderingHelper::CreateFramebuffer(specification);

		for (RenderTargetDeclaration* declaration : parameters.GetRenderTargetDeclarations())
		{
			std::shared_ptr<Texture> renderTarget = declaration->Declare(shared_from_this());
			framebuffer->AddAttachment(renderTarget);
		}
	}

	for (Declaration* declaration : parameters.GetDeclarations())
	{
		declaration->Declare(shared_from_this());
	}
}

void RenderGraph::ProcessReferences(std::shared_ptr<BaseRenderPass> pass)
{
	RenderPassParameters& parameters = pass->GetBaseParameters();

	if (parameters.Type == RenderPassType::Base)
	{
		BaseRenderPassParameters& castedParameters = static_cast<BaseRenderPassParameters&>(parameters);

		if (!castedParameters.DrawFramebuffer)
		{
			FramebufferSpecification specification;
			specification.Name = "DrawFramebuffer";
			castedParameters.DrawFramebuffer = RenderingHelper::CreateFramebuffer(specification);
		}

		std::shared_ptr<Framebuffer>& framebuffer = castedParameters.DrawFramebuffer;

		for (RenderTargetReference* reference : parameters.GetRenderTargetReferences())
		{
			std::shared_ptr<Texture> renderTarget = reference->SetValue(shared_from_this());
			framebuffer->AddAttachment(renderTarget);
		}
	}

	for (Reference* reference : pass->GetBaseParameters().GetReferences())
	{
		reference->SetValue(shared_from_this());
	}
}

void RenderGraph::Initilaize(std::shared_ptr<Renderer> renderer)
{
	m_Renderer = renderer;
	m_Context = renderer->GetContext();
}
