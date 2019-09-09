#include "PrintTexture.h"

#include "Filenames.h"

#include <stdio.h>
#include <iostream>
#include <fstream>

/*
========================================================================================================================

	PrintTexture goes through all textures per material and prints the index of the material it belongs to, and the file name.
		This can be evolved as we see fit. 

	Currently, we cannot print out the texture index, as it always prints 0 no matter which texture we are at.

	There are a lot of outcommented code in these functions, but a lot might be useful if we need more informations for the textures. Please don't delete.
	There is also code for layered textures, but it is not implemented for our files right now.

	// Martina Molid

========================================================================================================================
*/


string PrintTextureInfo(FbxTexture* pTexture, int pBlendMode)
{
	FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(pTexture);
	FbxProceduralTexture *lProceduralTexture = FbxCast<FbxProceduralTexture>(pTexture);

	ofstream binFile(BINARY_FILE, ofstream::binary | ofstream::app);
	string pString;

	//pString += PrintString("            Name: \"", (char *)pTexture->GetName(), "\"");
	if (lFileTexture)
	{
		//pString += PrintString("            Type: File Texture");
		//pString += PrintString("            File Name: \"", (char *)lFileTexture->GetFileName(), "\"");
		string fullFileName = lFileTexture->GetFileName();
		int ffnLength = strlen(fullFileName.c_str());
		int lastSpace = fullFileName.find_last_of("/");

		// MM: In case you would like to add a specific folder for textures, add the folder filepath to the filename below. (See FIlenames.h for example)
		string tempFileName = fullFileName.substr(lastSpace+1, (ffnLength - lastSpace));
		int fileNameLength = strlen(tempFileName.c_str());

		char fileName[NAME_SIZE];
		for (int i = 0; i < fileNameLength; i++) {
			fileName[i] = tempFileName[i];
		}
		fileName[fileNameLength] = '\0';
		pString += PrintString("            File Name: \"", fileName, "\"");
		binFile.write((char*)fileName, sizeof(char) * NAME_SIZE);
	}
	else if (lProceduralTexture)
	{
		//pString += PrintString("            Type: Procedural Texture");
	}
	/*DisplayDouble("            Scale U: ", pTexture->GetScaleU());
	DisplayDouble("            Scale V: ", pTexture->GetScaleV());
	DisplayDouble("            Translation U: ", pTexture->GetTranslationU());
	DisplayDouble("            Translation V: ", pTexture->GetTranslationV());
	DisplayBool("            Swap UV: ", pTexture->GetSwapUV());
	DisplayDouble("            Rotation U: ", pTexture->GetRotationU());
	DisplayDouble("            Rotation V: ", pTexture->GetRotationV());
	DisplayDouble("            Rotation W: ", pTexture->GetRotationW());

	//const char* lAlphaSources[] = { "None", "RGB Intensity", "Black" };

	//DisplayString("            Alpha Source: ", lAlphaSources[pTexture->GetAlphaSource()]);
	//DisplayDouble("            Cropping Left: ", pTexture->GetCroppingLeft());
	//DisplayDouble("            Cropping Top: ", pTexture->GetCroppingTop());
	//DisplayDouble("            Cropping Right: ", pTexture->GetCroppingRight());
	//DisplayDouble("            Cropping Bottom: ", pTexture->GetCroppingBottom());

	//const char* lMappingTypes[] = { "Null", "Planar", "Spherical", "Cylindrical",
	//	"Box", "Face", "UV", "Environment" };

	DisplayString("            Mapping Type: ", lMappingTypes[pTexture->GetMappingType()]);

	//if (pTexture->GetMappingType() == FbxTexture::ePlanar)
	//{
	//	const char* lPlanarMappingNormals[] = { "X", "Y", "Z" };

	//	DisplayString("            Planar Mapping Normal: ", lPlanarMappingNormals[pTexture->GetPlanarMappingNormal()]);
	//}

	//const char* lBlendModes[] = { "Translucent", "Additive", "Modulate", "Modulate2", "Over", "Normal", "Dissolve", "Darken", "ColorBurn", "LinearBurn",
	//								"DarkerColor", "Lighten", "Screen", "ColorDodge", "LinearDodge", "LighterColor", "SoftLight", "HardLight", "VividLight",
	//								"LinearLight", "PinLight", "HardMix", "Difference", "Exclusion", "Substract", "Divide", "Hue", "Saturation", "Color",
	//								"Luminosity", "Overlay" };

	//if (pBlendMode >= 0)
	//	DisplayString("            Blend Mode: ", lBlendModes[pBlendMode]);
	//DisplayDouble("            Alpha: ", pTexture->GetDefaultAlpha());

	//if (lFileTexture)
	//{
	//	const char* lMaterialUses[] = { "Model Material", "Default Material" };
	//	DisplayString("            Material Use: ", lMaterialUses[lFileTexture->GetMaterialUse()]);
	//}

	//const char* pTextureUses[] = { "Standard", "Shadow Map", "Light Map",
	//	"Spherical Reflexion Map", "Sphere Reflexion Map", "Bump Normal Map" };

	//DisplayString("            Texture Use: ", pTextureUses[pTexture->GetTextureUse()]);
	//DisplayString("");*/

	binFile.close();
	return pString;
}

string FindAndDisplayTextureInfoByProperty(FbxProperty pProperty, bool& pDisplayHeader, int pMaterialIndex, int currentTexture) {

	string pString;
	if (pProperty.IsValid())
	{
		int lTextureCount = pProperty.GetSrcObjectCount<FbxTexture>();

		//cout << "Number of textures in PrintTexture: " << lTextureCount << endl;

		for (int j = 0; j < lTextureCount; ++j)
		{
			//Here we have to check if it's layeredtextures, or just textures:
			FbxLayeredTexture *lLayeredTexture = pProperty.GetSrcObject<FbxLayeredTexture>(j);
			if (lLayeredTexture)
			{
				pString += PrintInt("    Layered Texture: ", j);
				int lNbTextures = lLayeredTexture->GetSrcObjectCount<FbxTexture>();
				for (int k = 0; k < lNbTextures; ++k)
				{
					FbxTexture* lTexture = lLayeredTexture->GetSrcObject<FbxTexture>(k);
					if (lTexture)
					{

						if (pDisplayHeader) {
							pString += PrintInt("    Textures connected to Material ", pMaterialIndex);
							pDisplayHeader = false;
						}

						//NOTE the blend mode is ALWAYS on the LayeredTexture and NOT the one on the texture.
						//Why is that?  because one texture can be shared on different layered textures and might
						//have different blend modes.

						FbxLayeredTexture::EBlendMode lBlendMode;
						lLayeredTexture->GetTextureBlendMode(k, lBlendMode);
						pString += PrintString("    Textures for ", pProperty.GetName());
						pString += PrintInt("        Texture ", k);
						pString += PrintTextureInfo(lTexture, (int)lBlendMode);
					}

				}
			} 
			else
			{
				//no layered texture simply get on the property
				FbxTexture* lTexture = pProperty.GetSrcObject<FbxTexture>(j);
				if (lTexture)
				{
					// MM: Binary file has to be opened here as it will be opened in PrintTextureInfo, in order to get the order to print right
					ofstream binFile(BINARY_FILE, ofstream::binary | ofstream::app);

					//display connected Material header only at the first time // MM: This is deactivated for now, to print the material index the texture is connected to.
					//if (pDisplayHeader) {
						// MM: Number of textures is printed inside PrintMaterial instead of here.
					pString += PrintInt("    Textures connected to Material ", pMaterialIndex);
					binFile.write((char*)&pMaterialIndex, sizeof(int));
					//pDisplayHeader = false;
					//}

					//pString += PrintString("    Textures for ", pProperty.GetName());

					// MM: Printing out texture index gets messed up here as it wil always print 0, no matter which texture it is
					//		Therefore, for now, we don't print texture index as it is not needed, we can compare texture names, and the textures still have the its material's id
					//cout << "Texture no: " << j << endl;
					//pString += PrintInt("        Texture ", j);
					//binFile.write((char*)&j, sizeof(int));

					// MM: Binary file has to be closed here as it will be opened in PrintTextureInfo, in order to get the order to print right
					binFile.close();
					pString += PrintTextureInfo(lTexture, -1);
				}
			}
		}
	}//end if pProperty
	return pString;
}


string PrintTexture(FbxSurfaceMaterial *lMaterial, int lMaterialIndex) //FbxGeometry* pGeometry
{
	//int lMaterialIndex;
	FbxProperty lProperty;
	//if (pGeometry->GetNode() == NULL)
		//return "";
	//int lNbMat = pGeometry->GetNode()->GetSrcObjectCount<FbxSurfaceMaterial>();

	string pString;

	int nrOfTextures = 0;

	//for (lMaterialIndex = 0; lMaterialIndex < lNbMat; lMaterialIndex++) {
		//FbxSurfaceMaterial *lMaterial = pGeometry->GetNode()->GetSrcObject<FbxSurfaceMaterial>(lMaterialIndex);
		bool lDisplayHeader = true;

		//go through all the possible textures
		if (lMaterial) {

			int lTextureIndex;
			FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
			{
				lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);
				if (lProperty.IsValid())
				{
					pString += FindAndDisplayTextureInfoByProperty(lProperty, lDisplayHeader, lMaterialIndex, nrOfTextures);
				}
			}

		}//end if(lMaterial)

	//}// end for lMaterialIndex     
	return pString;
}
