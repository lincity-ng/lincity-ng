import os
import string

l=os.listdir("./")

def isSourceFile(f):
	return (string.find(f,".c")>=0 or string.find(f,".h")>=0) and \
        string.find(f,"~")<0 and string.find(f,"#")<0 and \
        string.find(f,"main.c")<0
def getAllSourceFiles(dir):
	l=os.listdir(dir)
	return filter(isSourceFile,l)


libsrc=getAllSourceFiles("./")

Library('lincity',libsrc,CCFLAGS="-Igui_interface -I. -DHAVE_CONFIG_H")

#SConscript('gui_interface/SConscript')
SConscript('lincity/SConscript')
SConscript('lincity/modules/SConscript')
SConscript('oldgui/SConscript')
#SConscript('newgui/SConscript')
