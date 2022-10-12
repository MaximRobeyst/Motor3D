#pragma once
#include <type_traits>


template<typename Enum>
struct EnableBitMaskOperators
{
	static const bool enable = false;
};

template<typename Enum>
std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum>
operator |(Enum lhs, Enum rhs)
{
	using underlying = std::underlying_type_t<Enum>;
	return static_cast<Enum> (
		static_cast<underlying>(lhs) |
		static_cast<underlying>(rhs)
		);
}

template<typename Enum>
std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum>
operator &(Enum lhs, Enum rhs)
{
	using underlying = std::underlying_type_t<Enum>;
	return static_cast<Enum> (
		static_cast<underlying>(lhs) &
		static_cast<underlying>(rhs)
		);
}

template<typename Enum>
std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum>
operator &=(Enum& lhs, Enum rhs)
{
	using underlying = std::underlying_type_t<Enum>;
	lhs = static_cast<Enum> (
		static_cast<underlying>(lhs) &
		static_cast<underlying>(rhs)
		);

	return lhs;
}

template<typename Enum>
std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum>
operator |=(Enum& lhs, Enum rhs)
{
	using underlying = std::underlying_type_t<Enum>;
	lhs = static_cast<Enum> (
		static_cast<underlying>(lhs) |
		static_cast<underlying>(rhs)
		);

	return lhs;
}

template<typename Enum>
std::enable_if_t<EnableBitMaskOperators<Enum>::enable, Enum>
operator ~(Enum lhs)
{
	using underlying = std::underlying_type_t<Enum>;
	lhs = static_cast<Enum> (
		~static_cast<underlying>(lhs));

	return lhs;
}

template<typename Enum>
std::enable_if_t<EnableBitMaskOperators<Enum>::enable, bool>
isSet(Enum lhs, Enum rhs)
{
	return (lhs & rhs) == rhs;
}
