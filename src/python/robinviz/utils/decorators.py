# -*- coding: utf-8 -*-
def injectArguments(inFunction):
    def outFunction(*args,**kwargs):
        _self = args[0]
        _self.__dict__.update(kwargs)
        inFunction(*args,**kwargs)
    return outFunction
