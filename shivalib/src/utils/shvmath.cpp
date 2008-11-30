/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/utils/shvmath.h"

#include <math.h>

///\cond INTERNAL
#define SHVMATH_PI 3.14159265358979323846
double shvmath_doeval(const SHVTChar*& str, char level, SHVString& err);
///\endcond


// ========================================================================================================
//  SHVMath class
// ========================================================================================================

/*************************************
 * Sin
 *************************************/
/// Returns sinus of x in radians
double SHVMath::Sin(double x)
{
	return ::sin(x);
}

/*************************************
 * Cosin
 *************************************/
/// Returns cosinus of x in radians
double SHVMath::Cosin(double x)
{
	return ::cos(x);
}

/*************************************
 * Tan
 *************************************/
/// Returns tangens of x in radians
double SHVMath::Tan(double x)
{
	return ::tan(x);
}

/*************************************
 * Deg
 *************************************/
/// Converts radians to degrees
double SHVMath::Deg(double rad)
{
	return rad * (180.0/SHVMATH_PI);
}

/*************************************
 * Rad
 *************************************/
/// Converts degrees to radians
double SHVMath::Rad(double deg)
{
	return deg * (SHVMATH_PI/180.0);
}

/*************************************
 * Sqrt
 *************************************/
/// Returns square root of x
double SHVMath::Sqrt(double x)
{
	return ::sqrt(x);
}

/*************************************
 * Pow
 *************************************/
/// Returns x^y
double SHVMath::Pow(double x, double y)
{
	return ::pow(x,y);
}

/*************************************
 * Eval(str)
 *************************************/
/// Evaluates a string
/**
 \param str String to evaluate
 \return result of evaluation
 *
 * This function will evaluate the contents of the string as a
 * formula and return the result. A simple example of this could
 * be the following:
 \code
void foo()
{
	if (SHVMath::Eval(_T("2+2")) == 4)
		printf("bar\n");
}
 \endcode
 * It also supports a list of built in functions. These can be
 * used in the formula to do calculations, and are not case
 * sensitive:
 \arg Sin(x) - calculates sinus of x
 \arg Cosin(x) - calculates cosinus of x
 \arg Tan(x) - calculates tangens of x
 \arg Sqrt(x) or Q(x) - calculates square root of x
 \arg Rad(deg) - converts degrees to radians
 \arg Deg(rad) - converts radians to degrees
 *
 * Where (x) represents a sub-formula. It is therefore possible
 * to do the following:
\code
double foo()
{
	return SHVMath::Eval(_T(" sin( rad(180)*1.5 ) "));
}
\endcode
 *
 \note All double values in strings are formatted with a . as decimal separator in SHIVA
 \note All function names are case insensitive for better support for user entered values
 */
double SHVMath::Eval(const SHVStringC str)
{
SHVString err;
const SHVTChar* cstr = str.GetSafeBuffer();
	return shvmath_doeval(cstr,0,err);
}

/*************************************
 * Eval
 *************************************/
/// Evaluates a string
/**
 \param str String to evaluate
 \param err String to hold an error - null if none
 \return result of evaluation
 \see Eval
 */
double SHVMath::Eval(const SHVStringC str, SHVString& err)
{
const SHVTChar* cstr = str.GetSafeBuffer();
	err = NULL;
	return shvmath_doeval(cstr,0,err);
}

/*************************************
 * Eval(formatlist)
 *************************************/
/// Formats and evaluates a string
/**
 \param formatStr String to format and then evaluate
 \return result of evaluation
 \see Eval
 */
double SHVMath::EvalList(const SHVStringC formatStr, ...)
{
SHVString str;
SHVVA_LIST args;

	SHVVA_START(args,formatStr);
	str.FormatList(formatStr.GetSafeBuffer(),args);
	SHVVA_END(args);
	
	return SHVMath::Eval(str);
}

/*************************************
 * Eval(formatlist)
 *************************************/
/// Formats and evaluates a string
/**
 \param formatStr String to format and then evaluate
 \param err String to hold an error - null if none
 \return result of evaluation
 \see Eval
 */
double SHVMath::EvalListErr(const SHVStringC formatStr, SHVString& err, ...)
{
	SHVString str;
	SHVVA_LIST args;

	SHVVA_START(args,err);
	str.FormatList(formatStr.GetSafeBuffer(),args);
	SHVVA_END(args);
	
	return SHVMath::Eval(str,err);
}


///\cond INTERNAL
/*************************************
 * shvmath_trim
 *************************************/
void shvmath_trim(const SHVTChar*& str)
{
static const SHVTChar charTrimStr[] = {' ','\t','\0'};
SHVStringC trimStr(charTrimStr);
	
	while (*str != '\0')
	{
		if (trimStr.LocateChar(*str)>=0) str++;
		else break;
	}
}

double shvmath_calculate(SHVTChar op, double a, double b, SHVString& err)
{
	if (!err.IsNull())
		return a;
	
	switch (op)
	{
		case '+':	return a+b;
		case '-':	return a-b;
		case '*':	return a*b;
		case '/':	return a/b;
		case '^':	return SHVMath::Pow(a,b);
		default:
			err.Format(_T("Invalid operant '%c'"), op);
	}
	
	return a; // in order to preserve the original result
}

/*************************************
 * shvmath_gettoken
 *************************************/
SHVStringBuffer shvmath_gettoken(const SHVTChar*& str)
{
const SHVTChar* start;
static const SHVTChar charStopStr[] = {' ','(',')','+','-','*','/','^','\t','\r','\n','\0'};
SHVStringC stopStr(charStopStr);
	
	shvmath_trim(str);
	start = str;
	
	while (*str != '\0')
	{
		if (stopStr.LocateChar(*str) == -1) str++;
		else break;
	}
	
	return SHVStringC(start).Left(str-start);
}

/*************************************
 * shvmath_performfunc
 *************************************/
double shvmath_performfunc(const SHVStringC token, double val, SHVString& err)
{
	if (!err.IsNull())
		return 0.0;
	
	if (token.Compare(_T("sin")) == 0)
	{
		return SHVMath::Sin(val);
	}
	else if  (token.Compare(_T("cosin")) == 0)
	{
		return SHVMath::Cosin(val);
	}
	else if  (token.Compare(_T("tan")) == 0)
	{
		return SHVMath::Cosin(val);
	}
	else if  (token.Compare(_T("rad")) == 0)
	{
		return SHVMath::Rad(val);
	}
	else if  (token.Compare(_T("deg")) == 0)
	{
		return SHVMath::Deg(val);
	}
	else if  (token.Compare(_T("sqrt")) == 0 || token.Compare(_T("q")) == 0)
	{
		return SHVMath::Sqrt(val);
	}
	else
	{
		err.Format(_T("Invalid token %s"), token.GetSafeBuffer());
		return 0.0;
	}
}

/*************************************
 * shvmath_getnumber
 *************************************/
// level : 0 == +-
// level : 1 == */
// level : 2 == ^
double shvmath_getnumber(const SHVTChar*& str, char level, SHVString& err)
{
SHVTChar* endp;
double retVal;
	
	if (!err.IsNull())
		return 0.0;
	
	shvmath_trim(str);
	
	retVal = SHVStringC::StrToDouble(str,&endp);
	
	// test if we got a number
	if (endp > str)
	{
		str = endp;
	}
	// paranthesis mode
	else if (str[0] == '(')
	{
	char tmplevel = 0;
		str++;
		retVal = shvmath_doeval(str,tmplevel,err);
		
		if (err.IsNull() && *str != ')')
			err = _T("Missing paranthesis");
		else
			str++;
	}
	else // it must be some sort of function or variable
	{
	SHVString token;
			
		token = shvmath_gettoken(str);
		token.MakeLower();
			
		shvmath_trim(str);
			
		if (token == _T(""))
		{
			err = _T("Missing operant");
		}
		else if (str[0] == '(')
		{
			char tmplevel = 0;
			str++;
			retVal = shvmath_performfunc(token,shvmath_doeval(str,tmplevel,err),err);
			
			if (err.IsNull() && *str != ')')
				err = _T("Missing paranthesis");
			else
				str++;
		}
		else
		{
			if (token.Compare(_T("pi")) == 0)
			{
				retVal = SHVMATH_PI;
			}
			else
			{
				err.Format(_T("Invalid constant %s"), token.GetSafeBuffer());
			}
		}
	}
	
	if (err.IsNull())
	{
	SHVTChar op;
		
		shvmath_trim(str);
		
		op = *str;
		
		// elevate level according to operant
		switch (op)
		{
		case '/':
		case '*':
			if (level < 1)
			{
				str++;
				retVal = shvmath_calculate(op,retVal,shvmath_doeval(str,1,err),err);
			}
			break;
		case '^':
			if (level < 2)
			{
				str++;
				retVal = shvmath_calculate(op,retVal,shvmath_doeval(str,2,err),err);
			}
			break;
		}
	}
	
	return retVal;
}

/*************************************
 * shvmath_doeval
 *************************************/
// level : 0 == +-
// level : 1 == */
// level : 2 == ^
double shvmath_doeval(const SHVTChar*& str, char level, SHVString& err)
{
double retVal = 0.0;
double temp;
bool gotNumber = false;
SHVTChar operant = '\0';

	while(str != NULL && *str != '\0' && *str != ')' && err.IsNull() && level > -1)
	{
		if (!gotNumber)
		{
			temp = shvmath_getnumber(str,level,err);
			if (operant != '\0')
			{
				// perform operation
				retVal = shvmath_calculate(operant,retVal,temp,err);
			}
			else
			{
				retVal = temp;
			}
		}
		else // operator mode
		{
			shvmath_trim(str);
			
			operant = *str;
			str++;
			
			// check end of level
			switch (operant)
			{
			case '+':
			case '-':
				if (level > 0)
					level = -1; // exit loop
				break;
			case '/':
			case '*':
				if (level > 1)
					level = -1; // exit loop
				break;
			case '^':
				if (level > 2)
					level = -1; // exit loop
				break;
			}
		}
		gotNumber = !gotNumber;
	}
	
	return retVal;

/*
	if (pos >= (spos)str.GetLength() || err != (CString)"")
		return NULL;

	if (token == (CString)"") // parenthesis check
	{
		RFTRecFilter_impl* newNode;
		switch ( (char)str[pos] )
		{
			case '(':
				pos++; // eat parenthesis
				retVal = new RFTRecFilter_impl("","",RFTRecFilter::FilterTrue,caseSens);
				retVal->AddParenthesisRelation(newNode = CreateFromStringRec(str,pos,err,caseSens));
				AddRelations(newNode, str, pos, err, caseSens);
				if (pos >= (spos)str.GetLength() || str[pos] != ')')
					err = "Missing paranthesis";
				else
					pos++;
				break;
			case ')':
				break; // no error
			case '!':
				pos++;
				retVal = new RFTRecFilter_impl("","",RFTRecFilter::FilterNot,caseSens);
				retVal->AddParenthesisRelation(CreateFromStringRec(str,pos,err,caseSens));
				break;
			default:
				err.Format(_T("Unknown char '%d' at %d"), str[pos], pos);
		}
	}
	else
	{
		RFTRecFilter::FilterType Type = GetFType(str,pos);

		if (Type)
		{
			CString* val = GetFValue(str,pos,err);
			if (err == _T(""))
			{
				if (!val) // filter needs to be changed to Null based types
				{
					val = new CString;

					switch ( Type )
					{
						case RFTRecFilter::FilterIsNot:
							Type = RFTRecFilter::FilterIsNotNull;
							break;
						case RFTRecFilter::FilterIs:
							Type = RFTRecFilter::FilterIsNull;
							break;
						default:
							Type = RFTRecFilter::FilterUnknown;
							err = "Type mismatch";
							break;
					}
				}

				if (Type)
				{
					retVal = new RFTRecFilter_impl(token, *val, Type, caseSens);
				}
				
			}

			if (val)
				delete val;
		}
		else
			err = "Missing compare type";
*/
}
///\endcond
