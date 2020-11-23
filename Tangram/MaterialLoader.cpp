#include "MaterialLoader.h"
#include <fstream>
#include "Utility.h"

bool MaterialLoader::loadMaterial(Material& material, const string & file)
{
	ifstream f(file, ios::in);
	if (!f)
		return false;
	string clip, line;
	unsigned int type = 0;
	bool mat = false;
	while (1)
	{
		if (!getline(f, line)) {
			if (mat)
				return false;
			if(type != 0)
				material.shader.addShader(type, clip);
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
			else if (s[0] == "material") {
				mat = true;
			}
			else if (s[0] == "order") {
				if (s.size() == 2)
					material.shader.renderOrder = atoi(s[1].c_str());
				else
					return false;
			}
			else {
				int t = Shader::enumShaderType(s[0]);
				if (t == 0)
					clip += line + '\n';
				else {
					if (type != 0) {
						material.shader.addShader(type, clip);
						clip.clear();
					}
					type = t;
					mat = false;
				}
			}
		}
		else if (mat) {
			vector<string> v = split(line, ' ', 2);
			if (v.size() == 2) {
				try {
					if (v[0] == "Scalar")
						material.addScalar(parseScalar(v[1]));
					else if (v[0] == "Color")
						material.addColor(parseColor(v[1]));
					else if (v[0] == "Texture")
						material.addDefaultTexture(parseTexture(v[1]));
				}
				catch (exception e) {
					printf("Matertial attribute parsing error: %s\n", e.what());
					return false;
				}
			}
			else
				return false;
		}
		else {
			clip += line + '\n';
		}
	}
	return true;
}

pair<string, MatAttribute<float>> MaterialLoader::parseScalar(const string & src)
{
	vector<string> v = split(src, ':');
	vector<string> t;
	switch (v.size())
	{
	case 1:
		return pair<string, MatAttribute<float>>(trim(v[0], ' '), { 0 });
		break;
	case 2:
		t = split(v[1], ' ');
		if (t.size() == 1)
			return pair<string, MatAttribute<float>>(trim(v[0], ' '), { (float)atof(t[0].c_str()) });
		else
			throw runtime_error("Error parameter");
		break;
	default:
		throw runtime_error("Error args");
	}
	return pair<string, MatAttribute<float>>();
}

pair<string, MatAttribute<Color>> MaterialLoader::parseColor(const string & src)
{
	vector<string> v = split(src, ':');
	vector<string> t;
	switch (v.size())
	{
	case 1:
		return pair<string, MatAttribute<Color>>(trim(v[0], ' '), { {0, 0, 0, 255} });
		break;
	case 2:
		t = split(v[1], ',');
		switch (t.size())
		{
		case 3:
			return pair<string, MatAttribute<Color>>(trim(v[0], ' '), { {
					atoi(t[0].c_str()),
					atoi(t[1].c_str()),
					atoi(t[2].c_str()) } });
		case 4:
			return pair<string, MatAttribute<Color>>(trim(v[0], ' '), { {
					atoi(t[0].c_str()),
					atoi(t[1].c_str()),
					atoi(t[2].c_str()),
					atoi(t[3].c_str()) } });
		default:
			throw runtime_error("Error parameter");
		}
		break;
	default:
		throw runtime_error("Error args");
	}
	return pair<string, MatAttribute<Color>>();
}

pair<string, MatAttribute<string>> MaterialLoader::parseTexture(const string & src)
{
	vector<string> v = split(src, ':');
	if (v.size() == 1)
		return pair<string, MatAttribute<string>>(v[0], { "" });
	else if(v.size() == 2)
		return pair<string, MatAttribute<string>>(v[0], { v[1] });
	else
		throw runtime_error("Error parameter");
	return pair<string, MatAttribute<string>>();
}
