#include "EdRendererPrivate.h"
#include "RenderGraph.h"

// #include "RenderGraph.h"
// #include "Helpers/RenderingHelper.h"
// #include "Passes/RenderPass.h"
// 
// void RenderGraph::AddPass(std::shared_ptr<BaseRenderPass> pass)
// {
// 	if (pass->GetType() == RenderPassType::MultiPass)
// 	{
// 		std::static_pointer_cast<BaseMultiPassRenderPass>(pass)->CreatePasses();
// 	}
// 
// 	m_Passes.push_back(pass);
// }
// 
// void RenderGraph::Build()
// {
// 	InitializePasses();
// 	BuildNodes();
// }
// 
// void RenderGraph::InitializePasses()
// {
// 	for (u32 i = 0; i < m_Passes.size(); ++i)
// 	{
// 		ProcessDeclarations(m_Passes[i], i);
// 	}
// 
// 	for (u32 i = 0; i < m_Passes.size(); ++i)
// 	{
// 		ProcessReferences(m_Passes[i], i);
// 	}
// 
// 	for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
// 	{
// 		pass->Initialize(shared_from_this());
// 		pass->PostInitialization();
// 	}
// }
// 
// void RenderGraph::Update(f32 deltaSeconds)
// {
// 	for (const std::shared_ptr<RenderGraphNode>& node : m_Nodes)
// 	{
// 		node->bHasFinished = false;
// 
// 		if (node->Upstream.empty())
// 		{
// 			m_ExecutionQueue.push(node);
// 		}
// 	}
// 
// 	while (!m_ExecutionQueue.empty())
// 	{
// 		std::shared_ptr<RenderGraphNode> node = m_ExecutionQueue.front();
// 		m_ExecutionQueue.pop();
// 
// 		if (node->CanStart())
// 		{
// 			std::shared_ptr<BaseRenderPass> pass = node->Pass;
// 
// 			ExecutePass(pass);
// 
// 			node->bHasFinished = true;
// 
// 			for (const std::shared_ptr<RenderGraphNode>& downstreamNode : node->Downstream)
// 			{
// 				m_ExecutionQueue.push(downstreamNode);
// 			}
// 		}
// 		else
// 		{
// 			if (!node->bHasFinished)
// 			{
// 				m_ExecutionQueue.push(node);
// 			}
// 		}
// 	}
// }
// 
// std::shared_ptr<Renderer> RenderGraph::GetRenderer() const
// {
// 	return m_Renderer;
// }
// 
// std::shared_ptr<RenderingContext> RenderGraph::GetContext() const
// {
// 	return m_Context;
// }
// 
// std::shared_ptr<Resource>& RenderGraph::GetResource(const std::string& name) const
// {
// 	return *m_Resources.at(name);
// }
// 
// void RenderGraph::BeginPass(const RenderPassParameters& inParameters)
// {
// 	switch (inParameters.Type)
// 	{
// 		case RenderPassType::Base:
// 		{
// 			const BaseRenderPassParameters& parameters = static_cast<const BaseRenderPassParameters&>(inParameters);
// 
// 			m_Context->SetShader(parameters.Shader);
// 
// 			m_Context->SetFramebuffer(parameters.DrawFramebuffer);
// 
// 			if (parameters.bUseBlending)
// 			{
// 				m_Context->EnableBlending(parameters.SourceFactor, parameters.DestinationFactor);
// 			}
// 			else
// 			{
// 				m_Context->DisableBlending();
// 			}
// 
// 			if (parameters.bUseDepthTesting)
// 			{
// 				m_Context->EnableDethTest(parameters.DepthFunction);
// 			}
// 			else
// 			{
// 				m_Context->DisableDethTest();
// 			}
// 
// 			if (parameters.bClearColors)
// 			{
// 				m_Context->ClearColorTarget();
// 			}
// 
// 			if (parameters.bClearDepth)
// 			{
// 				m_Context->ClearDepthTarget();
// 			}
// 
// 			if (parameters.bEnableFaceCulling)
// 			{
// 				m_Context->EnableFaceCulling(parameters.FaceToCull);
// 			}
// 			else
// 			{
// 				m_Context->DisableFaceCulling();
// 			}
// 		} break;
// 		case RenderPassType::Compute:
// 		{
// 			const ComputeRenderPassParameters& parameters = static_cast<const ComputeRenderPassParameters&>(inParameters);
// 			m_Context->SetShader(parameters.Shader);
// 		} break;
// 		case RenderPassType::MultiPass:
// 			break;
// 		default:
// 			ED_ASSERT(0, "Unsupported render pass");
// 	}
// }
// 
// void RenderGraph::EndPass(const RenderPassParameters& inParameters)
// {
// 	if (inParameters.Type == RenderPassType::Base)
// 	{
// 		m_Context->SetDefaultFramebuffer();
// 	}
// }
// 
// void RenderGraph::ProcessDeclarations(std::shared_ptr<BaseRenderPass> pass, u32 index)
// {
// 	RenderPassParameters& parameters = pass->GetBaseParameters();
// 
// 	if (parameters.Type == RenderPassType::Base)
// 	{
// 		BaseRenderPassParameters& castedParameters = static_cast<BaseRenderPassParameters&>(parameters);
// 
// 		std::shared_ptr<Framebuffer>& framebuffer = castedParameters.DrawFramebuffer;
// 
// 		FramebufferSpecification specification;
// 		specification.Name = "DrawFramebuffer";
// 		framebuffer = RenderingHelper::CreateFramebuffer(specification);
// 
// 		for (RenderTargetDeclaration* declaration : parameters.GetRenderTargetDeclarations())
// 		{
// 			std::shared_ptr<Texture> renderTarget = declaration->Declare(shared_from_this());
// 			framebuffer->AddAttachment(renderTarget);
// 			
// 			ResourceUsages usage;
// 			usage.Declaration = index;
// 			m_ResourceUsages[declaration->ResourceName] = usage;
// 		}
// 	}
// 
// 	for (Declaration* declaration : parameters.GetDeclarations())
// 	{
// 		declaration->Declare(shared_from_this());
// 
// 		ResourceUsages usage;
// 		usage.Declaration = index;
// 		m_ResourceUsages[declaration->ResourceName] = usage;
// 	}
// 
// 	if (pass->GetBaseParameters().Type == RenderPassType::MultiPass)
// 	{
// 		std::shared_ptr<BaseMultiPassRenderPass> castedPass = std::static_pointer_cast<BaseMultiPassRenderPass>(pass);
// 		for (const std::shared_ptr<BaseRenderPass>& innerPass : castedPass->GetRenderPasses())
// 		{
// 			ProcessDeclarations(innerPass, index);
// 		}
// 	}
// }
// 
// void RenderGraph::ProcessReferences(std::shared_ptr<BaseRenderPass> pass, u32 index)
// {
// 	RenderPassParameters& parameters = pass->GetBaseParameters();
// 
// 	if (parameters.Type == RenderPassType::Base)
// 	{
// 		BaseRenderPassParameters& castedParameters = static_cast<BaseRenderPassParameters&>(parameters);
// 
// 		if (!castedParameters.DrawFramebuffer)
// 		{
// 			FramebufferSpecification specification;
// 			specification.Name = "DrawFramebuffer";
// 			castedParameters.DrawFramebuffer = RenderingHelper::CreateFramebuffer(specification);
// 		}
// 
// 		std::shared_ptr<Framebuffer>& framebuffer = castedParameters.DrawFramebuffer;
// 
// 		for (RenderTargetReference* reference : parameters.GetRenderTargetReferences())
// 		{
// 			std::shared_ptr<Texture> renderTarget = reference->SetValue(shared_from_this());
// 			framebuffer->AddAttachment(renderTarget);
// 
// 			m_ResourceUsages[reference->ResourceName].AddWriter(index);
// 		}
// 	}
// 
// 	for (Reference* reference : pass->GetBaseParameters().GetReferences())
// 	{
// 		reference->SetValue(shared_from_this());
// 
// 		if (reference->AccessMode == ReferenceAccessMode::Read)
// 		{
// 
// 			m_ResourceUsages[reference->ResourceName].AddReader(index);
// 		}
// 		else
// 		{
// 			m_ResourceUsages[reference->ResourceName].AddWriter(index);
// 		}
// 	}
// 
// 	if (pass->GetBaseParameters().Type == RenderPassType::MultiPass)
// 	{
// 		std::shared_ptr<BaseMultiPassRenderPass> castedPass = std::static_pointer_cast<BaseMultiPassRenderPass>(pass);
// 		for (const std::shared_ptr<BaseRenderPass>& innerPass : castedPass->GetRenderPasses())
// 		{
// 			ProcessReferences(innerPass, index);
// 		}
// 	}
// }
// 
// void RenderGraph::BuildNodes()
// {
// 	for (u32 i = 0; i < m_Passes.size(); ++i)
// 	{
// 		std::shared_ptr<RenderGraphNode> node = std::make_shared<RenderGraphNode>();
// 		node->Pass = m_Passes[i];
// 		m_Nodes.push_back(node);
// 	}
// 
// 	for (auto& [name, usage] : m_ResourceUsages)
// 	{
// 		if (!usage.Writers.empty())
// 		{
// 			if (usage.Declaration != usage.Writers[0])
// 			{
// 				std::shared_ptr<RenderGraphNode> declaration = m_Nodes[usage.Declaration];
// 				std::shared_ptr<RenderGraphNode> firstWriter = m_Nodes[usage.Writers[0]];
// 
// 				declaration->Downstream.push_back(firstWriter);
// 				firstWriter->Upstream.push_back(declaration);
// 			}
// 		}
// 
// 		for (u32 i = 1; i < usage.Writers.size(); ++i)
// 		{
// 			std::shared_ptr<RenderGraphNode> previousWriter = m_Nodes[usage.Writers[i - 1]];
// 			std::shared_ptr<RenderGraphNode> currentWriter = m_Nodes[usage.Writers[i]];
// 
// 			previousWriter->Downstream.push_back(currentWriter);
// 			currentWriter->Upstream.push_back(previousWriter);
// 		}
// 
// 		u32 lastWriterIndex = !usage.Writers.empty() ? *usage.Writers.rbegin() : usage.Declaration;
// 		std::shared_ptr<RenderGraphNode> lastWriter = m_Nodes[lastWriterIndex];
// 
// 		for (u32 Reader : usage.Readers)
// 		{
// 			std::shared_ptr<RenderGraphNode> reader = m_Nodes[Reader];
// 
// 			lastWriter->Downstream.push_back(reader);
// 			reader->Upstream.push_back(lastWriter);
// 		}
// 	}
// 
// 	CheckGraphForCycles();
// 
// 	m_ResourceUsages.clear();
// }
// 
// void RenderGraph::CheckGraphForCycles()
// {
// 	for (const std::shared_ptr<RenderGraphNode>& node : m_Nodes)
// 	{
// 		node->NodeState = RenderGraphNode::NotVisited;
// 	}
// 
// 	for (const std::shared_ptr<RenderGraphNode>& node : m_Nodes)
// 	{
// 		if (node->NodeState == RenderGraphNode::NotVisited)
// 		{
// 			TraverseGraph(node);
// 		}
// 	}
// }
// 
// void RenderGraph::TraverseGraph(std::shared_ptr<RenderGraphNode> node)
// {
// 	ED_ASSERT(node->NodeState != RenderGraphNode::VisitedButNotExited, "Render graph has cycles")
// 
// 	node->NodeState = RenderGraphNode::VisitedButNotExited;
// 
// 	for (const std::shared_ptr<RenderGraphNode>& dowstreamNode : node->Downstream)
// 	{
// 		TraverseGraph(dowstreamNode);
// 	}
// 
// 	node->NodeState = RenderGraphNode::VisitedAndExited;
// }
// 
// void RenderGraph::ExecutePass(std::shared_ptr<BaseRenderPass> pass)
// {
// 	pass->PreExecute();
// 
// 	BeginPass(pass->GetBaseParameters());
// 
// 	pass->Execute();
// 
// 	EndPass(pass->GetBaseParameters());
// }
// 
// void RenderGraph::Initilaize(Renderer* renderer)
// {
// 	m_Renderer = renderer;
// 	m_Context = renderer->GetContext();
// }
// 
// void ResourceUsages::AddWriter(u32 writer)
// {
// 	if (writer == Declaration)
// 	{
// 		return;
// 	}
// 
// 	// This vector will always have small amount of items
// 	for (u32 existingWriter : Writers)
// 	{
// 		if (existingWriter == writer)
// 		{
// 			return;
// 		}
// 	}
// 
// 	Writers.push_back(writer);
// }
// 
// void ResourceUsages::AddReader(u32 reader)
// {
// 	if (reader == Declaration)
// 	{
// 		return;
// 	}
// 
// 	// This vector will always have small amount of items
// 	for (u32 existingReader : Readers)
// 	{
// 		if (existingReader == reader)
// 		{
// 			return;
// 		}
// 	}
// 
// 	Readers.push_back(reader);
// }

RenderTarget* RenderGraph::CreateRenderTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy sizePolicy)
{
#if DEBUG_BUILD || DEVELOPMENT_BUILD
    for (RenderTarget* target : m_RenderTargets)
    {
        ED_ASSERT(0 != strncmp(target->GetName(), name, 1024), "Render target name is alredy occupied.");
    }
#endif

    RenderTarget* target = new RenderTarget(name, format, sizePolicy);
    m_RenderTargets.Add(target);
    return target;
}

RenderTarget* RenderGraph::CreateRenderTarget(ccstr8 name, PixelFormat format, u32 width, u32 height)
{
#if DEBUG_BUILD || DEVELOPMENT_BUILD
    for (RenderTarget* target : m_RenderTargets)
    {
        ED_ASSERT(0 != strncmp(target->GetName(), name, 1024), "Render target name is alredy occupied.");
    }
#endif

    RenderTarget* target = new RenderTarget(name, format, width, height);
    m_RenderTargets.Add(target);
    return target;
}

UAVTarget* RenderGraph::CreateUAVTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy sizePolicy)
{
#if DEBUG_BUILD || DEVELOPMENT_BUILD
    for (UAVTarget* target : m_UAVTargets)
    {
        ED_ASSERT(0 != strncmp(target->GetName(), name, 1024), "UAV target name is alredy occupied.");
    }
#endif

    UAVTarget* target = new UAVTarget(name, format, sizePolicy, 1);
    m_UAVTargets.Add(target);
    return target;
}

UAVTarget* RenderGraph::CreateUAVTarget(ccstr8 name, PixelFormat format, u32 width, u32 height)
{
    return CreateUAVTarget(name, format, width, height, 1);
}

UAVTarget* RenderGraph::CreateUAVTarget(ccstr8 name, PixelFormat format, u32 width, u32 height, u32 mips)
{
#if DEBUG_BUILD || DEVELOPMENT_BUILD
    for (UAVTarget* target : m_UAVTargets)
    {
        ED_ASSERT(0 != strncmp(target->GetName(), name, 1024), "UAV target name is alredy occupied.");
    }
#endif

    UAVTarget* target = new UAVTarget(name, format, width, height, mips);
    m_UAVTargets.Add(target);
    return target;
}

UAVTarget* RenderGraph::CreateUAVTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy sizePolicty, u32 mips)
{
    UAVTarget* target = new UAVTarget(name, format, sizePolicty, mips);
    m_UAVTargets.Add(target);
    return target;
}

RenderTarget* RenderGraph::GetRenderTarget(ccstr8 name) const
{
    // TODO: Replace with cached strings
    for (RenderTarget* target : m_RenderTargets)
    {
        if (strncmp(target->GetName(), name, 1024) == 0)
        {
            return target;
        }
    }

    return nullptr;
}

UAVTarget* RenderGraph::GetUAVTarget(ccstr8 name) const
{
    // TODO: Replace with cached strings
    for (UAVTarget* target : m_UAVTargets)
    {
        if (strncmp(target->GetName(), name, 1024) == 0)
        {
            return target;
        }
    }

    return nullptr;

}

void RenderGraph::Signal(ccstr8 name, CommandQueue* queue)
{
    FenceSignal& signal = m_Signals.Add();
    signal.Queue = queue;
    signal.Name = name;
    signal.Value = queue->Signal();
}

const RenderGraph::FenceSignal* RenderGraph::FindSignal(ccstr8 name)
{
    for (const FenceSignal& signal : m_Signals)
    {
        if (0 == strncmp(signal.Name, name, 1024))
        {
            return &signal;
        }
    }

    return nullptr;
}

void RenderGraph::Wait(RenderPass* pass, CommandQueue* queue)
{
    ccstr8* signals;
    u32 count;
    pass->GetWaitSignals(&signals, count);

    for (u32 i = 0; i < count; ++i)
    {
        const FenceSignal* signal = FindSignal(signals[i]);
        if (signal)
        {
            queue->GPUWait(signal->Queue, signal->Value);
        }
    }
}

CommandQueue* RenderGraph::SelectQueue(RenderPass* pass, CommandList* list)
{
    switch (pass->GetCommandQueueType())
    {
    case CommandListType::Direct: return m_Context->GetGraphicsCommandQueue();
    case CommandListType::Compute: return m_Context->GetComputeCommandQueue();
    case CommandListType::Copy: return m_Context->GetCopyCommandQueue();
    case CommandListType::Bundle:
    default:
        ED_ASSERT(0, "Selected command list type is not supported for render pass.");
    }

    return nullptr;
}

void RenderGraph::SetupState(RenderPass* pass, CommandList* list)
{
    list->Reset();

    list->Transition(m_ConstantBuffer, ResourceState::VertexAndConstantBuffer);

    list->SetRootSignature(m_Renderer->GetRootSignature(), list->GetType() == CommandListType::Compute);

    list->SetDescriptorHeap(m_Context->GetSRVHeap());

    switch (list->GetType())
    {
    case CommandListType::Direct:
    {
        list->SetGraphicsConstantBufferView(0, m_ConstantBuffer);
        list->SetGraphicsRootDescriptorTable(4, m_Context->GetSRVHeap()->GetStartLocation());
        list->SetGraphicsRootDescriptorTable(5, m_Context->GetSRVHeap()->GetStartLocation());
        list->SetGraphicsRootDescriptorTable(6, m_Context->GetSRVHeap()->GetStartLocation());
        list->SetGraphicsRootDescriptorTable(7, m_Context->GetSRVHeap()->GetStartLocation());
        break;
    }
    case CommandListType::Compute:
    {
        list->SetComputeConstantBufferView(0, m_ConstantBuffer);
        list->SetComputeRootDescriptorTable(4, m_Context->GetSRVHeap()->GetStartLocation());
        list->SetComputeRootDescriptorTable(5, m_Context->GetSRVHeap()->GetStartLocation());
        list->SetComputeRootDescriptorTable(6, m_Context->GetSRVHeap()->GetStartLocation());
        list->SetComputeRootDescriptorTable(7, m_Context->GetSRVHeap()->GetStartLocation());
        break;
    }
    default:
        break;
    }
}

void RenderGraph::RecordCommands(RenderPass* pass, CommandList* list)
{
    list->BeginEvent(pass->GetName());

    pass->Execute(list, m_ConstantBuffer, m_ParametersOffset);

    list->EndEvent();

    u32 size = pass->GetCustomShaderParametersStructSize();
    m_ParametersOffset = Align(m_ParametersOffset + size, CBVAlignment);
}

void RenderGraph::Initialize()
{
    m_Renderer = &Renderer::Get();
    m_Context = &RenderingContext::Get();

    m_ConstantBuffer = m_Context->GetUploadBuffersHeap()->CreateResource(RF_None, ResourceState::Common, 1, Memory::BytesToMB, "ConstantBuffer");
    m_ConstantBufferBaseOffset = CBVAlignment;
}

void RenderGraph::Deinitialize()
{
    for (RenderTarget* target : m_RenderTargets)
    {
        delete target;
    }
    m_RenderTargets.Clear();

    for (UAVTarget* target : m_UAVTargets)
    {
        delete target;
    }
    m_UAVTargets.Clear();

    for (RenderPass* pass : m_Passes)
    {
        delete pass;
    }
    m_Passes.Clear();
}

void RenderGraph::InitializePasses()
{
    for (RenderPass* pass : m_Passes)
    {
        pass->Initialize(this);
    }
}

void RenderGraph::Gather()
{
    CommandQueue* queue = m_Context->GetCopyCommandQueue();

    queue->BeginEvent("Gather Parameter");

    const u32 globalOffset = CBVAlignment * 2;

    u32 totalSize = globalOffset;

    TemporaryArray<u32> sizes(m_Passes.GetSize() + 1);

    for (u32 i = 0; i < m_Passes.GetSize(); ++i)
    {
        RenderPass* pass = m_Passes[i];
        
        u32 size = pass->GetCustomShaderParametersStructSize();
        totalSize = Align(totalSize + size, CBVAlignment);

        sizes.Add(size);
    }

    void* memory;
    m_ConstantBuffer->Map(0, 0, totalSize, reinterpret_cast<void**>(&memory));

    GatherCommonShaderParameters(memory);
    memory = OffsetBuffer(memory, globalOffset);

    u32 offset = 0;
    for (u32 i = 0; i < m_Passes.GetSize(); ++i)
    {
        u32 size = sizes[i];

        if (size != 0)
        {
            RenderPass* pass = m_Passes[i];
            pass->GatherCustomShaderParameters(OffsetBuffer(memory, offset));
            offset = Align(offset + size, CBVAlignment);
        }
    }

    m_ConstantBuffer->Unmap(0, 0, totalSize);


    queue->EndEvent();
}

void RenderGraph::Execute()
{
    m_ParametersOffset = CBVAlignment * 2;

    m_Signals.Clear();

    for (RenderPass* pass : m_Passes)
    {
        CommandList* list = pass->GetList();
        CommandQueue* queue = SelectQueue(pass, list);

        Wait(pass, queue);

        SetupState(pass, list);

        RecordCommands(pass, list);

        list->Close();
        queue->Execute(list);

        Signal(pass->GetName(), queue);
    }
}

void RenderGraph::GatherCommonShaderParameters(void* memory)
{
    const Camera& camera = m_Renderer->GetCamera();

    glm::mat4 view = camera.GetView();
    memory = PutValue(memory, glm::value_ptr(view), sizeof(glm::mat4));

    glm::mat4 projection = camera.GetProjection();
    memory = PutValue(memory, glm::value_ptr(projection), sizeof(glm::mat4));

    glm::mat4 projectionView = camera.GetProjectionView();
    memory = PutValue(memory, glm::value_ptr(projectionView), sizeof(glm::mat4));

    glm::mat4 inverseProjectionView = glm::inverse(projectionView);
    memory = PutValue(memory, glm::value_ptr(inverseProjectionView), sizeof(glm::mat4));

    Window* window = m_Context->GetWindow();
    memory = PutValue<u32>(memory, window->GetWidth());
    memory = PutValue<u32>(memory, window->GetHeight());
    memory = PutValue<f32>(memory, 1.0f / window->GetWidth());
    memory = PutValue<f32>(memory, 1.0f / window->GetHeight());
}
