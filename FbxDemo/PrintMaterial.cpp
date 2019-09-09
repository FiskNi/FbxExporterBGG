#include "PrintMaterial.h"

/*
========================================================================================================================
	PrintMaterial prints out the material information per mesh. 
	For each material connected to the mesh, the function prints material index, material name and the Phong attributes, nr of textures for the material.
	Currently only supports Phong materials, but code for Lambert is left in case it would be desired.
========================================================================================================================
*/

void PrintMaterial(FbxGeometry* pGeometry, vector<PhongMaterial>& mats, MeshHolder* mesh)
{
	int materialCount = 0;
	FbxNode* materialNode = NULL;

	string texturePath = "Resources/Textures/";

	PhongMaterial *materials = nullptr;

	if (pGeometry) 
	{
		materialNode = pGeometry->GetNode();
		if (materialNode) 
		{
			materialCount = materialNode->GetMaterialCount();
			// MM: Allocates memory for the materials array, based on how many materials are to be read
			materials = new PhongMaterial[materialCount];
		}
	}

	if (materialCount > 0)
	{
		FbxPropertyT<FbxDouble3> lKFbxDouble3;
		FbxPropertyT<FbxDouble> lKFbxDouble1;
		FbxColor theColor;

		for (int matIndex = 0; matIndex < materialCount; matIndex++)
		{
			FbxSurfaceMaterial *lMaterial = materialNode->GetMaterial(matIndex);

			// Applies the material name
			int nameLength = (int)strlen(lMaterial->GetName());
			string nameBuffer = lMaterial->GetName();
			for (int j = 0; j < nameLength; j++)
			{
				materials[matIndex].name[j] = nameBuffer[j];
			}
			materials[matIndex].name[nameLength] = '\0';
			// Puts a \0 at the end of the mesh name, still printing out whitespace into the binary file

			if (lMaterial->GetClassId().Is(FbxSurfacePhong::ClassId))
			{
				// We found a Phong material.  Display its properties.

				// Print the Ambient Color
				lKFbxDouble3 = ((FbxSurfacePhong *)lMaterial)->Ambient;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].ambient[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].ambient[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].ambient[2] = (float)lKFbxDouble3.Get()[2];

				// Print the Diffuse Color
				lKFbxDouble3 = ((FbxSurfacePhong *)lMaterial)->Diffuse;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].diffuse[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].diffuse[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].diffuse[2] = (float)lKFbxDouble3.Get()[2];

				// Print the Specular Color (unique to Phong materials)
				lKFbxDouble3 = ((FbxSurfacePhong *)lMaterial)->Specular;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].specular[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].specular[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].specular[2] = (float)lKFbxDouble3.Get()[2];

				// Print the Emissive Color
				lKFbxDouble3 = ((FbxSurfacePhong *)lMaterial)->Emissive;
				theColor.Set(lKFbxDouble3.Get()[0], lKFbxDouble3.Get()[1], lKFbxDouble3.Get()[2]);

				materials[matIndex].emissive[0] = (float)lKFbxDouble3.Get()[0];
				materials[matIndex].emissive[1] = (float)lKFbxDouble3.Get()[1];
				materials[matIndex].emissive[2] = (float)lKFbxDouble3.Get()[2];

				//Opacity is Transparency factor now
				lKFbxDouble1 = ((FbxSurfacePhong *)lMaterial)->TransparencyFactor;
				materials[matIndex].opacity = (float)lKFbxDouble1.Get();

				// -- GETTING THE NUMBER OF TEXTURES FOR THE MATERIAL --
				FbxProperty lProperty;
				//FbxDataType lTexProperty;
				int lTextureCount = 0;
				int nrOfTextures = 0;
				//int run = 0;
				int lTextureIndex = 0;
				bool hasAlbedo = false;
				bool hasNormal = false;
				FBXSDK_FOR_EACH_TEXTURE(lTextureIndex)
				{
					lProperty = lMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[lTextureIndex]);
					//run++;
					
					lTextureCount = lProperty.GetSrcObjectCount<FbxTexture>();
					
					FbxFileTexture* lTexture = lProperty.GetSrcObject<FbxFileTexture>(0);
					if (lTexture && lTextureIndex == 0)
					{
						//================================================
						//The following code gets the file name and edits
						//it so that only the file name gets fetched
						//================================================
						hasAlbedo = true;

						string fileName = lTexture->GetFileName();
						size_t pivotPos = 0;
						for (int index = 0; index < strlen(fileName.c_str()); index++)
						{
							char temp = fileName[index];
							if (temp == '/')
								pivotPos = index;
						}
						string fileNameEd = fileName.substr(pivotPos+1, strlen(fileName.c_str()));

						string finalAlbedo = OUTPUT_PATH + '_' + fileNameEd;
						//finalAlbedo += fileNameEd;
						std::ifstream in(fileName, ios_base::in | ios_base::binary);
						std::ofstream out(finalAlbedo, ios_base::out | ios_base::binary);

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

						fileNameEd = texturePath + fileNameEd;
						//Send string to struct
						for (int j = 0; j < strlen(finalAlbedo.c_str()); j++)
						{
							materials[matIndex].albedo[j] = fileNameEd[j];
							materials[matIndex].albedo[strlen(fileNameEd.c_str())] = '\0';
						}
						
					} 
					if (hasAlbedo == false)
						materials[matIndex].albedo[0] = '\0';

					//lTexture = lProperty.Get;
					if (lTexture && lTextureIndex == 9)
					{
						//================================================
						//The following code gets the file name and edits
						//it so that only the file name gets fetched
						//================================================
						hasNormal = true;
						string fileName = lTexture->GetFileName();
						size_t pivotPos = 0;
						for (int index = 0; index < strlen(fileName.c_str()); index++)
						{
							char temp = fileName[index];
							if (temp == '/')
								pivotPos = index;
						}
						string fileNameEd = fileName.substr(pivotPos + 1, strlen(fileName.c_str()));

						string finalNormal = OUTPUT_PATH + '_' + fileNameEd;
						
						std::ifstream in(fileName, ios_base::in | ios_base::binary);
						std::ofstream out(finalNormal, ios_base::out | ios_base::binary);

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

						fileNameEd = texturePath + fileNameEd;
						//Send string to struct
						for (int j = 0; j < strlen(finalNormal.c_str()); j++)
						{
							materials[matIndex].normal[j] = fileNameEd[j];
							materials[matIndex].normal[strlen(fileNameEd.c_str())] = '\0';
						}

					}
					if(hasNormal == false)
						materials[matIndex].normal[0] = '\0';

				}
				//cout << "Run: " << run << endl;

			}
			else
				cout << "Unknown type of Material" << endl;

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

