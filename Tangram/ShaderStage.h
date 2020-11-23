#pragma once
#ifndef _SHADERSTAGE_H_
#define _SHADERSTAGE_H_

#include "Utility.h"

enum ShaderFeature
{
	Shader_Default = 0, Shader_Custom = 1, Shader_Deferred = 2, Shader_Postprocess = 4,
	Shader_Skeleton = 8, Shader_Morph = 16, Shader_Particle = 32, Shader_Modifier = 64
};

enum ShaderStageType
{
	None_Shader_Stage, Vertex_Shader_Stage, Tessellation_Control_Shader_Stage, Tessellation_Evalution_Shader_Stage,
	Geometry_Shader_Stage, Fragment_Shader_Stage, Compute_Shader_Stage
};

class ShaderStage
{
public:
	ShaderStage(ShaderStageType stageType, const Enum<ShaderFeature> & feature, const string& name);
	virtual ~ShaderStage();

	bool isValid();

	unsigned int compile(const string& code, string& errorString);
	unsigned int compile();
	unsigned int getShaderID() const;
	ShaderStageType getShaderStageType() const;
	Enum<ShaderFeature> getShaderFeature() const;

	static const char* enumShaderType(int type);
	static int enumShaderType(const string& type);
	static int enumShaderType(ShaderStageType stageType);
	static int enumShaderBit(ShaderStageType stageType);
	static const char* enumShaderStageType(ShaderStageType stageType);
	static ShaderStageType enumShaderStageType(unsigned int type);
	static ShaderStageType enumShaderStageType(const string& type);
protected:
	string name;
	ShaderStageType stageType;
	Enum<ShaderFeature> shaderFeature = Shader_Default;
	string code;
	unsigned int shaderId = -1;
};

class ShaderAdapter
{
public:
	string name;
	string path;
	ShaderStageType stageType;
	map<Enum<ShaderFeature>, ShaderStage> shaderStageVersions;

	ShaderAdapter(const string& name, const string& path, ShaderStageType stageType);

	ShaderStage* getShaderStage(const Enum<ShaderFeature>& feature, bool autoFill = false);
	ShaderStage* addShaderStage(const Enum<ShaderFeature>& feature);
	ShaderStage* compileShaderStage(const Enum<ShaderFeature>& feature, const string& code);
};

class ShaderManager
{
public:
	struct Tag
	{
		string path;
		ShaderStageType stageType;

		bool operator<(const Tag& t) const;
	};
	vector<ShaderAdapter*> shaderAdapters;
	map<Tag, unsigned int> shaderAdapterTags;
	multimap<Tag, unsigned int> shaderAdapterNames;

	ShaderAdapter* addShaderAdapter(const string& name, const string& path, ShaderStageType stageType, const string& tagName = "");
	ShaderAdapter* getShaderAdapterByPath(const string& path, ShaderStageType stageType);
	ShaderAdapter* getShaderAdapterByName(const string& name, ShaderStageType stageType);

	ShaderStage* compileShaderStage(ShaderStageType stageType, const Enum<ShaderFeature>& feature, const string& name, const string& path, const string& code);
	bool loadShaderAdapter(const string& path);

	static void loadDefaultAdapter(const string& folder);
	static ShaderManager& getInstance();
};

class ShaderProgram
{
public:
	string name;
	unsigned int renderOrder = 0;
	Enum<ShaderFeature> shaderType;
	map<ShaderStageType, ShaderStage*> shaderStages;

	ShaderProgram();
	ShaderProgram(ShaderStage& meshStage);
	virtual ~ShaderProgram();

	bool isComputable() const;

	bool setMeshStage(ShaderStage& meshStage);
	bool addShaderStage(ShaderStage& stage);
	unsigned int bind();
	bool dispatchCompute(unsigned int dimX, unsigned int dimY, unsigned int dimZ);
	unsigned int getProgramID();

	static unsigned int getCurrentProgramID();
	static void memoryBarrier(unsigned int bitEnum);
protected:
	ShaderStageType meshStageType = None_Shader_Stage;
	unsigned int programId = 0;
	static unsigned int currentProgram;
};

#endif // !_SHADERSTAGE_H_
