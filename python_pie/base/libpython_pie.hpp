#pragma once

#include <libpython_pie.h>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <vector>

#include <error.h>
#include <gavc_extension.h>
#include <cache_extension.h>
#include <upload_extension.h>

#include <commands/base_errors.h>

////////////////////////////////////////////////////////////////////////////////
int version() {
    return MODULE_VERSION;
}

////////////////////////////////////////////////////////////////////////////////
void translate_exception(std::exception const& e)
{
    auto msg = piel::cmd::errors::format_exceptions_stack(e);
    PyErr_SetString(PyExc_RuntimeError, msg.c_str());
}

////////////////////////////////////////////////////////////////////////////////
BOOST_PYTHON_MODULE(MODULE_NAME)
{
    using namespace boost::python;

    register_exception_translator<std::exception>(&translate_exception);

    class_<std::vector<std::string> >("StringVec")
        .def(vector_indexing_suite<std::vector<std::string> >());

    def("version", version);

    class_<Gavc>("Gavc")
        .def("set_param",   &Gavc::set_paramW, ( arg("param"), arg("value") ))
        .def("get_param",   &Gavc::get_paramW, ( arg("param"), arg("default_value") ), return_value_policy<copy_const_reference>())
        .def("perform",     &Gavc::performW,   ( arg("gavc") ))
        .def("versions",    &Gavc::versions)
        ;

    class_<Cache>("Cache")
        .def("set_param",   &Cache::set_paramW, ( arg("param"), arg("value") ))
        .def("get_param",   &Cache::get_paramW, ( arg("param"), arg("default_value") ), return_value_policy<copy_const_reference>())
        .def("init",        &Cache::init)
        .def("clean",       &Cache::clean,     ( arg("max_age") ))
        ;

    class_<Upload>("Upload")
        .def("set_param",   &Upload::set_paramW, ( arg("param"), arg("value") ))
        .def("get_param",   &Upload::get_paramW, ( arg("param"), arg("default_value") ), return_value_policy<copy_const_reference>())
        .def("perform",     &Upload::performW,   ( arg("gavc") ))
        ;
}
