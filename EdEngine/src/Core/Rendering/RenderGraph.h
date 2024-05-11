#pragma once

#include "RenderingContex.h"
#include "Renderer.h"
#include "Passes/Parameters/RenderGraphParameters.h"
#include <set>

struct RenderPassParameters;
class BaseRenderPass;

template<typename ParameterStructClass, typename ShaderParametersClass>
class RenderPass;

struct ResourceUsages
{
	void AddWriter(uint32_t writer);
	void AddReader(uint32_t reader);

	uint32_t Declaration;
	std::vector<uint32_t> Readers;
	std::vector<uint32_t> Writers;
};

struct RenderGraphNode
{
	bool CanStart() const
	{
		for (const std::shared_ptr<RenderGraphNode>& node : Upstream)
		{
			if (!node->bHasFinished)
			{
				return false;
			}
		}

		return !bHasFinished;
	}

	std::shared_ptr<BaseRenderPass> Pass;
	bool bHasFinished = false;

	std::vector<std::shared_ptr<RenderGraphNode>> Upstream;
	std::vector<std::shared_ptr<RenderGraphNode>> Downstream;

protected:
	static const uint32_t NotVisited = 0;
	static const uint32_t VisitedButNotExited = 1;
	static const uint32_t VisitedAndExited = 2;

	uint8_t NodeState = NotVisited;

	friend class RenderGraph;
};

class RenderGraph : public std::enable_shared_from_this<RenderGraph>
{
public:
	void Initilaize(std::shared_ptr<Renderer> renderer);

	template<typename T>
	std::shared_ptr<T> GetPass()
	{
		for (const std::shared_ptr<BaseRenderPass>& pass : m_Passes)
		{
			if (std::shared_ptr<T> castedPass = std::dynamic_pointer_cast<T>(pass))
			{
				return castedPass;
			}
		}

		return nullptr;
	}

	void AddPass(std::shared_ptr<BaseRenderPass> pass);

	template<typename T>
	void AddPass()
	{
		AddPass(std::make_shared<T>());
	}

	void Build();

	void Update(float deltaSeconds);
	void ExecutePass(std::shared_ptr<BaseRenderPass> pass);

	std::shared_ptr<Renderer> GetRenderer() const;
	std::shared_ptr<RenderingContext> GetContext() const;

	template<typename T>
	void DeclareResource(const std::string& name, std::shared_ptr<T>& resource)
	{
		ED_ASSERT(!m_Resources.contains(name), "Resource with this name already exists")
		m_Resources[name] = reinterpret_cast<std::shared_ptr<Resource>*>(&resource);
	}

	std::shared_ptr<Resource>& GetResource(const std::string& name) const;
	
	template<typename T>
	std::shared_ptr<T>& GetResource(const std::string& name) const
	{
		ED_ASSERT(m_Resources.contains(name), "This parameter doesn't exist")
		return *reinterpret_cast<std::shared_ptr<T>*>(m_Resources.at(name));
	}
	
	template<typename T>
	void DeclareParameter(const std::string& name, T& value)
	{
		std::shared_ptr<RenderGraphParemeter<T>> parameter = std::make_shared<RenderGraphParemeter<T>>(name, value);
		m_Parameters[name] = std::move(parameter);
	}

	template<typename T>
	void DeclareObjectPtrParameter(const std::string& name, std::shared_ptr<T>& value)
	{
		std::shared_ptr<RenderGraphObjectPtrParameter<T>> parameter = std::make_shared<RenderGraphObjectPtrParameter<T>>(name, value);
		m_Parameters[name] = std::move(parameter);
	}

	template<typename T>
	T& GetParameterValue(const std::string& name)
	{
		ED_ASSERT(m_Parameters.contains(name), "This parameter doesn't exist")
		return std::static_pointer_cast<RenderGraphParemeter<T>>(m_Parameters[name])->GetValue();
	}

	template<typename T>
	std::shared_ptr<T>& GetObjectPtrParameterValue(const std::string& name)
	{
		ED_ASSERT(m_Parameters.contains(name), "This parameter doesn't exist")
		return std::static_pointer_cast<RenderGraphObjectPtrParameter<T>>(m_Parameters[name])->GetValue();
	}

	virtual void BeginPass(const RenderPassParameters& inParameters);
	virtual void EndPass(const RenderPassParameters& inParameters);

protected:
	void InitializePasses();
	void ProcessDeclarations(std::shared_ptr<BaseRenderPass> pass, uint32_t index);
	void ProcessReferences(std::shared_ptr<BaseRenderPass> pass, uint32_t index);
	
	void BuildNodes();
	void CheckGraphForCycles();
	void TraverseGraph(std::shared_ptr<RenderGraphNode> node);
protected:
	std::vector<std::shared_ptr<BaseRenderPass>> m_Passes;

	std::map<std::string, ResourceUsages> m_ResourceUsages;
	std::vector<std::shared_ptr<RenderGraphNode>> m_Nodes;
	std::queue<std::shared_ptr<RenderGraphNode>> m_ExecutionQueue;

	std::shared_ptr<RenderingContext>  m_Context;
	std::shared_ptr<Renderer> m_Renderer;

	std::map<std::string, std::shared_ptr<Resource>*> m_Resources;
	std::map<std::string, std::shared_ptr<RenderGraphBaseParameter>> m_Parameters;
};
