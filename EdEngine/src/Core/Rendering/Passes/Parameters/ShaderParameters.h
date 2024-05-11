#pragma once

#include "ParametersCommon.h"

struct ShaderParameter
{
	virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) = 0;
};

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

#define ED_BEGIN_SHADER_PARAMETERS_DECLARATION(name) struct name ## ShaderParameters : public ShaderParameters { \
	using ParametersStruct = name ## ShaderParameters;

#define ED_END_SHADER_PARAMETERS_DECLARATION() };

#define ED_SHADER_PARAMETER(shaderType, type, name) \
	type name; \
	private: \
	\
	struct name ## ShaderParameter : public ShaderParameter \
	{ \
		ParametersStruct& Parameters; \
		std::string Name; \
		\
		name ## ShaderParameter(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddParameter(this); \
			\
			std::stringstream ss; \
			ss << "u_" << #name; \
			Name = ss.str(); \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			context->SetShaderData ## shaderType(Name, Parameters.name); \
		} \
	}; \
	name ## ShaderParameter name ## ShaderParameterValue { *this }; \
	\
	public:

#define ED_SHADER_PARAMETER_PTR(shaderType, type, name) \
	std::shared_ptr<type> name; \
	private: \
	\
	struct name ## ShaderParameter : public ShaderParameter \
	{ \
		ParametersStruct& Parameters; \
		std::string Name; \
		\
		name ## ShaderParameter(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddParameter(this); \
			\
			std::stringstream ss; \
			ss << "u_" << #name; \
			Name = ss.str(); \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			context->SetShaderData ## shaderType(Name, Parameters.name); \
		} \
	}; \
	name ## ShaderParameter name ## ShaderParameterValue { *this }; \
	\
	public:

#define ED_SHADER_PARAMETER_ARRAY(shaderType, type, name, count) \
	type name[count]; \
	private: \
	\
	struct name ## ShaderParameter : public ShaderParameter \
	{ \
		ParametersStruct& Parameters; \
		std::vector<std::string> Names; \
		\
		name ## ShaderParameter(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddParameter(this); \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				std::stringstream ss; \
				ss << "u_" << #name << "[" << i << "]"; \
				Names.push_back(ss.str()); \
			} \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				context->SetShaderData ## shaderType(Names[i], Parameters.name[i]); \
			} \
		} \
	}; \
	name ## ShaderParameter name ## ShaderParameterValue { *this }; \
	\
	public:

#define ED_SHADER_PARAMETER_ARRAY_PTR(shaderType, type, name, count) \
	std::shared_ptr<type> name[count]; \
	private: \
	\
	struct name ## ShaderParameter : public ShaderParameter \
	{ \
		ParametersStruct& Parameters; \
		std::vector<std::string> Names; \
		\
		name ## ShaderParameter(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddParameter(this); \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				std::stringstream ss; \
				ss << "u_" << #name << "[" << i << "]"; \
				Names.push_back(ss.str()); \
			} \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			for (int32_t i = 0; i < count; ++i) \
			{ \
				context->SetShaderData ## shaderType(Names[i], Parameters.name[i]); \
			} \
		} \
	}; \
	name ## ShaderParameter name ## ShaderParameterValue { *this }; \
	\
	public:

#define ED_SHADER_PARAMETER_SUBSTRUCT(structName, shaderType, type, name) \
	type structName ## _ ## name; \
	private: \
	\
	struct structName ## name ## ShaderParameter : public ShaderParameter \
	{ \
		ParametersStruct& Parameters; \
		std::string Name; \
		\
		structName ## name ## ShaderParameter(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddParameter(this); \
			\
			std::stringstream ss; \
			ss << "u_" << #structName << "." << #name; \
			Name = ss.str(); \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			context->SetShaderData ## shaderType(Name, Parameters.structName ## _ ## name); \
		} \
	}; \
	structName ## name ## ShaderParameter structName ## name ## ShaderParameterValue { *this }; \
	\
	public:

#define ED_SHADER_PARAMETER_SUBSTRUCT_PTR(structName, shaderType, type, name) \
	std::shared_ptr<type> structName ## _ ## name; \
	private: \
	\
	struct structName ## name ## ShaderParameter : public ShaderParameter \
	{ \
		ParametersStruct& Parameters; \
		std::string Name; \
		\
		structName ## name ## ShaderParameter(ParametersStruct& parameters) : Parameters(parameters) \
		{ \
			parameters.AddParameter(this); \
			\
			std::stringstream ss; \
			ss << "u_" << #structName << "." << #name; \
			Name = ss.str(); \
		} \
		\
		virtual void SubmitParameter(std::shared_ptr<RenderingContext> context) override \
		{ \
			context->SetShaderData ## shaderType(Name, Parameters.structName ## _ ## name); \
		} \
	}; \
	structName ## name ## ShaderParameter structName ## name ## ShaderParameterValue { *this }; \
	\
	public:

#define ED_SHADER_PARAMETER_SUBSTRUCT_ARRAY(structName, shaderType, type, name, count) \
	type structName ## _ ## name[count]; \
	private: \
	\
	struct structName ## _ ## name ## ShaderParameter : public ShaderParameter \
	{ \
		ParametersStruct& Parameters; \
		std::vector<std::string> names; \
		\
		structName ## _ ## name ## ShaderParameter(ParametersStruct& parameters) : Parameters(parameters) \
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
				context->SetShaderData ## shaderType(names[i], Parameters.structName ## _ ## name[i]); \
			} \
		} \
	}; \
	structName ## _ ## name ## ShaderParameter structName ## _ ## name ## ShaderParameterValue { *this }; \
	\
	public:

#define ED_SHADER_PARAMETER_SUBSTRUCT_ARRAY_PTR(structName, shaderType, type, name, count) \
	std::shared_ptr<type> structName ## _ ## name[count]; \
	private: \
	\
	struct structName ## name ## ShaderParameter : public ShaderParameter \
	{ \
		ParametersStruct& Parameters; \
		std::vector<std::string> names; \
		\
		structName ## name ## ShaderParameter(ParametersStruct& parameters) : Parameters(parameters) \
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
				context->SetShaderData ## shaderType(names[i], Parameters.structName ## _ ## name[i]); \
			} \
		} \
	}; \
	structName ## name ## ShaderParameter structName ## name ## ShaderParameterValue { *this }; \
	\
	public:
