#pragma once

#include "Filenames.h"
#include "PrintMaterial.h"

using namespace std;

// Temporary skin data
typedef struct SkinData
{
	float	boneIndex[4]{ 0.0f };
	float	boneWeight[4]{ 0.0f };
	int		minWeightIndex = 0;
	float	minWeight = 0.0f;
} SkinData;

void DisplayUserProperties(FbxObject* pObject, MeshHolder* mesh);
void GetMesh(FbxNode* pNode, MeshHolder* mesh, vector<PhongMaterial>& materials, std::string outputPath);
void GetPolygons(FbxMesh* pMesh, MeshHolder* mesh);
void zeroValues(Vertex* mesh);

void GetSkin(FbxMesh* fbxMesh, FbxGeometry* fbxGeo, MeshHolder* mesh);
void GetAnimation(fbxsdk::FbxMesh* fbxMesh, MeshHolder* mesh, fbxsdk::FbxSkin* skin);
void MinMaxWeights(fbxsdk::FbxSkin* skin, std::vector<SkinData>& controlPointSkinData);
void GetBindPose(fbxsdk::FbxSkin* skin, MeshHolder* mesh, fbxsdk::FbxMesh* fbxMesh);
void GetVertexWeights(fbxsdk::FbxMesh* fbxMesh, std::vector<SkinData>& controlPointSkinData, MeshHolder* mesh);
void GetSkeleton(FbxNode* fbxNode, int nodeIndex, int parent, MeshHolder* meshToPopulate);

FbxVector2 GetUV(FbxLayerElementUV* uvElement, int faceIndex, int vertexIndex);
FbxVector4 GetNormal(FbxGeometryElementNormal* normalElement, int faceIndex, int vertexIndex);
