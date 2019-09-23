#include "FbxExporter.h"

FbxExporter::FbxExporter()
{
	FbxManager* lSdkManager = NULL;
	FbxScene* fileScene = NULL;

	lResult = false;


	lSdkManager = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);
	fileScene = FbxScene::Create(lSdkManager, "My Scene");
	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");

}

FbxExporter::~FbxExporter()
{
}

void FbxExporter::PrintContent()
{
}
