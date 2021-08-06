#define PY_SSIZE_T_CLEAN
#include <Python.h> 
#include "solver.hpp"
#include "adapter.hpp"

PyObject* Solve(PyObject* self, PyObject* args)
{
    const char* rawTask;
    if (!PyArg_ParseTuple(args, "s", &rawTask))
        Py_RETURN_NONE;

    auto task = json::parse(rawTask).get<Task>();
    bool solvable = puzzle::Solver<5, 5>::Solvable(task.board);
    auto steps = solvable ?
        puzzle::Solver<5, 5>::Solve(task.board, task.steps, task.maxSteps) :
        std::nullopt;
    auto result = json(steps).dump();
    return PyUnicode_FromString(result.c_str());
}

PyObject* GenerateTasks(PyObject* self, PyObject* args)
{
    const char* rawTask;
    int preferredCount;
    if (!PyArg_ParseTuple(args, "si", &rawTask, &preferredCount))
        Py_RETURN_NONE;

    auto task = json::parse(rawTask).get<Task>();
    if (!puzzle::Solver<5, 5>::Solvable(task.board))
        return PyUnicode_FromString("[]");
    auto tasks = puzzle::Solver<5, 5>::GenerateTasks(task.board, task.steps, preferredCount);
    return PyUnicode_FromString(json(tasks).dump().c_str());
}

static PyMethodDef methods[] =
{
    {
        "GenerateTasks",
        &GenerateTasks,
        METH_VARARGS,
        NULL
    },
    {
        "Solve",
        &Solve,
        METH_VARARGS,
        NULL
    },
    {
        NULL, NULL, 0, NULL
    }
};

static PyModuleDef module =
{
    PyModuleDef_HEAD_INIT,
    "func",
    NULL,
    -1,
    methods
};

PyMODINIT_FUNC
PyInit_func()
{
    return PyModule_Create(&module);
}