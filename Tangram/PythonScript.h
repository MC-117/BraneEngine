#pragma once
#ifndef _PYTHONSCRIPT_H_
#define _PYTHONSCRIPT_H_

#include "PythonManager.h"

class PythonScript
{
public:
	PythonScript();
	PythonScript(const string& file);

	bool isValid();
	bool load(const string& file);

	bool ErrorFetch(const char* funcName);

	string getName();
	string getCodePath();
	string getSourceCode();

	bool setSourceCode(const string& code);
	bool refresh();

	bool saveSourceCode();

	PyObject* construct(void* ptr);
protected:
	PyTypeObject* pytype = NULL;
	string sourceCode;
	string codePath;
	string name;
};

class PythonRuntimeObject
{
public:
	PythonRuntimeObject();
	PythonRuntimeObject(void* ptr);
	PythonRuntimeObject(void* ptr, PythonScript& script);
	~PythonRuntimeObject();

	bool isValid();
	bool isReady();
	PythonScript* getScript();
	void setScript(PythonScript& script);
	void removeScript();

	void* getBindedObject();
	void bindObject(void* ptr);

	bool setup();
	bool ErrorFetch(const char* funcName);

	bool begin();
	bool tick(float deltaTime);
	bool afterTick();
	bool end();
protected:
	PythonScript* script = NULL;
	void* ptr = NULL;
	PyObject* pyobject = NULL;
	bool beginFunc = NULL;
	bool tickFunc = NULL;
	bool afterTickFunc = NULL;
	bool endFunc = NULL;
};

#endif // !_PYTHONSCRIPT_H_
