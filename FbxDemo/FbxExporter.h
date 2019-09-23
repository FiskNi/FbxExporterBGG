#pragma once
#include "Filenames.h"
#include "../Common/Common.h"
#include "DisplaySkeleton.h"
#include "DisplayAnimation.h"

#include "PrintInfo.h"
#include "PrintMaterial.h"
#include "PrintMesh.h"
#include "PrintLight.h"

class Exporter
{
private:
	FbxManager* lSdkManager;
	FbxScene* fileScene;

	FbxString lFilePath;
	string outputName;
	string outputFilepath;

	// Error check
	bool lResult;

	// In commandline parameters
	bool writeGroups;
	bool writeMeshes;
	bool writeMaterials;
	bool writeSkeleton;
	bool writeAnimations;
	bool writeLights;

	vector<string> inParameters;

public:
	Exporter();
	~Exporter();
	void PrintContent();


};

