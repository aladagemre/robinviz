# -*- mode: python -*-
################################################################
# funcassociate/client.py
#
# Copyright (c) 2009 President and Fellows of Harvard College
# and Gabriel F. Berriz (gberriz@hms.harvard.edu).  All rights
# reserved.
################################################################

import re
import sys
import time
import signal
import socket
from inspect import getargspec, getargvalues, currentframe

import httplib

try:
    import json
except ImportError:
    import simplejson as json

"""
A client class for the Funcassociate web service.

http://llama.med.harvard.edu/cgi/funcassociate/serv
"""

class ServiceError(Exception):
    """
    Exception raised when the response from server includes a non-null
    error.
    """

class NetworkError(ServiceError):
    """
    Exception raised when server fails to respond to a ping request.
    """
    
class TimeoutError(ServiceError):
    """
    Exception raised when response from the server fails to arrive
    within the prescribed time interval.
    """

class InputError(Exception):
    """
    Exception raised by functionate method upon detecting bad inputs
    from user.
    """

class _fc(object):
    host = 'llama.med.harvard.edu'
    url = '/cgi/funcassociate/serv'
    headers = {
     'Content-type': 'application/json',
    }
    timeout = 180

    def __init__(self, host=host, url=url, timeout=timeout):
        self.host = re.sub("\Ahttps?://", "", host)
        self.url = re.sub("\Ahttps?://[^/]+/", "", url)

        self.timeout = 30
        try:
            if self.ping() != "OK":
                raise ValueError()
        except (socket.gaierror, ValueError):
            raise NetworkError("No response from %s%s" % (host, url))

        try:
            self.timeout = int(timeout)
            if self.timeout <= 0: raise Exception()
        except:
            raise ValueError("timeout parameter must be a positive number")

        return

    @staticmethod
    def _timeout(signo, frame):
        raise TimeoutError()
    
    def _make_request(self, method, params=[]):
        """
        Make a request to the Funcassociate web service.

        @return: result
        """
        conn = httplib.HTTPConnection(self.host)
        payload = json.dumps({'method': method, 'params': params, 'id': 0})

        conn.request("POST", self.url, body=payload,
                     headers=_fc.headers)


	response = conn.getresponse()

        conn.close() 

	response_data = response.read()

        decoded_response = json.loads(response_data)
            
        if decoded_response['error']:
            raise ServiceError(str(decoded_response['error']['message']))
        return decoded_response['result']

    @staticmethod
    def _check_args(f, given):
        spec = getargspec(f)
        formals = spec[0]
        defaults = spec[3]
        # we disregard the first argument, self; hence the decrement
        # by 1 below
        max_ = len(formals) - 1
        min_ = max_ if defaults is None else max_ - len(defaults)
        qual = None

        if given < min_:
            if max_ > min_:
                qual = "at least %d" % min_
            else:
                qual = "exactly %d" % min_
            s = "s" if min_ > 1 else ""
        elif given > max_:
            if max_ > min_:
                qual = "at most %d" % max_
            elif max_ > 0:
                qual = "exactly %d" % max_
            else:
                qual = "no"
            s = "" if max_ == 1 else "s"
        
        if qual is not None:
            raise TypeError("%s() takes %s "
                            "argument%s (%d given)" %
                            (f.__name__, qual, s, given))

    def _decorate(f):
        def wrapper(self, *args):
            _fc._check_args(f, len(args))
            return self._make_request(f.__name__, args)
        return wrapper

    @_decorate
    def available_species(self):
        """
        @return: List of species supported by Funcassociate.
        service.
        """

    @_decorate
    def available_namespaces(self, species):
        """
        @return: List of namespaces supported by Funcassociate for the
        given species.
        """

    @_decorate
    def go_associations(self, species, namespace, support=None):
        """
        @return: List of GO associations used by Funcassociate
        for the specified species, namespace, and support
        """

    @_decorate
    def go_attrib_dict(self):
        """
        @return: Dictionary whose keys are GO attribute IDs and whose
        values are their corresponding human-readable names.
        """

    @_decorate
    def version(self):
        """
        @return: Version of Funcassociate server.
        """

    @_decorate
    def ping(self):
        """
        @return: The string "OK".
        """

    @_decorate
    def fail(self):
        """
        @return: nothing.
        """

    def functionate(self, query=None, associations=None,
                    attrib_dict=None, species=None, namespace=None,
                    genespace=None, mode=None, which=None,
                    cutoff=None, reps=None):
        """
        @return: functionate results structure
        """
        params = locals()
        del params['self']
        keys = params.keys()
        for k in keys:
            if params[k] is None:
                del params[k]
        result = self._make_request('functionate', [params])

        if result.has_key("input_error"):
            raise InputError(result["input_error"])

        return result
fc = _fc



if '__main__' == __name__:
	if len(sys.argv) < 3:
		print "Insufficient arguments."
		print "Usage: python geneq.py inputfile outputfile"
		print
		sys.exit(-1)

	input_file = sys.argv[1]
	output_file = sys.argv[2]

	sample = ("YBL071W-A", "YCL055W", "YCR094W",
									"YDL127W", "YFL029C", "YGR271C-A",
									"YHR099W", "YJR066W", "YKL203C",
									"YNL289W")
	query_list = []
	f = open(input_file)
	
	
	for line in f:
		line = line.strip("\t")
		line = line.strip()
		line = line.replace('"', '')
		
		if "," in line:
			line = line[:line.rfind(",")]
		
		if line:
			l = line.split(",")			
			query_list.extend(l)

	print query_list
	#sys.exit(-1)
	reps = 1500
	c = fc()
	response = c.functionate(query=query_list,
							 species="Saccharomyces cerevisiae",
							 namespace="sgd_systematic",
							 cutoff=0.05,
							 reps=reps)

	if response:		
		g = open(output_file, "w")
		for row in response["over"]:
			row.pop(1)
			print row[4]
			g.write(str(row[4])+"\n")
			
		
		g.close()
	else:
		log = open("debug.log", "a")
		log.write("Unsuccessful: %s -> %s" % (input_file, output_file))
		print "Unsuccessful response!"
