#pragma once

class GameObject;

class Class
{
public:
    Class(const std::string& name) : m_Name(name)
    {

    }

    virtual GameObject* Create() = 0;
    virtual bool IsAbstract() const = 0;

    template<typename T>
    T* Create()
    {
        return nullptr; reinterpret_cast<T*>(Create());
    }

    const std::string& GetName() const
    {
        return m_Name;
    }

    ~Class()
    {
        //for (u32 i = 0; i < MaxCapacity; ++i)
        //{
        //    if (GameObject*& object = m_Objects[i])
        //    {
        //        delete object;
        //        object = nullptr;
        //    }
        //}
    }
protected:
    template <typename GameObjectClass>
    GameObject* CreateAndRegister()
    {
        ED_ASSERT(m_FreeSlot < MaxCapacity, "Can not create more objects of this type.");

        GameObject* object = new GameObjectClass;

        m_Objects[m_FreeSlot] = object;

        return object;
    }

protected:
	static const inline u32 MaxCapacity = 1000; // TODO: Move this whole allocation system to some other place ;)

    std::string m_Name;

    GameObject* m_Objects[MaxCapacity];
    u32 m_FreeSlot = 0;
};

#define ED_CLASS(name) class name; \
	class name ## Class : public Class \
	{ \
	friend class name; \
	public: \
		name ## Class() : Class(#name) \
		{ \
			GameObjectFactory::RegisterClass(this); \
		} \
		\
		virtual bool IsAbstract() const override \
		{ \
			return false; \
		} \
		\
		virtual GameObject* Create() override \
		{ \
			return CreateAndRegister<name>(); \
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
			GameObjectFactory::RegisterClass(this); \
		} \
		\
		virtual bool IsAbstract() const override \
		{ \
			return true; \
		} \
		\
		virtual GameObject* Create() override \
		{ \
			ED_ASSERT(0, "Cannot instantiate abstract class {}", m_Name); \
			return nullptr; \
		} \
	}; \
	inline name ## Class name ## Clazz; \
	class name
