#pragma once

#include <fbxsdk.h>

#include <string>
using namespace std;

//void DisplayMetaDataConnections(FbxObject* pNode);
string PrintString(const char* pHeader, const char* pValue = "", const char* pSuffix = "");
string PrintBool(const char* pHeader, bool pValue, const char* pSuffix = "");
string PrintInt(const char* pHeader, int pValue, const char* pSuffix = "");
string PrintDouble(const char* pHeader, double pValue, const char* pSuffix = "");
string Print2DVector(const char* pHeader, FbxVector2 pValue);
string Print3DVector(const char* pHeader, FbxVector4 pValue);
string Print4DVector(const char* pHeader, FbxVector4 pValue);
string PrintColor(const char* pHeader, FbxColor pValue);

