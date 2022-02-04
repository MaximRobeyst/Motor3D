#pragma once



template<int N, int M, typename T>
struct Matrix
{
	//=== Data ===
	//Double array in C++ has row based layout ([row][column])!
	//we use store vectors in row based layout for good locality BUT we index them as column major matrices!
	T data[N][M];

	//=== Member Access Operators ===
#pragma region MemberAccessOperators
		//Access parameter order still happens as row,column indexing (standard in programming)
	inline T operator()(uint8_t r, uint8_t c) const
	{
		assert((r < M && c < N) && "ERROR: indices of Matrix () const operator are out of bounds!");
		return (data[c][r]);
	}

	inline T& operator()(uint8_t r, uint8_t c)
	{
		assert((r < M && c < N) && "ERROR: indices of Matrix () operator are out of bounds!");
		return (data[c][r]);
	}

	//Get a vector representation of a column (usage), but internally returns one of the rows
	inline const Vector<N, T>& operator[](uint8_t c) const
	{
		assert((c < N) && "ERROR: index of Matrix [] operator is out of bounds!");
		return (*reinterpret_cast<Vector<N, T>*>(data[c]));
	}

	inline Vector<N, T>& operator[](uint8_t c)
	{
		assert((c < N) && "ERROR: index of Matrix [] operator is out of bounds!");
		return (*reinterpret_cast<Vector<N, T>*>(data[c]));
	}
#pragma endregion
};