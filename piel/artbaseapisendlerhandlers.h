#ifndef ARTBASEAPISENDLERHANDLERS_H
#define ARTBASEAPISENDLERHANDLERS_H
#include <artbaseapihandlers.h>
#include <boost_property_tree_ext.hpp>
#include "streamssequencepartitionallyoutputhelper.h"

namespace art { namespace lib {

class ArtBaseApiSendlerHandlers : public ArtBaseApiHandlers
{
    typedef std::map<std::string, std::string> Attributes;
public:
    ArtBaseApiSendlerHandlers(const std::string& api_token);
    ArtBaseApiSendlerHandlers(const std::string& api_token, const std::string& uri, const std::string& repo, const std::string& path);
    virtual ~ArtBaseApiSendlerHandlers(){}

    virtual size_t handle_input(char *ptr, size_t size);
    virtual size_t handle_output(char *ptr, size_t size);

    virtual boost::shared_ptr<std::istream> prepare_header();

    virtual void set_url(const std::string& url);
    virtual void set_repo(const std::string& repo);
    virtual void set_path(const std::string& path);

    virtual std::string get_url() {  return url_; }
    virtual std::string get_repo() { return repo_; }
    virtual std::string get_path() { return  path_; }

    virtual std::string gen_uri();

    virtual void gen_additional_tree(boost::property_tree::ptree &) {}

    void update_attributes(const std::string& key, const std::string& value);
    void update_attributes(const std::string& key, const char* value);

    void push_input_stream(boost::shared_ptr<std::istream> is);
    size_t putto(char* ptr, size_t size);
protected:
    std::string trim(const std::string& src);

private:
    boost::property_tree::ptree tree_;
    StreamsSequencePartitionallyOutputHelper uploader_;
    //Attributes  attributes_;
    size_t      send_size_;
    std::stringstream os_;
    Attributes  answer_;
    std::string url_;
    std::string repo_;
    std::string path_;

    bool    first_call_;
};

} } // namespace art::lib

#endif // ARTBASEAPISENDLERHANDLERS_H
