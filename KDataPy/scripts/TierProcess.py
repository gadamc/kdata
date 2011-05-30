from couchdbkit import Server, Database, Document

class TierProcess(object):
  '''   
    This is a generic Tier Processing class used to handle data processing
    based upon documents found in an instance of a CouchDB. 
    In order for this to work, the function reference that you 
    pass into this objects constructor must be of the form
    
    def myfunction(*args, **kwargs)
    
    '''
  def __init__(self, servername, dbname, tiername, dofunction):
    self.sv = Server(servername)
    self.db = self.sv.get_or_create_db(dbname)
    self.name = tiername
    self.func = dofunction
  
  
  def doprocess(self,  *args, **kwargs):
    
    return self.func(*args, **kwargs)
  
  
  def record(self, doc, tierdict)
  '''
    Save the document to the database. 
    '''
  doc[str(self.name)] = tierdict
  
  if self.db.doc_exist(doc['_id']):
    doc['_rev'] = self.db.get_rev(doc['_id'])
  self.db.save_doc(doc)
  return doc['_id']
  
  
  def view(self, view_name, schema=None, wrapper=None, **params):
    return self.db.view(view_name, schema, wrapper, **params)


class CopyProcess(TierProcess):
  '''   
    This is a Copy Processing class based on the TierProcess used to handle data processing
    based upon documents found in an instance of a CouchDB. 
    '''
  import shutil
  def copyfunction(self, *args, **kwargs):
    '''
      args must be 
      source = args[0]
      target = args[1]
      kwargs are not used
      '''
    return self.shutil.copy( args[0], args[1])
  
  def __init__(self, servername, dbname, tiername):
    
    TierProcess.__init__( self, servername, dbname, tiername, self.copyfunction)


class RemoteCopyProcess(CopyProcess):
  '''   
    This is a Remote Copy Processing class based on the TierProcess used to handle data processing
    based upon documents found in an instance of a CouchDB. 
    Non-interactive authentication (SSL key sharing) should be set up between the two 
    machines used for the remote copying. Otherwise, user intervention will be 
    required when secure copy is called. This requires that the command 'scp' exists
    on the machine where this class is used. 
    '''
  import subprocess
  def copyfunction(self, *args, **kwargs):
    '''
      args must either be
      
      source = args[0]
      node2 = args[1]
      target = args[2]
      or
      
      node1 = args[0]
      source = args[1]
      node2 = args[2]
      target = args[3]
      
      kwargs are not used
      '''
    cmd = ''
    if len(args) == 4:
      cmd = 'scp -r %s:%s %s:%s' % (args[0],args[1],args[2],args[3])
    elif len(args) == 3:
      cmd = 'scp -r %s %s:%s' % (args[0],args[1],args[2])
    else:
      print 'incorrect number of args'
      return None
    
    proc = self.subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE,)
    val = proc.communicate()[0]
    if val != '':
      print val

