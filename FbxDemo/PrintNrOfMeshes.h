#pragma once

#include "PrintInfo.h"

//string PrintHierarchy(FbxScene* pScene);
//string PrintHierarchy(FbxNode* pNode, int pDepth);
string PrintNrOfMeshes(FbxScene* pScene);
int PrintNrOfMeshes(FbxNode* pNode, int pDepth);