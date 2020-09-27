#pragma once

#ifndef CvDLLPythonIFaceBase_h__
#define CvDLLPythonIFaceBase_h__

//
// abstract interface for Python functions used by DLL
// Creator - Mustafa Thamer
// Copyright 2005 Firaxis Games
//

#ifndef __INTELLISENSE__
#include <boost/python/object.hpp>
namespace python = boost::python;
#endif

class CvUnit;
class CvPlot;
class CvDLLPythonIFaceBase
{
public:

	virtual bool isInitialized() = 0;

	virtual const char* getMapScriptModule() = 0;

	template <typename T>	void setSeqFromArray(const T* /*src*/, int size, PyObject* /*dst*/);
	template <typename T>	int putSeqInArray(PyObject* /*src*/, T** /*dst*/);
	template <typename T>	int putStringSeqInArray(PyObject* /*src*/, T** /*dst*/);
	template <typename T>	int putFloatSeqInArray(PyObject* /*src*/, T** /*dst*/);
	template <typename T>	PyObject* makePythonObject(T*);

	virtual PyObject* MakeFunctionArgs(void** args, int argc) = 0;

	virtual bool moduleExists(const char* moduleName, bool bLoadIfNecessary) = 0;

	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg=NULL) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, long* result) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, CvString* result) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, CvWString* result) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, std::vector<uint8_t>* pList) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, std::vector<int> *pIntList) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, int* pIntList, int* iListSize) = 0;
	virtual bool callFunction(const char* moduleName, const char* fxnName, void* fxnArg, std::vector<float> *pFloatList) = 0;

	virtual bool callPythonFunction(const char* szModName, const char* szFxnName, int iArg, long* result) = 0; // HELPER version that handles 1 arg for you

	virtual bool pythonUsingDefaultImpl() = 0;
};


/* THESE MACROS ARE DEPRECATED, use CvPython::call and CyArgsList() << operation like so:

std::vector<int> arr = CvPython::call(PYGameModule, "getOrderArray", CyArgsList() << arg1 << arg2 << CyArrayArg(enabled, 4));
or
std::vector<int> arr = CvPython::call(PYGameModule, "getOrderArray", arg1, arg2, CyArrayArg(enabled, 4));
*/

#define PYTHON_CALL_FUNCTION2(_callingfn_, _module_, _function_)					gDLL->getPythonIFace()->callFunction(_module_, _function_)
#define PYTHON_CALL_FUNCTION(_callingfn_, _module_, _function_, _args_)				gDLL->getPythonIFace()->callFunction(_module_, _function_, _args_)
#define PYTHON_CALL_FUNCTION4(_callingfn_, _module_, _function_, _args_, _result_)	gDLL->getPythonIFace()->callFunction(_module_, _function_, _args_, _result_)

template <typename T>
PyObject* CvDLLPythonIFaceBase::makePythonObject(T* pObj)
{
	if (!pObj)
		return Py_None;

	boost::python::object bpo(pObj);
	PyObject* pyobj = bpo.ptr();
	Py_INCREF(pyobj);
	assert(pyobj->ob_refcnt==2);
	return pyobj;	// decrefs pyobj when bpo goes out of scope
}

//
// static
// convert array to python list
//
template <typename T>
void CvDLLPythonIFaceBase::setSeqFromArray(const T* aSrc, int size, PyObject* dst)
{
	if (size<1)
		return;

	int iSeqSize=PySequence_Length(dst);
	FAssertMsg(iSeqSize>=size, "sequence length too small");

	int i;
	for (i=0;i<size;i++)
	{
		PyObject* x = PyInt_FromLong(aSrc[i]);
#ifdef FASSERT_ENABLE
		int ret=
#endif
			PySequence_SetItem(dst, i, x); 
		FAssertMsg(ret!=-1, "PySequence_SetItem failed");
		Py_DECREF(x);
	}

	// trim extra space
	if (iSeqSize>size)
	{
#ifdef FASSERT_ENABLE
		int ret=
#endif
			PySequence_DelSlice(dst, size, iSeqSize);
		FAssertMsg(ret!=-1, "PySequence_DelSlice failed");
	}
}

//
// static 
// convert python list to array
// allocates array
//
template <typename T>
int CvDLLPythonIFaceBase::putSeqInArray(PyObject* src, T** aDst)
{
	*aDst = NULL;
	int size = PySequence_Length(src);
	if (size<1)
		return 0;

	*aDst = new T[size];
	int i;
	for (i=0;i<size;i++)
	{
		PyObject* item = PySequence_GetItem(src, i); /* Can't fail */
		FAssertMsg(PyInt_Check(item), "sequence item is not an int");
		(*aDst)[i] = (T)PyInt_AsLong(item);
		Py_DECREF(item);
	}
	return size;
}

//
// static 
// convert python list to array
// allocates array
//
template <typename T>
int CvDLLPythonIFaceBase::putFloatSeqInArray(PyObject* src, T** aDst)
{
	*aDst = NULL;
	int size = PySequence_Length(src);
	if (size<1)
		return 0;

	*aDst = new T[size];
	int i;
	for (i=0;i<size;i++)
	{
		PyObject* item = PySequence_GetItem(src, i); /* Can't fail */
		FAssertMsg(PyFloat_Check(item), "sequence item is not an float");
		(*aDst)[i] = (T)PyFloat_AsDouble(item);
		Py_DECREF(item);
	}
	return size;
}

//
// static 
// convert python list to array
// allocates array
//
template <typename T>
int CvDLLPythonIFaceBase::putStringSeqInArray(PyObject* src, T** aDst)
{
	*aDst = NULL;
	int size = PySequence_Length(src);
	if (size<1)
		return 0;

	*aDst = new T[size];
	int i;
	for (i=0;i<size;i++)
	{
		PyObject* item = PySequence_GetItem(src, i); /* Can't fail */
		FAssertMsg(PyString_Check(item), "sequence item is not a string");
		(*aDst)[i] = (T)PyString_AsString(item);
		Py_DECREF(item);
	}
	return size;
}
#endif // CvDLLPythonIFaceBase_h__
