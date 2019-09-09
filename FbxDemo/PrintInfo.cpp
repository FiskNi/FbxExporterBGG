#include "PrintInfo.h"

/*
========================================================================================================================

	PrintInfo is based off DisplayCommon, and is mainly just used to make printing information from the original FBX SDK simpler.
		We might not need it later if we would desire to completely eradicate it, but we would have to rewrite how we print information.

	The vector printing functions, however, are quite useful.

	// Martina Molid

========================================================================================================================
*/


string PrintString(const char* pHeader, const char* pValue /* = "" */, const char* pSuffix /* = "" */)
{
	string lString;

	lString = pHeader;
	lString += pValue;
	lString += pSuffix;
	lString += "\n";
	return lString;
}

string PrintBool(const char* pHeader, bool pValue, const char* pSuffix /* = "" */)
{
	string lString;

	lString = pHeader;
	lString += pValue ? "true" : "false";
	lString += pSuffix;
	lString += "\n";
	return lString;
}

string PrintInt(const char* pHeader, int pValue, const char* pSuffix /* = "" */)
{
	string lString;

	lString = pHeader;
	lString += to_string(pValue);
	lString += pSuffix;
	lString += "\n";
	return lString;
}


string PrintDouble(const char* pHeader, double pValue, const char* pSuffix /* = "" */)
{
	string lString;
	FbxString lFloatValue = (float)pValue;

	lFloatValue = pValue <= -HUGE_VAL ? "-INFINITY" : lFloatValue.Buffer();
	lFloatValue = pValue >= HUGE_VAL ? "INFINITY" : lFloatValue.Buffer();

	lString = pHeader;
	lString += lFloatValue;
	lString += pSuffix;
	lString += "\n";
	return lString;
}

string Print2DVector(const char* pHeader, FbxVector2 pValue)
{
	string lString;
	FbxString lFloatValue1 = (float)pValue[0];
	FbxString lFloatValue2 = (float)pValue[1];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += " ";
	lString += lFloatValue2;
	lString += "\n";
	return lString;
}

string Print3DVector(const char* pHeader, FbxVector4 pValue)
{
	string lString;
	FbxString lFloatValue1 = (float)pValue[0];
	FbxString lFloatValue2 = (float)pValue[1];
	FbxString lFloatValue3 = (float)pValue[2];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
	lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
	lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += " ";
	lString += lFloatValue2;
	lString += " ";
	lString += lFloatValue3;
	lString += "\n";
	return lString;
}

string Print4DVector(const char* pHeader, FbxVector4 pValue)
{
	string lString;
	FbxString lFloatValue1 = (float)pValue[0];
	FbxString lFloatValue2 = (float)pValue[1];
	FbxString lFloatValue3 = (float)pValue[2];
	FbxString lFloatValue4 = (float)pValue[3];

	lFloatValue1 = pValue[0] <= -HUGE_VAL ? "-INFINITY" : lFloatValue1.Buffer();
	lFloatValue1 = pValue[0] >= HUGE_VAL ? "INFINITY" : lFloatValue1.Buffer();
	lFloatValue2 = pValue[1] <= -HUGE_VAL ? "-INFINITY" : lFloatValue2.Buffer();
	lFloatValue2 = pValue[1] >= HUGE_VAL ? "INFINITY" : lFloatValue2.Buffer();
	lFloatValue3 = pValue[2] <= -HUGE_VAL ? "-INFINITY" : lFloatValue3.Buffer();
	lFloatValue3 = pValue[2] >= HUGE_VAL ? "INFINITY" : lFloatValue3.Buffer();
	lFloatValue4 = pValue[3] <= -HUGE_VAL ? "-INFINITY" : lFloatValue4.Buffer();
	lFloatValue4 = pValue[3] >= HUGE_VAL ? "INFINITY" : lFloatValue4.Buffer();

	lString = pHeader;
	lString += lFloatValue1;
	lString += ", ";
	lString += lFloatValue2;
	lString += ", ";
	lString += lFloatValue3;
	lString += ", ";
	lString += lFloatValue4;
	lString += "\n";
	return lString;
}

string PrintColor(const char* pHeader, FbxColor pValue)
{
	string lString;

	lString = pHeader;
	lString += to_string((float)pValue.mRed);

	lString += ", ";
	lString += to_string((float)pValue.mGreen);

	lString += ", ";
	lString += to_string((float)pValue.mBlue);

	lString += "\n";
	return lString;
}

