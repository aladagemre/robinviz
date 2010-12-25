# -*- coding: utf-8 -*-
def injectArguments(inFunction):
    def outFunction(*args,**kwargs):
        _self = args[0]
        _self.__dict__.update(kwargs)
        inFunction(*args,**kwargs)
    return outFunction

def onerror(func, path, exc_info):
    """
    Taken from pythonutils
    Error handler for ``shutil.rmtree``.

    If the error is due to an access error (read only file)
    it attempts to add write permission and then retries.

    If the error is for another reason it re-raises the error.

    Usage : ``shutil.rmtree(path, onerror=onerror)``
    """
    import stat
    if not os.access(path, os.W_OK):
        # Is the error an access error ?
        os.chmod(path, stat.S_IWUSR)
        func(path)
    else:
        raise