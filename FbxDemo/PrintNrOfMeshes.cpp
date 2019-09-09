#include <fbxsdk.h>
#include "PrintNrOfMeshes.h"

#include "Filenames.h"

#include <fstream>
#include <iostream>
using namespace std;

/*
========================================================================================================================

	PrintNrOfMeshes prints the number of meshes at the very top of the file. 
		This is currently the first function called, which also wipes the previously existing file.

	These functions are based of the Hierarchy functions, which have been left in here (as we probably want to use them later), 
		to go through the hierarchies and count number of meshes.

	PrintHierarchy will most likely be needed later, leave it be for now.

	// Martina Molid

========================================================================================================================
*/

//string PrintHierarchy(FbxNode* pNode, int pDepth);

//string PrintHierarchy(FbxScene* pScene)
//{
//	int i;
//	FbxNode* lRootNode = pScene->GetRootNode();
//	string pString;
//
//	ofstream binFile("xBinVtx.bin", ofstream::binary);
//
//	for (i = 0; i < lRootNode->GetChildCount(); i++)
//	{
//		pString += PrintHierarchy(lRootNode->GetChild(i), 0);
//	}
//	int temp = lRootNode->GetChildCount();
//	pString += to_string(temp);
//	binFile.write((char*)&temp, sizeof(int));
//	binFile.close();
//	return pString;
//}
//
//string PrintHierarchy(FbxNode* pNode, int pDepth)
//{
//	string lString;
//	int i;
//
//	for (i = 0; i < pDepth; i++)
//	{
//		lString += "     ";
//	}
//
//	lString += pNode->GetName();
//	lString += "\n";
//
//	//FBXSDK_printf(lString.Buffer());
//
//	for (i = 0; i < pNode->GetChildCount(); i++)
//	{
//		lString += PrintHierarchy(pNode->GetChild(i), pDepth + 1);
//		return lString;
//	}
//
//	return lString;
//}

string PrintNrOfMeshes(FbxScene* pScene)
{
	int i;
	FbxNode* lRootNode = pScene->GetRootNode();
	string pString;
	int nrOfMeshes = 0;

	// MM: Opens the binary file in binary mode. As this is the first time something it written to the binary file, this will wipe the existing file.
	// The file gets wiped when this saves.
	ofstream binFile(BINARY_FILE, ofstream::binary);

	// Loop of all the elements in the scene
	for (i = 0; i < lRootNode->GetChildCount(); i++)
	{
		// Will loop trough all the children of the secondary root elements (elements under the scene)
		nrOfMeshes += PrintNrOfMeshes(lRootNode->GetChild(i), 0);
	}

	// Proper way of getting scene element count?
	// This will receive the child count of the scene (root) node
	// When true, will also aquire all the children of the children recurisvely
	int elementCount = lRootNode->GetChildCount(true);

	pString += "Nr of Meshes: " +to_string(nrOfMeshes)+"\n";
	cout << "Nr of meshes: " << nrOfMeshes << endl;
	binFile.write((char*)&nrOfMeshes, sizeof(int));
	binFile.close();
	return pString;
}

int PrintNrOfMeshes(FbxNode* pNode, int pDepth)
{
	int nrOfMeshes = 1;

	for (int i = 0; i < pNode->GetChildCount(); i++)
	{
		nrOfMeshes += PrintNrOfMeshes(pNode->GetChild(i), pDepth + 1);
		return nrOfMeshes;
	}

	return nrOfMeshes;
}
