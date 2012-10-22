#!/usr/bin/env python 
#

from couchdbkit import Server, Database
import sys, os, time, datetime, string
from backwards import BackwardsReader


def upload(*argv):
  
  s = Server(argv[0])
  db = s[argv[1]]
  fileEndTime = datetime.datetime.fromtimestamp(os.path.getmtime(argv[2]))
    
  theFile = BackwardsReader(argv[2])
  
  line = theFile.readline()
  doc = {}
  doc['_id'] = os.path.basename(argv[2])
  if db.doc_exist(doc['_id']):
    doc['_rev'] = db.get_rev(doc['_id'])
  doc['type'] = 'radondatafile'
  data = []
  
  previousEventHour = fileEndTime.hour
  
  while line:  
    if line.strip('\n\r\t') != '' and line != '':
      eventTimeAsci, adcValue = line.strip('\r\n').split('\t ')
      adcValue = int(adcValue.strip())
      ehour, eminute, esecond, ems = eventTimeAsci.strip().split(':')
      ehour = int(ehour)
      eminute = int(eminute)
      esecond = int(esecond)
      ems = int(ems)
  
      if previousEventHour == 0 and ehour == 23:
        fileEndTime = fileEndTime - datetime.timedelta(days = 1)

      previousEventHour = ehour
      
      eventDateTime = datetime.datetime(year = fileEndTime.year, month = fileEndTime.month,
                                      day = fileEndTime.day, hour = ehour, minute = eminute, second = esecond, microsecond = 1000*ems)
    
      eventEpoch = time.mktime(eventDateTime.timetuple()) + ems/1000. #have to add the milliseconds back manually because the timetuple truncates to seconds
    
      data.append([eventEpoch, adcValue])
      
        
    line = theFile.readline()
  
  doc['data'] = data
  
  db.save_doc(doc)
    
#
if __name__ == '__main__':
  upload(*sys.argv[1:])
