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

#define BOOST_TEST_MODULE MavenMetadataTest
#include <boost/test/unit_test.hpp>

#include <mavenmetadata.h>
#include <sstream>

using namespace art::lib;

std::string test_metadata = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<metadata modelVersion=\"1.1.0\">\n"
"  <groupId>adk.trunk</groupId>\n"
"  <artifactId>adk</artifactId>\n"
"  <version>63</version>\n"
"  <versioning>\n"
"    <latest>61</latest>\n"
"    <release>62</release>\n"
"    <versions>\n"
"      <version>45</version>\n"
"      <version>46</version>\n"
"      <version>47</version>\n"
"      <version>48</version>\n"
"      <version>49</version>\n"
"      <version>50</version>\n"
"      <version>51</version>\n"
"      <version>52</version>\n"
"      <version>53</version>\n"
"      <version>56</version>\n"
"      <version>57</version>\n"
"      <version>58</version>\n"
"      <version>59</version>\n"
"      <version>60</version>\n"
"      <version>61</version>\n"
"      <version>62</version>\n"
"      <version>63</version>\n"
"    </versions>\n"
"    <lastUpdated>20171211011023</lastUpdated>\n"
"  </versioning>\n"
"</metadata>\n";

std::string test_metadata_no_group = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<metadata modelVersion=\"1.1.0\">\n"
"  <artifactId>adk</artifactId>\n"
"  <version>63</version>\n"
"  <versioning>\n"
"    <latest>61</latest>\n"
"    <release>62</release>\n"
"    <versions>\n"
"      <version>45</version>\n"
"      <version>46</version>\n"
"      <version>47</version>\n"
"      <version>48</version>\n"
"      <version>49</version>\n"
"      <version>50</version>\n"
"      <version>51</version>\n"
"      <version>52</version>\n"
"      <version>53</version>\n"
"      <version>56</version>\n"
"      <version>57</version>\n"
"      <version>58</version>\n"
"      <version>59</version>\n"
"      <version>60</version>\n"
"      <version>61</version>\n"
"      <version>62</version>\n"
"      <version>63</version>\n"
"    </versions>\n"
"    <lastUpdated>20171211011023</lastUpdated>\n"
"  </versioning>\n"
"</metadata>\n";

BOOST_AUTO_TEST_CASE(ParseMavenMetadata)
{
    std::istringstream is(test_metadata);
    std::optional<MavenMetadata> op = MavenMetadata::parse(is);

    BOOST_CHECK(op);

    MavenMetadata m = *op;

    BOOST_CHECK_EQUAL("adk.trunk",      m.group());
    BOOST_CHECK_EQUAL("adk",            m.artifact());
    BOOST_CHECK_EQUAL("63",             m.version());

    BOOST_CHECK_EQUAL("61",             m.versioning().latest());
    BOOST_CHECK_EQUAL("62",             m.versioning().release());

    BOOST_CHECK_EQUAL("20171211011023", m.versioning().last_updated());

    std::optional<GavcQuery> op_q = GavcQuery::parse("adk.trunk:adk:*");
    std::vector<std::string>  versions = m.versions_for(*op_q);

    BOOST_CHECK_EQUAL(17,               versions.size());
    BOOST_CHECK_EQUAL("45",             versions[0]);
    BOOST_CHECK_EQUAL("46",             versions[1]);
    BOOST_CHECK_EQUAL("47",             versions[2]);
    BOOST_CHECK_EQUAL("48",             versions[3]);
    BOOST_CHECK_EQUAL("49",             versions[4]);
    BOOST_CHECK_EQUAL("50",             versions[5]);
    BOOST_CHECK_EQUAL("51",             versions[6]);
    BOOST_CHECK_EQUAL("52",             versions[7]);
    BOOST_CHECK_EQUAL("53",             versions[8]);
    BOOST_CHECK_EQUAL("56",             versions[9]);
    BOOST_CHECK_EQUAL("57",             versions[10]);
    BOOST_CHECK_EQUAL("58",             versions[11]);
    BOOST_CHECK_EQUAL("59",             versions[12]);
    BOOST_CHECK_EQUAL("60",             versions[13]);
    BOOST_CHECK_EQUAL("61",             versions[14]);
    BOOST_CHECK_EQUAL("62",             versions[15]);
    BOOST_CHECK_EQUAL("63",             versions[16]);

    std::optional<GavcQuery> op_q_1 = GavcQuery::parse("adk.trunk:adk:52");
    std::vector<std::string>  versions_1 = m.versions_for(*op_q_1);

    BOOST_CHECK_EQUAL(1,                versions_1.size());
    BOOST_CHECK_EQUAL("52",             versions_1[0]);

    std::optional<GavcQuery> op_q_2 = GavcQuery::parse("adk.trunk:adk:49");
    std::vector<std::string>  versions_2 = m.versions_for(*op_q_2);

    BOOST_CHECK_EQUAL(1,                versions_2.size());
    BOOST_CHECK_EQUAL("49",             versions_2[0]);

}

BOOST_AUTO_TEST_CASE(ParseMavenMetadata_NoGroup)
{
    std::istringstream is(test_metadata_no_group);
    std::optional<MavenMetadata> op = MavenMetadata::parse(is);

    BOOST_CHECK(!op);
}
