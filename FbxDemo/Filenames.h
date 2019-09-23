#pragma once

#include "DisplayCommon.h"

#include <fbxsdk.h>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdio>

using namespace std;
#define NAME_SIZE 256

// Filepaths can be input manually here or be recieved as inputs with -i <path> -o <path>
const std::string IN_FBX_FILEPATH = "";
const std::string OUTPUT_PATH = "";

// File header
struct MehHeader
{
	int meshCount;
	int groupCount;
	int materialCount;
	int pointLightCount;
	int dirLightCount;
};

// Grouping
struct Group	// Type 0;
{
	char	name[NAME_SIZE];
		
	float	translation[3];
	float	rotation[3];
	float	scale[3];

	bool	isChild;
	char	parentName[256];
	int		parentType;
};

struct Camera
{
	char	name[NAME_SIZE];

	float	translation[3];
	float	rotation[3];

};


// Skeleton data (inside mesh)
struct Skeleton
{
	char	name[NAME_SIZE];
	int		jointCount;
	int		aniCount;
};

// Mesh data
struct Mesh		// Type 1;
{
	char name[NAME_SIZE];
	int materialID;

	float translation[3];
	float rotation[3];
	float scale[3];

	bool isChild;
	char parentName[NAME_SIZE];
	int parentType;

	int vertexCount;
	int faceCount;

	Skeleton skeleton;
};

// Vertex data (parsed)
struct Vertex
{
	float position[3];
	float uv[2];
	float normal[3];
	float tangent[3];
	float bitangent[3];

	float bone[4];
	float weight[4];
};

struct Face
{
	int indices[3];
};

struct PhongMaterial
{
	char	name[NAME_SIZE];
	float	ambient[3];
	float	diffuse[3];
	float	specular[3];
	float	emissive[3];
	float	opacity;

	char	albedo[NAME_SIZE];
	char	normal[NAME_SIZE];
};

// Joint data (parsed)
struct Joint
{
	char	name[NAME_SIZE];
	int		parentIndex;
	float	invBindPose[16];
};

// Animation data (parsed)
struct Animation
{
	char	name[NAME_SIZE];
	int		keyframeFirst;
	int		keyframeLast;
	float	duration;
	float	rate;
	int		keyframeCount;
};

// Keyframe data (parsed)
struct KeyFrame
{
	int		id;
	int		transformCount;
};

struct Transform
{
	float	transform[3];
	float	rotate[4];
	float	scale[3];
};


// Light data (directional)
struct DirLight 
{
	float position[3];
	float rotation[3];
	float color[3];
	float intensity;
};

// Light data (point)
struct PointLight 
{
	float position[3];
	float color[3];
	float intensity;
};

// =============== Temporary fbx data ===============
struct MeshSkeleton
{
	std::vector<Joint> joint;
};

struct MeshAnis
{
	struct MeshAnimation
	{
		struct KeyFrameL
		{
			struct TransformL
			{
				Transform t;
			};

			KeyFrame key;
			std::vector<TransformL> transforms;
		};

		Animation ani;
		std::vector<KeyFrameL> keyFrames;
	};

	std::vector<MeshAnimation> animations;
};



struct AnimationHolder
{
	struct KeyFrameHolder
	{
		int id;
		// local transform, good for interpolation and then making a final global.
		vector<FbxVector4>		localJointsT;
		vector<FbxQuaternion>	localJointsR;
		vector<FbxVector4>		localJointsS;
	};

	char name[NAME_SIZE];
	int keyframeFirst;
	int keyframeLast;
	float duration;
	float rate;
	vector<KeyFrameHolder> keyframes;
};

struct SkeletonHolder
{
	struct JointHolder
	{
		char name[NAME_SIZE];
		int parentIndex;
		FbxAMatrix invBindPose;
	};

	char name[NAME_SIZE];
	vector<JointHolder> joints;
	vector<AnimationHolder> animations;
};

struct MeshHolder
{
	char name[NAME_SIZE];
	char materialName[256];
	float translation[3];
	float rotation[3];
	float scale[3];

	bool isChild;
	char parentName[256];
	int parentType;

	// Might make this into a vector
	//vector<Vertex> vertices;
	int vertexCount;
	int faceCount;

	Vertex* vertices;
	Face* faces;
	SkeletonHolder skeleton;

	// Constructor that may not be needed
	MeshHolder()
	{
		vertices = nullptr;
	}

	int materialID;
};
// =============== Temporary fbx data ===============
//