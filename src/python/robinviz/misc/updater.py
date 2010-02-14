from __init__ import __VERSION__, __TYPE__
import urllib
import os
import os.path
import zipfile


ROOT_PATH = "../../../.."
updateServers = ("http://hacivat.khas.edu.tr/~robinviz/updates/",
                 "http://www.emrealadag.com/robinviz/updates/", )

UPDATE_SCRIPT = "updatescript.py"

if os.name == "posix":
    updateFile = "linux/%s/updates.txt" % __TYPE__
else:
    updateFile = "win/%s/updates.txt" % __TYPE__



class UpdateChecker:
    def __init__(self):
        self.currentVersion = __VERSION__

    def checkUpdate(self):


        
        checkDone = False
        
        for server in updateServers:
            try:
                f = urllib.urlopen(server + updateFile)
                for line in f:
                    # 0.1.0 0.1.1 updatefile.zip
                    fromVersion, toVersion, updateZip = line.strip().split(" ")
                    if fromVersion == self.currentVersion:
                        # DO IT
                        self.updateZip = updateZip
                        self.zipFileName = self.updateZip.split('/')[-1]
                        checkDone = True
                        break

            except:
                print "Could not download file %s" % (server+updateFile)

            if checkDone:
                break
        else:
            print "Could not connect to any of the update servers."
        
    def downloadUpdate(self):
        """Downloads the update file"""
        if getattr(self, "updateZip", None):
            webFile = urllib.urlopen(self.updateZip)

            localFile = open(self.zipFileName, 'wb')
            localFile.write(webFile.read())

            webFile.close()
            localFile.close()
        else:
            print "No update zip defined."
            
    def unzip(self):
        if getattr(self, "zipFileName", None) and os.path.exists(self.zipFileName):
            self.unzipper = Unzipper()
            self.unzipper.extract(self.zipFileName, ROOT_PATH)
        else:
            print "No update zip file found"
        
    def startUpdate(self):
        os.chdir(ROOT_PATH)
        os.system("python %s" % UPDATE_SCRIPT)
        #os.remove(UPDATE_SCRIPT)


class Unzipper:
    """Written By Doug Tolton on ActiveState.com"""
    def __init__(self, verbose = False, percent = 10):
        self.verbose = verbose
        self.percent = percent

    def extract(self, file, dir):
        if not dir.endswith(':') and not os.path.exists(dir):
            os.mkdir(dir)

        zf = zipfile.ZipFile(file)

        # create directory structure to house files
        self._createstructure(file, dir)

        num_files = len(zf.namelist())
        percent = self.percent
        divisions = 100 / percent
        perc = int(num_files / divisions)

        # extract files to directory structure
        for i, name in enumerate(zf.namelist()):

            if self.verbose == True:
                print "Extracting %s" % name
            elif perc > 0 and (i % perc) == 0 and i > 0:
                complete = int (i / perc) * percent
                print "%s%% complete" % complete

            if not name.endswith('/'):
                outfile = open(os.path.join(dir, name), 'wb')
                outfile.write(zf.read(name))
                outfile.flush()
                outfile.close()
                if name.endswith(".exe"):
                    os.chmod(os.path.join(dir, name), 0755)


    def _createstructure(self, file, dir):
        self._makedirs(self._listdirs(file), dir)


    def _makedirs(self, directories, basedir):
        """ Create any directories that don't currently exist """
        for dir in directories:
            curdir = os.path.join(basedir, dir)
            if not os.path.exists(curdir):
                os.mkdir(curdir)

    def _listdirs(self, file):
        """ Grabs all the directories in the zip structure
        This is necessary to create the structure before trying
        to extract the file to it. """
        zf = zipfile.ZipFile(file)

        dirs = []

        for name in zf.namelist():
            if name.endswith('/'):
                dirs.append(name)

        dirs.sort()
        return dirs

if __name__ == "__main__":
    uc = UpdateChecker()

    uc.checkUpdate()
    uc.downloadUpdate()
    uc.unzip()
    uc.startUpdate()