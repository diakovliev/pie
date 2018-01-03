#ifndef PIEL_TEST_UTILS_HPP
#define PIEL_TEST_UTILS_HPP

#include <boost_filesystem_ext.hpp>

#include <map>
#include <sstream>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

namespace piel { namespace lib { namespace test_utils {

// Test utils
inline std::string generate_random_string()
{
    boost::random::random_device rng;
    boost::random::uniform_int_distribution<> index_dist(5, 256);
    int count = index_dist(rng);
    std::ostringstream oss;
    for(int i = 0; i < count; ++i) {
        std::string::value_type ch = (std::string::value_type)index_dist(rng);
        oss << ch;
    }
    return oss.str();
}

inline std::string generate_random_printable_string(unsigned min_size = 5, unsigned max_size = 256)
{
    boost::random::random_device rng;
    std::string ch_vector = "123456789qwertyuiopasdfghjklzxcvbnm";
    boost::random::uniform_int_distribution<> index_dist(min_size, max_size);
    boost::random::uniform_int_distribution<> vector_dist(0, ch_vector.size() -1);
    int count = index_dist(rng);
    std::ostringstream oss;
    for(int i = 0; i < count; ++i) {
        std::string::value_type ch = ch_vector[vector_dist(rng)];
        oss << ch;
    }
    return oss.str();
}

struct TempFileHolder: public std::pair<boost::filesystem::path,std::string> {
    typedef boost::shared_ptr<TempFileHolder> Ptr;
    TempFileHolder(const boost::filesystem::path& path, const std::string& content)
        : std::pair<boost::filesystem::path,std::string>(std::make_pair(path, content))
    {
    }
    ~TempFileHolder()
    {
        if (exists(first)) remove(first);
    }
};

inline TempFileHolder::Ptr create_random_temp_file(unsigned min_size = 5, unsigned max_size = 256)
{
    boost::filesystem::path temp_file = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::random::random_device rng;
    boost::random::uniform_int_distribution<> index_dist(min_size, max_size);
    int count = index_dist(rng);
    boost::shared_ptr<std::ostream> ofs = boost::filesystem::ostream(boost::filesystem::path(temp_file));
    std::ostringstream oss;
    for(int i = 0; i < count; ++i) {
        std::string::value_type ch = (std::string::value_type)index_dist(rng);
        (*ofs) << ch;
        oss << ch;
    }
    return TempFileHolder::Ptr(new TempFileHolder(temp_file, oss.str()));
}

inline std::map<std::string,std::string> generate_unique_map()
{

}

} } } // namespace piel::lib::test_utils

#endif // PIEL_TEST_UTILS_HPP
