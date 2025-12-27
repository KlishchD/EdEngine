#pragma once

class GameObject
{
public:
    GameObject(const std::string& name = "Object");

    virtual const Class& GetClass() const = 0;

    virtual void Serialize(Archive<>& archive);

    std::string GetName() const { return m_Name; }
    void SetName(const std::string& name) { m_Name = name; }

    virtual ~GameObject() = default;
protected:
    std::string m_Name;
};