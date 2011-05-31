#!/usr/bin/python
# -*- coding: utf-8 -*-


#Script that converts all Bolometer Raw-Files in a directory to root Files (Raw-level). Only converts files in this directory once.
#It puts all files that are still available in the input directory and have already been converted in the 
#InputDirectory/ksamba2kdata.in file and excludes these files from conversion in the future. Automatically removes 
#files that have been deleted in the input directory from the ksamba2kdata.in file.
#
#Usage: nohup nice /kalinka/home/schmidt-b/kData/KDataPy/scripts/ksamba2kdata.py /kalinka/home/schmidt-b/uspaceBen/testData2/ /kalinka/home/schmidt-b/uspaceBen/testData2/rootData/ 1 > /kalinka/home/schmidt-b/uspaceBen/testData2/rootData/nohupTest.txt &
#1st argument: Input directory
#2nd argument: Output directory
#3rd argument override=true (for x > 0  and override=false for x==0)
import sys, os, re, time
from ROOT import *

def getListOfSambaFilesToRootify(topDir):
  
  filesToRootify = list()
  if os.path.isdir(topDir)==False:
    print topDir, 'is not a directory'
    return ntps

  sambadirex = '[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]'
  sambafileex = sambadirex + '_[0-9][0-9][0-9]'

  for dirpath, dirs, files in os.walk(topDir):
    for f in files:
      if re.match(sambafileex, f)  and re.search('.root',f)==None:  #matches pattern and its NOT a .root file
          filesToRootify.append(os.path.join(dirpath, f))
  print 'ksamba2kdata.py:getListOfSambaFilesToRootify: filesToRootify', filesToRootify
  filesToRootify=removeFilesAlreadyRootifiedFromList(filesToRootify, topDir)
  return filesToRootify
  

def removeFilesAlreadyRootifiedFromList(filesToRootify, rawDataFolder):
#checks the ksama2kdata.in file in the rawDataFolder to exclude files already rootified
	rawDataInfo=rawDataFolder+'ksamba2kdata.in'
	if os.path.exists(rawDataInfo):
		f=open(rawDataInfo,'r')
		for line in f:
			for file in filesToRootify:
				if line.find(file):
					print 'removing ', file, 'from list to rootify'
					filesToRootify.remove(file)
		return filesToRootify

	fil=open(rawDataInfo,'w')
	fil.close()
	return filesToRootify

def updateKsamba2kdataInput(rawDataFolder, filesToRootify):
#check that the ksamba2kdata.in File only excludes files present in the raw data format and add the files just converted to the list
	#print 'entered updateKsamba2kdataInput'
	rawDataInfo=rawDataFolder+'ksamba2kdata.in'
	updatedList=list()
	if os.path.exists(rawDataInfo):
		fil=open(rawDataInfo,'r')
		alreadyDone=fil.readlines()
		print alreadyDone
		for dirpath, dirs, files in os.walk(rawDataFolder):
			for f in files:
				if stripAndCompareList(alreadyDone, os.path.join(dirpath, f)) or filesToRootify.count(os.path.join(dirpath, f))>=1: 
					#if it is in the old list and is still there as raw data  or was just rootified put it in the list of files not to rootify
					print 'Updated List of files already rootified:', os.path.join(dirpath, f), filesToRootify.count(os.path.join(dirpath, f))
					updatedList.append(os.path.join(dirpath, f))
				
				
			
	fil.close()
	wFile=open(rawDataInfo,'w')
	for item in updatedList:
		wFile.write(item+'\n')
	wFile.close()
	return updatedList	

def stripAndCompareList(myList, myStr):
	for stri in myList:
		if stri.rstrip('\n')==myStr:
			return True
	return False


def main(*argv):
	print 'ksamba2kdata:main(*argv): Starting Bolo rootification routine at ', time.localtime(), '\n The processor time used so far as returned by time.clock():', time.clock()
	startTime=time.time() #store startTime in Seconds
	gSystem.Load('libksamba')

	if len(argv) < 1:
		print 'need more arguments'
		exit(-1)

	topDir = argv[0]
	outputDir = ''
	override = 0

	if len(argv) >= 2:
		outputDir = argv[1]
	if len(argv) >= 3:
		override = argv[2]
		print override
		

	fileList = getListOfSambaFilesToRootify(topDir) 
	print 'Files to rootify: ', fileList
	conv = KSamba2KData()
	startTimeList=list()
	for file in fileList:
		startTimeList.append(time.time())
		if outputDir== '':
			outputFile = file + '.root'
		else:
			outputFile = os.path.join(outputDir, os.path.basename(file)) + '.root'
		if override > 0 or os.path.isfile(outputFile) == False:
			print 'Converting', file, 'to', outputFile
			conv.ConvertFile(file, outputFile)
	updateKsamba2kdataInput(topDir, fileList)
	
	print 'ksamba2kdata:main(*argv): All start times saved (UTC in s): programm start time:', startTime
	print 'ksamba2kdata:main(*argv): loop conversion start times:', startTimeList
	print 'ksamba2kdata:main(*argv): Time now: ', time.time()
	print 'ksamba2kdata:main(*argv): Total time in hours', ((time.time()-startTime)/3600.0)
	print 'ksamba2kdata:main(*argv): Processeor Time:', time.clock()
if __name__ == '__main__':
  print main(*sys.argv[1:])


