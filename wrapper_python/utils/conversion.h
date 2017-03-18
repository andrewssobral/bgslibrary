/*
This file is part of BGSLibrary.

BGSLibrary is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

BGSLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BGSLibrary.  If not, see <http://www.gnu.org/licenses/>.
*/
// Author: Sudeep Pillai (spillai@csail.mit.edu)
// Note: Stripped from Opencv (opencv/modules/python/src2/cv2.cpp)

#pragma once

#include <Python.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <numpy/ndarrayobject.h>

static PyObject* opencv_error = 0;

static int failmsg(const char *fmt, ...);

class PyAllowThreads;

class PyEnsureGIL;

#define ERRWRAP2(expr) \
try \
{ \
    PyAllowThreads allowThreads; \
    expr; \
} \
catch (const cv::Exception &e) \
{ \
    PyErr_SetString(opencv_error, e.what()); \
    return 0; \
}

static PyObject* failmsgp(const char *fmt, ...);

static size_t REFCOUNT_OFFSET = (size_t)&(((PyObject*)0)->ob_refcnt) +
    (0x12345678 != *(const size_t*)"\x78\x56\x34\x12\0\0\0\0\0")*sizeof(int);

static inline PyObject* pyObjectFromRefcount(const int* refcount)
{
    return (PyObject*)((size_t)refcount - REFCOUNT_OFFSET);
}

static inline int* refcountFromPyObject(const PyObject* obj)
{
    return (int*)((size_t)obj + REFCOUNT_OFFSET);
}


class NumpyAllocator;

enum { ARG_NONE = 0, ARG_MAT = 1, ARG_SCALAR = 2 };

class NDArrayConverter
{
private:
    void init();
public:
    NDArrayConverter();
    cv::Mat toMat(const PyObject* o);
    PyObject* toNDArray(const cv::Mat& mat);
};
