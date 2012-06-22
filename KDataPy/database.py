import couchdbkit

class KDataBase(object):
  
  def __init__(self, database='datadb',server=None):
    self.server = couchdbkit.Server('http://edwdbik.fzk.de:5984')
    self.db = self.server[database]

  def view(self, view_name, schema=None, wrapper=None, **params):
    '''
    calls couchdbkit.Database.view method.
    '''
    return self.db.view(view_name, schema, wrapper, **params)
    
    
  