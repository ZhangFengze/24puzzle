#define PY_SSIZE_T_CLEAN
#include <Python.h> 
#include "solver.hpp"
#include "adapter.hpp"

PyObject* ToPy(const puzzle::Solver<5, 5>::Board& board)
{
    auto list = PyList_New(0);
    for (const auto& position : board.board)
        PyList_Append(list, PyLong_FromLong(position.index));
    return list;
}

PyObject* ToPy(const std::vector<puzzle::Direction>& vec)
{
    auto list = PyList_New(0);
    for (auto dir : vec)
        PyList_Append(list, PyLong_FromLong((long)dir));
    return list;
}

PyObject* ToPy(const puzzle::Solver<5, 5>::Task& task)
{
    auto dict = PyDict_New();
    PyDict_SetItem(dict, PyUnicode_FromString("board"), ToPy(task.board));
    PyDict_SetItem(dict, PyUnicode_FromString("steps"), ToPy(task.steps));
    return dict;
}

PyObject* GenerateTasks(PyObject* self, PyObject* args)
{
    const char* rawTask;
    int preferredCount;
    if (!PyArg_ParseTuple(args, "si", &rawTask, &preferredCount))
        Py_RETURN_NONE;

    auto task = ToTask(rawTask);
    if (!puzzle::Solver<5, 5>::Solvable(task.board))
        return PyList_New(0);
    auto tasks = puzzle::Solver<5, 5>::GenerateTasks(task.board, task.steps, preferredCount);

    auto result = PyList_New(0);
    for (const auto& task : tasks)
        PyList_Append(result, ToPy(task));

    return result;
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