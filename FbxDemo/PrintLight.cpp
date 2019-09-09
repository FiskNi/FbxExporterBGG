#include "PrintLight.h"

/*
========================================================================================================================

	PrintLight prints the light information per light, including Light name, light type, if it casts light, colour, intensity, outer angle and fog values.

	// Martina Molid

========================================================================================================================
*/

int PrintLight(FbxNode* pNode, DirLight* dirlight, PointLight* pointLight)
{
	FbxLight* lLight = (FbxLight*)pNode->GetNodeAttribute();

	DisplayString("Light Name: ", (char *)pNode->GetName());

	string tempName = (char *)pNode->GetName();
	int nameLength = (int)strlen(tempName.c_str());
	char lightName[NAME_SIZE];
	for (int i = 0; i < nameLength; i++) {
		lightName[i] = tempName[i];
	}
	lightName[nameLength] = '\0'; // Puts a \0 at the end of the name, still printing out whitespace into the binary file
	//std::cout << "Light name: " << lightName << endl;

	DisplayMetaDataConnections(lLight);

	const char* lLightTypes[] = { "Point", "Directional" };

	FbxDouble3 c = lLight->Color.Get();
	FbxColor lColor(c[0], c[1], c[2]);
	DisplayColor("        Default Color: ", lColor);

	int type = 0;

	if (lLightTypes[lLight->LightType.Get()] == "Directional")
	{
		for (int i = 0; i < 3; i++)
			dirlight->color[i] = (float)c[i];

		dirlight->intensity = (float)(lLight->Intensity.Get()/ 100);
		type = 1;
	}

	if (lLightTypes[lLight->LightType.Get()] == "Point")
	{
		for (int i = 0; i < 3; i++)
			pointLight->color[i] = (float)c[i];

		pointLight->intensity = (float)(lLight->Intensity.Get() / 100);
		type = 2;
		//build
	}

	DisplayString("    Type: ", lLightTypes[lLight->LightType.Get()]);
	tempName = lLightTypes[lLight->LightType.Get()];
	nameLength = (int)strlen(tempName.c_str());
	char lightType[NAME_SIZE];
	for (int i = 0; i < nameLength; i++) {
		lightType[i] = tempName[i];
	}
	lightType[nameLength] = '\0'; // Puts a \0 at the end of the name, still printing out whitespace into the binary file
	//std::cout << "Light type: " << lightType << endl;

	DisplayBool("    Cast Light: ", lLight->CastLight.Get());
	//lightInfo.castLight = lLight->CastLight.Get();

	DisplayString("    Default Animation Values");
	//lightInfo.colour[0] = (float)c[0];
	//lightInfo.colour[1] = (float)c[1];
	//lightInfo.colour[2] = (float)c[2];

	DisplayDouble("        Default Intensity: ", lLight->Intensity.Get());
	//lightInfo.intensity = (float)lLight->Intensity.Get();
	DisplayDouble("        Default Outer Angle: ", lLight->OuterAngle.Get());
	//lightInfo.outAngle = (float)lLight->OuterAngle.Get();
	DisplayDouble("        Default Fog: ", lLight->Fog.Get());
	//lightInfo.fog = (float)lLight->Fog.Get();
	DisplayDouble("        Decay Type: ", lLight->DecayType.Get());

	DisplayDouble("        Near Attenuation Start: ", lLight->NearAttenuationStart.Get());
	DisplayDouble("        Near Attenuation End: ", lLight->NearAttenuationEnd.Get());
	DisplayDouble("        Far Attenuation Start: ", lLight->FarAttenuationStart.Get());
	DisplayDouble("        Far Attenuation End: ", lLight->FarAttenuationEnd.Get());

	return type;
}