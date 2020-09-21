/*
 * Copyright (c) 2017-2018
 *
 *  Dmytro Iakovliev daemondzk@gmail.com
 *  Oleksii Kogutenko https://github.com/oleksii-kogutenko
 *
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

#define BOOST_TEST_MODULE UploadFileSpec
#include <boost/test/unit_test.hpp>

#include <logging.h>
#include <uploadfilesspec.h>

using namespace art::lib;

const int files_spec_data_items = 3;

typedef const std::string CheckItemArray[files_spec_data_items];
typedef const std::string CheckStringArray[][files_spec_data_items];

std::string get_str_from_files_spec_data(const ufs::files_spec_data& data, int i){
    switch (i) {
    case 0 : return data.classifier ? (*data.classifier) : "";
    case 1 : return data.extension ? (*data.extension) : "";
    case 2 : return data.file_name;
    }
    return "";
}

bool check_files_spec_data(const ufs::files_spec_data& data, CheckItemArray check_data)
{
    bool ret_val = true;
    for(int i = 0; (i < 3) && ret_val; i++) {
        ret_val &= get_str_from_files_spec_data(data, i) == check_data[i];
        if (!ret_val) {
            LOGE << "[" << i << "] value " << get_str_from_files_spec_data(data, i) << " != " << check_data[i] << ELOG;
        }
    }
    return ret_val;
}

bool check_UploadFileSpec(UploadFileSpec& s, CheckStringArray check_data)
{
    bool ret_val = true;
    ufs::UFSVector data = s.get_data();

    int i = 0;
    for (ufs::UFSVector::const_iterator it = data.begin(), end = data.end(); it != end; ++it, i++)
    {
        ret_val &= check_files_spec_data(*it, check_data[i]);
    }
    return ret_val;
}

static const std::string parse2_1 = "class2_1_1.ext2_1_1:file2_1_1,class2_1_2:file2_1_2,.ext2_1_3:file2_1_3,:file2_1_4";
static CheckStringArray check_result2_1 = {
    {"class2_1_1", "ext2_1_1", "file2_1_1"},
    {"class2_1_2", "", "file2_1_2"},
    {"", "ext2_1_3", "file2_1_3"},
    {"", "", "file2_1_4"}
};

static const std::string parse2_2 = "class2_2_1.ext2_2_1:file2_2_1";
static CheckStringArray check_result2_2 = {
    {"class2_2_1", "ext2_2_1", "file2_2_1"},
};

static const std::string parse2_3 = "class2_3_1.ext2_3_1:"; /// Must fail!
static const std::string parse2_4 = "";  /// Must fail!

BOOST_AUTO_TEST_CASE(UploadFileSpec_2)
{
    LOGT << "UploadFileSpec test 2" << ELOG;

    LOGT << "test case -- 1 --" << ELOG;
    std::optional<UploadFileSpec> result1 = UploadFileSpec::parse(parse2_1);
    BOOST_CHECK(result1);
    LOGI << "1:" << result1->to_string() << ELOG;
    BOOST_CHECK(check_UploadFileSpec(*result1, check_result2_1));

    LOGT << "test case -- 2 --" << ELOG;
    std::optional<UploadFileSpec> result2 = UploadFileSpec::parse(parse2_2);
    BOOST_CHECK(result2);
    LOGI << "2:" << result2->to_string() << ELOG;
    BOOST_CHECK(check_UploadFileSpec(*result2, check_result2_2));

    LOGT << "test case -- 3 --" << ELOG;
    std::optional<UploadFileSpec> result3 = UploadFileSpec::parse(parse2_3);
    BOOST_CHECK(!result3);

    LOGT << "test case -- 4 --" << ELOG;
    std::optional<UploadFileSpec> result4 = UploadFileSpec::parse(parse2_4);
    BOOST_CHECK(!result4);

}
