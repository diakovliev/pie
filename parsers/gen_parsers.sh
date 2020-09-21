#! /bin/sh

gen_parser() {
    echo "RAGEL $1.rl -> $1"
    ragel $1.rl -o $1
}

gen_parser gavc_query.cpp
gen_parser gavc_query_version.cpp
gen_parser gavc_query_versions_range.cpp
gen_parser uploadfilesspec.cpp
