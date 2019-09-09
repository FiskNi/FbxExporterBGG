#pragma once

#include "Filenames.h"
#include "PrintMaterial.h"

using namespace std;

void DisplayUserProperties(FbxObject* pObject, MeshHolder* mesh);
void GetMesh(FbxNode* pNode, MeshHolder* mesh, vector<PhongMaterial>& materials);
void GetPolygons(FbxMesh* pMesh, MeshHolder* mesh);
void GetSkin(FbxMesh* fbxMesh, FbxGeometry* fbxGeo, MeshHolder* mesh);
void GetSkeleton(FbxNode* fbxNode, int nodeIndex, int parent, MeshHolder* meshToPopulate);
