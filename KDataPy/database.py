import couchdbkit

def getDbList():
  return['http://localhost:5984', 'https://edelweiss.cloudant.com', 'http://edwdbik.fzk.de:5984']
  
def getdatabase(database, serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database
  '''
  def tryDb(sName):
    try:
      server = couchdbkit.Server(sName)
      db = server[database]
      db.info() #this will throw if there's a problem
      return db
    except:
      return None


  if serverName != '': 
    db = tryDb(serverName) 
    if db: return db

  print "...couldn't find your database. trying other known edelweiss database servers."
  for name in getDbList():
    print ".... ", name
    db = tryDb(name) 
    if db: return db

def datadb(serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'datadb' database
  '''
  return getdatabase('datadb',serverName)

def analysis(serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'analysis' database
  '''
  return getdatabase('analysis',serverName)

def pulsetemplates(serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'pulsetemplates' database
  '''
  return getdatabase('pulsetemplates',serverName)

def muonhv(serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'muonhv' database
  '''
  return getdatabase('muonhv',serverName)

def muonvetohardwaremap(serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'muonvetohardwaremap' database
  '''
  return getdatabase('muonvetohardwaremap',serverName)

def bolohardwaremap(serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'bolohardwaremap' database
  '''
  return getdatabase('bolohardwaremap',serverName)

def radondb(serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'radondb' database
  '''
  return getdatabase('radondb',serverName)


def automat(serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'automat' database, 
  which contains all of the cryostat recorded data.
  '''
  return getdatabase('automat',serverName)
  

def multiprocessanadb(serverName='https://edelweiss.cloudant.com'):
  '''
  returns an instance of couchdbkit.Database initialized to the 'multiprocessanadb' database
  '''
  return getdatabase('multiprocessanadb',serverName)