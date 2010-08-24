//______________________________________________________________________

// KEraEventReader.h
// Author: Adam Cox <mailto:adam.cox@kit.edu>
//
// *Copyright 2010 Karlsruhe Inst. of Technology. All Rights Reserved
//
//


#ifndef __KERAEVENTREADER_H__
#define __KERAEVENTREADER_H__

class KEraEventReader  { 

public:
  //Constructors
  KEraEventReader(void);
  virtual ~KEraEventReader(void);
  virtual void Clear(Option_t *opt = "C");

  //getters

  //setters
private:

  //private methods
  void InitializeMembers(void);

  ClassDef(KEraEventReader,1);
};


#endif // __KERAEVENTREADER_H__
