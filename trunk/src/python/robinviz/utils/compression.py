# -*- coding: utf-8 -*-
"""Receives parameter from the command line, downloads if that's an URL and/or extracts if it's tar.gz or gz."""
import gzip
import urllib
import sys
import os
import tarfile
import shutil
import zipfile
from PyQt4.QtCore import QString
from urllib2 import Request, urlopen, URLError, HTTPError

def download_file2(url):
    filename = url.split('/')[-1]
    print "Downloading", url
    
    req = Request(url)
    try:
        response = urlopen(req)
    except HTTPError, e:
        message = 'The server couldn\'t fulfill the request.\nError code: %d' % e.code
        return QString(message)
    except URLError, e:
        message = 'We failed to reach a server.\n+Reason: %s' % e.reason
        return QString(message)
    else:
        localFile = open(filename, 'w')
        localFile.write(response.read())
        response.close()
        localFile.close()
        return str(filename)

def download_file(url, binary=False):
    filename = url.split('/')[-1]
    print "Downloading", url
    webFile = urllib.urlopen(url)

    if binary:
        mode = 'wb'
    else:
        mode = 'w'
    localFile = open(filename, mode)
    localFile.write(webFile.read())
    webFile.close()
    localFile.close()
    return filename

def download_file_to(url, path):
    filename = download_file(url)
    shutil.move(filename, path)
    return filename
    
def ungz(filename):
    new_filename = ".".join(filename.split('.')[:-1])
    print "Extracting",filename, "as", new_filename    
    
    f_in = gzip.open(filename, 'rb')
    f_out = open(new_filename, 'wb')
    f_out.writelines(f_in)
    f_out.close()
    f_in.close()
    os.remove(filename)
    return new_filename
    
def untar(filename):
    if not filename: return
    new_filename = ".".join(filename.split('.')[:-2])
    
    tar = tarfile.open(filename, 'r:gz')
    names = tar.getnames()
    
    for item in tar:
	tar.extract(item)
    print 'Extracted.'
    
    return names


def download_targz(url):
    
    filename = download_file(url, binary=True)
    if filename:
	extracted_files = untar(filename)
    else:
	print "Could not download"
    
    # The file extracted
    return extracted_files
    
def without_targz(filename):
    return ".".join(filename.split(".")[:-2])

def unzip(zipname):
    zf = zipfile.ZipFile(zipname)
    zf.extractall()
    print "Extracted."

def unzip_file_into_dir(file, dir):
    os.mkdir(dir, 0777)
    zfobj = zipfile.ZipFile(file)
    for name in zfobj.namelist():
        if name.endswith('/'):
            os.mkdir(os.path.join(dir, name))
        else:
            outfile = open(os.path.join(dir, name), 'wb')
            outfile.write(zfobj.read(name))
            outfile.close()


def handle_param(param):
    if "://" in param:
	download_file(param)
	filename = param.split("/")[-1] # get the part after last / =>> filename
    else:
	filename = param

    new_filename = None
    
    # got the file
    if filename.endswith(".tar.gz"):
	untar(filename)
	os.remove(filename)
    elif filename.endswith(".gz"):
	new_filename = ungz(filename)
	os.remove(filename)
    elif filename.endswith(".zip"):
	unzip(filename)
    else:
	print "File with unknown extension:", filename
	new_filename = ungz(filename)
    return new_filename
	
	
if __name__ == "__main__":
    if len(sys.argv) == 2:
	# we have filename
	param = sys.argv[1]
	new_filename = handle_param(param)
    elif len(sys.argv) == 3:
	param = sys.argv[1]
	new_filename = handle_param(param)
	shutil.move(new_filename, sys.argv[2])
	    
    else:
	print "Usage:"
	print "python ungz.py http://www.site.com/file.tar.gz [/home/emre/Desktop]"
	print "python ungz.py file.tar.gz [/home/emre/Desktop]"



    #download_file("http://hintdb.hgc.jp/htp/download/4932_hc_interactions.tar.gz")
    #untar("4932_hc_interactions.tar.gz")
    #ungz("test.gz")
