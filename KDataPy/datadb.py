from restkit import request
import urllib, json

class datadb():
  '''
    This class encapsulates the server-side functions that are built into the 'datadb'
    database's 'proc' design document, and provides some common calls that are
    often made to the 'datadb' database. This does not use couchdbkit. Rather it uses
    restkit (upon with couchdbkit is built) since I only want to make the HTTP request

    If you have moved the database to a different server and changed the names of the database 
    (something other than 'datadb') or the design doc, then you can specify those parameters 
    when you instantiate the class.

    You must provide the server with login credentials for methods that write new values
    to database documents. 

    ONLY use this class if you absolutely know what you are doing. Remember, you will be affecting
    data on the database with these tools.

    ***
    
    Are you SURE you want to alter data on the database?
    
    ***

  '''

  def __init__(self, server='https://edelweiss.cloudant.com', databasename='datadb', designDocName = 'proc'):
    '''
    '''

    self.server = server
    self.databasename = databasename
    self.designdoc = designDocName


# LOW-LEVEL methods

  def _call(self, docid, designDocName, functionName, reqOpts, method='GET'):
    '''
    '''
    reqUrl = '%s/%s/_design/%s/_%s/%s/%s?%s' % \
      (self.server, self.databasename,  self.designdoc, designDocName, functionName, docid, urllib.urlencode(reqOpts))
    return json.loads( request(reqUrl, method=method).body_string() )

  def _update(self, docid, functionName, reqOpts):
    '''
      call an update funciton.
    '''
    return self._call(docid, "update", functionName, reqOpts, 'PUT')

  def _show(self, docid, functionName, reqOpts):
    '''
      call a show function
    '''
    return self._call(docid, "show", functionName, reqOpts, 'GET')
    


  def _update_inplace(self, docid, reqOpts):
    '''
      call update/in-place function
    '''
    return self._update(docid, 'in-place', reqOpts)
  

  def _show_value(self, docid, reqOpts):
    '''
      call the show/value function
    '''
    return self._show(docid, 'value', reqOpts)


# HIGH-LEVEL methods

  def setkey(self, docid, key, value):
    '''
    Are you SURE you want to alter data on the database?
    '''
    return self._update_inplace(docid, {'update':key, 'value':value} )


  def getkey(self, docid, key):
    '''
    '''
    return self._show_value(docid, {'field':key} )
    
  def removekey(self, docid, key):
    '''
    Are you SURE you want to alter data on the database?
    '''
    return self._update_inplace(docid, {'remove':key} )


