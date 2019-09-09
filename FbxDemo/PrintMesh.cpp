#include "PrintMesh.h"


void GetMesh(FbxNode* fbxNode, MeshHolder* mesh, vector<PhongMaterial>& materials)
{
	FbxMesh* fbxMesh = (FbxMesh*)fbxNode->GetNodeAttribute();
	FbxGeometry* fbxGeo = (FbxGeometry*)fbxMesh;

	// Applies the mesh name
	int nameLength = (int)strlen(fbxNode->GetName());
	string nameBuffer = fbxNode->GetName();
	for (int j = 0; j < nameLength; j++)
		mesh->name[j] = nameBuffer[j];
	mesh->name[nameLength] = '\0';
	// Puts a \0 at the end of the mesh name, still printing out whitespace into the binary file

	GetPolygons(fbxMesh, mesh);
	PrintMaterial(fbxMesh, materials, mesh);	// MM: Textures are called to be printed from PrintMaterial
	DisplayUserProperties(fbxNode, mesh);
	if (fbxGeo->GetDeformerCount(FbxDeformer::eSkin) > 0)
	{
		FbxNode* rootNode = fbxNode->GetScene()->GetRootNode();
		for (int index = 0; index < rootNode->GetChildCount(); index++)
			GetSkeleton(rootNode->GetChild(index), 0, -1, mesh);
		GetSkin(fbxMesh, fbxGeo, mesh);
	}
}

void GetPolygons(FbxMesh* fbxMesh, MeshHolder* mesh)
{
	int lPolygonCount = fbxMesh->GetPolygonCount();
	FbxVector4* lControlPoints = fbxMesh->GetControlPoints();
	char header[100];

	// --- MM: Allocating memory for the vertices array based on the vertex count ---
	int vtxCount = fbxMesh->GetPolygonVertexCount();
	Vertex *vertices = new Vertex[vtxCount];
	mesh->vertices = new Vertex[vtxCount];


	// MM: Builds vertices for each polygon, and adds to the vertices array we allocated memory for
	int vertexId = 0;
	for (int p = 0; p < lPolygonCount; p++)
	{
		int lPolygonSize = fbxMesh->GetPolygonSize(p);

		for (int v = 0; v < lPolygonSize; v++)
		{
			int lControlPointIndex = fbxMesh->GetPolygonVertex(p, v);
			if (lControlPointIndex < 0)
			{
				//DisplayString("            Coordinates: Invalid index found!");
				continue;
			}
			else
			{
				// MM: Prints and adds -vertex positions- to the pString and to the vertices array
				vertices[vertexId].position[0] = (float)lControlPoints[lControlPointIndex][0];
				vertices[vertexId].position[1] = (float)lControlPoints[lControlPointIndex][1];
				vertices[vertexId].position[2] = (float)lControlPoints[lControlPointIndex][2];
			}

			for (int l = 0; l < fbxMesh->GetElementUVCount(); ++l)
			{
				FbxGeometryElementUV* leUV = fbxMesh->GetElementUV(l);
				FBXSDK_sprintf(header, 100, "vt ");
				// MM: Prints and adds -UV positions- to the pString and to the vertices array
				switch (leUV->GetMappingMode())
				{
				default:
					break;
				case FbxGeometryElement::eByControlPoint:
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						vertices[vertexId].uv[0] = (float)leUV->GetDirectArray().GetAt(lControlPointIndex)[0];
						vertices[vertexId].uv[1] = (float)leUV->GetDirectArray().GetAt(lControlPointIndex)[1];
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						vertices[vertexId].uv[0] = (float)leUV->GetDirectArray().GetAt(id)[0];
						vertices[vertexId].uv[1] = (float)leUV->GetDirectArray().GetAt(id)[1];
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
					break;

				case FbxGeometryElement::eByPolygonVertex:
				{
					int lTextureUVIndex = fbxMesh->GetTextureUVIndex(p, v);
					switch (leUV->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
					case FbxGeometryElement::eIndexToDirect:
					{
						vertices[vertexId].uv[0] = (float)leUV->GetDirectArray().GetAt(lTextureUVIndex)[0];
						vertices[vertexId].uv[1] = (float)leUV->GetDirectArray().GetAt(lTextureUVIndex)[1];
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
				break;

				case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
				case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
				case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
					break;
				}
			}


			for (int l = 0; l < fbxMesh->GetElementNormalCount(); ++l)
			{
				FbxGeometryElementNormal* leNormal = fbxMesh->GetElementNormal(l);

				FBXSDK_sprintf(header, 100, "vn ");
				// MM: Prints and adds -normal positions- to the pString and to the vertices array
				if (leNormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leNormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						vertices[vertexId].normal[0] = (float)leNormal->GetDirectArray().GetAt(vertexId)[0];
						vertices[vertexId].normal[1] = (float)leNormal->GetDirectArray().GetAt(vertexId)[1];
						vertices[vertexId].normal[2] = (float)leNormal->GetDirectArray().GetAt(vertexId)[2];
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leNormal->GetIndexArray().GetAt(vertexId);
						vertices[vertexId].normal[0] = (float)leNormal->GetDirectArray().GetAt(id)[0];
						vertices[vertexId].normal[1] = (float)leNormal->GetDirectArray().GetAt(id)[1];
						vertices[vertexId].normal[2] = (float)leNormal->GetDirectArray().GetAt(id)[2];
					}
					break;
					default:
						break; // other reference modes not shown here!
					}

				}
				// MM: This one is used for spheres and planes, as they previously did not output normals
				if (leNormal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
				{
					if (leNormal->GetReferenceMode() == FbxGeometryElement::eDirect)
					{
						vertices[vertexId].normal[0] = (float)leNormal->GetDirectArray().GetAt(lControlPointIndex)[0];
						vertices[vertexId].normal[1] = (float)leNormal->GetDirectArray().GetAt(lControlPointIndex)[1];
						vertices[vertexId].normal[2] = (float)leNormal->GetDirectArray().GetAt(lControlPointIndex)[2];
					}
				}

			}
			for (int l = 0; l < fbxMesh->GetElementTangentCount(); ++l)
			{
				FbxGeometryElementTangent* leTangent = fbxMesh->GetElementTangent(l);
				FBXSDK_sprintf(header, 100, "vtan ");
				// MM: Prints and adds -tangents- to the pString and to the vertices array
				if (leTangent->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leTangent->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						vertices[vertexId].tangent[0] = (float)leTangent->GetDirectArray().GetAt(vertexId)[0];
						vertices[vertexId].tangent[1] = (float)leTangent->GetDirectArray().GetAt(vertexId)[1];
						vertices[vertexId].tangent[2] = (float)leTangent->GetDirectArray().GetAt(vertexId)[2];
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leTangent->GetIndexArray().GetAt(vertexId);
						vertices[vertexId].tangent[0] = (float)leTangent->GetDirectArray().GetAt(id)[0];
						vertices[vertexId].tangent[1] = (float)leTangent->GetDirectArray().GetAt(id)[1];
						vertices[vertexId].tangent[2] = (float)leTangent->GetDirectArray().GetAt(id)[2];
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}

			}
			for (int l = 0; l < fbxMesh->GetElementBinormalCount(); ++l)
			{

				FbxGeometryElementBinormal* leBinormal = fbxMesh->GetElementBinormal(l);
				FBXSDK_sprintf(header, 100, "vbin ");
				// MM: Prints and adds -binormals- to the pString and to the vertices array
				if (leBinormal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
				{
					switch (leBinormal->GetReferenceMode())
					{
					case FbxGeometryElement::eDirect:
						vertices[vertexId].bitangent[0] = (float)leBinormal->GetDirectArray().GetAt(vertexId)[0];
						vertices[vertexId].bitangent[1] = (float)leBinormal->GetDirectArray().GetAt(vertexId)[1];
						vertices[vertexId].bitangent[2] = (float)leBinormal->GetDirectArray().GetAt(vertexId)[2];
						break;
					case FbxGeometryElement::eIndexToDirect:
					{
						int id = leBinormal->GetIndexArray().GetAt(vertexId);
						vertices[vertexId].bitangent[0] = (float)leBinormal->GetDirectArray().GetAt(id)[0];
						vertices[vertexId].bitangent[1] = (float)leBinormal->GetDirectArray().GetAt(id)[1];
						vertices[vertexId].bitangent[2] = (float)leBinormal->GetDirectArray().GetAt(id)[2];
					}
					break;
					default:
						break; // other reference modes not shown here!
					}
				}
			}
			vertexId++;
		} // for polygonSize
	} // for polygonCount

	// Copies the pointerdata created here into the pointer in the recived mesh struct
	// Mesh should be arriving from a vector
	// memcpy - the vertices struct containts 14 floats, additionally there are vtxCount amount of vertices
	// This is the amount of memory that will be copied into the mesh pointer
	mesh->vertexCount = vtxCount;
	memcpy(mesh->vertices, vertices, sizeof(Vertex) * vtxCount);


	// MM: Delete the allocated memory for vertices
	if (vertices)
		delete[] vertices;
}

void GetSkin(FbxMesh* fbxMesh, FbxGeometry* fbxGeo, MeshHolder* mesh)
{
	// Temporary skin data
	typedef struct SkinData
	{
		float	boneIndex[4] { 0.0f };
		float	boneWeight[4] { 0.0f };
		int		minWeightIndex = 0;
		float	minWeight = 0.0f;
	} SkinData;

	FbxVector4* controlPoints = fbxMesh->GetControlPoints();
	// temporary vector for weights and indices, of the exact size;
	vector<SkinData> controlPointSkinData(fbxMesh->GetControlPointsCount());
	FbxSkin* skin = (FbxSkin*)fbxGeo->GetDeformer(0, FbxDeformer::eSkin);

	// Collects vertex weights
	for (int boneIndex = 0; boneIndex < skin->GetClusterCount(); boneIndex++)
	{
		FbxCluster* cluster = skin->GetCluster(boneIndex);					// One cluster is a collection of weights for a bone
		int* indices = cluster->GetControlPointIndices();					// Control point indices for bone at boneIndex
		double* weights = cluster->GetControlPointWeights();				// matching weights for each vertex
		for (int x = 0; x < cluster->GetControlPointIndicesCount(); x++)
		{
			// Weights for one control point (vertex)
			SkinData& ctrlPoint = controlPointSkinData[indices[x]];
			// this block of code checks if the new weight is higher than the smallest existing weight
			// If it is, it means we have to drop/replace and find the new minimum for the next control point.
			if (weights[x] > ctrlPoint.minWeight)
			{
				ctrlPoint.boneWeight[ctrlPoint.minWeightIndex] = (float)weights[x];
				ctrlPoint.boneIndex[ctrlPoint.minWeightIndex] = (float)boneIndex;

				// Find new minimum
				int minIndex = 0;
				float minWeight = ctrlPoint.boneWeight[minIndex];
				for (int j = 1; j < 4; j++)
				{
					if (ctrlPoint.boneWeight[j] < minWeight)
					{
						minIndex = j;
						minWeight = ctrlPoint.boneWeight[j];

					}
				}
				ctrlPoint.minWeightIndex = minIndex;
				ctrlPoint.minWeight = minWeight;
			}
		}
	}

	int vertexId = 0;
	for (int p = 0; p < fbxMesh->GetPolygonCount(); p++)
	{
		for (int v = 0; v < 3; v++)
		{
			int ctrlPointIndex = fbxMesh->GetPolygonVertex(p, v);
			SkinData& skinData = controlPointSkinData[ctrlPointIndex];
			float* bones = skinData.boneIndex;
			float* weights = skinData.boneWeight;
			Vertex& vertexRef = mesh->vertices[vertexId];
			float sumWeights = weights[0] + weights[1] + weights[2] + weights[3];
			vertexRef.bone[0] = bones[0];
			vertexRef.bone[1] = bones[1];
			vertexRef.bone[2] = bones[2];
			vertexRef.bone[3] = bones[3];

			vertexRef.weight[0] = weights[0] / sumWeights;
			vertexRef.weight[1] = weights[1] / sumWeights;
			vertexRef.weight[2] = weights[2] / sumWeights;
			vertexRef.weight[3] = weights[3] / sumWeights;
			vertexId++;
		}
	}

	// Initialize for animation data
	// Might brake if no animation but has a skeleton, need to test
	FbxAnimStack* currStack = fbxMesh->GetScene()->GetSrcObject<FbxAnimStack>(mesh->skeleton.animations.size());
	char animationName[NAME_SIZE];
	for (int c = 0; c < NAME_SIZE; c++)
		animationName[c] = currStack->GetName()[c];
	animationName[NAME_SIZE - 1] = '\0';
	FbxTakeInfo* takeInfo = fbxMesh->GetScene()->GetTakeInfo(animationName);
	FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
	FbxTime stop = takeInfo->mLocalTimeSpan.GetStop();
	int startFrame = (int)start.GetFrameCount(FbxTime::eFrames24);
	int endFrame = (int)stop.GetFrameCount(FbxTime::eFrames24);
	int keyframeCount = endFrame - startFrame + 1;

	// References to the in-mesh for more readable code
	SkeletonHolder& skeleton = mesh->skeleton;
	skeleton.name[0] = '\0';
	skeleton.animations.push_back(AnimationHolder{});
	AnimationHolder& animation = skeleton.animations[skeleton.animations.size() - 1];

	for (int c = 0; c < NAME_SIZE; c++)
		animation.name[c] = animationName[c];
	animation.name[NAME_SIZE - 1] = '\0';
	animation.keyframeFirst = startFrame;
	animation.keyframeLast = endFrame;
	animation.duration = (float)takeInfo->mLocalTimeSpan.GetDuration().GetSecondDouble();
	animation.rate = (float)takeInfo->mLocalTimeSpan.GetDuration().GetFrameRate(FbxTime::EMode::eFrames24);

	// Allocate memory
	animation.keyframes.resize(keyframeCount);
	for (int boneIndex = 0; boneIndex < skin->GetClusterCount(); boneIndex++)
	{
		FbxCluster* cluster = skin->GetCluster(boneIndex);
		char linkName[256];
		for (int c = 0; c < NAME_SIZE; c++)
			linkName[c] = cluster->GetLink()->GetName()[c];
		linkName[NAME_SIZE - 1] = '\0';

		int jointIndex = 0;
		for (jointIndex; jointIndex < skeleton.joints.size(); jointIndex++)
			if ((string)linkName == (string)skeleton.joints[jointIndex].name)
				break;
		if (jointIndex == skeleton.joints.size())
			cout << "ERROR!, Cluster Link " << linkName << " not found in Skeleton" << endl;

		// Geometry Transform
		// this could account for an offset of the geometry from the bone?, usually identity.
		FbxNode* fbxNode = fbxMesh->GetNode();

		FbxAMatrix geometryTransform(
			fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricRotation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricScaling(FbxNode::eSourcePivot));

		FbxAMatrix meshGlobalTransform;
		cluster->GetTransformMatrix(meshGlobalTransform);

		FbxAMatrix globalBindPoseTransform;
		cluster->GetTransformLinkMatrix(globalBindPoseTransform);

		FbxAMatrix associateModelTransform;
		cluster->GetTransformAssociateModelMatrix(associateModelTransform);

		FbxAMatrix parentTransform;
		cluster->GetTransformParentMatrix(associateModelTransform);

		FbxAMatrix invGlobalBindPose = globalBindPoseTransform.Inverse() * meshGlobalTransform * geometryTransform * associateModelTransform * parentTransform;
		//FbxAMatrix invGlobalBindPose = globalBindPoseTransform.Inverse() * meshGlobalTransform * geometryTransform * associateModelTransform;
		skeleton.joints[jointIndex].invBindPose = invGlobalBindPose;

		for (int t = startFrame - 1; t <= (int)endFrame - 1; t++)
		{
			AnimationHolder::KeyFrameHolder& keyframe = animation.keyframes[t];

			FbxTime curr;
			curr.SetFrame(t, FbxTime::eFrames24);

			FbxAMatrix localJoint = cluster->GetLink()->EvaluateLocalTransform(curr);
			keyframe.localJointsR.push_back(localJoint.GetQ());
			keyframe.localJointsT.push_back(localJoint.GetT());
			keyframe.localJointsS.push_back(localJoint.GetS());
		}
	}

	int bonesAmnt = (int)skeleton.joints.size();
	for (int boneIndex = skin->GetClusterCount(); boneIndex < bonesAmnt; boneIndex++)
	{
		for (int t = startFrame - 1; t <= (int)endFrame - 1; t++)
		{
			AnimationHolder::KeyFrameHolder& keyframe = animation.keyframes[t];

			FbxVector4 fillv;
			FbxQuaternion fillq;

			keyframe.localJointsR.push_back(fillq);
			keyframe.localJointsT.push_back(fillv);
			keyframe.localJointsS.push_back(fillv);
		}
	}

}

// Recursive function going through all the children
void GetSkeleton(FbxNode* fbxNode, int nodeIndex, int parent, MeshHolder* meshToPopulate)
{
	FbxNodeAttribute* skeleton = fbxNode->GetNodeAttributeByIndex(0);
	if (skeleton->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		SkeletonHolder::JointHolder newJoint;
		for (int c = 0; c < NAME_SIZE; c++)
			newJoint.name[c] = fbxNode->GetName()[c];
		newJoint.name[NAME_SIZE - 1] = '\0';
		newJoint.parentIndex = parent;
		newJoint.invBindPose;					// Only here for code readability, FbxAMatrix is default identity
		meshToPopulate->skeleton.joints.push_back(newJoint);
	}
	for (int index = 0; index < fbxNode->GetChildCount(); index++)
		GetSkeleton(fbxNode->GetChild(index), (int)meshToPopulate->skeleton.joints.size(), nodeIndex, meshToPopulate);
}




/*
========================================================================================================================
This function loops through an FbxObjects properties. Which is the custom attributes added in Maya.

First setting "Type" and "Link" in the Meshholder to 0;
Loops through each property.
If the property is an int, (eFbxInt)
then it checks if the name of the property is "Type" or "Link"
Then retrives the information and stores it.
========================================================================================================================
*/
void DisplayUserProperties(FbxObject* pObject, MeshHolder* mesh)
{
	int lCount = 0;
	FbxString lTitleStr = "    Property Count: ";

	FbxProperty lProperty = pObject->GetFirstProperty();
	while (lProperty.IsValid())
	{
		if (lProperty.GetFlag(FbxPropertyFlags::eUserDefined))
			lCount++;

		lProperty = pObject->GetNextProperty(lProperty);
	}

	if (lCount == 0)
		return; // there are no user properties to display

	DisplayInt(lTitleStr.Buffer(), lCount);

	mesh->type = 0;
	mesh->link = 0;
	mesh->dir = 0;
	mesh->dist = 0.0f;
	mesh->collect = 0;

	lProperty = pObject->GetFirstProperty();
	int i = 0;
	while (lProperty.IsValid())
	{
		if (lProperty.GetFlag(FbxPropertyFlags::eUserDefined))
		{
			DisplayInt("        Property ", i);
			FbxString lString = lProperty.GetLabel();
			DisplayString("            Display Name: ", lString.Buffer());
			lString = lProperty.GetName();
			DisplayString("            Internal Name: ", lString.Buffer());
			DisplayString("            Type: ", lProperty.GetPropertyDataType().GetName());
			if (lProperty.HasMinLimit()) DisplayDouble("            Min Limit: ", lProperty.GetMinLimit());
			if (lProperty.HasMaxLimit()) DisplayDouble("            Max Limit: ", lProperty.GetMaxLimit());
			DisplayBool("            Is Animatable: ", lProperty.GetFlag(FbxPropertyFlags::eAnimatable));

			FbxDataType lPropertyDataType = lProperty.GetPropertyDataType();

			// INTEGER
			if (lPropertyDataType.GetType() == eFbxInt)
			{
				DisplayInt("            Default Value: ", lProperty.Get<FbxInt>());

				if (lProperty.GetName() == "Type")
				{
					DisplayString("            Found: Type ");
					mesh->type = lProperty.Get<FbxInt>();
				}

				if (lProperty.GetName() == "Link")
				{
					DisplayString("            Found: Link ");
					mesh->link = lProperty.Get<FbxInt>();
				}

				if (lProperty.GetName() == "Dir")
				{
					DisplayString("            Found: Dir ");
					mesh->dir = lProperty.Get<FbxInt>();
				}

				if (lProperty.GetName() == "Collect")
				{
					DisplayString("            Found: Collect ");
					mesh->collect = lProperty.Get<FbxInt>();
				}
			}

			if (lPropertyDataType.GetType() == eFbxDouble)
			{
				DisplayDouble("            Default Value: ", lProperty.Get<FbxFloat>());
				if (lProperty.GetName() == "Dist")
				{
					DisplayString("            Found: Dist ");
					mesh->dist = lProperty.Get<FbxFloat>();
				}
			}
			// UNIDENTIFIED
			else
			{
				DisplayString("            Default Value: UNIDENTIFIED");
			}
			i++;
		}

		lProperty = pObject->GetNextProperty(lProperty);
	}
}

