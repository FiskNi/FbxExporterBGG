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

	// Temporary 
	FbxGeometry* fbxGeo = (FbxGeometry*)fbxMesh;
	vector<SkinData> controlPointSkinData(fbxMesh->GetControlPointsCount());
	FbxSkin* skin = (FbxSkin*)fbxGeo->GetDeformer(0, FbxDeformer::eSkin);

	if (fbxMesh->GetDeformerCount(FbxDeformer::eSkin) > 0)
	{
		MinMaxWeights(skin, controlPointSkinData);
		GetVertexWeights(fbxMesh, controlPointSkinData, mesh);
	}



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

			tempVertex.bone[0] = controlPointSkinData[vertexIndex].boneIndex[0];
			tempVertex.bone[1] = controlPointSkinData[vertexIndex].boneIndex[1];
			tempVertex.bone[2] = controlPointSkinData[vertexIndex].boneIndex[2];
			tempVertex.bone[3] = controlPointSkinData[vertexIndex].boneIndex[3];

			tempVertex.weight[0] = controlPointSkinData[vertexIndex].boneWeight[0];
			tempVertex.weight[1] = controlPointSkinData[vertexIndex].boneWeight[1];
			tempVertex.weight[2] = controlPointSkinData[vertexIndex].boneWeight[2];
			tempVertex.weight[3] = controlPointSkinData[vertexIndex].boneWeight[3];

			vertices[faceIndex] = tempVertex;
			
			bool pushBack = true;
			for (int c = faceIndex - 1; c >= 0; c--)
				if (tempVertex.position[0] == vertices[c].position[0]	&&
					tempVertex.position[1] == vertices[c].position[1]	&&
					tempVertex.position[2] == vertices[c].position[2]	&&
					tempVertex.uv[0] == vertices[c].uv[0]				&&
					tempVertex.uv[1] == vertices[c].uv[1]				&&
					tempVertex.normal[0] == vertices[c].normal[0]		&&
					tempVertex.normal[1] == vertices[c].normal[1]		&&
					tempVertex.normal[2] == vertices[c].normal[2]		&&
					tempVertex.bone[0] == vertices[c].bone[0]			&&
					tempVertex.bone[1] == vertices[c].bone[1]			&&
					tempVertex.bone[2] == vertices[c].bone[2]			&&
					tempVertex.bone[3] == vertices[c].bone[3]			&&
					tempVertex.weight[0] == vertices[c].weight[0]		&&
					tempVertex.weight[1] == vertices[c].weight[1]		&&
					tempVertex.weight[2] == vertices[c].weight[2]		&&
					tempVertex.weight[3] == vertices[c].weight[3])
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
	vector<SkinData> controlPointSkinData(fbxGeo->GetControlPointsCount());
	FbxSkin* skin = (FbxSkin*)fbxMesh->GetDeformer(0, FbxDeformer::eSkin);

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
		animation.keyframeFirst = 0;
		animation.keyframeLast = keyframeCount - 1;
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

			int currFrame = startFrame;
			for (int t = 0; t < keyframeCount; t++)
			{
				AnimationHolder::KeyFrameHolder& keyframe = animation.keyframes[t];

				FbxTime currentTime;
				currentTime.SetFrame(currFrame, FbxTime::eFrames24);
				currFrame++;

				FbxAMatrix localJoint = cluster->GetLink()->EvaluateLocalTransform(currentTime);

				//FbxAMatrix localJoint2 = cluster->GetLink()->EvaluateGlobalTransform(currentTime);
				//localJoint = localJoint.Inverse() * cluster->GetLink()->EvaluateGlobalTransform(currentTime);

				keyframe.jointId.push_back(jointIndex);
				keyframe.localJointsR.push_back(localJoint.GetQ());
				keyframe.localJointsT.push_back(localJoint.GetT());
				keyframe.localJointsS.push_back(localJoint.GetS());
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
		int* index = cluster->GetControlPointIndices();						// Control point indices for bone at boneIndex
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

		// Direct joint data where GetLink() returns the joint
		/*FbxAMatrix globalTransform = cluster->GetLink()->EvaluateGlobalTransform();
		FbxAMatrix globalTransformInv = cluster->GetLink()->EvaluateGlobalTransform().Inverse();
		FbxAMatrix localTransform = cluster->GetLink()->EvaluateLocalTransform();
		FbxAMatrix localpParent = cluster->GetLink()->GetParent()->EvaluateLocalTransform();
		FbxAMatrix localppParent = cluster->GetLink()->EvaluateLocalTransform().Inverse() * cluster->GetLink()->GetParent()->EvaluateLocalTransform();
		FbxVector4 lTranslation = cluster->GetLink()->LclTranslation.Get();*/


		FbxAMatrix geometryTransform(
			fbxNode->GetGeometricTranslation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricRotation(FbxNode::eSourcePivot),
			fbxNode->GetGeometricScaling(FbxNode::eSourcePivot));
		// This geometry transform is something I cannot understand
		// I think it is from MotionBuilder
		// If you are using Maya for your models, 99% this is just an identity matrix
		// But I am taking it into account anyways......

		FbxAMatrix transformMatrix;
		cluster->GetTransformMatrix(transformMatrix);
		//transform - the initial global transform of the mesh that the bone is controlling.
		//These should be safe to ignore, apparently they are largely legacy and will be the same for every bone in the case of a single skinned mesh.
		//If your artists do "Freeze Transformations", then this matrix would just be an identity matrix.

		FbxAMatrix transformLinkMatrix;
		cluster->GetTransformLinkMatrix(transformLinkMatrix);
		//transformLink - the bone's global transform at the moment of binding. From joint space to world space in Maya.

		FbxAMatrix associateModelTransform;
		cluster->GetTransformAssociateModelMatrix(associateModelTransform);
		// TransformAssociateModel - this also appears to be ignored in FBX SDK example code,
		// and is only needed dependant on the value of ELinkMode according to docs.

		FbxAMatrix parentTransform;
		cluster->GetTransformParentMatrix(parentTransform);

		FbxAMatrix invGlobalBindPose = transformLinkMatrix.Inverse() * transformMatrix;// * geometryTransform * associateModelTransform * parentTransform;
		//FbxAMatrix invGlobalBindPose = transformLinkMatrix.Inverse();
		skeleton.joints[jointIndex].invBindPose = invGlobalBindPose;


		//FbxAMatrix transformTest = CalculateGlobalTransform(cluster->GetLink());
		//skeleton.joints[jointIndex].invBindPose = transformTest.Inverse();


	}
}

FbxAMatrix CalculateGlobalTransform(FbxNode* pNode)
{
	FbxAMatrix lTranlationM, lScalingM, lScalingPivotM, lScalingOffsetM, lRotationOffsetM, lRotationPivotM, \
		lPreRotationM, lRotationM, lPostRotationM, lTransform;

	FbxAMatrix lParentGX, lGlobalT, lGlobalRS;

	if (!pNode)
	{
		lTransform.SetIdentity();
		return lTransform;
	}

	// Construct translation matrix
	FbxVector4 lTranslation = pNode->LclTranslation.Get();
	lTranlationM.SetT(lTranslation);

	// Construct rotation matrices
	FbxVector4 lRotation = pNode->LclRotation.Get();
	FbxVector4 lPreRotation = pNode->PreRotation.Get();
	FbxVector4 lPostRotation = pNode->PostRotation.Get();
	lRotationM.SetR(lRotation);
	lPreRotationM.SetR(lPreRotation);
	lPostRotationM.SetR(lPostRotation);

	// Construct scaling matrix
	FbxVector4 lScaling = pNode->LclScaling.Get();
	lScalingM.SetS(lScaling);

	// Construct offset and pivot matrices
	FbxVector4 lScalingOffset = pNode->ScalingOffset.Get();
	FbxVector4 lScalingPivot = pNode->ScalingPivot.Get();
	FbxVector4 lRotationOffset = pNode->RotationOffset.Get();
	FbxVector4 lRotationPivot = pNode->RotationPivot.Get();
	lScalingOffsetM.SetT(lScalingOffset);
	lScalingPivotM.SetT(lScalingPivot);
	lRotationOffsetM.SetT(lRotationOffset);
	lRotationPivotM.SetT(lRotationPivot);

	// Calculate the global transform matrix of the parent node
	FbxNode* lParentNode = pNode->GetParent();
	if (lParentNode)
	{
		lParentGX = CalculateGlobalTransform(lParentNode);
	}
	else
	{
		lParentGX.SetIdentity();
	}

	//Construct Global Rotation
	FbxAMatrix lLRM, lParentGRM;
	FbxVector4 lParentGR = lParentGX.GetR();
	lParentGRM.SetR(lParentGR);
	lLRM = lPreRotationM * lRotationM * lPostRotationM;

	//Construct Global Shear*Scaling
	//FBX SDK does not support shear, to patch this, we use:
	//Shear*Scaling = RotationMatrix.Inverse * TranslationMatrix.Inverse * WholeTranformMatrix
	FbxAMatrix lLSM, lParentGSM, lParentGRSM, lParentTM;
	FbxVector4 lParentGT = lParentGX.GetT();
	lParentTM.SetT(lParentGT);
	lParentGRSM = lParentTM.Inverse() * lParentGX;
	lParentGSM = lParentGRM.Inverse() * lParentGRSM;
	lLSM = lScalingM;

	//Do not consider translation now
	FbxTransform::EInheritType lInheritType = pNode->InheritType.Get();
	if (lInheritType == FbxTransform::eInheritRrSs)
	{
		lGlobalRS = lParentGRM * lLRM * lParentGSM * lLSM;
	}
	else if (lInheritType == FbxTransform::eInheritRSrs)
	{
		lGlobalRS = lParentGRM * lParentGSM * lLRM * lLSM;
	}
	else if (lInheritType == FbxTransform::eInheritRrs)
	{
		FbxAMatrix lParentLSM;
		FbxVector4 lParentLS = lParentNode->LclScaling.Get();
		lParentLSM.SetS(lParentLS);

		FbxAMatrix lParentGSM_noLocal = lParentGSM * lParentLSM.Inverse();
		lGlobalRS = lParentGRM * lLRM * lParentGSM_noLocal * lLSM;
	}
	else
	{
		FBXSDK_printf("error, unknown inherit type! \n");
	}

	// Construct translation matrix
	// Calculate the local transform matrix
	lTransform = lTranlationM * lRotationOffsetM * lRotationPivotM * lPreRotationM * lRotationM * lPostRotationM * lRotationPivotM.Inverse()\
		* lScalingOffsetM * lScalingPivotM * lScalingM * lScalingPivotM.Inverse();
	FbxVector4 lLocalTWithAllPivotAndOffsetInfo = lTransform.GetT();
	// Calculate global translation vector according to: 
	// GlobalTranslation = ParentGlobalTransform * LocalTranslationWithPivotAndOffsetInfo
	FbxVector4 lGlobalTranslation = lParentGX.MultT(lLocalTWithAllPivotAndOffsetInfo);
	lGlobalT.SetT(lGlobalTranslation);

	//Construct the whole global transform
	lTransform = lGlobalT * lGlobalRS;

	return lTransform;
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

		if (parent == 0)
			for (int c = 0; c < NAME_SIZE; c++)
				meshToPopulate->skeleton.name[c] = meshToPopulate->skeleton.joints[0].name[c];
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

