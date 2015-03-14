# Introduction #

Here you can find instructions on how to install Robinviz.

# Runtime Requirements #
  * Python 2.6
  * PyQt 4.7

# Library Requirements #
  * LEDA 5.1+ Library from Algorithmic Solutions

# Linux #
## Binary ##
  1. Use the following command to install PyQt4:
```
sudo apt-get install python-qt4
```

  1. Run the installer file: Robinviz-1.0-beta-Linux-x86-Install. If it doesn't run by clicking, use the following command to give it executable rights and run it:
```
chmod +x Robinviz-1.0-beta-Linux-x86-Install
./Robinviz-1.0-beta-Linux-x86-Install
```
  1. After the installation, open a new terminal/konsole to run the program with updated PATH variables. Move into the installation dir and run robinviz.exe
```
cd ~/robinviz
./robinviz.exe
```

## Source ##
  1. Use the following command to install the required packages:
```
sudo apt-get install python-qt4 g++ libX11-dev
```
  1. Copy the distribution folder to anywhere you like (for example inside your home dir)
```
cp robinviz-1.0-beta-linux-source ~/robinviz
```
  1. Add the following lines to your ~/.bashrc (Make sure that incl folder is inside this LEDA\_ROOT):
```
export LEDAROOT=/path/to/LEDA
export PATH=$PATH:$LEDAROOT/Manual/cmd
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$LEDAROOT
```
  1. Give the following command on console:
```
source ~/.bashrc
```
  1. Move to the robinviz directory on console and run the following command:
```
cd ~/robinviz
sh compile.sh
./robinviz.exe
```

# Windows #

## Requirements ##
  * Windows XP SP3 / Vista (recommended)
  * Visual Studio 2005 C++ or over
  * [Python 2.7](http://www.python.org/download/) and [PyQt 4.7+](http://www.riverbankcomputing.co.uk/software/pyqt/download).

## Binary ##
  1. Start the installation wizard to install the program under C:\Robinviz
  1. Double click on the Robinviz icon on your desktop.

## Source ##

  1. Follow the instructions at http://www.algorithmicsolutions.info/leda_manual/DLL_s_MS_Visual.html

or

  1. Use sample Visual Studio 2005 Solution Template located at the main source distribution folder.
  1. Setup LEDA 5.1+ Library from Algorithmic Solutions
  1. Follow the instructions at http://www.algorithmic-solutions.info/leda_manual/DLL_s_MS_Visual.html or if you use the template, you will need to add library path and include folder from menu bar Tools->Options->Project and Solutions->VC++ Directories.