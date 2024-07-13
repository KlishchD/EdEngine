#pragma once

#include "Core/Ed.h"

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

  Resource(const std::string& name) : m_Name(name)
  {
  }

  Resource(const ResourceSpecification& specification) : m_Name(specification.Name)
  {
  }

  void SetName(const std::string& name) { m_Name = name; }
  const std::string& GetName() const { return m_Name; }

  virtual void* GetNativeResource() const = 0;
  virtual void SetNativeResource(void* resource) = 0;

  template <typename T> requires (!std::is_void_v<T>)
  T GetNativeResource() const
  {
    return reinterpret_cast<T>(GetNativeResource());
  }

  template <typename T> requires (!std::is_void_v<T>)
  void SetNativeResource(T resource)
  {
    SetNativeResource(reinterpret_cast<void*>(resource));
  }

protected:
  std::string m_Name;
};
