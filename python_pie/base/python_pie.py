import sys

if sys.version_info.major == 2:
    import libpython2_pie as nlib
elif sys.version_info.major == 3:
    import libpython3_pie as nlib
else:
    raise ImportError()

################################################################################
class Base:
    __PARAM_TOKEN            = "token"
    __PARAM_SERVER           = "server"
    __PARAM_REPOSITORY       = "repository"
    __PARAM_CACHE_PATH       = "cache-path"

    def __init__(self, obj):
        self._obj = obj

    def _set_param(self, param, value):
        self._obj.set_param(param, value)

    def _get_param(self, param, default_value = ""):
        self._obj.get_param(param, default_value)


    def set_token(self, token):
        return self._set_param(self.__PARAM_TOKEN, token)

    def get_token(self):
        return self._get_param(self.__PARAM_TOKEN)


    def set_server(self, server):
        return self._set_param(self.__PARAM_SERVER, server)

    def get_server(self):
        return self._get_param(self.__PARAM_SERVER)


    def set_repository(self, repository):
        return self._set_param(self.__PARAM_REPOSITORY, repository)

    def get_repository(self):
        return self._get_param(self.__PARAM_REPOSITORY)


    def set_cache_path(self, cache_path):
        return self._set_param(self.__PARAM_CACHE_PATH, cache_path)

    def get_cache_path(self):
        return self._get_param(self.__PARAM_CACHE_PATH)


################################################################################
class Gavc(Base):
    __PARAM_DOWNLOAD         = "download"
    __PARAM_DELETE           = "delete"
    __PARAM_DELETE_VERSIONS  = "delete-versions"
    __PARAM_OUTPUT           = "output"
    __PARAM_DISABLE_CACHE    = "disable-cache"
    __PARAM_MAX_ATTEMPTS     = "max-attempts"
    __PARAM_RETRY_TIMEOUT    = "retry-timeout"
    __PARAM_FORCE_OFFLINE    = "force-offline"

    def __init__(self, no_ops_exceptions = True):
        Base.__init__(self, nlib.Gavc())

        self.__no_ops_exceptions    = no_ops_exceptions

    def version(self, gavc):
        self.perform(gavc)

        result = self._obj.versions()

        if result:
            return result[0]
        else:
            return None

    def perform(self, gavc):
        ret = 1
        try:
            ret = self._obj.perform(gavc)
        except Exception as e:
            if not self.__no_ops_exceptions:
                raise e
        return ret


    def set_download(self, download):
        return self._set_param(self.__PARAM_DOWNLOAD, download)

    def get_download(self):
        return self._get_param(self.__PARAM_DOWNLOAD)


    def set_delete(self, delete):
        return self._set_param(self.__PARAM_DELETE, delete)

    def get_delete(self):
        return self._get_param(self.__PARAM_DELETE)


    def set_delete_versions(self, delete):
        return self._set_param(self.__PARAM_DELETE_VERSIONS, delete)

    def get_delete_versions(self):
        return self._get_param(self.__PARAM_DELETE_VERSIONS)


    def set_output(self, output):
        return self._set_param(self.__PARAM_OUTPUT, output)

    def get_output(self):
        return self._get_param(self.__PARAM_OUTPUT)


    def set_disable_cache(self, disable_cache):
        return self._set_param(self.__PARAM_DISABLE_CACHE, disable_cache)

    def get_disable_cache(self):
        return self._get_param(self.__PARAM_DISABLE_CACHE)


    def set_max_attempts(self, max_attempts):
        return self._set_param(self.__PARAM_MAX_ATTEMPTS, max_attempts)

    def get_max_attempts(self):
        return self._get_param(self.__PARAM_MAX_ATTEMPTS)


    def set_retry_timeout(self, retry_timeout):
        return self._set_param(self.__PARAM_RETRY_TIMEOUT, retry_timeout)

    def get_retry_timeout(self):
        return self._get_param(self.__PARAM_RETRY_TIMEOUT)


    def set_force_offline(self, force_offline):
        return self._set_param(self.__PARAM_FORCE_OFFLINE, force_offline)

    def get_force_offline(self):
        return self._get_param(self.__PARAM_FORCE_OFFLINE)


    def download(self, output_file, gavc):
        self.set_output(output_file)
        self.set_download("1")
        # print("gavc: %s" % repr(gavc.encode('ascii', 'ignore')))
        return self.perform(gavc.encode('ascii', 'ignore'))

################################################################################
class Cache(Base):
    def __init__(self):
        Base.__init__(self, nlib.Cache())

    def init(self):
        return self._obj.init()

    def clean(self, max_age = 5):
        return self._obj.clean(max_age)


################################################################################
class Upload(Base):
    __PARAM_FILELIST         = "filelist"

    def __init__(self):
        Base.__init__(self, nlib.Upload())

    def perform(self, target):
        return self._obj.perform(target)


    def set_filelist(self, filelist):
        return self._set_param(self.__PARAM_FILELIST, filelist)

    def get_filelist(self):
        return self._get_param(self.__PARAM_FILELIST)


################################################################################
if __name__ == "__main__":
    import os
    import unittest

    class TestLib(unittest.TestCase):
        pass

    print("Version: %d" % nlib.version())
    unittest.main(verbosity = 5)
