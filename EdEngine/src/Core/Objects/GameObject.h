#pragma once

#include "Class.h"
#include "Helpers/Serializable.h" // TODO: Move to core

class GameObject : public Serializable
{
public:
    GameObject(const std::string& name = "Object");
    
    virtual const Class& GetClass() const = 0;
    
    std::string GetName() const { return m_Name; }
    
    virtual ~GameObject() = default;
    void SetName(const std::string& name);
    
    virtual void Serialize(Archive& archive) override;

protected:
    std::string m_Name;
};