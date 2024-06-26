#pragma once

#include "Core/Rendering/RenderGraph.h"
#include "Helpers/RenderingHelper.h"

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

	operator T& ()
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
	std::string ResourceName;

	virtual void Declare(std::shared_ptr<RenderGraph> graph) = 0;
};

enum class ReferenceAccessMode : uint8_t
{
	Read,
	Write
};

struct Reference
{
	std::string ResourceName;
	ReferenceAccessMode AccessMode;

	virtual void SetValue(std::shared_ptr<RenderGraph> graph) = 0;
};
