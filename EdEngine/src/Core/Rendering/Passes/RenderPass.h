#pragma once

#include "Parameters/RenderPassParameters.h"
#include "Parameters/ShaderParameters.h"

class BaseRenderPass : public std::enable_shared_from_this<BaseRenderPass>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph);
	virtual void PostInitialization();
	virtual void PreExecute();
	virtual void Execute();

	virtual RenderPassParameters& GetBaseParameters() = 0;

	RenderPassType GetType() { return GetBaseParameters().Type; }

protected:
	std::shared_ptr<RenderGraph> m_Graph;
	std::shared_ptr<Renderer> m_Renderer;
	std::shared_ptr<RenderingContext> m_Context;
};

class BaseRegularRenderPass : public BaseRenderPass
{
public:
	virtual ShaderParameters& GetBaseShaderParameters() = 0;
};

template<typename ParameterStruct, typename ShaderParametersStruct>
class RenderPass : public BaseRegularRenderPass
{
public:
	virtual RenderPassParameters& GetBaseParameters() override { return m_Parameters; }
	virtual ShaderParameters& GetBaseShaderParameters() override { return m_ShaderParameters; }

	const ParameterStruct& GetParameters() const { return m_Parameters; }
	const ShaderParametersStruct& GetShaderParameters() const { return m_ShaderParameters; }

	void SubmitShaderParameters()
	{
		for (ShaderParameter* parameter : m_ShaderParameters.GetParameters())
		{
			parameter->SubmitParameter(m_Context);
		}
	}

protected:
	ParameterStruct m_Parameters;
	ShaderParametersStruct m_ShaderParameters;
};

class BaseMultiPassRenderPass : public BaseRenderPass
{
public:
	virtual void PostInitialization() override;

	virtual void CreatePasses() = 0;

	virtual std::vector<std::shared_ptr<BaseRenderPass>> GetRenderPasses() const { return m_Passes; }

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
protected:
	template<typename T>
	void AddPass()
	{
		AddPass(std::make_shared<T>());
	}

	void AddPass(std::shared_ptr<BaseRenderPass> pass)
	{
		m_Passes.push_back(pass);
	}
protected:
	std::vector<std::shared_ptr<BaseRenderPass>> m_Passes;
};

template<typename ParameterStruct>
class MultiPassRenderPass : public BaseMultiPassRenderPass
{
public:
	virtual RenderPassParameters& GetBaseParameters() override { return m_Parameters; }

	const ParameterStruct& GetParameters() const { return m_Parameters; }
protected:
	ParameterStruct m_Parameters;
};
