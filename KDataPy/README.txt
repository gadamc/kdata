===========
KDataPy
===========


    #!/usr/bin/env python

    import KDataPy.pulsetempy as kptl #(kptl = kdata pulse template, but you can call it whatever you want)
    from ROOT import *
    
    for event in KDataReader('path/to/file'):
      for bolo in event.boloRecords():
        for pulse in bolo.pulseRecords():
          fitresults = kptl.Fit(pulse, type='single_decay', init=[4096,-1,10000.0])



Paragraphs are separated by blank lines. *Italics*, **bold**,
and ``monospace`` look like this.


A Section
=========

Lists look like this:

* First

* Second. Can be multiple lines
  but must be indented properly.

A Sub-Section
-------------

Numbered lists look like you'd expect:

1. hi there

2. must be going

Urls are http://like.this and links can be
written `like this <http://www.example.com/foo/bar>`_.