import couchdbkit
  
def getdatabase(database, serverName='http://edwdbik.fzk.de:5984'):
  '''
  returns an instance of couchdbkit.Database
  '''
  server = couchdbkit.Server(serverName)
  return server[database]
    
def kdatadb(serverName='http://edwdbik.fzk.de:5984'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'datadb' database
  '''
  return getdatabase('datadb',serverName)

def kradondb(serverName='http://edwdbik.fzk.de:5984'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'radondb' database
  '''
  return getdatabase('radondb',serverName)
  
def kcryodb(serverName='http://edwdbik.fzk.de:5984'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'automat' database
  '''
  return getdatabase('automat',serverName)
