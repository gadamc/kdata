from ROOT import gSystem
kdatalibs = ['libkds', 'libkpta', 'libksamba', 'libkamping']
for lib in kdatalibs:
  try:
    gSystem.Load(lib)
  except: pass
from kdatapy import *
import scripts