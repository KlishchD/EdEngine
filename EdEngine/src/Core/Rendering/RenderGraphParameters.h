#pragma once

class RenderGraphBaseParameter
{
public:
	RenderGraphBaseParameter(const std::string& name = "Empty") : m_Name(name) { }

	void SetName(const std::string& name) { m_Name = name; }
	const std::string& GetName() const { return m_Name; }

protected:
	std::string m_Name;
};

template<typename T>
class RenderGraphParemeter : public RenderGraphBaseParameter
{
public:

	RenderGraphParemeter(const std::string& name, T& value) : RenderGraphBaseParameter(name), m_Value(value) { }

	void SetValue(T& value) { m_Value = value; }
	T& GetValue() { return m_Value; }

protected:
	T& m_Value;
};

template<typename T>
class RenderGraphObjectPtrParameter : public RenderGraphBaseParameter
{
public:
	RenderGraphObjectPtrParameter(const std::string& name, std::shared_ptr<T>& value) : RenderGraphBaseParameter(name), m_Value(value)
	{
	}

	void SetValue(std::shared_ptr<T>& value) { m_Value = value; }
	std::shared_ptr<T>& GetValue() { return m_Value; }

protected:
	std::shared_ptr<T>& m_Value;
};