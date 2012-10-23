#KDataExceptions.py
import traceback

class KDataException(Exception):
  def __init__(self, value):
    
    stack = traceback.extract_stack()
    self.value = value + '\n'.join(str(x) for x in stack[:len(stack)-1])

  def __str__(self):
    return str(self.value)

class KDataTransfer(KDataException):
  pass

class KDataRootification(KDataException):
  pass

class KDataSambaHeaderCouch(KDataException):
  pass

class KDataKampingError(KDataException):
  pass