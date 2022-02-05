#pragma once
#include <cstdint>
#include <cstdlib>
#include <cfloat>
#include <limits>
#include <type_traits>

#define F_PI 3.1415926f
#define G_PI 3.14159265358979323846

#define TO_REGREES (180.0 / G_PI)
#define TO_RADIANS (G_PI / 180.0)

#undef min

template<typename T>
//typename std::enable_if<!std::numeric_limits<T>::is_integer, bool>::type
bool AreEqual(T a, T b, uint16_t ulp = 2)//Only works when non-integer types used
{
	// Source: cpp-reference, the machine epsilon has to be scaled to the magnitude of the values used
	// and multiplied by the desired precision in ULPs (units in the last place)
	return std::abs(a - b) <= std::numeric_limits<T>::epsilon() * std::abs(a + b) * ulp
		// unless the result is subnormal
		|| std::abs(a - b) < std::numeric_limits<T>::min();
}

/*! Function to convert degrees to radians */
template<typename T, typename = std::enable_if<std::is_pod<T>::value>>
constexpr T ToRadians(const T angle)
{
	return angle * (static_cast<T>(G_PI) / static_cast<T>(180.f));
}

template<typename T>
constexpr T Clamp(const T a, T min, T max)
{
	if (a < min)
		return min;
	if (a > max)
		return max;

	return a;
}