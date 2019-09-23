#include "PrintMaterial.h"

/*
========================================================================================================================
	PrintMaterial prints out the material information per mesh. 
	For each material connected to the mesh, the function prints material index, material name and the Phong attributes, nr of textures for the material.
	Currently only supports Phong materials, but code for Lambert is left in case it would be desired.
========================================================================================================================
*/

void PrintMaterial(FbxGeometry* pGeometry, vector<PhongMaterial>& mats, MeshHolder* mesh, std::string outputPath)
{
	int materialCount = 0;
	FbxNode* materialNode = NULL;
	PhongMaterial *materials = nullptr;


	if (pGeometry) 
	{
		materialNode = pGeometry->GetNode();
		if (materialNode) 
		{
			materialCount = materialNode->GetMaterialCount();
			materials = new PhongMaterial[materialCount];
		}
	}

	if (materialCount >= 1)
	{
		FbxPropertyT<FbxDouble3> lKFbxDouble3;
		FbxPropertyT<FbxDouble> lKFbxDouble1;
		FbxColor theColor;


		for (int matIndex = 0; matIndex < materialCount; matIndex++)
		{
			// Get material from node
			FbxSurfaceMaterial *lMaterial = materialNode->GetMaterial(matIndex);

			// Defaults the names
			string temp = "-1";

			materials[matIndex].albedo[0] = '-';
			materials[matIndex].albedo[1] = '1';
			materials[matIndex].albedo[2] = '\0';

			materials[matIndex].normal[0] = '-';
			materials[matIndex].normal[1] = '1';
			materials[matIndex].normal[2] = '\0';

			// Applies the material name
			int nameLength = (int)strlen(lMaterial->GetName());
			string nameBuffer = lMaterial->GetName();
			for (int j = 0; j < nameLength; j++)
			{
				materials[matIndex].name[j] = nameBuffer[j];
			}
			materials[matIndex].name[nameLength] = '\0';

			if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
			{
				// Print the Ambient Color
				lKFbxDouble3 = ((FbxSurfacePhong*)lMaterial)->Ambient;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].ambient[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].ambient[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].ambient[2] = (float)lKFbxDouble3.Get()[2];

				// Print the Diffuse Color
				lKFbxDouble3 = ((FbxSurfacePhong*)lMaterial)->Diffuse;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].diffuse[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].diffuse[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].diffuse[2] = (float)lKFbxDouble3.Get()[2];

				// Print the Specular Color (unique to Phong materials)
				lKFbxDouble3 = ((FbxSurfacePhong*)lMaterial)->Specular;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].specular[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].specular[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].specular[2] = (float)lKFbxDouble3.Get()[2];

				// Print the Emissive Color
				lKFbxDouble3 = ((FbxSurfacePhong*)lMaterial)->Emissive;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].emissive[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].emissive[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].emissive[2] = (float)lKFbxDouble3.Get()[2];

				//Opacity is Transparency factor now
				lKFbxDouble1 = ((FbxSurfacePhong*)lMaterial)->TransparencyFactor;
				materials[matIndex].opacity = (float)lKFbxDouble1.Get();

			}
			
			if (lMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId))
			{
				// Print the Ambient Color
				lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Ambient;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].ambient[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].ambient[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].ambient[2] = (float)lKFbxDouble3.Get()[2];

				// Print the Diffuse Color
				lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Diffuse;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].diffuse[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].diffuse[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].diffuse[2] = (float)lKFbxDouble3.Get()[2];

				// Print the Emissive Color
				lKFbxDouble3 = ((FbxSurfaceLambert*)lMaterial)->Emissive;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].emissive[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].emissive[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].emissive[2] = (float)lKFbxDouble3.Get()[2];

				//Opacity is Transparency factor now
				lKFbxDouble1 = ((FbxSurfaceLambert*)lMaterial)->TransparencyFactor;
				materials[matIndex].opacity = (float)lKFbxDouble1.Get();

			}

			// -- GETTING THE NUMBER OF TEXTURES FOR THE MATERIAL --
			int lTextureCount = 0;
			int nrOfTextures = 0;
			//int run = 0;
			int lTextureIndex = 0;
			bool hasAlbedo = false;
			bool hasNormal = false;
			FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
			{
				FbxProperty lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);
				lTextureCount = lProperty.GetSrcObjectCount<FbxTexture>();
				FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>(0);

				// Diffuse 
				if (lTexture && lTextureIndex == 0)
				{
					//================================================
					//The following code gets the file name and edits
					//it so that only the file name gets fetched
					//================================================
					hasAlbedo = true;

					string filePath = lTexture->GetFileName();
					size_t pivotPos = 0;
					for (int index = 0; index < strlen(filePath.c_str()); index++)
					{
						char temp = filePath[index];
						if (temp == '.')
							pivotPos = index;
					}
					string fileExtension = filePath.substr(pivotPos + 1, strlen(filePath.c_str()));

					for (int index = 0; index < strlen(outputPath.c_str()); index++)
					{
						char temp = outputPath[index];
						if (temp == '/')
							pivotPos = index;
					}
					string albedoExtension = "_Albedo." + fileExtension;
					string fileName = outputPath.substr(pivotPos+1, strlen(outputPath.c_str())) + albedoExtension;
					string texOutputPath = outputPath + albedoExtension;
					
					std::ifstream in(filePath, ios_base::in | ios_base::binary);
					std::ofstream out(texOutputPath, ios_base::out | ios_base::binary);
					char buffer[4096];
					if (in)
					{
						do	
						{
							in.read(&buffer[0], 4096);
							out.write(&buffer[0], in.gcount());
						} while (in.gcount() > 0);
					}
					else
					{
						std::cout << "Error reading and writing to file" << std::endl;
					}
					in.close();
					out.close();

					//Send string to struct
					for (int j = 0; j < strlen(texOutputPath.c_str()); j++)
					{
						materials[matIndex].albedo[j] = fileName[j];
						materials[matIndex].albedo[strlen(fileName.c_str())] = '\0';
					}
				} 


				// Normal map
				if (lTexture && lTextureIndex == 9)
				{
					//================================================
					//The following code gets the file name and edits
					//it so that only the file name gets fetched
					//================================================
					hasNormal = true;

					string filePath = lTexture->GetFileName();
					size_t pivotPos = 0;
					for (int index = 0; index < strlen(filePath.c_str()); index++)
					{
						char temp = filePath[index];
						if (temp == '.')
							pivotPos = index;
					}
					string fileExtension = filePath.substr(pivotPos + 1, strlen(filePath.c_str()));

					for (int index = 0; index < strlen(outputPath.c_str()); index++)
					{
						char temp = outputPath[index];
						if (temp == '/')
							pivotPos = index;
					}
					string normalExtension = "_Normal." + fileExtension;
					string fileName = outputPath.substr(pivotPos + 1, strlen(outputPath.c_str())) + normalExtension;
					string texOutputPath = outputPath + normalExtension;
						
					std::ifstream in(filePath, ios_base::in | ios_base::binary);
					std::ofstream out(texOutputPath, ios_base::out | ios_base::binary);
					char buffer[4096];
					if (in)
					{
						do
						{
							in.read(&buffer[0], 4096);
							out.write(&buffer[0], in.gcount());
						} while (in.gcount() > 0);
					}
					else
					{
						std::cout << "Error reading and writing Normal map" << std::endl;
					}
					in.close();
					out.close();

					for (int j = 0; j < strlen(texOutputPath.c_str()); j++)
					{
						materials[matIndex].normal[j] = fileName[j];
						materials[matIndex].normal[strlen(fileName.c_str())] = '\0';
					}

				}
			}
			

		}
	}

	int nameAt = -1;
	// Checks if the material already exists in the vector
	bool nameExists = false;
	for (int i = 0; i < materialCount; i++)
	{
		for (int j = 0; j < mats.size(); j++)
		{
			if ((string)mats[j].name == (string)materials[i].name)
			{
				nameExists = true;
				nameAt = j;
			}
		}

		if (!nameExists)
		{
			mats.push_back(materials[i]);
			mesh->materialID = (int)mats.size() - 1;

			for (int j = 0; j < strlen(materials[i].name); j++)
			{
				mesh->materialName[j] = materials[i].name[j];
				mesh->materialName[strlen(materials[i].name)] = '\0';
			}
		}
		else
		{
			//this num is going to be wrong as it already exists in file
			//We need to know what position the name is at
			mesh->materialID = nameAt;
			for (int j = 0; j < strlen(materials[i].name); j++)
			{
				mesh->materialName[j] = materials[i].name[j];
				mesh->materialName[strlen(materials[i].name)] = '\0';
			}
			nameExists = false;
		}
	}

	// Special case for the first material
	// Might not be needed
	if (materialCount && mats.size() == 0)
	{
		mats.push_back(materials[0]);
		mesh->materialID = 0;
		for (int j = 0; j < strlen(materials[0].name); j++)
		{
			mesh->materialName[j] = materials[0].name[j];
			mesh->materialName[strlen(materials[0].name)] = '\0';
		}
	}
		

	if (materials)
	{
		delete[] materials;
	}
}

