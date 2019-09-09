#pragma once

#include "PrintInfo.h"

string FindAndDisplayTextureInfoByProperty(FbxProperty pProperty, bool& pDisplayHeader, int pMaterialIndex);
string PrintTexture(FbxSurfaceMaterial *lMaterial, int lMaterialIndex);
string PrintTextureInfo(FbxTexture* pTexture, int pBlendMode);