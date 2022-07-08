#pragma once
#include <unordered_map>
#include <functional>

// https://stackoverflow.com/questions/43653962/is-that-possible-to-know-all-the-name-of-derived-classes
template<typename Base>
class Factory
{
public:
	static Factory<Base>& GetInstance()
	{
		static Factory<Base> instance{};
		return instance;
	}

	template <class Derived>
	void RegisterClassToFactory()
	{
		m_ClassCreators.insert({ typeid(Derived).name(), []()-> Base* {return new Derived();  } });
	}

	Base* Create(const std::string& derivedName)
	{
		// Safer way but is not o(1)
		//const auto it = m_ClassCreators.find(derivedName);
		//if (it == m_ClassCreators.end()) return nullptr;
		//return (it->second)();

		// This would be an o(1) operation but does not have safety
		return m_ClassCreators[derivedName]();
	}

	void PrintRegisteredClasses()
	{
		for (const auto& classRegister : m_ClassCreators)
		{
			// Writer debugger
		}
	}

	std::vector<std::string> GetComponentNames() const
	{
		std::vector<std::string> componentNames;

		for (auto iter = m_ClassCreators.begin(); iter != m_ClassCreators.end(); ++iter)
		{
			componentNames.emplace_back(iter->first);
		}

		return componentNames;
	}

private:
	Factory<Base>() = default;

	std::unordered_map<std::string, std::function<Base* ()>> m_ClassCreators;

};

template <typename Base, typename Derived>
class Creator
{
public:
	Creator()
	{
		Factory<Base>::GetInstance().RegisterClassToFactory<Derived>();
	}
};
