#pragma once

class GameObjectFactory
{
public:
    template<typename T>
    static T* CreateObject(const std::string& name)
    {
        return reinterpret_cast<T*>(CreateObject(name));
    }

    static GameObject* CreateObject(const std::string& name)
    {
        ED_ASSERT(m_Classes.count(name), "Class with this name is not registered");
        return m_Classes.at(name)->Create();
    }

    static void RegisterClass(Class* clazz)
    {
        ED_ASSERT(!m_Classes.count(clazz->GetName()), "Class with this name is already registered");
        m_Classes[clazz->GetName()] = clazz;
    }

    static Class* GetClass(const std::string& name)
    {
        return m_Classes.count(name) ? m_Classes.at(name) : nullptr;
    }
private:
    static inline std::map<std::string, Class*> m_Classes;

    GameObjectFactory()
    {
    }
};