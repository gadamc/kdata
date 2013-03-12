.. kdatapy documentation master file, created by
   sphinx-quickstart on Sat Sep 15 10:44:35 2012.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

KDataPy 
========

KDataPy is a set of Python tools that were crammed into KData. Many of these tools are scripts that perform specific operations, such as uploading .csv files to the database. In reality, these scripts should live in a different code repository (and they may be moved out).

However, KDataPy does contain a set of Python-based extension to the KData Data Structure library classes (KDataReader, KEvent, KBoloPulseRecords, etc...) and general purpose helper functions (util.py and database.py). 

The KDataPy.util module will likely be the most interesting module. The KDataPy.database module provides just a few quick functions to save you some typing to get a connection to an Edelweiss database server. The KDataPy.samba_utilities module contains some tools to read/write Samba file name strings, as well as providing functions to test for valid Samba/KData file names (useful for finding files data in a directory) and providing regular expressions. The KDataPy.signals module provides the tools needed to simulate raw pulses, which can then be used to test signal processing tools. 



Contents:

.. toctree::
   :maxdepth: 2

   code
   
   extraDocs

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`

