#pragma once
#ifndef _UTILITY_H_
#define _UTILITY_H_
#include "Console.h"
#include "Curve.h"
#include <filesystem>

const char* getGLErrorInfo(int errorId);
const char* getALErrorInfo(int errorId);

vector<string> split(const string& str, char ch, unsigned int num = -1, void(*post)(string&) = NULL);
vector<string> split(const string& str, const string& chs, unsigned int num = -1, void(*post)(string&) = NULL);
string& trim(string& str, char ch);
string& trimL(string& str, const string& chs);
string& trimR(string& str, const string& chs);
string& trim(string& str, const string& chs);

inline void trimSpace(string& str) { trim(str, " \t\n"); }

bool splitPattern(const string & str, char start, char end, vector<pair<bool, string>>& re, bool(*post)(bool, string&) = NULL);
bool splitMap(const string & str, char start, char end, map<string, string>& re, bool(*post)(string&, string&) = NULL);

template<class V>
void findSimilar(const map<string, V>& container, string key, vector<V>& re)
{
	auto b = container.lower_bound(key);
	key.back() += 1;
	auto e = container.lower_bound(key);
	auto end = container.end();
	while (b != end) {
		if (b == e)
			break;
		re.push_back(b->second);
		b++;
	}
}

bool equal(const Quaternionf& q0, const Quaternionf& q1, float threahold = 0.000001f);

static int gl_debug_error = GL_NO_ERROR;
static vector<string> gl_debug_error_str;

#define GL_ERR while(true) { int err = glGetError(); if (err == GL_NO_ERROR) break; else { gl_debug_error = err; gl_debug_error_str.push_back(string(getGLErrorInfo(gl_debug_error))); } }\
if (gl_debug_error != GL_NO_ERROR) {\
	/*gl_debug_error_str = string(getGLErrorInfo(gl_debug_error));*/\
	/*if (gl_debug_error != GL_INVALID_OPERATION)\*/\
		throw 0;\
	gl_debug_error = GL_NO_ERROR;\
	gl_debug_error_str.clear();\
}\

#define GL_PASS while(glGetError() != GL_NO_ERROR);

static int al_debug_error = GL_NO_ERROR;

#define AL_ERR while(true) { int err = alGetError(); if (err == GL_NO_ERROR) break; else al_debug_error = err; }\
if (al_debug_error != GL_NO_ERROR) {\
	string str = getALErrorInfo(al_debug_error);\
	/*if (_debug_error != GL_INVALID_OPERATION)\*/\
		throw runtime_error("");\
	al_debug_error = GL_NO_ERROR;\
}\

PVec3 toPVec3(const Vector3f& v);
PQuat toPQuat(const Quaternionf& q);
PTransform toPTransform(const Vector3f& v, const Quaternionf& q = Quaternionf::Identity());

Vector3f toVector3f(const PVec3& v);
Quaternionf toQuaternionf(const PQuat& q);

string& validateRelativePath(string& path);
string getFilePath(const string& path, const string& pwd);
bool readHeadFile(const string& codeLine, string& code, const string& envPath = "", unordered_set<string>& headFiles = unordered_set<string>());

template<class TT, class ST>
inline bool isClassOf(ST* ptr)
{
	return dynamic_cast<TT*>(ptr) != NULL;
}

namespace ImGui {

	struct PlotTimeLineData
	{
		const char* label;
		float val;
		ImU32 col;
		ImU32 col_hovered;
	};

	void PlotTimeLine(const char* label, PlotTimeLineData(*values_getter)(void* data, int idx), void* data, int values_count, int values_offset, ImVec2 frame_size);

	bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);
}

#endif // !_UTILITY_H_

class GLBuffer
{
public:
	GLuint type;
	GLuint cellSize = 1;
	GLuint size = 0;
	GLuint capacity = 0;

	GLBuffer(GLuint type, GLuint cellSize, GLuint size = 0);
	~GLBuffer();
	GLuint bind();
	GLuint bindBase(GLuint index);
	void unbind();
	GLuint resize(GLuint size);
	GLuint uploadSubData(GLuint first, GLuint size, void* data);
	GLuint uploadData(GLuint size, void* data);

	operator GLuint*();
protected:
	GLuint id = 0;
};

struct Path
{
	vector<string> logicPath;

	Path();
	Path(const Path& path);
	Path(Path&& path);
	Path(const string& path);
	Path(const char* path);

	bool empty() const;
	size_t size() const;

	void up();
	void down(const string& name);
	Path popTop() const;

	Path& operator=(const Path& path);
	Path& operator=(Path&& path);
	bool operator==(const Path& path) const;
	bool operator!=(const Path& path) const;
	string& operator[](size_t i);
	string operator[](size_t i) const;
	operator string() const;
};

struct AssetPath
{
	string name;
	string path;

	bool operator<(const AssetPath& a) const;
};

template<class K, class V>
class ArrayMap
{
public:
	class Iterator
	{
	public:
		typename map<K, size_t>::iterator iter;
		vector<V>* values = NULL;

		Iterator() { }
		Iterator(const typename map<K, size_t>::iterator& iter, vector<V>* values) : iter(iter), values(values) { }
		Iterator(const Iterator& iter) : iter(iter.iter), values(iter.values) { }
		Iterator(Iterator&& iter) : iter(iter.iter), values(iter.values) { }
		Iterator& operator=(const Iterator& iter) { this->iter = iter.iter; values = iter.values; return *this; }
		Iterator& operator++() { iter++; return *this; }
		Iterator operator++(int) { auto t = Iterator(iter, values); ++iter; return t; }
		Iterator& operator--() { iter--; return *this; }
		Iterator operator--(int) { auto t = Iterator(iter, values); --iter; return t; }
		bool operator==(const Iterator& iter) const { return this->iter == iter.iter; }
		bool operator!=(const Iterator& iter) const { return this->iter != iter.iter; }
		V* operator->() { return &(*values)[iter->second]; }
		V& operator*() { return (*values)[iter->second]; }
		size_t getIndex() { return iter->second; }
	};

	pair<Iterator, bool> insert(const K& key, const V& value)
	{
		auto iter = keys.insert(make_pair(key, values.size()));
		if (iter.second)
			values.push_back(value);
		return make_pair(Iterator(iter.first, &values), iter.second);
	}

	size_t size() const
	{
		return values.size();
	}

	Iterator find(const K& key)
	{
		return Iterator(key.find(key), &values);
	}

	V& at(size_t index)
	{
		return values.at(index);
	}

	Iterator begin()
	{
		return Iterator(keys.begin(), &values);
	}

	Iterator end()
	{
		return Iterator(keys.end(), &values);
	}

	pair<Iterator, Iterator> findSimilar(const K& skey, const K& bkey)
	{
		auto b = keys.lower_bound(skey);
		auto e = keys.lower_bound(bkey);
		return pair<Iterator, Iterator>(Iterator(b, &values), Iterator(e, &values));
	}

	V& front()
	{
		return values.front();
	}

	V& back()
	{
		return values.back();
	}
protected:
	map<K, size_t> keys;
	vector<V> values;
};

template<class E>
class Enum
{
public:
	unsigned int enumValue = 0;

	Enum();
	Enum(const E& e);
	Enum(unsigned int e);

	bool has(const Enum& e) const;
	Enum intersect(const Enum& e) const;

	Enum operator|(const Enum& e) const;
	Enum operator^(const Enum& e) const;
	Enum operator!() const;
	Enum operator~() const;
	
	Enum& operator=(const Enum& e);
	Enum& operator|=(const Enum& e);
	Enum& operator^=(const Enum& e);

	bool operator==(const Enum& e) const;
	bool operator!=(const Enum& e) const;

	bool operator<(const Enum& e) const;

	operator unsigned int() const;
	operator E() const;
};

template<class E>
inline Enum<E>::Enum()
{
}

template<class E>
inline Enum<E>::Enum(const E & e)
{
	enumValue = (unsigned int)e;
}

template<class E>
inline Enum<E>::Enum(unsigned int e)
{
	enumValue = e;
}

template<class E>
inline bool Enum<E>::has(const Enum & e) const
{
	return (enumValue & e.enumValue) == e.enumValue;
}

template<class E>
inline Enum<E> Enum<E>::intersect(const Enum & e) const
{
	return enumValue & e.enumValue;
}

template<class E>
inline Enum<E> Enum<E>::operator|(const Enum & e) const
{
	return Enum(enumValue | e.enumValue);
}

template<class E>
inline Enum<E> Enum<E>::operator^(const Enum & e) const
{
	return Enum(enumValue ^ e.enumValue);
}

template<class E>
inline Enum<E> Enum<E>::operator!() const
{
	return Enum(~enumValue);
}

template<class E>
inline Enum<E> Enum<E>::operator~() const
{
	return Enum(~enumValue);
}

template<class E>
inline Enum<E> & Enum<E>::operator=(const Enum & e)
{
	enumValue = e.enumValue;
	return *this;
}

template<class E>
inline Enum<E> & Enum<E>::operator|=(const Enum & e)
{
	enumValue |= e.enumValue;
	return *this;
}

template<class E>
inline Enum<E> & Enum<E>::operator^=(const Enum & e)
{
	enumValue ^= e.enumValue;
	return *this;
}

template<class E>
inline bool Enum<E>::operator==(const Enum & e) const
{
	return enumValue == e.enumValue;
}

template<class E>
inline bool Enum<E>::operator!=(const Enum & e) const
{
	return enumValue != e.enumValue;
}

template<class E>
inline bool Enum<E>::operator<(const Enum & e) const
{
	return enumValue < e.enumValue;
}

template<class E>
inline Enum<E>::operator unsigned int() const
{
	return enumValue;
}

template<class E>
inline Enum<E>::operator E() const
{
	return (E)enumValue;
}