#pragma once

#include "Core/Ed.h"

class GameObject
{
    friend class boost::serialization::access;

    template <class Archive>
    void serialize(Archive& ar, const uint32_t version)
    {
        ar & m_Name;
    }
public:
    GameObject(const std::string& name = "Object");

    std::string GetName() const { return m_Name; }

    virtual ~GameObject() = default;
    void SetName(const std::string& name);

protected:
    std::string m_Name = std::string(1024, 0);
};

BOOST_CLASS_EXPORT_KEY(GameObject)