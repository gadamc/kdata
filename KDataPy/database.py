import couchdbkit

def getDbList():
  return['https://edelweiss.cloudant.com', 'http://localhost:5984']
  
def getdatabase(database, serverName=None):
  '''
  
  returns an instance of couchdbkit.Database. If you don't give a serverName, this will
  attempt to connect to one of the default Edelweiss database servers.
  
  '''
  
  def tryDb(sName):
    try:
      server = couchdbkit.Server(sName)
      db = server[database]
      db.info() #this will throw if there's a problem
      return db
    except:
      return None


  if serverName: 
    db = tryDb(serverName) 
    if db: return db
    else: 
      print "unable to connect to", serverName, "...trying known edelweiss database servers."

  for name in getDbList():
    db = tryDb(name) 
    if db: return db

  print "unable to connect to a database server."
  return None


def datadb(serverName=None):
  '''

  returns an instance of couchdbkit.Database initialized to the 'datadb' database

  '''
  return getdatabase('datadb',serverName)

def kdatadb(serverName=None):
  '''

  returns an instance of couchdbkit.Database initialized to the 'datadb' database
  this is supporting legacy code... 
  
  '''
  return datadb(serverName)

def analysis(serverName=None):
  '''
  
  returns an instance of couchdbkit.Database initialized to the 'analysis' database
  
  '''
  return getdatabase('analysis',serverName)

def pulsetemplates(serverName=None):
  '''
  
  returns an instance of couchdbkit.Database initialized to the 'pulsetemplates' database
  
  '''
  return getdatabase('pulsetemplates',serverName)

def muonhv(serverName=None):
  '''
  
  returns an instance of couchdbkit.Database initialized to the 'muonhv' database
  
  '''
  return getdatabase('muonhv',serverName)

def muonvetohardwaremap(serverName=None):
  '''
  
  returns an instance of couchdbkit.Database initialized to the 'muonvetohardwaremap' database
  
  '''
  return getdatabase('muonvetohardwaremap',serverName)

def bolohardwaremap(serverName=None):
  '''
  
  returns an instance of couchdbkit.Database initialized to the 'bolohardwaremap' database
  
  '''
  return getdatabase('bolohardwaremap',serverName)

def radon(serverName=None):
  '''
  
  returns an instance of couchdbkit.Database initialized to the 'radondb' database
  
  '''
  return getdatabase('radon',serverName)

def vetopos(serverName=None):
  '''
  
  returns an instance of couchdbkit.Database initialized to the 'vetopos' database
  
  '''
  return getdatabase('vetopos',serverName)

def automat(serverName=None):
  '''
  
  returns an instance of couchdbkit.Database initialized to the 'automat' database, 
  which contains all of the cryostat recorded data.
  
  '''
  return getdatabase('automat',serverName)
  

def multiprocessanadb(serverName=None):
  '''
  
  returns an instance of couchdbkit.Database initialized to the 'multiprocessanadb' database
  
  '''
  return getdatabase('multiprocessanadb',serverName)