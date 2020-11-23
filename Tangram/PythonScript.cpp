#include "PythonScript.h"
#include <fstream>
#include "Utility.h"

PythonScript::PythonScript()
{
}

PythonScript::PythonScript(const string & file)
{
	load(file);
}

bool PythonScript::isValid()
{
	return pytype != NULL;
}

bool PythonScript::load(const string & file)
{
	experimental::filesystem::path p = file;
	string ext = p.extension().generic_string();
	string name = p.filename().generic_string();
	name = name.substr(0, name.size() - ext.size());
	ifstream f(file);
	if (f.fail())
		return false;
	ostringstream sin;
	sin << f.rdbuf();
	sourceCode = sin.str();
	f.close();
	f.clear();
	if (sourceCode.empty())
		return false;
	sourceCode += '\n';
	if (PyRun_SimpleString(sourceCode.c_str()) == -1)
		return false;
	PyObject* obj = PyObject_GetAttrString(PythonManager::getMainModule(), name.c_str());
	if (obj == NULL || !PyObject_TypeCheck(obj, &PyType_Type))
		return false;
	this->name = name;
	codePath = file;
	pytype = (PyTypeObject*)obj;
	Py_INCREF(pytype);
	return true;
}

string PythonScript::getName()
{
	return name;
}

string PythonScript::getCodePath()
{
	return codePath;
}

string PythonScript::getSourceCode()
{
	return sourceCode;
}

PyObject * PythonScript::construct(void* ptr)
{
	if (!isValid())
		return NULL;
	//PyObject *argList = Py_BuildValue("i", (int)ptr);
	string ps = to_string((long long)ptr);
	string oname = "___" + ps + "___";
	string code = oname + " = " + name + '(' + ps + ')';
	PyRun_SimpleString(code.c_str());
	//PyObject *obj = PyObject_CallFunction((PyObject *)pytype, "L", ptr);
	//Py_DECREF(argList);
	/*PyObject *obj = _PyObject_New(pytype);
	PyObject_Init(obj, pytype);
	((PyCPointer*)obj)->nativeHandle = ptr;*/
	//Py_DECREF(pytype);
	PyObject* obj = PyObject_GetAttrString(PythonManager::getMainModule(), oname.c_str());
	if (obj == NULL || !PyObject_TypeCheck(obj, pytype))
		return NULL;
	return obj;
}

PythonRuntimeObject::PythonRuntimeObject()
{
}

PythonRuntimeObject::PythonRuntimeObject(void * ptr) : ptr(ptr)
{
}

PythonRuntimeObject::PythonRuntimeObject(void * ptr, PythonScript & script) : ptr(ptr), script(&script)
{
	setup();
}

PythonRuntimeObject::~PythonRuntimeObject()
{
	if (pyobject != NULL) {
		Py_DECREF(pyobject);
		pyobject = NULL;
	}
}

bool PythonRuntimeObject::isValid()
{
	return ptr != NULL && script != NULL && script->isValid();
}

bool PythonRuntimeObject::isReady()
{
	return pyobject != NULL;
}

PythonScript * PythonRuntimeObject::getScript()
{
	return script;
}

void PythonRuntimeObject::setScript(PythonScript & script)
{
	if (this->script == &script)
		return;
	if (pyobject != NULL) {
		Py_DECREF(pyobject);
		pyobject = NULL;
	}
	this->script = &script;
	setup();
}

void PythonRuntimeObject::removeScript()
{
	if (pyobject != NULL) {
		Py_DECREF(pyobject);
		pyobject = NULL;
	}
	script = NULL;
}

void * PythonRuntimeObject::getBindedObject()
{
	return ptr;
}

void PythonRuntimeObject::bindObject(void* ptr)
{
	if (this->ptr == ptr)
		return;
	if (pyobject != NULL) {
		Py_DECREF(pyobject);
		pyobject = NULL;
	}
	this->ptr = ptr;
	setup();
}

bool PythonRuntimeObject::setup()
{
	if (!isValid())
		return false;
	if (pyobject == NULL)
		pyobject = script->construct(ptr);
	if (pyobject == NULL)
		return false;
	beginFunc = PyObject_HasAttrString(pyobject, "begin");
	tickFunc = PyObject_HasAttrString(pyobject, "tick");
	afterTickFunc = PyObject_HasAttrString(pyobject, "afterTick");
	endFunc = PyObject_HasAttrString(pyobject, "end");
	return true;
}

bool PythonRuntimeObject::ErrorFetch(const char* funcName)
{
	PyObject * extype, *exvalue, *extraceback;
	PyErr_Fetch(&extype, &exvalue, &extraceback);
	PyErr_NormalizeException(&extype, &exvalue, &extraceback);
	if (extype != NULL) {
		PyObject* extypestr = PyObject_Str(extype);
		PyObject* valuestr = PyObject_Str(exvalue);
		const char* type = extypestr->ob_type->tp_name;
		const char* value = PyUnicode_AsUTF8(valuestr);
		Console::pyError(
			"Traceback(most recent call last)\n"
			"\tFile \"%s\", method \"%s\", in __main__\n"
			"%s: %s", script == NULL ? "" : script->getCodePath().c_str(),
			funcName, type, value);
		return true;
	}
	return false;
}

bool PythonRuntimeObject::begin()
{
	if (pyobject == NULL || !beginFunc)
		return false;
	string code = "___" + to_string((long long)ptr) + "___.begin()";
	PyRun_SimpleString(code.c_str());
	return !ErrorFetch("begin");
}

bool PythonRuntimeObject::tick(float deltaTime)
{
	if (pyobject == NULL || !tickFunc)
		return false;
	string code = "___" + to_string((long long)ptr) + "___.tick" + '(' + to_string(deltaTime) + ')';
	PyRun_SimpleString(code.c_str());
	return !ErrorFetch("tick");
}

bool PythonRuntimeObject::afterTick()
{
	if (pyobject == NULL || !afterTickFunc)
		return false;
	string code = "___" + to_string((long long)ptr) + "___.afterTick()";
	PyRun_SimpleString(code.c_str());
	return !ErrorFetch("afterTick");
}

bool PythonRuntimeObject::end()
{
	if (pyobject == NULL || !endFunc)
		return false;
	string code = "___" + to_string((long long)ptr) + "___.end()";
	PyRun_SimpleString(code.c_str());
	return !ErrorFetch("end");
}
