kdatapy database access
=======================
This module facilitates connections to our various CouchDB databases. It saves you a few lines of code
because these calls are generally quite common. Also, it will connect to the default Edelweiss database
server (edelweiss.cloudant.com) for you automatically. However, you may specify a particular database if you wish.

Unfortunately, there are not yet any helper methods for some of the common tasks. You will have to know which CouchDB "View" to call and how to iterate through the results of that call in order to find what you want. 
Ask me (adam.cox@kit.edu / gadamc@gmail.com) for help!

.. automodule:: KDataPy.database
   :members:

