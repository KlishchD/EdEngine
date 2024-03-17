#pragma once

#include <string>

struct ResourceSpecification
{
	std::string Name;
};

class Resource
{
public:
	Resource() : m_Name("Empty name")
	{

	}

	Resource(const ResourceSpecification& specification) : m_Name(specification.Name)
	{

	}

	void SetName(const std::string& name) { m_Name = name; }
	const std::string& GetName() const { return m_Name; }

protected:
	std::string m_Name;

};
