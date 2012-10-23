#exceptions.py

class KDataTransfer(Exception):
  def __init__(self, value):
    self.value = value
  def __str__(self):
    return repr(self.value)


class KDataRootification(Exception):
  def __init__(self, value):
    self.value = value
  def __str__(self):
    return repr(self.value)

class KDataSambaHeaderCouch(Exception):
  def __init__(self, value):
    self.value = value
  def __str__(self):
    return repr(self.value)
