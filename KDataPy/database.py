import couchdbkit

class kdatabase(object):
  
  def __init__(self, database, server=None):
    self.server = couchdbkit.Server('http://edwdbik.fzk.de:5984')
    self.db = self.server[database]

  def view(self, view_name, schema=None, wrapper=None, **params):
    '''
    calls couchdbkit.Database.view method.
    '''
    return self.db.view(view_name, schema, wrapper, **params)
    
    
  
class kdatadb(kdatabase):
  
  def __init__(self, server=None):
    kdatabase.__init__(self, 'datadb',server)
    

class kradondb(kdatabase):

  def __init__(self, server=None):
    kdatabase.__init__(self, 'radondb',server)
    

class kcryodb(kdatabase):

  def __init__(self, server=None):
    kdatabase.__init__(self, 'automat',server)