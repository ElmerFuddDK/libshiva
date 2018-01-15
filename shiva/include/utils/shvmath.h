#ifndef __SHIVA_UTILS_MATH_H
#define __SHIVA_UTILS_MATH_H

#include "shvstring.h"
#include "shvmathtokenmap.h"


//-=========================================================================================================
/// SHVMath class
/**
 * This class holds some static functions for doing math.
 \note All trigonometry functions are in radians instead of degrees - use Rad/Deg for conversion
 */

class SHVMath
{
public:

	// Trigonometry functions
	static double SHVAPI Sin(double x);
	static double SHVAPI Cosin(double x);
	static double SHVAPI Tan(double x);
	static double SHVAPI Deg(double rad);
	static double SHVAPI Rad(double deg);
	
	// Square root
	static double SHVAPI Sqrt(double x);

	// Power of
	static double SHVAPI Pow(double x, double y);

	// Modulus
	static double SHVAPI Mod(double x, double y);

	// Random
	static int SHVAPI Rand(SHVInt& seed);

	// Formula functions
	static double SHVAPI Eval(const SHVStringC str);
	static double SHVAPI Eval(const SHVStringC str, SHVString& err);
	static double SHVAPI EvalList(const SHVStringC formatStr, ...);
	static double SHVAPI EvalListErr(const SHVStringC formatStr, SHVString& err, ...);
	static double SHVAPI EvalMap(const SHVStringC str, SHVMathTokenMap& map);
	static double SHVAPI EvalMap(const SHVStringC str, SHVMathTokenMap& map, SHVString& err);
};

#endif
