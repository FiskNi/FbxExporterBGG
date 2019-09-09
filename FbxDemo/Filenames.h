#pragma once

/*
========================================================================================================================

	Filenames.h holds all filenames to be used for the run of the program.
	**This should be included in any file intending to use the filenames.

	ASCII_FILE is the filename for the textfile to which the ASCII version of the object will be printed to,
		this is the "cheat sheet" for what is in the binary file, but structured in a easily read way.
	BINARY_FILE is the filename for the binary file which is the main file we write to, and which will be read from,
		this will be used the most, as the file has to be opened and closed in different functions.
	IN_FBX_FILEPATH is the .fbx file which will be converted into our custom binary format.

	NAME_SIZE is used for printing names a specific size to be read in.

	// Martina Molid

========================================================================================================================
*/


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
const std::string IN_FBX_FILEPATH	= "D:/!VisualStudio/FiskNi/Project-Kiddo/Resources/Assets/WIP/Maya Scenes/TestScene.fbx";
const std::string OUTPUT_PATH		= "D:/!VisualStudio/FiskNi/Project-Kiddo/Resources/Assets/WIP/Maya Scenes/TestSceneasd";

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
	char	name[NAME_SIZE];
	int		materialID;

	float	translation[3];
	float	rotation[3];
	float	scale[3];

	bool isChild;
	char parentName[NAME_SIZE];
	int parentType;

	int type;
	int link;
	int dir;
	float dist;
	int collect;

	unsigned int vertexCount;

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

//
//
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
	Vertex* vertices;
	SkeletonHolder skeleton;

	int type;
	int link;
	int dir;
	float dist;
	int collect;

	// Constructor that may not be needed
	MeshHolder()
	{
		vertices = nullptr;
	}

	int materialID;
};
// =============== Temporary fbx data ===============
//