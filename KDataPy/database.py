import couchdbkit
  
def getdatabase(database, serverName='http://edwdbik.fzk.de:5984'):
  server = couchdbkit.Server(serverName)
  return server[database]
    
def kdatadb(serverName='http://edwdbik.fzk.de:5984'):
  return getdatabase('datadb',serverName)

def kradondb(serverName='http://edwdbik.fzk.de:5984'):
  return getdatabase('radondb',serverName)
  
def kcryodb(serverName='http://edwdbik.fzk.de:5984'):
    return getdatabase('automat',serverName)
