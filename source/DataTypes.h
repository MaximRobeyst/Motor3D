
#include <math.h>
#include <cassert>

//Utils
#include "Utils.h"

// Vectors
#include "Vector.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "RGBColor.h"

// Matrices
#include "Matrix.h"
#include "Matrix3.h"
#include "Matrix4.h"

// Textures
#include "Texture.h"

/* --- TYPE DEFINES --- */
typedef Vector<2, float>	FVector2;
typedef Vector<2, double>	DVector2;
typedef Vector<2, int>		IVector2;
typedef Vector<3, float>	FVector3;
typedef Vector<3, double>	DVector3;
typedef Vector<3, int>		IVector3;
typedef Vector<4, float>	FVector4;
typedef Vector<4, double>	DVector4;
typedef Vector<4, int>		IVector4;

typedef Matrix<2, 2, float>		FMatrix2;
typedef Matrix<2, 2, double>	DMatrix2;
typedef Matrix<3, 3, float>		FMatrix3;
typedef Matrix<3, 3, double>	DMatrix3;
typedef Matrix<4, 4, float>		FMatrix4;
typedef Matrix<4, 4, double>	DMatrix4;