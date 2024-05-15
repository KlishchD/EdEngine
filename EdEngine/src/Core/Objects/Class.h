#pragma once

#include "Core/Ed.h"

#define ED_CLASS(name) class name; \
	class name ## Class : public Class \
	{ \
	friend class name; \
	public: \
		name ## Class() : Class(#name) \
		{ \
			ObjectFactory::RegisterClass(*this); \
		} \
		\
		virtual bool IsAbstract() const override \
		{ \
			return false; \
		} \
		\
		virtual std::shared_ptr<GameObject> Create() const override \
		{ \
			return std::static_pointer_cast<GameObject>(std::make_shared<name>()); \
		} \
	}; \
	inline name ## Class name ## Clazz; \
	class name

#define ED_CLASS_BODY(name, superClass) \
	public: \
	typedef superClass Super; \
	static const Class& GetClassStatic() \
	{ \
		return (Class&) name ## Clazz; \
	} \
	\
	virtual const Class& GetClass() const override \
	{ \
		return name::GetClassStatic(); \
	} \
	private:

#define ED_ABSTRACT_CLASS(name) class name; \
	class name ## Class : public Class \
	{ \
	friend class name; \
	public: \
		name ## Class() : Class(#name) \
		{ \
			ObjectFactory::RegisterClass(*this); \
		} \
		\
		virtual bool IsAbstract() const override \
		{ \
			return true; \
		} \
		\
		virtual std::shared_ptr<GameObject> Create() const override \
		{ \
			ED_ASSERT(0, "Cannot instantiate abstract class {}", m_Name); \
			return nullptr; \
		} \
	}; \
	inline name ## Class name ## Clazz; \
	class name

class Class
{
public:
	Class(const std::string& name) : m_Name(name)
	{

	}

	template<typename T>
	std::shared_ptr<T> Create() const
	{
		return std::static_pointer_cast<T>(Create());
	}

	virtual std::shared_ptr<class GameObject> Create() const = 0;
	virtual bool IsAbstract() const = 0;

	std::string GetName() const
	{
		return m_Name;
	}
protected:
	std::string m_Name;
};

class ObjectFactory
{
public:
	template<typename T>
	static std::shared_ptr<T> CreateObject(const std::string& name)
	{
		return std::static_pointer_cast<T>(CreateObject(name));
	}

	static std::shared_ptr<GameObject> CreateObject(const std::string& name)
	{
		ED_ASSERT(m_CreationFunctions.count(name), "Class with this name is not registered")
			return m_CreationFunctions.at(name)->Create();
	}

	static void RegisterClass(const Class& clazz)
	{
		ED_ASSERT(!m_CreationFunctions.count(clazz.GetName()), "Class with this name is already registered")
			m_CreationFunctions[clazz.GetName()] = &clazz;
	}

	static const Class* GetClass(const std::string& name)
	{
		return m_CreationFunctions.count(name) ? m_CreationFunctions.at(name) : nullptr;
	}
private:
	static inline std::map<std::string, const Class*> m_CreationFunctions;

	ObjectFactory()
	{

	}
};