# -*- coding: utf-8 -*-
"""Receives parameter from the command line, downloads if that's an URL and/or extracts if it's tar.gz or gz."""
import os.path
import gzip
import urllib
import sys
import os
import tarfile
import shutil
import zipfile
from PyQt4.QtCore import QString, QThread, QObject, pyqtSignal
from urllib2 import Request, urlopen, URLError, HTTPError
from functools import partial

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
    filename = url.split('/')[-1].split("?")[0]
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

def download_file_to(url, path, binary=False):
    filename = download_file(url, binary)
    shutil.move(filename, path)
    return filename
    
def ungz(filename, path=None):
    new_filename = ".".join(filename.split('.')[:-1])
    print "Extracting",filename, "as", new_filename    
    
    f_in = gzip.open(filename, 'rb')
    f_out = open(new_filename, 'wb')
    f_out.writelines(f_in)
    f_out.close()
    f_in.close()
    os.remove(filename)

    if path:
        shutil.move(new_filename, path)
        
    return new_filename
    
def untar(filename, path="."):
    if not filename: return    
    tar = tarfile.open(filename, 'r:gz')
    names = tar.getnames()

    tar.extractall(path=path)
    print 'Extracted.'
    
    return names

def compressdir(directory, tarname):
    tar = tarfile.open(str(tarname), "w:gz")
    tar.add(str(directory))
    tar.close()

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

def unzip(zipname, path=None):
    zf = zipfile.ZipFile(zipname)
    zf.extractall(path=path)
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

class Extractor(QThread):
    """
    e = Extractor()
    e.setup("/path/to/test.tar.gz", "/path/to/target/dir")
    e.setup("/path/to/test.zip", "/path/to/target/dir")
    e.setup("/path/to/test.gz")
    e.extract()
    
    """
    def __init__(self, parent = None):
        QThread.__init__(self, parent)
        self.archive_path = "tester"
        self.target_path = "."
        self.tar = None
        self.zip = None


    def __del__(self):
        self.wait()

    def setup(self, path, target="."):
        """Target parameter is optional. It won't be used if the file is gz."""
        self.archive_path = path
        self.target_path = target
        
        if self.archive_path.endswith(".tar.gz"):
            try:
                self.tar = tarfile.open(self.archive_path, "r:gz")
            except tarfile.ReadError,e:
                #print "r:gz mode did not work, opening in r mode."
                self.tar = tarfile.open(self.archive_path, "r")
            
        elif self.archive_path.endswith(".gz"):
            self.ungz_name = new_filename = ".".join(self.archive_path.split('.')[:-1])
            #print "Extracting",self.archive_path, "as", new_filename

            self.gz_in = gzip.open(self.archive_path, 'rb')
            self.gz_out = open(new_filename, 'wb')
        
        elif self.archive_path.endswith(".zip"):
            self.zip = zipfile.ZipFile(path)
        
    def extract(self):
        print "Extraction starts..."
        self.start()

    def run(self):
        if self.archive_path.endswith(".tar.gz"):
            self.tar.extractall(path=self.target_path)
        elif self.archive_path.endswith(".gz"):
            self.gz_out.writelines(self.gz_in)
            self.gz_out.close()
            self.gz_in.close()
            os.remove(self.archive_path)

            if self.target_path != ".":
                print self.ungz_name, self.target_path
                if os.path.isdir(self.target_path):
                    self.target_path = os.path.normcase( "%s/%s" % ( self.target_path , os.path.basename(self.ungz_name)) )
                os.rename(self.ungz_name, self.target_path)
                
        elif self.archive_path.endswith(".zip"):
            self.zip.extractall(path=self.target_path)

"""class MultiExtractor(QObject):
    finished = pyqtSignal('QList<QString>')

    def __init__(self):
        QObject.__init__(self)
        self.pairs = []
        self.d = None

    def setup(self, file_target_pairs):
        self.pairs = file_target_pairs

    def extract(self, index):
        del self.d
        
        try:
            pair = self.pairs[index]
        except:
            self.finished.emit([pair[0].split("/")[-1] for pair in self.pairs] )
            return

        self.d = Extractor()
        self.d.setup(pair[0], pair[1])
        func = partial(self.extract, index=index+1)
        self.d.finished.connect(func)
        self.d.extract()

    def start(self):
        self.extract(0)"""

        
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


    #thread = Extractor()
    #thread.setup("/home/emre/robinviz/trunk/src/python/robinviz/databases/identifier.db.tar.gz", "/home/emre/Desktop/test")
    #thread.setup("/home/emre/Desktop/test/datamanager.py.gz", "/home/emre/Desktop/release/datamanager2.py")
    #thread.extract()
    #IDENTIFIER_PATH = "/home/emre/robinviz/trunk/src/python/robinviz/databases/identifier.db"
    #TARNAME = "/home/emre/robinviz/trunk/src/python/robinviz/databases/identifier.db.tar.gz"
    #thread.setup(TARNAME, IDENTIFIER_PATH)
    #thread.extract()

    #download_file("http://hintdb.hgc.jp/htp/download/4932_hc_interactions.tar.gz")
    #untar("4932_hc_interactions.tar.gz")
    #ungz("test.gz")

    thread = MultiExtractor()
    thread.setup( ( ("/home/emre/robinviz/trunk/src/python/robinviz/databases/3702_hc_interactions.tar.gz", "/home/emre/Desktop/test") , ) )
    thread.start()

    #thread = Extractor()
    #thread.setup("/home/emre/robinviz/trunk/src/python/robinviz/databases/3702_hc_interactions.tar.gz", "/home/emre/Desktop/test")
    #thread.extract()