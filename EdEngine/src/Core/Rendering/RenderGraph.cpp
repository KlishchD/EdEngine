#include "RenderGraph.h"
#include "Helpers/RenderingHelper.h"
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
	InitializePasses();
	BuildNodes();
}

void RenderGraph::InitializePasses()
{
	for (uint32_t i = 0; i < m_Passes.size(); ++i)
	{
		ProcessDeclarations(m_Passes[i], i);
	}

	for (uint32_t i = 0; i < m_Passes.size(); ++i)
	{
		ProcessReferences(m_Passes[i], i);
	}

	for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
	{
		pass->Initialize(shared_from_this());
		pass->PostInitialization();
	}
}

void RenderGraph::Update(float deltaSeconds)
{
	for (const std::shared_ptr<RenderGraphNode>& node : m_Nodes)
	{
		node->bHasFinished = false;

		if (node->Upstream.empty())
		{
			m_ExecutionQueue.push(node);
		}
	}

	while (!m_ExecutionQueue.empty())
	{
		std::shared_ptr<RenderGraphNode> node = m_ExecutionQueue.front();
		m_ExecutionQueue.pop();

		if (node->CanStart())
		{
			std::shared_ptr<BaseRenderPass> pass = node->Pass;

			ExecutePass(pass);

			node->bHasFinished = true;

			for (const std::shared_ptr<RenderGraphNode>& downstreamNode : node->Downstream)
			{
				m_ExecutionQueue.push(downstreamNode);
			}
		}
		else
		{
			if (!node->bHasFinished)
			{
				m_ExecutionQueue.push(node);
			}
		}
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

void RenderGraph::ProcessDeclarations(std::shared_ptr<BaseRenderPass> pass, uint32_t index)
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
			
			ResourceUsages usage;
			usage.Declaration = index;
			m_ResourceUsages[declaration->ResourceName] = usage;
		}
	}

	for (Declaration* declaration : parameters.GetDeclarations())
	{
		declaration->Declare(shared_from_this());

		ResourceUsages usage;
		usage.Declaration = index;
		m_ResourceUsages[declaration->ResourceName] = usage;
	}

	if (pass->GetBaseParameters().Type == RenderPassType::MultiPass)
	{
		std::shared_ptr<BaseMultiPassRenderPass> castedPass = std::static_pointer_cast<BaseMultiPassRenderPass>(pass);
		for (const std::shared_ptr<BaseRenderPass>& innerPass : castedPass->GetRenderPasses())
		{
			ProcessDeclarations(innerPass, index);
		}
	}
}

void RenderGraph::ProcessReferences(std::shared_ptr<BaseRenderPass> pass, uint32_t index)
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

			m_ResourceUsages[reference->ResourceName].AddWriter(index);
		}
	}

	for (Reference* reference : pass->GetBaseParameters().GetReferences())
	{
		reference->SetValue(shared_from_this());

		if (reference->AccessMode == ReferenceAccessMode::Read)
		{

			m_ResourceUsages[reference->ResourceName].AddReader(index);
		}
		else
		{
			m_ResourceUsages[reference->ResourceName].AddWriter(index);
		}
	}

	if (pass->GetBaseParameters().Type == RenderPassType::MultiPass)
	{
		std::shared_ptr<BaseMultiPassRenderPass> castedPass = std::static_pointer_cast<BaseMultiPassRenderPass>(pass);
		for (const std::shared_ptr<BaseRenderPass>& innerPass : castedPass->GetRenderPasses())
		{
			ProcessReferences(innerPass, index);
		}
	}
}

void RenderGraph::BuildNodes()
{
	for (uint32_t i = 0; i < m_Passes.size(); ++i)
	{
		std::shared_ptr<RenderGraphNode> node = std::make_shared<RenderGraphNode>();
		node->Pass = m_Passes[i];
		m_Nodes.push_back(node);
	}

	for (auto& [name, usage] : m_ResourceUsages)
	{
		if (!usage.Writers.empty())
		{
			if (usage.Declaration != usage.Writers[0])
			{
				std::shared_ptr<RenderGraphNode> declaration = m_Nodes[usage.Declaration];
				std::shared_ptr<RenderGraphNode> firstWriter = m_Nodes[usage.Writers[0]];

				declaration->Downstream.push_back(firstWriter);
				firstWriter->Upstream.push_back(declaration);
			}
		}

		for (uint32_t i = 1; i < usage.Writers.size(); ++i)
		{
			std::shared_ptr<RenderGraphNode> previousWriter = m_Nodes[usage.Writers[i - 1]];
			std::shared_ptr<RenderGraphNode> currentWriter = m_Nodes[usage.Writers[i]];

			previousWriter->Downstream.push_back(currentWriter);
			currentWriter->Upstream.push_back(previousWriter);
		}

		uint32_t lastWriterIndex = !usage.Writers.empty() ? *usage.Writers.rbegin() : usage.Declaration;
		std::shared_ptr<RenderGraphNode> lastWriter = m_Nodes[lastWriterIndex];

		for (uint32_t Reader : usage.Readers)
		{
			std::shared_ptr<RenderGraphNode> reader = m_Nodes[Reader];

			lastWriter->Downstream.push_back(reader);
			reader->Upstream.push_back(lastWriter);
		}
	}

	CheckGraphForCycles();

	m_ResourceUsages.clear();
}

void RenderGraph::CheckGraphForCycles()
{
	for (const std::shared_ptr<RenderGraphNode>& node : m_Nodes)
	{
		node->NodeState = RenderGraphNode::NotVisited;
	}

	for (const std::shared_ptr<RenderGraphNode>& node : m_Nodes)
	{
		if (node->NodeState == RenderGraphNode::NotVisited)
		{
			TraverseGraph(node);
		}
	}
}

void RenderGraph::TraverseGraph(std::shared_ptr<RenderGraphNode> node)
{
	ED_ASSERT(node->NodeState != RenderGraphNode::VisitedButNotExited, "Render graph has cycles")

	node->NodeState = RenderGraphNode::VisitedButNotExited;

	for (const std::shared_ptr<RenderGraphNode>& dowstreamNode : node->Downstream)
	{
		TraverseGraph(dowstreamNode);
	}

	node->NodeState = RenderGraphNode::VisitedAndExited;
}

void RenderGraph::ExecutePass(std::shared_ptr<BaseRenderPass> pass)
{
	pass->PreExecute();

	BeginPass(pass->GetBaseParameters());

	pass->Execute();

	EndPass(pass->GetBaseParameters());
}

void RenderGraph::Initilaize(std::shared_ptr<Renderer> renderer)
{
	m_Renderer = renderer;
	m_Context = renderer->GetContext();
}

void ResourceUsages::AddWriter(uint32_t writer)
{
	if (writer == Declaration)
	{
		return;
	}

	// This vector will always have small amount of items
	for (uint32_t existingWriter : Writers)
	{
		if (existingWriter == writer)
		{
			return;
		}
	}

	Writers.push_back(writer);
}

void ResourceUsages::AddReader(uint32_t reader)
{
	if (reader == Declaration)
	{
		return;
	}

	// This vector will always have small amount of items
	for (uint32_t existingReader : Readers)
	{
		if (existingReader == reader)
		{
			return;
		}
	}

	Readers.push_back(reader);
}
