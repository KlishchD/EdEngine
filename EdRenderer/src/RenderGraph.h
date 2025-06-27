#pragma once

// #include "RenderingContex.h"
// #include "Renderer.h"
// #include "Passes/Parameters/RenderGraphParameters.h"
// 
// struct RenderPassParameters;
// class BaseRenderPass;
// 
// template<typename ParameterStructClass, typename ShaderParametersClass>
// class RenderPass;
// 
// class Resource // TODO: Add proper resource with composition ;)
// {
// 
// };
// 
// struct ResourceUsages
// {
// 	void AddWriter(u32 writer);
// 	void AddReader(u32 reader);
// 
// 	u32 Declaration;
// 	std::vector<u32> Readers;
// 	std::vector<u32> Writers;
// };
// 
// struct RenderGraphNode
// {
// 	bool CanStart() const
// 	{
// 		for (const std::shared_ptr<RenderGraphNode>& node : Upstream)
// 		{
// 			if (!node->bHasFinished)
// 			{
// 				return false;
// 			}
// 		}
// 
// 		return !bHasFinished;
// 	}
// 
// 	std::shared_ptr<BaseRenderPass> Pass;
// 	bool bHasFinished = false;
// 
// 	std::vector<std::shared_ptr<RenderGraphNode>> Upstream;
// 	std::vector<std::shared_ptr<RenderGraphNode>> Downstream;
// 
// protected:
// 	static const u32 NotVisited = 0;
// 	static const u32 VisitedButNotExited = 1;
// 	static const u32 VisitedAndExited = 2;
// 
// 	u8 NodeState = NotVisited;
// 
// 	friend class RenderGraph;
// };
// 
// class RenderGraph
// {
// public:
// 	void Initilaize(Renderer* renderer);
// 
// 	template<typename T>
// 	T& GetPass()
// 	{
// 		for (BaseRenderPass& pass : m_Passes)
// 		{
// 			if (std::shared_ptr<T> castedPass = std::dynamic_pointer_cast<T>(pass))
// 			{
// 				return castedPass;
// 			}
// 		}
// 
// 		return nullptr;
// 	}
// 
// 	void AddPass(std::shared_ptr<BaseRenderPass> pass);
// 
// 	template<typename T>
// 	void AddPass()
// 	{
// 		AddPass(std::make_shared<T>());
// 	}
// 
// 	void Build();
// 
// 	void Update(f32 deltaSeconds);
// 	void ExecutePass(std::shared_ptr<BaseRenderPass> pass);
// 
// 	std::shared_ptr<Renderer> GetRenderer() const;
// 	std::shared_ptr<RenderingContext> GetContext() const;
// 
// 	template<typename T>
// 	void DeclareResource(const std::string& name, std::shared_ptr<T>& resource)
// 	{
// 		ED_ASSERT(!m_Resources.contains(name), "Resource with this name already exists")
// 		m_Resources[name] = reinterpret_cast<std::shared_ptr<Resource>*>(&resource);
// 	}
// 
// 	std::shared_ptr<Resource>& GetResource(const std::string& name) const;
// 	
// 	template<typename T>
// 	std::shared_ptr<T>& GetResource(const std::string& name) const
// 	{
// 		ED_ASSERT(m_Resources.contains(name), "This parameter doesn't exist")
// 		return *reinterpret_cast<std::shared_ptr<T>*>(m_Resources.at(name));
// 	}
// 	
// 	template<typename T>
// 	void DeclareParameter(const std::string& name, T& value)
// 	{
// 		std::shared_ptr<RenderGraphParemeter<T>> parameter = std::make_shared<RenderGraphParemeter<T>>(name, value);
// 		m_Parameters[name] = std::move(parameter);
// 	}
// 
// 	template<typename T>
// 	void DeclareObjectPtrParameter(const std::string& name, std::shared_ptr<T>& value)
// 	{
// 		std::shared_ptr<RenderGraphObjectPtrParameter<T>> parameter = std::make_shared<RenderGraphObjectPtrParameter<T>>(name, value);
// 		m_Parameters[name] = std::move(parameter);
// 	}
// 
// 	template<typename T>
// 	T& GetParameterValue(const std::string& name)
// 	{
// 		ED_ASSERT(m_Parameters.contains(name), "This parameter doesn't exist")
// 		return std::static_pointer_cast<RenderGraphParemeter<T>>(m_Parameters[name])->GetValue();
// 	}
// 
// 	template<typename T>
// 	std::shared_ptr<T>& GetObjectPtrParameterValue(const std::string& name)
// 	{
// 		ED_ASSERT(m_Parameters.contains(name), "This parameter doesn't exist")
// 		return std::static_pointer_cast<RenderGraphObjectPtrParameter<T>>(m_Parameters[name])->GetValue();
// 	}
// 
// 	virtual void BeginPass(const RenderPassParameters& inParameters);
// 	virtual void EndPass(const RenderPassParameters& inParameters);
// 
// protected:
// 	void InitializePasses();
// 	void ProcessDeclarations(std::shared_ptr<BaseRenderPass> pass, u32 index);
// 	void ProcessReferences(std::shared_ptr<BaseRenderPass> pass, u32 index);
// 	
// 	void BuildNodes();
// 	void CheckGraphForCycles();
// 	void TraverseGraph(std::shared_ptr<RenderGraphNode> node);
// protected:
// 	std::vector<std::shared_ptr<BaseRenderPass>> m_Passes;
// 
// 	std::map<std::string, ResourceUsages> m_ResourceUsages;
// 	std::vector<std::shared_ptr<RenderGraphNode>> m_Nodes;
// 	std::queue<std::shared_ptr<RenderGraphNode>> m_ExecutionQueue;
// 
// 	std::shared_ptr<RenderingContext>  m_Context;
// 	std::shared_ptr<Renderer> m_Renderer;
// 
// 	std::map<std::string, std::shared_ptr<Resource>*> m_Resources;
// 	std::map<std::string, std::shared_ptr<RenderGraphBaseParameter>> m_Parameters;
// };

class RenderTarget;
class UAVTarget;
class RenderPass;
class CommandQueue;
class CommandList;

class RenderGraph
{
    struct FenceSignal {
        CommandQueue* Queue;
        ccstr8 Name;
        u32 Value;
    };

public:
    constexpr static inline u32 CBVAlignment = 256;
public:
    RenderTarget* CreateRenderTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy sizePolicy);
    RenderTarget* CreateRenderTarget(ccstr8 name, PixelFormat format, u32 width, u32 height);

    UAVTarget* CreateUAVTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy sizePolicty);
    UAVTarget* CreateUAVTarget(ccstr8 name, PixelFormat format, RenderTargetSizePolicy sizePolicty, u32 mips);
    UAVTarget* CreateUAVTarget(ccstr8 name, PixelFormat format, u32 width, u32 height);
    UAVTarget* CreateUAVTarget(ccstr8 name, PixelFormat format, u32 width, u32 height, u32 mips);

    RenderTarget* GetRenderTarget(ccstr8 name) const;
    UAVTarget* GetUAVTarget(ccstr8 name) const;

    void Initialize();
    void Deinitialize();

    void InitializePasses();

    template <typename T>
    void AddRenderPass()
    {
        m_Passes.Add(new T());
    }

    void Gather();
    void Execute();
protected:

    void GatherCommonShaderParameters(void* memory);
    
    const FenceSignal* FindSignal(ccstr8 name);

    CommandQueue* SelectQueue(RenderPass* pass, CommandList* list);

    void Signal(ccstr8 name, CommandQueue* queue);
    void Wait(RenderPass* pass, CommandQueue* queue);
    
    void SetupState(RenderPass* pass, CommandList* list);
    void RecordCommands(RenderPass* pass, CommandList* list);
protected:
    Array<RenderTarget*> m_RenderTargets;
    Array<UAVTarget*> m_UAVTargets;

    Array<RenderPass*> m_Passes;

    Resource* m_ConstantBuffer;
    u64 m_ConstantBufferBaseOffset;

    Renderer* m_Renderer;
    RenderingContext* m_Context;

    Array<FenceSignal> m_Signals;

    u32 m_ParametersOffset;
};