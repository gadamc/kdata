#KDataExceptions.py
import traceback

class KDataError(Exception):
  def __init__(self, value):
    
    #stack = traceback.extract_stack()
    #lets not put the stack traceback in here because this information 
    #gets logged on a potentially publically available website and may
    #inclde password information
    #self.value = value #+ '\n'.join(str(x) for x in stack[:len(stack)-1])

    self.value = value 

  def __str__(self):
    return str(self.value)

class KDataTransferError(KDataError):
  pass

class KDataRootificationError(KDataError):
  pass

class KDataSambaHeaderCouchError(KDataError):
  pass

class KDataKampingError(KDataError):
  pass

class KDataDatabaseError(KDataError):
  pass

class KDataIrodsError(KDataError):
  pass
