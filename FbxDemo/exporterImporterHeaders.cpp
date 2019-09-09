struct filetypeHeader
{
	int meshGroupCount;
	int meshCount;
	int materialCount;
	int animationCount;
	int lightSourceCount;
	//
};

struct meshGroup
{
	char groupName[256];
	float transformation[16];
	bool isChild;
	char parentName[256];
};

struct mesh
{
	char meshName[256];
	//bool isChild;
	//char parentName[256];
	unsigned int vertexCount;
	char materialName;
	bool customAttribute;
	char customAttributeName[256];

	//unsigned int bones;

};

struct vertex
{
	float pos[3];
	float uv[2];
	float normal[3];
	float tangent[3];
	float bitangent[3];
	//unsigned int morphStates; 
	//unsigned int boneID[4];	
	//float weight[4];
};

struct material
{
	char materialName[256];
	char materialType[256];
	char albedoFile[256];
	char normalFile[256];
};

struct Phong
{
	float ambient;
	float diffuse;
	float specular;
	float emissive;
};

struct lightSource
{
	float pos[3];
	float direction[3];
};


// Later
struct morphState
{
	unsigned int morphID;
	float pos[3];
};

struct bone
{
	char name[256];
	char parent[256];
	float offsetTransformation[16];
};

struct animation
{
	float duration;
	float framesPerSecond;
	unsigned int keyFrames;
};

struct animationKeyframe
{
	unsigned int frame;
	unsigned int boneTransforms;
	unsigned int morphTransforms;
};

struct morphTransform
{
	unsigned int morphStateID;
	unsigned int morphAmount;
};







