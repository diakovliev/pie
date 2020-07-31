#include <libpython_pie.h>

#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <vector>
#include <set>
#include <string>

#include <gavc_extension.h>
#include <cache_extension.h>
#include <upload_extension.h>

////////////////////////////////////////////////////////////////////////////////
int version() {
    return LIBPYTHON_PIE_VERSION;
}

////////////////////////////////////////////////////////////////////////////////
void translate_exception(std::exception const& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

////////////////////////////////////////////////////////////////////////////////
/*static*/ bool Utils::string2bool(std::string value) {
    static std::set<std::string> negative_values {
        "",
        "0",
        "false",
        "False",
    };

    if (value.empty()) return false;

    return negative_values.find(value) == negative_values.end();
}

////////////////////////////////////////////////////////////////////////////////
BOOST_PYTHON_MODULE(libpython3_pie)
{
    using namespace boost::python;

    register_exception_translator<Error>(&translate_exception);

    class_<std::vector<std::string> >("StringVec")
        .def(vector_indexing_suite<std::vector<std::string> >());

    def("version", version);

    class_<Gavc>("Gavc")
        .def("set_param",   &Gavc::set_param, args("param", "value"))
        .def("get_param",   &Gavc::get_param, args("param", "default_value"), return_value_policy<copy_const_reference>())
        .def("perform",     &Gavc::perform,   args("gavc"))
        .def("versions",    &Gavc::versions)
        ;

    class_<Cache>("Cache")
        .def("set_param",   &Cache::set_param, args("param", "value"))
        .def("get_param",   &Cache::get_param, args("param", "default_value"), return_value_policy<copy_const_reference>())
        .def("init",        &Cache::init)
        .def("clean",       &Cache::clean,     args("max_age"))
        ;

    class_<Upload>("Upload")
        .def("set_param",   &Upload::set_param, args("param", "value"))
        .def("get_param",   &Upload::get_param, args("param", "default_value"), return_value_policy<copy_const_reference>())
        .def("perform",     &Upload::perform,   args("gavc"))
        ;
}
