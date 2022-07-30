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
};

template <typename Class, typename T>
using member_ptr_t = T Class::*;

template<typename Class, typename T>
class Member : public IMember<Class>
{
public:
	Member(std::string name, member_ptr_t<Class, T> ptr);

	std::string GetName() const;
	member_ptr_t<Class, T> GetPtr();
private:
	std::string m_Name;
	member_ptr_t<Class, T> m_Ptr;
};

template<typename Class, typename T>
inline Member<Class, T>::Member(std::string name, member_ptr_t<Class, T> ptr)
	: m_Name{name}
	, m_Ptr{ptr}
{
}

template<typename Class, typename T>
inline std::string Member<Class, T>::GetName() const
{
	return m_Name;
}

template<typename Class, typename T>
inline member_ptr_t<Class, T> Member<Class, T>::GetPtr()
{
	return m_Ptr;
}

template <typename Class>
class ClassMeta
{
public:/*
	template<typename T>
	void RenderGUI()
	{
		for (const auto& pair : m_Members)
		{
			const auto& name = pair.first;
			const auto& memberPtr = pair.second;

			ImGui::Input(name, memberPtr);
		}
	}*/

private:
	using MemberPtrType = std::unique_ptr<IMember<Class>>;
	using MemberMapType = std::unordered_map<std::string, MemberPtrType>;

	MemberMapType m_Members;
};
