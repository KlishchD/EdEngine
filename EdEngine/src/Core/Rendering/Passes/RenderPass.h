#pragma once

#include "Core/Rendering/RenderGraph.h"
#include "Utils/RenderingHelper.h"

enum class RenderPassType : uint8_t
{
	Base,
	Compute,
	MultiPass
};

template<typename T>
class Ref
{
public:
	Ref(T* ptr = nullptr) : m_Ptr(ptr)
	{

	}

	void SetPtr(T* ptr)
	{
		m_Ptr = ptr;
	}

	operator T&() 
	{
		ED_ASSERT(m_Ptr, "Ptr could not be null")
		return *m_Ptr;
	}

	T* operator->() // For smart ptrs :)
	{
		ED_ASSERT(m_Ptr, "Ptr could not be null")
		return m_Ptr;
	}

	T& Get()
	{
		ED_ASSERT(m_Ptr, "Ptr could not be null")
		return *m_Ptr;
	}
private:
	T* m_Ptr;
};

template<typename T>
class RefFromShared
{
public:
	RefFromShared(std::shared_ptr<T>* ptr = nullptr) : m_Ptr(ptr)
	{
	}

	void SetPtr(std::shared_ptr<T>* ptr)
	{
		m_Ptr = ptr;
	}

	operator std::shared_ptr<T>() const
	{
		return *m_Ptr;
	}

	operator T& () const
	{
		ED_ASSERT(m_Ptr, "Ptr could not be null")
		return **m_Ptr;
	}

	T* operator->() const // For smart ptrs :)
	{
		ED_ASSERT(m_Ptr, "Ptr could not be null")
		return (*m_Ptr).get();
	}

	std::shared_ptr<T>& Get()
	{
		ED_ASSERT(m_Ptr, "Ptr could not be null")
		return *m_Ptr;
	}
private:
	std::shared_ptr<T>* m_Ptr;
};

struct Declaration
{
	virtual void Declare(std::shared_ptr<RenderGraph> graph) = 0;
};

struct Reference
{
	virtual void SetValue(std::shared_ptr<RenderGraph> graph) = 0;
};


struct RenderTargetDeclaration
{
	virtual std::shared_ptr<Texture> Declare(std::shared_ptr<RenderGraph> graph) = 0;
};

struct RenderTargetReference
{
	virtual std::shared_ptr<Texture> SetValue(std::shared_ptr<RenderGraph> graph) = 0;
};

struct RenderPassParameters
{
public:
	void AddDeclaration(Declaration* declaration)
	{
		m_Declarations.push_back(declaration);
	}

	const std::vector<Declaration*> GetDeclarations() const
	{
		return m_Declarations;
	}


	void AddReference(Reference* reference)
	{
		m_References.push_back(reference);
	}

	const std::vector<Reference*> GetReferences() const
	{
		return m_References;
	}

	void AddRenderTargetDeclaration(RenderTargetDeclaration* declaration)
	{
		m_RenderTargetsDeclarations.push_back(declaration);
	}

	const std::vector<RenderTargetDeclaration*> GetRenderTargetDeclarations() const
	{
		return m_RenderTargetsDeclarations;
	}

	void AddRenderTargetReference(RenderTargetReference* reference)
	{
		m_RenderTargetsReferences.push_back(reference);
	}

	const std::vector<RenderTargetReference*> GetRenderTargetReferences() const
	{
		return m_RenderTargetsReferences;
	}

public:
	std::string Name;

	RenderPassType Type;
private:
	std::vector<Declaration*> m_Declarations;
	std::vector<Reference*> m_References;

	std::vector<RenderTargetDeclaration*> m_RenderTargetsDeclarations;
	std::vector<RenderTargetReference*> m_RenderTargetsReferences;
};

struct BaseRenderPassParameters : public RenderPassParameters
{
	BaseRenderPassParameters()
	{
		Type = RenderPassType::Base;
	}

	std::shared_ptr<class Shader> Shader;
	
	std::shared_ptr<class Framebuffer> DrawFramebuffer;

	bool bUseBlending = true;
	bool bUseDepthTesting = true;

	DepthTestFunction DepthFunction = DepthTestFunction::Lesser;
	BlendFactor SourceFactor = BlendFactor::SourceAlpha;
	BlendFactor DestinationFactor = BlendFactor::OneMinusSourceAlpha;

	bool bClearColors = false;
	bool bClearDepth = false;

	bool bEnableFaceCulling = false;
	Face FaceToCull = Face::Back;
};

struct ComputeRenderPassParameters : public RenderPassParameters
{
	ComputeRenderPassParameters()
	{
		Type = RenderPassType::Compute;
	}

	std::shared_ptr<class Shader> Shader;
};


struct MultiRenderPassParameters : public RenderPassParameters
{
	MultiRenderPassParameters()
	{
		Type = RenderPassType::MultiPass;
	}
};

#define ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(name, type) struct name : public type ## RenderPassParameters { \
	static inline uint32_t InstanceCount = 0; \
	\
	uint32_t InstanceNumber; \
	\
	name() \
	{ \
		++InstanceCount; \
		InstanceNumber = InstanceCount; \
	}

#define ED_END_RENDER_PASS_PARAMETERS_DECLARATION() };

#define ED_RENDER_PASS_GRAPH_PARAMETER(type, name, parameterName) \
	Ref<type> name; \
	struct name ## ParameterReference : public Reference \
	{ \
		Ref<type>& Value; \
		uint32_t InstanceNumber; \
		\
		name ## ParameterReference(RenderPassParameters& parameters, Ref<type>& value) : InstanceNumber(InstanceCount + 1), Value(value) \
		{ \
			parameters.AddReference(this); \
		} \
		\
		virtual void SetValue(std::shared_ptr<RenderGraph> graph) override \
		{ \
			Value = &graph->GetParameterValue<type>(parameterName); \
		} \
	};\
	name ## ParameterReference name ## ParameterReferenceValue{ *this, name };

#define ED_RENDER_PASS_GRAPH_OBJECT_PTR_PARAMETER(type, name, parameterName) \
	RefFromShared<type> name; \
	struct name ## ParameterReference : public Reference \
	{ \
		RefFromShared<type>& Value; \
		uint32_t InstanceNumber; \
		\
		name ## ParameterReference(RenderPassParameters& parameters, RefFromShared<type>& value) : InstanceNumber(InstanceCount + 1), Value(value) \
		{ \
			parameters.AddReference(this); \
		} \
		\
		virtual void SetValue(std::shared_ptr<RenderGraph> graph) override \
		{ \
			Value = &graph->GetObjectPtrParameterValue<type>(parameterName); \
		} \
	};\
	name ## ParameterReference name ## ParameterReferenceValue{ *this, name };


#define ED_RENDER_PASS_RENDER_TARGET_REFERENCE(type, name, resourceName) \
	RefFromShared<type> name; \
	struct name ## ResourceReference : public RenderTargetReference \
	{ \
		RefFromShared<type>& Value; \
		uint32_t InstanceNumber; \
		\
		name ## ResourceReference(RenderPassParameters& parameters, RefFromShared<type>& value) : InstanceNumber(InstanceCount + 1), Value(value) \
		{ \
			parameters.AddRenderTargetReference(this); \
		} \
		\
		virtual std::shared_ptr<Texture> SetValue(std::shared_ptr<RenderGraph> graph) override\
		{ \
			Value = &graph->GetResource<type>(resourceName); \
			return std::static_pointer_cast<Texture>(Value.Get()); \
		} \
	}; \
	name ## ResourceReference name ## ResourceReferenceValue{ *this, name };

#define ED_RENDER_PASS_DECLARE_RENDER_TARGET(type, name, attachmentType, resourceName) \
	std::shared_ptr<type> name; \
	struct name ## RenderTargetDeclaration : public RenderTargetDeclaration \
	{ \
		std::shared_ptr<type>& Value; \
		uint32_t InstanceNumber; \
		\
		name ## RenderTargetDeclaration(RenderPassParameters& parameters, std::shared_ptr<type>& value) : InstanceNumber(InstanceCount + 1), Value(value) \
		{ \
			parameters.AddRenderTargetDeclaration(this); \
		} \
		\
		virtual std::shared_ptr<Texture> Declare(std::shared_ptr<RenderGraph> graph) override \
		{ \
			RenderTargetSpecification specification; \
			specification.Name = resourceName; \
			specification.Type = FramebufferAttachmentType::attachmentType; \
			Value = RenderingHelper::CreateRenderTarget<type>(specification, TextureType::type); \
			graph->DeclareResource(specification.Name, Value); \
			return Value; \
		} \
	}; \
	name ## RenderTargetDeclaration name ## RenderTargetDeclarationValue{ *this, name };


#define ED_RENDER_PASS_RESOURCE_REFERENCE(type, name, resourceName) \
	RefFromShared<type> name; \
	struct name ## ResourceReference : public Reference \
	{ \
		RefFromShared<type>& Value; \
		uint32_t InstanceNumber; \
		\
		name ## ResourceReference(RenderPassParameters& parameters, RefFromShared<type>& value) : InstanceNumber(InstanceCount + 1), Value(value) \
		{ \
			parameters.AddReference(this); \
		} \
		\
		virtual void SetValue(std::shared_ptr<RenderGraph> graph) override \
		{ \
			Value = &graph->GetResource<type>(resourceName); \
		} \
	}; \
	name ## ResourceReference name ## ResourceReferenceValue{ *this, name };

#define ED_RENDER_PASS_DECLARE_GRAPH_PARAMETER(type, name, parameterName) \
	type name; \
	struct name ## ParameterDeclaration : public Declaration \
	{ \
		type& Value; \
		uint32_t InstanceNumber; \
		\
		name ## ParameterDeclaration(RenderPassParameters& parametes, type& value) : InstanceNumber(InstanceCount + 1), Value(value) \
		{ \
			parametes.AddDeclaration(this); \
		} \
		\
		virtual void Declare(std::shared_ptr<RenderGraph> graph) override \
		{ \
			graph->DeclareParameter(parameterName, Value); \
		} \
	}; \
	name ## ParameterDeclaration name ## ParameterDeclarationValue{ *this, name };


#define ED_RENDER_PASS_DECLARE_GRAPH_OBJECT_PTR_PRAMETER(type, name, parameterName) \
	std::shared_ptr<type> name; \
	struct name ## ParameterDeclaration : public Declaration \
	{ \
		std::shared_ptr<type>& Value; \
		uint32_t InstanceNumber; \
		\
		name ## ParameterDeclaration(RenderPassParameters& parametes, std::shared_ptr<type>& value) : InstanceNumber(InstanceCount + 1), Value(value) \
		{ \
			parametes.AddDeclaration(this); \
		} \
		\
		virtual void Declare(std::shared_ptr<RenderGraph> graph) override \
		{ \
			graph->DeclareObjectPtrParameter(parameterName, Value); \
		} \
	}; \
	name ## ParameterDeclaration name ## ParameterDeclarationValue{ *this, name };

#define ED_RENDER_PASS_DECLARE_RESOURCE(type, name, resourceName) \
	std::shared_ptr<type> name; \
	struct name ## ResourceDeclaration : public Declaration \
	{ \
		std::shared_ptr<type>& Value; \
		uint32_t InstanceNumber; \
		\
		name ## ResourceDeclaration(RenderPassParameters& parametes, std::shared_ptr<type>& value)  : InstanceNumber(InstanceCount + 1), Value(value) \
		{ \
			parametes.AddDeclaration(this); \
		} \
		\
		virtual void Declare(std::shared_ptr<RenderGraph> graph) override \
		{ \
			graph->DeclareResource<type>(resourceName, Value); \
		} \
	}; \
	name ## ResourceDeclaration name ## ResourceDeclarationValue{ *this, name };

struct ShaderParameter
{
	virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) = 0;
};

// Shaders :)
struct ShaderParameters
{
	void AddParameter(ShaderParameter* parameter)
	{
		m_Parameters.push_back(parameter);
	}

	const std::vector<ShaderParameter*>& GetParameters() const
	{
		return m_Parameters;
	}

private:
	std::vector<ShaderParameter*> m_Parameters;
};

#define ED_BEGIN_SHADER_PARAMETERS_DECLARATION(name) struct name : public ShaderParameters {
#define ED_END_SHADER_PARAMETERS_DECLARATION() };

#define ED_SHADER_PARAMETER(shaderType, type, name) \
	type name; \
	struct name ## ShaderParameter : public ShaderParameter \
	{ \
		type& Value; \
		std::string nameStr; \
		name ## ShaderParameter(ShaderParameters& parameters, type& value) : Value(value) \
		{ \
			parameters.AddParameter(this); \
			std::stringstream ss; \
			ss << "u_" << #name; \
			nameStr = ss.str(); \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			context->SetShaderData ## shaderType(nameStr, Value); \
		} \
	}; \
	name ## ShaderParameter name ## ShaderParameterValue { *this, name };

#define ED_SHADER_PARAMETER_PTR(shaderType, type, name) \
	std::shared_ptr<type> name; \
	struct name ## ShaderParameter : public ShaderParameter \
	{ \
		std::shared_ptr<type>& Value; \
		std::string nameStr; \
		name ## ShaderParameter(ShaderParameters& parameters, std::shared_ptr<type>& value) : Value(value) \
		{ \
			parameters.AddParameter(this); \
			std::stringstream ss; \
			ss << "u_" << #name; \
			nameStr = ss.str(); \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			context->SetShaderData ## shaderType(nameStr, Value); \
		} \
	}; \
	name ## ShaderParameter name ## ShaderParameterValue { *this, name };

#define ED_SHADER_PARAMETER_ARRAY(shaderType, type, name, count) \
	type name[count]; \
	struct name ## ShaderParameter : public ShaderParameter \
	{ \
		type (&Value)[count]; \
		std::vector<std::string> names; \
		name ## ShaderParameter(ShaderParameters& parameters, type (&value)[count]) : Value(value) \
		{ \
			parameters.AddParameter(this); \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				std::stringstream ss; \
				ss << "u_" << #name << "[" << i << "]"; \
				names.push_back(ss.str()); \
			} \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				context->SetShaderData ## shaderType(names[i], Value[i]); \
			} \
		} \
	}; \
	name ## ShaderParameter name ## ShaderParameterValue { *this, name };

#define ED_SHADER_PARAMETER_ARRAY_PTR(shaderType, type, name, count) \
	std::shared_ptr<type> name[count]; \
	struct name ## ShaderParameter : public ShaderParameter \
	{ \
		std::shared_ptr<type> (&Value)[count]; \
		std::vector<std::string> names; \
		name ## ShaderParameter(ShaderParameters& parameters, std::shared_ptr<type> (&value)[count]) : Value(value) \
		{ \
			parameters.AddParameter(this); \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				std::stringstream ss; \
				ss << "u_" << #name << "[" << i << "]"; \
				names.push_back(ss.str()); \
			} \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				context->SetShaderData ## shaderType(names[i], Value[i]); \
			} \
		} \
	}; \
	name ## ShaderParameter name ## ShaderParameterValue { *this, name };

#define ED_SHADER_PARAMETER_SUBSTRUCT(structName, shaderType, type, name) \
	type structName ## _ ## name; \
	struct structName ## name ## ShaderParameter : public ShaderParameter \
	{ \
		type& Value; \
		std::string nameStr; \
		structName ## name ## ShaderParameter(ShaderParameters& parameters, type& value) : Value(value) \
		{ \
			parameters.AddParameter(this); \
			std::stringstream ss; \
			ss << "u_" << #structName << "." << #name; \
			nameStr = ss.str(); \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			context->SetShaderData ## shaderType(nameStr, Value); \
		} \
	}; \
	structName ## name ## ShaderParameter structName ## name ## ShaderParameterValue { *this, structName ## _ ## name };

#define ED_SHADER_PARAMETER_SUBSTRUCT_PTR(structName, shaderType, type, name) \
	std::shared_ptr<type> structName ## _ ## name; \
	struct structName ## name ## ShaderParameter : public ShaderParameter \
	{ \
		std::shared_ptr<type>& Value; \
		std::string nameStr; \
		structName ## name ## ShaderParameter(ShaderParameters& parameters, std::shared_ptr<type>& value) : Value(value) \
		{ \
			parameters.AddParameter(this); \
			std::stringstream ss; \
			ss << "u_" << #structName << "." << #name; \
			nameStr = ss.str(); \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			context->SetShaderData ## shaderType(nameStr, Value); \
		} \
	}; \
	structName ## name ## ShaderParameter structName ## name ## ShaderParameterValue { *this, structName ## _ ## name };

#define ED_SHADER_PARAMETER_SUBSTRUCT_ARRAY(structName, shaderType, type, name, count) \
	type structName ## _ ## name[count]; \
	struct structName ## _ ## name ## ShaderParameter : public ShaderParameter \
	{ \
		type (&Value)[count]; \
		std::vector<std::string> names; \
		structName ## _ ## name ## ShaderParameter(ShaderParameters& parameters, type (&value)[count]) : Value(value) \
		{ \
			parameters.AddParameter(this); \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				std::stringstream ss; \
				ss << "u_" << #structName << "." << #name << "[" << i << "]"; \
				names.push_back(ss.str()); \
			} \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				context->SetShaderData ## shaderType(names[i], Value[i]); \
			} \
		} \
	}; \
	structName ## _ ## name ## ShaderParameter structName ## _ ## name ## ShaderParameterValue { *this, structName ## _ ## name };

#define ED_SHADER_PARAMETER_SUBSTRUCT_ARRAY_PTR(structName, shaderType, type, name, count) \
	std::shared_ptr<type> structName ## _ ## name[count]; \
	struct structName ## name ## ShaderParameter : public ShaderParameter \
	{ \
		std::shared_ptr<type> (&Value)[count]; \
		std::vector<std::string> names; \
		structName ## name ## ShaderParameter(ShaderParameters& parameters, std::shared_ptr<type> (&value)[count]) : Value(value) \
		{ \
			parameters.AddParameter(this); \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				std::stringstream ss; \
				ss << "u_" << #structName << "." << #name << "[" << i << "]"; \
				names.push_back(ss.str()); \
			} \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				context->SetShaderData ## shaderType(names[i], Value[i]); \
			} \
		} \
	}; \
	structName ## name ## ShaderParameter structName ## name ## ShaderParameterValue { *this, structName ## _ ## name };

class BaseRenderPass : public std::enable_shared_from_this<BaseRenderPass>
{
public:
	virtual void Initialize(std::shared_ptr<RenderGraph> graph);
	virtual void PreUpdate();
	virtual void Update(float deltaSeconds);

	virtual RenderPassParameters& GetBaseParameters() = 0;
	virtual ShaderParameters& GetBaseShaderParameters() = 0;

	RenderPassType GetType() { return GetBaseParameters().Type; }

protected:
	std::shared_ptr<RenderGraph> m_Graph;
	std::shared_ptr<Renderer> m_Renderer;
	std::shared_ptr<RenderingContext> m_Context;
};

template<typename ParameterStruct, typename ShaderParametersStruct>
class RenderPass : public BaseRenderPass
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
	virtual void CreatePasses() = 0;

	virtual std::vector<std::shared_ptr<BaseRenderPass>> GetRenderPasses() const = 0;
};


template<typename ParameterStruct, typename ShaderParametersStruct>
class MultiPassRenderPass : public BaseMultiPassRenderPass
{
public:
	virtual RenderPassParameters& GetBaseParameters() override { return m_Parameters; }
	virtual ShaderParameters& GetBaseShaderParameters() override { return m_ShaderParameters; }

	const ParameterStruct& GetParameters() const { return m_Parameters; }
	const ShaderParametersStruct& GetShaderParameters() const { return m_ShaderParameters; }

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

	void UpdatePass(std::shared_ptr<BaseRenderPass> pass, float deltaSeconds)
	{
		pass->PreUpdate();

		m_Graph->BeginPass(pass->GetBaseParameters());

		pass->Update(deltaSeconds);

		m_Graph->EndPass(pass->GetBaseParameters());
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
	ParameterStruct m_Parameters;
	ShaderParametersStruct m_ShaderParameters;

	std::vector<std::shared_ptr<BaseRenderPass>> m_Passes;
};
