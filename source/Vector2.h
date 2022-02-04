#pragma once
/*=============================================================================*/
// Copyright 2019 Elite Engine 2.0
// Authors: Matthieu Delaere
/*=============================================================================*/
// EVector2.h: Vector2D struct
/*=============================================================================*/

#include <cassert>
#include "Vector.h"
#include "Utils.h"

template<typename T>
struct Vector<2, T>
{
	union
	{
		T data[2];
		struct { T x, y; };
		struct { T r, g; };
	};

	// Contructors
	Vector<2, T>() = default;
	Vector<2, T>(T x, T y)
		: x(x), y(y) {}
	Vector<2, T>(const Vector<2, T>& v)
		: x(v.x), y(v.y) {}
	Vector<2,T>(Vector<2,T>&& v) noexcept
		: x(std::move(v.x)), y(std::move(v.y)) {}
	explicit Vector<2,T>(const Vector<3,T>& v)
		: x(v.x), y(v.y){}
	explicit Vector<2,T>(const Vector<4,T>& v)
		: x(v.x), y(v.y) {}

	// Conversion operator
	template<typename U>
	operator Vector<2, U>() const
	{
		return Vector<2, U>(
			static_cast<U>(this->x),
			static_cast<U>(this->y)
			);
	}

	// Arithmetics
	template<typename U>
	inline Vector<2, T> operator+(const Vector<2, U>& v) const
	{
		return Vector<2, T>(x + static_cast<T>(v.x), y + static_cast<T>(v.y));
	}

	template<typename U>
	inline Vector<2, T> operator-(const Vector<2, U>& v) const
	{
		return Vector<2, T>(x - static_cast<T>(v.x), y - static_cast<T>(v.y));
	}

	inline Vector<2, T> operator*(T scale) const
	{
		return Vector<2, T>(x * scale, y * scale);
	}

	inline Vector<2, T> operator/(T scale) const
	{
		const T revS = static_cast<T>(1.f / scale);
		return Vector<2, T>(x * revS, y * revS);
	}

	// Compound Assigment operators
	inline Vector<2,T>& operator=(const Vector<2,T>& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	inline Vector<2, T>& operator+=(const Vector<2, T>& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	inline Vector<2, T>& operator-=(const Vector<2, T>& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	inline Vector<2, T>& operator*=(T scale)
	{
		x *= scale;
		y *= scale;
		return *this;
	}

	// Unary operators
	inline Vector<2, T> operator-() const
	{
		return Vector<2, T>(-x, -y);
	}

	// Relational operators
	inline bool operator==(const Vector<2,T>& v) const
	{
		return AreEqual<T>(x, v.x) && AreEqual<T>(y, v.y);
	}

	inline bool operator!=(const Vector<2, T>& v) const
	{
		return !(*this == v);
	}

	// Member Access operators
	inline T operator[](uint8_t i) const
	{
		assert((i < 2));
		return data[i];
	}

	inline T& operator[](uint8_t i)
	{
		assert(i < 2);
		return data[i];
	}

	// Static Functions
	static Vector<2, T> ZeroVector();
};

// Global operators
template<typename T, typename U>
inline Vector<2, T> operator*(U scale, const Vector<2, T>& v)
{
	T s = static_cast<T>(scale);
	return Vector<2, T>(v.x * s, v.y * s);
}

// Global Functions
template<typename T>
inline Vector<2, T> Vector<2, T>::ZeroVector()
{
	T z = static_cast<T>(0);
	return Vector<2, T>(z, z);
}

template<typename T>
inline T Dot(const Vector<2, T>& v1, const Vector<2, T>& v2)
{
	return v1.x * v2.x + v1.y * v2.y;
}

template<typename T>
inline T Cross(const Vector<2, T>& v1, const Vector<2, T>& v2)
{
	return v1.x * v2.y - v1.y * v2.x;
}

//Returns 2D vector rotated 90 degrees counter-clockwise (where y-axis is up)
template<typename T>
inline Vector<2, T> Perpendicular(const Vector<2, T>& v)
{
	return Vector<2, T>(-v.y, v.x);
}

template<typename T>
inline Vector<2, T> GetAbs(const Vector<2, T>& v)
{
	return Vector<2, T>(abs(v.x), abs(v.y));
}

template<typename T>
inline Vector<2, T> Max(const Vector<2, T>& v1, const Vector<2, T>& v2)
{
	Vector<2, T>v = v1;
	if (v2.x > v.x) v.x = v2.x;
	if (v2.y > v.y) v.y = v2.y;
	return v;
}

template<typename T>
inline Vector<2, T> Min(const Vector<2, T>& v1, const Vector<2, T>& v2)
{
	Vector<2, T>v = v1;
	if (v2.x < v.x) v.x = v2.x;
	if (v2.y < v.y) v.y = v2.y;
	return v;
}
