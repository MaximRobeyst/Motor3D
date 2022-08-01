#pragma once
#include <string>
#include <unordered_map>
#include <memory>

#include "ImGuiHelpers.h"

//https://eliasdaler.github.io/meta-stuff/

template<class Class>
class IMember
{
public:
	virtual ~IMember() = default;

	virtual void RenderGUI(Class& obj, const std::string& name) = 0;
};

template <typename Class, typename T>
using member_ptr_t = T Class::*;

template<typename Class, typename T>
class Member : public IMember<Class>
{
public:
	Member(T Class::* ptr);

	member_ptr_t<Class, T> GetPtr();

	void RenderGUI(Class& obj, const std::string& name)
	{
		ImGui::Input(name.c_str(), obj.*m_Ptr);
	}
private:
	T Class::* m_Ptr;
};

template<typename Class, typename T>
inline Member<Class, T>::Member(T Class::* ptr)
	: m_Ptr{ptr}
{
}

template<typename Class, typename T>
inline member_ptr_t<Class, T> Member<Class, T>::GetPtr()
{
	return m_Ptr;
}

template <typename Class>
class ClassMeta
{
public:
	template <typename T>
	void AddMemberPtr(std::string name, T Class::* ptr)
	{
		m_Members.emplace(name, std::make_unique<Member<Class, T>>(ptr));
	}

	template<typename T>
	void RenderGUI(Class& obj)
	{
		for (const auto& pair : m_Members)
		{
			const auto& name = pair.first;
			const auto& memberPtr = pair.second;

			memberPtr->RenderGUI(obj, name);
		}
	}

	using MemberPtrType = std::unique_ptr<IMember<Class>>;
	using MemberMapType = std::unordered_map<std::string, MemberPtrType>;
private:

	MemberMapType m_Members;
};
