#pragma once

#include "ParametersCommon.h"

enum class RenderPassType : uint8_t
{
	Base,
	Compute,
	MultiPass
};

struct RenderTargetDeclaration
{
	std::string ResourceName;

	virtual std::shared_ptr<Texture> Declare(std::shared_ptr<RenderGraph> graph) = 0;
};

struct RenderTargetReference
{
	std::string ResourceName;

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

#define ED_BEGIN_RENDER_PASS_PARAMETERS_DECLARATION(name, type) struct name ## Parameters : public type ## RenderPassParameters { \
	using ParametersStruct = name ## Parameters; \
	\
	static inline uint32_t InstanceCount = 0; \
	\
	uint32_t InstanceNumber; \
	\
	name ## Parameters() \
	{ \
		++InstanceCount; \
		InstanceNumber = InstanceCount; \
	}

#define ED_END_RENDER_PASS_PARAMETERS_DECLARATION() };

#define ED_RENDER_PASS_DECLARE_RENDER_TARGET(type, name, attachmentType, resourceName) \
	std::shared_ptr<type> name; \
	private: \
	\
	struct name ## RenderTargetDeclaration : public RenderTargetDeclaration \
	{ \
		ParametersStruct& Parameters; \
		\
		name ## RenderTargetDeclaration(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddRenderTargetDeclaration(this); \
		} \
		\
		virtual std::shared_ptr<Texture> Declare(std::shared_ptr<RenderGraph> graph) override \
		{ \
			RenderTargetSpecification specification; \
			\
			ResourceName = resourceName; \
			specification.Name = ResourceName; \
			\
			specification.Type = FramebufferAttachmentType::attachmentType; \
			Parameters.name = RenderingHelper::CreateRenderTarget<type>(specification, TextureType::type); \
			graph->DeclareResource(specification.Name, Parameters.name); \
			return Parameters.name; \
		} \
	}; \
	name ## RenderTargetDeclaration name ## RenderTargetDeclarationValue{ *this }; \
	\
	public:

#define ED_RENDER_PASS_RENDER_TARGET_REFERENCE(type, name, resourceName) \
	RefFromShared<type> name; \
	private: \
	\
	struct name ## ResourceReference : public RenderTargetReference \
	{ \
		ParametersStruct& Parameters; \
		\
		name ## ResourceReference(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddRenderTargetReference(this); \
		} \
		\
		virtual std::shared_ptr<Texture> SetValue(std::shared_ptr<RenderGraph> graph) override\
		{ \
			ResourceName = resourceName; \
			Parameters.name = &graph->GetResource<type>(ResourceName); \
			return std::static_pointer_cast<Texture>(Parameters.name.Get()); \
		} \
	}; \
	name ## ResourceReference name ## ResourceReferenceValue{ *this }; \
	\
	public:

#define ED_RENDER_PASS_DECLARE_RESOURCE(type, name, resourceName) \
	std::shared_ptr<type> name; \
	private: \
	\
	struct name ## ResourceDeclaration : public Declaration \
	{ \
		ParametersStruct& Parameters; \
		\
		name ## ResourceDeclaration(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddDeclaration(this); \
		} \
		\
		virtual void Declare(std::shared_ptr<RenderGraph> graph) override \
		{ \
			ResourceName = resourceName; \
			graph->DeclareResource<type>(ResourceName, Parameters.name); \
		} \
	}; \
	name ## ResourceDeclaration name ## ResourceDeclarationValue{ *this }; \
	\
	public:

#define ED_RENDER_PASS_RESOURCE_REFERENCE(type, name, resourceName, accessMode) \
	RefFromShared<type> name; \
	private: \
	\
	struct name ## ResourceReference : public Reference \
	{ \
		ParametersStruct& Parameters; \
		\
		name ## ResourceReference(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddReference(this); \
			AccessMode = ReferenceAccessMode::accessMode; \
		} \
		\
		virtual void SetValue(std::shared_ptr<RenderGraph> graph) override \
		{ \
			ResourceName = resourceName; \
			Parameters.name = &graph->GetResource<type>(ResourceName); \
		} \
	}; \
	name ## ResourceReference name ## ResourceReferenceValue{ *this }; \
	\
	public:

#define ED_RENDER_PASS_DECLARE_PARAMETER(type, name, parameterName) \
	type name; \
	private: \
	\
	struct name ## ParameterDeclaration : public Declaration \
	{ \
		ParametersStruct& Parameters; \
		\
		name ## ParameterDeclaration(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddDeclaration(this); \
		} \
		\
		virtual void Declare(std::shared_ptr<RenderGraph> graph) override \
		{ \
			ResourceName = parameterName; \
			graph->DeclareParameter(ResourceName, Parameters.name); \
		} \
	}; \
	name ## ParameterDeclaration name ## ParameterDeclarationValue{ *this }; \
	\
	public:

#define ED_RENDER_PASS_PARAMETER(type, name, parameterName, accessMode) \
	Ref<type> name; \
	private: \
	\
	struct name ## ParameterReference : public Reference \
	{ \
		ParametersStruct& Parameters; \
		\
		name ## ParameterReference(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddReference(this); \
			AccessMode = ReferenceAccessMode::accessMode; \
		} \
		\
		virtual void SetValue(std::shared_ptr<RenderGraph> graph) override \
		{ \
			ResourceName = parameterName; \
			Parameters.name = &graph->GetParameterValue<type>(ResourceName); \
		} \
	}; \
	\
	name ## ParameterReference name ## ParameterReferenceValue{ *this }; \
	\
	public: 

#define ED_RENDER_PASS_DECLARE_OBJECT_PTR_PRAMETER(type, name, parameterName) \
	std::shared_ptr<type> name; \
	private: \
	\
	struct name ## ParameterDeclaration : public Declaration \
	{ \
		ParametersStruct& Parameters; \
		\
		name ## ParameterDeclaration(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddDeclaration(this); \
		} \
		\
		virtual void Declare(std::shared_ptr<RenderGraph> graph) override \
		{ \
			ResourceName = parameterName; \
			graph->DeclareObjectPtrParameter(ResourceName, Parameters.name); \
		} \
	}; \
	\
	name ## ParameterDeclaration name ## ParameterDeclarationValue{ *this }; \
	public:

#define ED_RENDER_PASS_OBJECT_PTR_PARAMETER(type, name, parameterName, accessMode) \
	RefFromShared<type> name; \
	private: \
	\
	struct name ## ParameterReference : public Reference \
	{ \
		ParametersStruct& Parameters; \
		\
		name ## ParameterReference(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddReference(this); \
			AccessMode = ReferenceAccessMode::accessMode; \
		} \
		\
		virtual void SetValue(std::shared_ptr<RenderGraph> graph) override \
		{ \
			ResourceName = parameterName; \
			Parameters.name = &graph->GetObjectPtrParameterValue<type>(ResourceName); \
		} \
	}; \
	\
	name ## ParameterReference name ## ParameterReferenceValue{ *this }; \
	public:
