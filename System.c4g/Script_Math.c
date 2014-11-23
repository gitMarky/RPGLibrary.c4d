/*-- Math libraries.
@version 0.1.0
@author Unknown
--*/

#strict 2


/** Addition of two vectors
*/
global func Vec_Add(array x, array y)
{
	return [x[0] + y[0], x[1] + y[1]];
}

/** Subtraction of two vectors
*/
global func Vec_Sub(array x, array y)
{
	return [x[0] - y[0], x[1] - y[1]];
}

/** Multiplication of a vector and a scalar
*/
global func Vec_Mul(array x, int i)
{
	return [x[0] * i, x[1] * i];
}

/** Division of a vector and a scalar
*/
global func Vec_Div(array x, int i)
{
	return [x[0] / i, x[1] / i];
}

/** Dot product of two vectors
*/
global func Vec_Dot(array x, array y)
{
	return x[0] * y[0] + x[1] * y[1];
}

/** Cross product of one vectors
*/
global func Vec_Cross(array x)
{
	return [x[1], -x[0]];
}


/** Euclidean length of a vector
*/
global func Vec_Length(array x)
{
	return Sqrt(x[0] * x[0] + x[1] * x[1]);
}

/** Angle between two vectors
*/
global func Vec_Angle(array x, array y)
{
	return Angle(0, 0, y[0], y[1]) - Angle(0, 0, x[0], x[1]);
}

/** Normalizes a vector with precision
*/
global func Vec_Normalize(array x, int precision)
{
	var prec = precision;
	if (!prec)
		prec = 1;
	return Vec_Div(Vec_Mul(x, prec), Vec_Length(x));
}

