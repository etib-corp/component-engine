#include <stdexcept>

#include "python/object.hpp"

python::Object::Object() : _object(nullptr)
{
    Py_Initialize();
}

python::Object::Object(PyObject *object)
    : _object(object)
{
    Py_Initialize();

    if (!_object)
        throw std::runtime_error("Failed to create Python object");
}

python::Object::~Object()
{
    if (_object)
        Py_XDECREF(_object);
    Py_Finalize();
}