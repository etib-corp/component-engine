#pragma once

#include <Python.h>

namespace python
{
    class Object
    {
    private:
        PyObject *_object;

    protected:
    public:
        /**
         * @brief Construct a new Python Object
         *
         */
        Object();

        /**
         * @brief Construct a new Python Object
         *
         * @param object A pointer to a Python object, the ownership is transferred to the new Object
         */
        Object(PyObject *object);

        /**
         * @brief Destroy the Python Object
         *
         */
        ~Object();
    };
} // namespace python
