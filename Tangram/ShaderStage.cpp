#include "ShaderStage.h"
#include <fstream>

ShaderStage::ShaderStage(ShaderStageType stageType, const Enum<ShaderFeature>& feature, const string & name)
	: stageType(stageType), shaderFeature(feature), name(name)
{
}

ShaderStage::~ShaderStage()
{
	if (shaderId != -1)
		glDeleteShader(shaderId);
}

bool ShaderStage::isValid()
{
	return shaderId != -1;
}

unsigned int ShaderStage::compile(const string & code, string & errorString)
{
	unsigned int type = enumShaderType(stageType);
	if (type == 0)
		return shaderId;
	int sid = glCreateShader(type);
	if (sid == 0) {
		errorString = " create failed";
		return shaderId;
	}
	const char* vs = code.c_str();
	glShaderSource(sid, 1, &vs, NULL);
	int state;
	glCompileShader(sid);
	glGetShaderiv(sid, GL_COMPILE_STATUS, &state);
	if (state == GL_FALSE) {
		int infoLogLength;
		glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &infoLogLength);
		char *strInfoLog = new char[infoLogLength + 1];
		glGetShaderInfoLog(sid, infoLogLength, NULL, strInfoLog);
		errorString = strInfoLog;
		const char* shaderTypeName = enumShaderType(type);
		glDeleteShader(sid);
		delete[] strInfoLog;
		return shaderId;
	}
	this->code = code;
	shaderId = sid;
	/*programId = glCreateProgram();
	glProgramParameteri(programId, GL_PROGRAM_SEPARABLE, GL_TRUE);
	glAttachShader(programId, shaderId);
	if (stageType == Vertex_Shader_Stage) {
		glBindAttribLocation(programId, 0, "pos");
		glBindAttribLocation(programId, 1, "uv");
		glBindAttribLocation(programId, 2, "norm");
		glBindAttribLocation(programId, 3, "boneId");
		glBindAttribLocation(programId, 4, "weights");
	}
	else if (stageType == Vertex_Shader_Stage) {
		glBindFragDataLocation(programId, 0, "FragColor");
	}
	glLinkProgram(programId);*/
	return shaderId;
}

unsigned int ShaderStage::compile()
{
	if (code.empty())
		return 0;
	unsigned int type = enumShaderType(stageType);
	if (type == 0)
		return shaderId;
	int sid = glCreateShader(type);
	if (sid == 0) {
		return sid;
	}
	const char* vs = code.c_str();
	glShaderSource(sid, 1, &vs, NULL);
	glCompileShader(sid);
	return sid;
}

unsigned int ShaderStage::getShaderID() const
{
	return shaderId;
}

ShaderStageType ShaderStage::getShaderStageType() const
{
	return stageType;
}

Enum<ShaderFeature> ShaderStage::getShaderFeature() const
{
	return shaderFeature;
}

const char * ShaderStage::enumShaderType(int type)
{
	switch (type)
	{
	case GL_VERTEX_SHADER:
		return "Vertex";
	case GL_FRAGMENT_SHADER:
		return "Fragment";
	case GL_GEOMETRY_SHADER:
		return "Geometry";
	default:
		return "";
	}
}

int ShaderStage::enumShaderType(const string & type)
{
	if (type == "Vertex" || type == "vertex")
		return GL_VERTEX_SHADER;
	else if (type == "Fragment" || type == "fragment")
		return GL_FRAGMENT_SHADER;
	else if (type == "Geometry" || type == "geometry")
		return GL_GEOMETRY_SHADER;
	else
		return 0;
}

int ShaderStage::enumShaderType(ShaderStageType stageType)
{
	switch (stageType)
	{
	case Vertex_Shader_Stage:
		return GL_VERTEX_SHADER;
	case Tessellation_Control_Shader_Stage:
		return GL_TESS_CONTROL_SHADER;
	case Tessellation_Evalution_Shader_Stage:
		return GL_TESS_EVALUATION_SHADER;
	case Geometry_Shader_Stage:
		return GL_GEOMETRY_SHADER;
	case Fragment_Shader_Stage:
		return GL_FRAGMENT_SHADER;
	case Compute_Shader_Stage:
		return GL_COMPUTE_SHADER;
	default:
		break;
	}
	return 0;
}

int ShaderStage::enumShaderBit(ShaderStageType stageType)
{
	switch (stageType)
	{
	case Vertex_Shader_Stage:
		return GL_VERTEX_SHADER_BIT;
	case Tessellation_Control_Shader_Stage:
		return GL_TESS_CONTROL_SHADER_BIT;
	case Tessellation_Evalution_Shader_Stage:
		return GL_TESS_EVALUATION_SHADER_BIT;
	case Geometry_Shader_Stage:
		return GL_GEOMETRY_SHADER_BIT;
	case Fragment_Shader_Stage:
		return GL_FRAGMENT_SHADER_BIT;
	case Compute_Shader_Stage:
		return GL_COMPUTE_SHADER_BIT;
	default:
		break;
	}
	return 0;
}

const char * ShaderStage::enumShaderStageType(ShaderStageType stageType)
{
	switch (stageType)
	{
	case Vertex_Shader_Stage:
		return "Vertex";
	case Fragment_Shader_Stage:
		return "Fragment";
	case Geometry_Shader_Stage:
		return "Geometry";
	case Compute_Shader_Stage:
		return "Compute";
	case Tessellation_Control_Shader_Stage:
		return "Tessellation Control";
	case Tessellation_Evalution_Shader_Stage:
		return "Tessellation Evalution";
	default:
		return "";
	}
}

ShaderStageType ShaderStage::enumShaderStageType(unsigned int type)
{
	switch (type)
	{
	case GL_VERTEX_SHADER:
		return Vertex_Shader_Stage;
	case GL_FRAGMENT_SHADER:
		return Fragment_Shader_Stage;
	case GL_GEOMETRY_SHADER:
		return Geometry_Shader_Stage;
	case GL_COMPUTE_SHADER:
		return Compute_Shader_Stage;
	case GL_TESS_CONTROL_SHADER:
		return Tessellation_Control_Shader_Stage;
	case GL_TESS_EVALUATION_SHADER:
		return Tessellation_Evalution_Shader_Stage;
	default:
		break;
	}
	return None_Shader_Stage;
}

ShaderStageType ShaderStage::enumShaderStageType(const string & type)
{
	if (type == "Vertex" || type == "vertex")
		return Vertex_Shader_Stage;
	else if (type == "Fragment" || type == "fragment")
		return Fragment_Shader_Stage;
	else if (type == "Geometry" || type == "geometry")
		return Geometry_Shader_Stage;
	else if (type == "Compute" || type == "compute")
		return Compute_Shader_Stage;
	else if (type == "Tess_Ctrl" || type == "tess_ctrl")
		return Tessellation_Control_Shader_Stage;
	else if (type == "Tess_Eval" || type == "tess_eval")
		return Tessellation_Evalution_Shader_Stage;
	else
		return None_Shader_Stage;
}

unsigned int ShaderProgram::currentProgram = 0;

ShaderProgram::ShaderProgram()
{
}

ShaderProgram::ShaderProgram(ShaderStage & meshStage)
{
	setMeshStage(meshStage);
}

ShaderProgram::~ShaderProgram()
{
	if (programId != -1)
		glDeleteProgram(programId);
}

bool ShaderProgram::isComputable() const
{
	return meshStageType == Compute_Shader_Stage && shaderStages.size() == 1;
}

bool ShaderProgram::setMeshStage(ShaderStage & meshStage)
{
	if (meshStageType == None_Shader_Stage) {
		meshStageType = meshStage.getShaderStageType();
		return addShaderStage(meshStage);
	}
	return false;
}

bool ShaderProgram::addShaderStage(ShaderStage & stage)
{
	auto iter = shaderStages.find(stage.getShaderStageType());
	if (iter == shaderStages.end()) {
		shaderStages.insert(make_pair(stage.getShaderStageType(), &stage));
		return true;
	}
	return false;
}

unsigned int ShaderProgram::bind()
{
	if (programId == 0) {
		programId = glCreateProgram();
		for (auto b = shaderStages.begin(), e = shaderStages.end(); b != e; b++) {
			if (!b->second->isValid())
				throw runtime_error("Use invalid shader stage");
			glAttachShader(programId, b->second->getShaderID());
		}
		glLinkProgram(programId);
		/*glGenProgramPipelines(1, &programId);
		for (auto b = shaderStages.begin(), e = shaderStages.end(); b != e; b++) {
			unsigned int sid = b->second->getProgramID();
			unsigned int stype = ShaderStage::enumShaderBit(b->first);
			if (sid != -1)
				glUseProgramStages(programId, stype, sid);
		}*/
	}
	if (programId == currentProgram)
		return currentProgram;
	//glBindProgramPipeline(programId);
	glUseProgram(programId);
	currentProgram = programId;
	return programId;
}

bool ShaderProgram::dispatchCompute(unsigned int dimX, unsigned int dimY, unsigned int dimZ)
{
	if (!isComputable())
		return false;
	if (currentProgram != programId)
		return false;
	glDispatchCompute(dimX, dimY, dimZ);
	return true;
}

unsigned int ShaderProgram::getProgramID()
{
	return programId;
}

unsigned int ShaderProgram::getCurrentProgramID()
{
	return currentProgram;
}

void ShaderProgram::memoryBarrier(unsigned int bitEnum)
{
	glMemoryBarrier(bitEnum);
}

ShaderAdapter * ShaderManager::addShaderAdapter(const string& name, const string & path, ShaderStageType stageType, const string& tagName)
{
	Tag tag = { path, stageType };
	auto iter = shaderAdapterTags.find(tag);
	if (iter == shaderAdapterTags.end()) {
		shaderAdapterTags.insert(make_pair(tag, shaderAdapters.size()));
		if (!tagName.empty()) {
			Tag tag = { tagName, stageType };
			shaderAdapterNames.insert(make_pair(tag, shaderAdapters.size()));
		}
		ShaderAdapter* adapter = new ShaderAdapter(name, path, stageType);
		shaderAdapters.emplace_back(adapter);
		return adapter;
	}
	else
		return shaderAdapters[iter->second];
}

ShaderAdapter * ShaderManager::getShaderAdapterByPath(const string& path, ShaderStageType stageType)
{
	Tag tag = { path, stageType };
	auto iter = shaderAdapterTags.find(tag);
	if (iter != shaderAdapterTags.end())
		return shaderAdapters[iter->second];
	return NULL;
}

ShaderAdapter * ShaderManager::getShaderAdapterByName(const string & name, ShaderStageType stageType)
{
	Tag tag = { name, stageType };
	auto iter = shaderAdapterNames.find(tag);
	if (iter != shaderAdapterNames.end())
		return shaderAdapters[iter->second];
	return NULL;
}

ShaderStage * ShaderManager::compileShaderStage(ShaderStageType stageType, const Enum<ShaderFeature>& feature, const string & name, const string & path, const string & code)
{
	ShaderAdapter* adapter = addShaderAdapter(name, path, stageType);
	if (adapter == NULL)
		return NULL;
	return adapter->compileShaderStage(feature, code);
}

bool ShaderManager::loadShaderAdapter(const string & path)
{
	ifstream f(path, ios::in);
	if (!f)
		return false;
	string clip, line, name, envPath, adapterName;
	experimental::filesystem::path _path = path;
	name = _path.filename().generic_u8string();
	envPath = _path.parent_path().generic_u8string();
	ShaderStageType stageType = None_Shader_Stage;
	Enum<ShaderFeature> feature;
	unordered_set<string> headFiles;
	map<ShaderStageType, ShaderAdapter*> adapters;
	auto getAdapter = [&adapters](ShaderStageType type)->ShaderAdapter* {
		auto iter = adapters.find(type);
		if (iter == adapters.end())
			return NULL;
		return iter->second;
	};
	bool successed = true;
	while (1)
	{
		if (!getline(f, line)) {
			if (stageType != None_Shader_Stage) {
				ShaderAdapter* adapter = NULL;
				adapter = getAdapter(stageType);
				if (adapter == NULL) {
					adapter = ShaderManager::getInstance().addShaderAdapter(name, path, stageType, adapterName);
					if (adapter == NULL) {
						successed = false;
						break;
					}
					adapters.insert(make_pair(stageType, adapter));
				}
				adapter->compileShaderStage(feature, clip);
			}
			break;
		}
		if (line.empty())
			continue;
		size_t loc = line.find_first_of('#');
		if (loc != string::npos) {
			string ss = line.substr(loc + 1);
			vector<string> s = split(line.substr(loc + 1), ' ');
			if (s.size() == 0)
				continue;
			else if (s[0] == "adapter") {
				if (s.size() > 2 && s[1] == "name") {
					adapterName = s[2];
				}
			}
			else {
				ShaderStageType _stageType = ShaderStage::enumShaderStageType(s[0]);
				if (_stageType == None_Shader_Stage) {
					if (!readHeadFile(line, clip, envPath, headFiles)) {
						successed = false;
						break;
					}
					if (stageType == Fragment_Shader_Stage && s[0] == "version") {
						clip += "layout(early_fragment_tests) in;\n";
					}
				}
				else {
					Enum<ShaderFeature> _feature = Shader_Default;
					for (int i = 1; i < s.size(); i++)
						if (s[i] == "custom")
							_feature |= Shader_Custom;
						else if (s[i] == "deferred")
							_feature |= Shader_Deferred;
						else if (s[i] == "postprocess")
							_feature |= Shader_Postprocess;
						else if (s[i] == "skeleton")
							_feature |= Shader_Skeleton;
						else if (s[i] == "morph")
							_feature |= Shader_Morph;
						else if (s[i] == "particle")
							_feature |= Shader_Particle;
						else if (s[i] == "modifier")
							_feature |= Shader_Modifier;
					if (stageType != None_Shader_Stage) {
						ShaderAdapter* adapter = NULL;
						adapter = getAdapter(stageType);
						if (adapter == NULL) {
							adapter = ShaderManager::getInstance().addShaderAdapter(name, path, stageType, adapterName);
							if (adapter == NULL) {
								successed = false;
								break;
							}
							adapters.insert(make_pair(stageType, adapter));
						}
						adapter->compileShaderStage(feature, clip);
						clip.clear();
						headFiles.clear();
					}
					stageType = _stageType;
					feature = _feature;
				}
			}
		}
		else {
			if (!readHeadFile(line, clip, envPath, headFiles)) {
				successed = false;
				break;
			}
		}
	}
	f.close();
	return successed;
}

void ShaderManager::loadDefaultAdapter(const string& folder)
{
	namespace FS = experimental::filesystem;
	for (auto& p : FS::recursive_directory_iterator(folder)) {
		string path = p.path().generic_string();
		if (p.status().type() == experimental::filesystem::file_type::regular) {
			string ext = p.path().extension().generic_string();
			string name = p.path().filename().generic_string();
			name = name.substr(0, name.size() - ext.size());
			if (!_stricmp(ext.c_str(), ".shadapter")) {
				if (ShaderManager::getInstance().loadShaderAdapter(path))
					Console::log("%s load", path.c_str());
				else
					Console::error("%s load failed", path.c_str());
			}
		}
	}
}

ShaderManager & ShaderManager::getInstance()
{
	static ShaderManager manager;
	return manager;
}

ShaderAdapter::ShaderAdapter(const string & name, const string & path, ShaderStageType stageType)
	: name(name), path(path), stageType(stageType)
{
}

ShaderStage * ShaderAdapter::getShaderStage(const Enum<ShaderFeature>& feature, bool autoFill)
{
	if (autoFill && shaderStageVersions.size() == 1)
		return &shaderStageVersions.begin()->second;
	auto iter = shaderStageVersions.find(feature);
	if (iter != shaderStageVersions.end())
		return &iter->second;
	else if (autoFill) {
		iter = shaderStageVersions.find(Shader_Default);
		if (iter != shaderStageVersions.end())
			return &iter->second;
	}
	return NULL;
}

ShaderStage * ShaderAdapter::addShaderStage(const Enum<ShaderFeature> & feature)
{
	auto iter = shaderStageVersions.find(feature);
	if (iter == shaderStageVersions.end())
		return &shaderStageVersions.insert(make_pair(feature, ShaderStage(stageType, feature, name))).first->second;
	else
		return &iter->second;
}

ShaderStage * ShaderAdapter::compileShaderStage(const Enum<ShaderFeature>& feature, const string & code)
{
	ShaderStage* stage = addShaderStage(feature);
	if (!stage->isValid()) {
		string errorStr;
		if (stage->compile(code, errorStr) == -1) {
			const char* shaderTypeName = ShaderStage::enumShaderStageType(stageType);
			Console::log("%s Shader\n%s", shaderTypeName, code.c_str());
			Console::error("%s (%s Shader) compile failed:\n%s", name.c_str(), shaderTypeName, errorStr.c_str());
		}
	}
	return stage;
}

bool ShaderManager::Tag::operator<(const Tag & t) const
{
	if (path < t.path)
		return true;
	else if (path == t.path)
		return stageType < t.stageType;
	return false;
}
