#!/usr/bin/python
# -*- coding: utf-8 -*-


#Script that puts all root raw data in the folder topDir in a sorted list of files and passes them as daily bundles
#to a KMergeRoot object which creates daily root files of all bolometers. The KMergeRoot object only uses KData base clases,
#thus this script works on every level of data (HLA, RAW, ...).
#
#Usage: nohup nice /kalinka/home/schmidt-b/kData/KDataPy/scripts/mergeKdataRaw.py /kalinka/home/schmidt-b/uspaceBen/testData/rootData/ /kalinka/home/schmidt-b/uspaceBen/testData/rootData/mergedData/ 1  >/kalinka/home/schmidt-b/uspaceBen/testData/rootData/mergedData/nohupOut1.txt 2>/kalinka/home/schmidt-b/uspaceBen/testData/rootData/mergedData/nohupOut1.err&
#First argument:	input directory of root (raw) files
#Second argument:	output directory for merged root files
#Third argu	ment:	>=1 to overwrite existing files, default is 0
#
#

import sys, os, re, time
from ROOT import *

outputDir = '~/uspaceBen/testData/rootData/mergedData/'
topDir= '~/uspaceBen/testData/rootData/'

def getDaysList(myList, dayList):
    compareString=myList[0]
   # compare=compareString[compareString.find('.root')-12:compareString.find('.root')-8]
    compare=compareString[compareString.find('.root')-20:compareString.find('.root')-16] # only compare the lc18 part which of course sits at a different place for _NoPulse.root files.
    print compare #check that you compare the right part of the file name
    firstDayInMyList=list()
    #print 'compare: '
    #print compare
    myList2=myList[0:len(myList)]
    for s in myList2:
        if (os.path.basename(s).find(compare)>=0):
            firstDayInMyList.append(s)
            myList.remove(s)
            #print s
            
    dayList.append(firstDayInMyList)

    if(len(myList)>0):
        return getDaysList(myList, dayList)
        
    return dayList


def getMergeList(dayList):
    mergeList=list()
    mergeList=[dayList[i]+dayList[i+1] for i in range(len(dayList)-1)]
    return mergeList


def getListOfRootFilesToMerge(topDir, outputDir):
	filesToMerge = list()
	if os.path.isdir(topDir)==False:
		print 'mergeKdataRaw:getListOfRootFilesToMerge(topDir, outputDir):', topDir, 'is not a directory'
		return ntps
		
	rootdirex = '[a-z][a-z][0-9][0-9][a-z][0-9][0-9][0-9]'
	rootfileex = rootdirex + '_[0-9][0-9][0-9]_NoPulse.root'

	for dirpath, dirs, files in os.walk(topDir):
		for f in files:
			if (re.match(rootfileex, f)): # and (int(f[2:4])==date or int(f[2:4])==(date-1)):
				filesToMerge.append(os.path.join(dirpath, f))
	#sort list to merge
	filesToMerge.sort(key=str.lower)
	#print filesToMerge
	dayList=list()
	dayList=getDaysList(filesToMerge, dayList)
	return dayList
	

def GetOutFileDay(outFileList, outDir):
	if os.path.isdir(outDir)==False:
		print 'mergeKdataRaw:GetOutFileDay(outFileList, outDir):',outDir, 'is not a directory'
		return ntps
	outFile=atoi((os.path.basename(outFileList[len(outFileList)-1]))[2:4])
	print 'OutFile day is ', outFile
	return outFile

def GetOutFileName(outFileList, outDir):
	if os.path.isdir(outDir)==False:
		print 'mergeKdataRaw:GetOutFileName(outFileList, outDir)',outDir, 'is not a directory'
		return ntps
	outFile=outDir+'kData'+(os.path.basename(outFileList[len(outFileList)-1]))[0:4]+'.root'
	print 'Creating outputFile ', outFile
	return outFile


def GetLastStamp(mergeDataFolder):
#checks the standard input file mereKdataRaw.in where the last stamp and the last day ("lb22") is stored.
	mergeInputFile=mergeDataFolder+"mergeKdataRaw.in"
	if os.path.exists(mergeInputFile):
		#print 'is os.path.exits(mergeInputFile) returned true' 
		f=open(mergeInputFile,'r')
		stamp=int( f.readline())
		f.close()
		return stamp
	print 'mergeKdataRaw:GetLastStamp(mergeDataFolder):', mergeInputFile, 'does not exist!'
	return -1

def GetLastDay(mergeDataFolder):
	#checks the standard input file mereKdataRaw.in where the last stamp and the last day ("lb22") is stored.
	mergeInputFile=mergeDataFolder+"mergeKdataRaw.in"
	if os.path.exists(mergeInputFile):
		f=open(mergeInputFile,'r')
		stamp=int(f.readline())
		lastDay=f.readline()
		f.close()
		return lastDay
		print 'mergeKdataRaw:GetLastDay(mergeDataFolder):', mergeInputFile, 'does not exist!'
	return "none"

def SetNewInputFile(stamp, lastDay, mergeDataFolder):
	mergeInputFile=mergeDataFolder+"mergeKdataRaw.in"
	f=open(mergeInputFile, 'w')
	f.write(str(stamp))
	f.write(str(lastDay)+'\n')
	f.close()


def main(*argv):
	print 'mergeKdataRaw:main(*argv): Starting Merge routine at ', time.localtime(), '\n The processor time used so far as returned by time.clock():', time.clock()
	startTime=time.time() #store startTime in Seconds
	print 'mergeKdataRaw:main(*argv): Loading libksamba and libkeb'
	gSystem.Load('libksamba')
	gSystem.Load('libkeb')
	#gSystem.ProcessLine('~\rootlogon.C')

	if len(argv) < 1:
		print 'mergeKdataRaw:main(*argv): need more arguments'
		exit(-1)
		
	topDir = argv[0]
	override = False
	if len(argv) >= 2:
		outputDir = argv[1]
	if len(argv) >= 3:
		override = argv[2]
	fileListTemp = getListOfRootFilesToMerge(topDir, outputDir)
	#print fileListTemp
	fileList = getMergeList(fileListTemp)
	#print fileList
	startTimeList=list()

	j=0
	for j in range(0,len(fileList)):
		print 'mergeKdataRaw:main(*argv):forLoop:', j
		startTimeList.append(time.time())
		r=KDataReader(fileList[j][0])
		stamp=GetLastStamp(outputDir)
		day=GetOutFileDay(fileList[j], outputDir)
		for i in range(1,len(fileList[j])):
			file=fileList[j][i]
			if i%10==0 | i==len(fileList[j]):
				print 'mergeKdataRaw:main(*argv):', file
			r.AddFile(file)
			if i%10==0 | i==len(fileList[j]):
				print 'mergeKdataRaw:main(*argv):', 'File i=',i,'Processing day j=',j,'KDataReader entries=',r.GetEntries()
			#if outputDir== '':
			#  outputFile = file + '.root'
			#else:
			#  outputFile = os.path.join(outputDir, os.path.basename(file)) + '.root'
			#  
			#if override == True or os.path.isfile(outputFile) == False:
			#  print 'Converting', file, 'to', outputFile
				
		outputFile=GetOutFileName(fileList[j], outputDir)
		print 'mergeKdataRaw:main(*argv): OutputFileName', outputFile
		print 'mergeKdataRaw:main(*argv): Arguemnts are r', outputFile, stamp, day
		merge=KMergeRoot(r, outputFile, stamp, day)
		newStamp=merge.Merge()
		SetNewInputFile(newStamp, day, outputDir)
		
		mergeSuccessfull=False
		if mergeSuccessfull:
			for i in range(1,len(fileList[j])):
				print 'mergeKdataRaw:main(*argv): Removing file', file
				os.remove(file)
		print 'mergeKdataRaw:main(*argv): Closing the KDataReader r'
		r.Close()
		#delete merge


	print 'mergeKdataRaw:main(*argv): All start times saved (UTC in s): programm start time:', startTime
	print 'mergeKdataRaw:main(*argv): loop conversion start times:', startTimeList
	print 'mergeKdataRaw:main(*argv): Time now: ', time.time()
	print 'mergeKdataRaw:main(*argv): Total time in hours', (time.time()-startTime/3600.0)
	print 'mergeKdataRaw:main(*argv): Processeor Time:', time.clock()

if __name__ == '__main__':
	print main(*sys.argv[1:])
else:
	print 'Sorry, __name was %r, not "__main__".' % __name__
	


