#! /usr/bin/python

import sys
import os
import shutil
import zipfile

#############################################################
# ZIP support
#############################################################
def ZipDir(top, archive, dir) :
    entries = os.listdir(top)
    for entry in entries: 
        path = os.path.join(top, entry)
        if os.path.isdir(path):
            ZipDir(path, archive, os.path.join(dir, entry))
        else:
            zip_name = os.path.join(dir, entry)
            archive.write(path, zip_name)

def ZipIt(dir) :
    zip_filename = dir+'.zip'
   
    if os.path.exists(zip_filename):
        os.remove(zip_filename)

    archive = zipfile.ZipFile(zip_filename, "w", zipfile.ZIP_DEFLATED)
    ZipDir(dir, archive, dir)
    archive.close()
    
#############################################################
# Main
#############################################################
BLUETUNE_KIT_VERSION = '1-4-1'

BLUETUNE_KIT_HOME = os.path.abspath(os.path.dirname(__file__))+'/..'

SDK_REVISION = sys.argv[1]
print "Exporting Revision", SDK_REVISION

# compute paths
SDK_NAME='BlueTune-SRC-'+BLUETUNE_KIT_VERSION+'-'+SDK_REVISION
SDK_OUTPUT_ROOT=BLUETUNE_KIT_HOME+'/SDK'
SDK_ROOT=SDK_OUTPUT_ROOT+'/'+SDK_NAME
            
print SDK_NAME

# remove any previous SDK directory
if os.path.exists(SDK_ROOT):
    shutil.rmtree(SDK_ROOT)

# ensure that the output dir exists
if not os.path.exists(SDK_OUTPUT_ROOT):
    os.makedirs(SDK_OUTPUT_ROOT)

### export
cmd = 'svn export -r'+SDK_REVISION+' https://zebulon.bok.net/svn/BlueTuneKit/trunk '+SDK_NAME
os.system(cmd)

### zip it
ZipIt(SDK_NAME)

### move the output
shutil.move(SDK_NAME+'.zip', SDK_OUTPUT_ROOT)
shutil.move(SDK_NAME, SDK_ROOT)
