import os
import unittest

import libpython3_pie as lib

################################################################################
class GavcParams:
    PARAM_TOKEN            = "token";
    PARAM_TOKEN_ENV        = "GAVC_SERVER_API_ACCESS_TOKEN";

    PARAM_SERVER           = "server";
    PARAM_SERVER_ENV       = "GAVC_SERVER_URL";

    PARAM_REPOSITORY       = "repository";
    PARAM_REPOSITORY_ENV   = "GAVC_SERVER_REPOSITORY";

    PARAM_CACHE_PATH       = "cache-path";
    PARAM_CACHE_PATH_ENV   = "GAVC_CACHE";

    # Gavc
    PARAM_DOWNLOAD         = "download";
    PARAM_DELETE           = "delete";
    PARAM_OUTPUT           = "output";
    PARAM_DISABLE_CACHE    = "disable-cache";
    PARAM_MAX_ATTEMPTS     = "max-attempts";
    PARAM_RETRY_TIMEOUT    = "retry-timeout";
    PARAM_FORCE_OFFLINE    = "force-offline";

    # Upload
    PARAM_FILELIST         = "filelist"

################################################################################
class TestLib(unittest.TestCase):

    CACHE_PATH = ".test_cache"

    @classmethod
    def setUp(self):
        cache = lib.Cache()
        cache.set_param(GavcParams.PARAM_CACHE_PATH, self.CACHE_PATH)
        cache.init()

    def test_version(self):
        self.assertEqual(1, lib.version())

    def test_unsupported_params(self):
        gavc = lib.Gavc()

        self.assertNotEqual(0, gavc.set_param("param1", "value1"))

        with self.assertRaises(RuntimeError):
            gavc.get_param("param1", "value1")

    def test_params_from_env(self):

        gavc = lib.Gavc()

        # print("! %s" % gavc.get_param(GavcParams.PARAM_TOKEN, ""))
        # print("! %s" % gavc.get_param(GavcParams.PARAM_SERVER, ""))
        # print("! %s" % gavc.get_param(GavcParams.PARAM_REPOSITORY, ""))
        # print("! %s" % gavc.get_param(GavcParams.PARAM_CACHE_PATH, ""))

        self.assertEqual(os.environ.get(GavcParams.PARAM_TOKEN_ENV, ""),                gavc.get_param(GavcParams.PARAM_TOKEN, ""))
        self.assertEqual(os.environ.get(GavcParams.PARAM_SERVER_ENV, ""),               gavc.get_param(GavcParams.PARAM_SERVER, ""))
        self.assertEqual(os.environ.get(GavcParams.PARAM_REPOSITORY_ENV, ""),           gavc.get_param(GavcParams.PARAM_REPOSITORY, ""))
        # self.assertEqual(os.environ.get(GavcParams.PARAM_CACHE_PATH_ENV, ""),           gavc.get_param(GavcParams.PARAM_CACHE_PATH, ""))

    def test_basic_query(self):

        gavc = lib.Gavc()

        self.assertEqual(0, gavc.set_param(GavcParams.PARAM_CACHE_PATH, self.CACHE_PATH))
        self.assertEqual(0, gavc.set_param(GavcParams.PARAM_DOWNLOAD, "1"))
        self.assertEqual(0, gavc.perform("zodiac.aosp.oemsdk.release:aosp:+"))

        for v in gavc.versions():
            print(" -- version: %s" % v)

    def test_versions_query(self):

        gavc = lib.Gavc()

        self.assertEqual(0, gavc.set_param(GavcParams.PARAM_CACHE_PATH, self.CACHE_PATH))
        self.assertEqual(0, gavc.perform("zodiac.aosp.oemsdk.release:aosp:*"))

        for v in gavc.versions():
            print(" -- version: %s" % v)

    def test_cache_clean(self):

        cache = lib.Cache()

        cache.set_param(GavcParams.PARAM_CACHE_PATH, self.CACHE_PATH)

        self.assertEqual(0, cache.clean(3))

    def test_upload(self):
        upload = lib.Upload()

        upload.set_param(GavcParams.PARAM_FILELIST, "cmake_install:cmake_install.cmake,makefile:Makefile")

        self.assertEqual(0, upload.perform("test_upload:test_upload:1"))

print("Version: %d" % lib.version())
unittest.main(verbosity = 5)
