#pragma once

#include "Core/Ed.h"

class GameObject : public Serializable
{
public:
    GameObject(const std::string& name = "Object");

    std::string GetName() const { return m_Name; }

    virtual ~GameObject() = default;
    void SetName(const std::string& name);

    virtual void Serialize(Archive& archive) override;

protected:
    std::string m_Name = std::string(1024, 0);
};