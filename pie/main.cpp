/*
 * Copyright (c) 2017, Dmytro Iakovliev daemondzk@gmail.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Dmytro Iakovliev daemondzk@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Dmytro Iakovliev daemondzk@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/log/trivial.hpp>
//#include <boost/format.hpp>

#include <fsindexer.h>
#include <zipindexer.h>
#include <baseindex.h>

#include <checksumdigestbuilder.hpp>
#include <curleacyclient.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

struct DownloadFileHandlers {
    DownloadFileHandlers(std::ostream& dest)
        : _dest(dest)
        , _checksums_builder()
    {
        _checksums_builder.init();
    }

    piel::lib::CurlEasyHandlers::headers_type custom_header()
    {
        return piel::lib::CurlEasyHandlers::headers_type();
    }

    size_t handle_header(char *ptr, size_t size)
    {
        return -1;
    }

    size_t handle_output(char *ptr, size_t size)
    {
        _dest.write(ptr, size);
        _checksums_builder.update(ptr, size);
        return size;
    }

    size_t handle_input(char *ptr, size_t size)
    {
        return -1;
    }

    piel::lib::MultiChecksumsDigestBuilder::StrDigests str_digests()
    {
        return _checksums_builder.finalize<piel::lib::MultiChecksumsDigestBuilder::StrDigests>();
    }

private:
    std::ostream& _dest;    //!< destination stream.
    piel::lib::MultiChecksumsDigestBuilder _checksums_builder;

};
template<> const bool piel::lib::CurlEasyHandlersTraits<DownloadFileHandlers>::have_custom_header   = false;
template<> const bool piel::lib::CurlEasyHandlersTraits<DownloadFileHandlers>::have_handle_header   = false;
template<> const bool piel::lib::CurlEasyHandlersTraits<DownloadFileHandlers>::have_handle_output   = true;
template<> const bool piel::lib::CurlEasyHandlersTraits<DownloadFileHandlers>::have_handle_input    = false;

struct ArtRestApiHandlers {

    typedef char char_type;

    piel::lib::CurlEasyHandlers::headers_type custom_header()
    {
        piel::lib::CurlEasyHandlers::headers_type result;
        result.push_back("X-JFrog-Art-Api:<api_token>");
        return result;
    }

    size_t handle_header(char *ptr, size_t size)
    {
        return -1;
    }

    size_t handle_output(char *ptr, size_t size)
    {

        BOOST_LOG_TRIVIAL(trace) << "buffer: " << std::string(ptr, size);

        std::vector<char_type> buffer(size);
        std::copy(ptr, ptr+size, buffer.begin());
        _output_buffers.push_back(buffer);

        return size;
    }

    size_t handle_input(char *ptr, size_t size)
    {
        // Prepare command parameters
        return -1;
    }

    std::istringstream &responce_stream()
    {
        std::string data;
        typedef std::list<std::vector<char_type> >::const_iterator Iter;
        for(Iter i = _output_buffers.begin(); i != _output_buffers.end(); ++i)
        {
            data.append((*i).begin(), (*i).end());
        }
        _stream = boost::shared_ptr<std::istringstream>(new std::istringstream(data));
        return *_stream.get();
    }

private:
    std::list<std::vector<char_type> > _output_buffers;
    boost::shared_ptr<std::istringstream> _stream;

};
template<> const bool piel::lib::CurlEasyHandlersTraits<ArtRestApiHandlers>::have_custom_header   = true;
template<> const bool piel::lib::CurlEasyHandlersTraits<ArtRestApiHandlers>::have_handle_header   = false;
template<> const bool piel::lib::CurlEasyHandlersTraits<ArtRestApiHandlers>::have_handle_output   = true;
template<> const bool piel::lib::CurlEasyHandlersTraits<ArtRestApiHandlers>::have_handle_input    = false;

int main(int argc, char **argv) {

    int result = -1;

//    if (argc < 2) {
//        return result;
//    }

    //piel::lib::FsIndexer indexer;
    //piel::lib::ZipIndexer indexer;
    //piel::lib::BaseIndex index = indexer.build(argv[1]);

    //if (!index.empty())
    //{
    //    result = 0;
    //}

//    std::ofstream out(argv[2], std::ofstream::out);
//    DownloadFileHandlers ostream_provider(out);
//    piel::lib::CurlEasyClient<DownloadFileHandlers> client(argv[1], &ostream_provider);
//    client.perform();

//    piel::lib::MultiChecksumsDigestBuilder::StrDigests str_digests = ostream_provider.str_digests();

//    BOOST_LOG_TRIVIAL(trace) << piel::lib::Sha256::t::name() << ": " << str_digests[piel::lib::Sha256::t::name()];
//    BOOST_LOG_TRIVIAL(trace) << piel::lib::Sha::t::name()    << ": " << str_digests[piel::lib::Sha::t::name()];
//    BOOST_LOG_TRIVIAL(trace) << piel::lib::Md5::t::name()    << ": " << str_digests[piel::lib::Md5::t::name()];

    // /api/search/gavc?[g=groupId][&a=artifactId][&v=version][&c=classifier][&repos=x[,y]]

    ArtRestApiHandlers apiHandlers;
    std::string url = "https://art.server.url/artifactory";
    url.append("/api/search/gavc");
    url.append("?g=test.group");
    url.append("&a=test");
    //url.append("&v=+");
    url.append("&c=classifier");
    url.append("&r=repository");
    piel::lib::CurlEasyClient<ArtRestApiHandlers> client(url, &apiHandlers);
    client.perform();

    // Short alias for this namespace
    namespace pt = boost::property_tree;

    // Create a root
    pt::ptree root;

    // Load the json file in this ptree
    pt::read_json(apiHandlers.responce_stream(), root);

    typedef pt::ptree::const_iterator Iter;

    //results = root.get_child("results");
    pt::ptree results = root.get_child("results");
    //root.get_value();

    for(Iter i = results.begin(); i != results.end(); ++i) {

        pt::ptree::value_type pair = (*i);

        //BOOST_LOG_TRIVIAL(trace) << std::string(pair.first) << ": " << std::string(pair.second.data());

        pt::ptree val = pair.second;

        for(Iter v = val.begin(); v != val.end(); ++v) {

            pt::ptree::value_type p = (*v);

            BOOST_LOG_TRIVIAL(trace) << std::string(p.first) << ": " << std::string(p.second.data());

        }
    }

    return result;
}
