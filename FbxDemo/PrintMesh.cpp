#include "PrintMesh.h"


void GetMesh(FbxNode* fbxNode, MeshHolder* mesh, vector<PhongMaterial>& materials, std::string outputPath)
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
	PrintMaterial(fbxMesh, materials, mesh, outputPath);	// Collect material
	//DisplayUserProperties(fbxNode, mesh);		// Collect custom attributes
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
	FbxVector4* lControlPoints = fbxMesh->GetControlPoints();

	// Get vertex positions
	int vtxCount = fbxMesh->GetPolygonVertexCount();
	Vertex *vertices = new Vertex[vtxCount];
	mesh->vertices = new Vertex[vtxCount];

	// Get faces
	int faceCount = fbxMesh->GetPolygonCount();
	mesh->faces = new Face[faceCount];
	Face* indexBuffer = new Face[faceCount];


	vector<Vertex> tempVertices;
	int faceIndex = 0;
	int bufferIndex = 0;
	for (int f = 0; f < faceCount; f++)
	{
		// Face has 3 vertices
		for (int polyVertIndex = 0; polyVertIndex < 3; polyVertIndex++)
		{

			int vertexIndex = (int)fbxMesh->GetPolygonVertex(f, polyVertIndex);

			FbxVector2 uv2 = GetUV(fbxMesh->GetElementUV(), faceIndex, vertexIndex);
			FbxVector4 normal4 = GetNormal(fbxMesh->GetElementNormal(), faceIndex, vertexIndex);
			Vertex tempVertex;
			
			tempVertex.position[0] = (float)lControlPoints[vertexIndex][0];
			tempVertex.position[1] = (float)lControlPoints[vertexIndex][1];
			tempVertex.position[2] = (float)lControlPoints[vertexIndex][2];

			tempVertex.uv[0] = (float)uv2[0];
			tempVertex.uv[1] = (float)uv2[1];

			tempVertex.normal[0] = (float)normal4[0];
			tempVertex.normal[1] = (float)normal4[1];
			tempVertex.normal[2] = (float)normal4[2];

			vertices[faceIndex] = tempVertex;
			
			bool pushBack = true;
			for (int c = faceIndex - 1; c >= 0; c--)
				if (tempVertex.position[0] == vertices[c].position[0])
					if (tempVertex.position[1] == vertices[c].position[1])
						if (tempVertex.position[2] == vertices[c].position[2])
							if (tempVertex.uv[0] == vertices[c].uv[0])
								if (tempVertex.uv[1] == vertices[c].uv[1])
									if (tempVertex.normal[0] == vertices[c].normal[0])
										if (tempVertex.normal[1] == vertices[c].normal[1])
											if (tempVertex.normal[2] == vertices[c].normal[2])
											{
												int faceAt = floor(c / 3);
												int vertexAt = c % 3 ;

												indexBuffer[f].indices[polyVertIndex] = indexBuffer[faceAt].indices[vertexAt];


												pushBack = false;
											}
			if (pushBack)
			{
				indexBuffer[f].indices[polyVertIndex] = bufferIndex;
				tempVertices.push_back(tempVertex);
				bufferIndex++;
			}


			faceIndex++;
		}
	}

	
	Vertex* newVertices = new Vertex[bufferIndex];
	for (int v = 0; v < tempVertices.size(); v++)
	{
		newVertices[v] = tempVertices[v];
	}




	// Copy vertex and face data
	mesh->vertexCount = bufferIndex;
	mesh->faceCount = faceCount;
	memcpy(mesh->vertices, newVertices, sizeof(Vertex) * bufferIndex);
	memcpy(mesh->faces, indexBuffer, sizeof(Face) * faceCount);


	// MDelete the allocated memory for vertices
	if (vertices)
		delete[] vertices;
	if (indexBuffer)
		delete[] indexBuffer;
	if (newVertices)
		delete[] newVertices;
}

void GetSkin(FbxMesh* fbxMesh, FbxGeometry* fbxGeo, MeshHolder* mesh)
{
	FbxVector4* controlPoints = fbxMesh->GetControlPoints();
	// temporary vector for weights and indices, of the exact size;
	vector<SkinData> controlPointSkinData(fbxMesh->GetControlPointsCount());
	FbxSkin* skin = (FbxSkin*)fbxGeo->GetDeformer(0, FbxDeformer::eSkin);

	MinMaxWeights(skin, controlPointSkinData);
	GetVertexWeights(fbxMesh, controlPointSkinData, mesh);
	GetBindPose(skin, mesh, fbxMesh);
	GetAnimation(fbxMesh, mesh, skin);
	

}

void GetAnimation(fbxsdk::FbxMesh* fbxMesh, MeshHolder* mesh, fbxsdk::FbxSkin* skin)
{
	// Initialize for animation data
	// Might brake if no animation but has a skeleton, need to test

	FbxAnimStack* currStack = fbxMesh->GetScene()->GetSrcObject<FbxAnimStack>(0);
	if (currStack)
	{
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
				cout << "ERROR!, Cluster Link " << linkName << " not found in Skeleton" << endl;;

			for (int t = startFrame; t <= (int)endFrame - 1; t++)
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
			for (int t = startFrame; t <= (int)endFrame - 1; t++)
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
}

void MinMaxWeights(fbxsdk::FbxSkin* skin, std::vector<SkinData>& controlPointSkinData)
{
	// Collects vertex weights
	for (int boneIndex = 0; boneIndex < skin->GetClusterCount(); boneIndex++)
	{
		FbxCluster* cluster = skin->GetCluster(boneIndex);					// One cluster is a collection of weights for a bone
		int* index = cluster->GetControlPointIndices();					// Control point indices for bone at boneIndex
		double* weights = cluster->GetControlPointWeights();				// matching weights for each vertex
		for (int x = 0; x < cluster->GetControlPointIndicesCount(); x++)
		{
			// Weights for one control point (vertex)
			SkinData& ctrlPoint = controlPointSkinData[index[x]];
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
}

void GetVertexWeights(fbxsdk::FbxMesh* fbxMesh, std::vector<SkinData>& controlPointSkinData, MeshHolder* mesh)
{
	int faceIndex = 0;
	for (int f = 0; f < fbxMesh->GetPolygonCount(); f++)
	{
		for (int polyVertIndex = 0; polyVertIndex < 3; polyVertIndex++)
		{
			int vertexIndex = fbxMesh->GetPolygonVertex(f, polyVertIndex);

			SkinData& skinData = controlPointSkinData[vertexIndex];
			float* bones = skinData.boneIndex;
			float* weights = skinData.boneWeight;
			Vertex& vertexRef = mesh->vertices[vertexIndex];
			float sumWeights = weights[0] + weights[1] + weights[2] + weights[3];
			vertexRef.bone[0] = bones[0];
			vertexRef.bone[1] = bones[1];
			vertexRef.bone[2] = bones[2];
			vertexRef.bone[3] = bones[3];

			vertexRef.weight[0] = weights[0] / sumWeights;
			vertexRef.weight[1] = weights[1] / sumWeights;
			vertexRef.weight[2] = weights[2] / sumWeights;
			vertexRef.weight[3] = weights[3] / sumWeights;
			faceIndex++;
		}
	}
}

void GetBindPose(fbxsdk::FbxSkin* skin, MeshHolder* mesh, fbxsdk::FbxMesh* fbxMesh)
{

	for (int boneIndex = 0; boneIndex < skin->GetClusterCount(); boneIndex++)
	{
		// References to the in-mesh for more readable code
		SkeletonHolder& skeleton = mesh->skeleton;

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

FbxVector2 GetUV(FbxLayerElementUV* uvElement, int faceIndex, int vertexIndex)
{
	if (uvElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (uvElement->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			return uvElement->GetDirectArray().GetAt(vertexIndex);
		}
		else
		{
			int i = uvElement->GetIndexArray().GetAt(vertexIndex);
			return uvElement->GetDirectArray().GetAt(i);
		}
	}
	else if (uvElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (uvElement->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			return uvElement->GetDirectArray().GetAt(faceIndex);
		}
		else if (uvElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
		{
			int i = uvElement->GetIndexArray().GetAt(faceIndex);
			return uvElement->GetDirectArray().GetAt(i);
		}
	}

	return FbxVector2();
}

FbxVector4 GetNormal(FbxGeometryElementNormal* normalElement, int faceIndex, int vertexIndex)
{
	if (normalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			return normalElement->GetDirectArray().GetAt(vertexIndex);
		}
		else
		{
			int i = normalElement->GetIndexArray().GetAt(vertexIndex);
			return normalElement->GetDirectArray().GetAt(i);
		}
	}
	else if (normalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (normalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			return normalElement->GetDirectArray().GetAt(faceIndex);
		}
		else if (normalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
		{
			int i = normalElement->GetIndexArray().GetAt(faceIndex);
			return normalElement->GetDirectArray().GetAt(i);
		}
	}

	return FbxVector4();
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
					//mesh->type = lProperty.Get<FbxInt>();
				}

			}

			if (lPropertyDataType.GetType() == eFbxDouble)
			{
				DisplayDouble("            Default Value: ", lProperty.Get<FbxFloat>());
				if (lProperty.GetName() == "Dist")
				{
					DisplayString("            Found: Dist ");
					//mesh->dist = lProperty.Get<FbxFloat>();
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

