/*
  Copyright (c) 2009 Dave Gamble
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
 
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
 
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
 
 Modified: april 28 2011 Adam Cox
 Modified: aug 10 2011 Adam Cox. I moved everything inside of a KJson class. 
 Works better now with KData / ROOT. 
 The code is now some weird hybrid between C and C++. 
 
 Please, be my guest to clean this code up and make it all C++!!!  

*/

#ifndef KJson__h
#define KJson__h

#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>
#include <limits.h>
#include <ctype.h>

 enum KJson_types
 {KJson_False=0,
    KJson_True,
    KJson_NULL,
    KJson_Number,
    KJson_String,
    KJson_Array, 
    KJson_Object, 
    KJson_IsReference= 256
  };

/* The KJson structure: */
class KJson {
public:
	KJson *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	KJson *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==KJson_String */
	int valueint;				/* The item's number, if type==KJson_Number */
	double valuedouble;			/* The item's number, if type==KJson_Number */

	char *key;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
	
   
  typedef struct Hooks {
        void *(*malloc_fn)(size_t sz);
        void (*free_fn)(void *ptr);
  } JHook;


	/* Supply malloc, realloc and free functions to KJson */
  static void InitHooks(JHook* hooks);


  /* Supply a block of JSON, and this returns a KJson object you can interrogate. Call Delete when finished. */
  static KJson *Parse(const char *value);
  /* Render a KJson entity to text for transfer/storage. Free the char* when finished. */
  static char  *Print(KJson *item);
  /* Render a KJson entity to text for transfer/storage without any formatting. Free the char* when finished. */
  static char  *PrintUnformatted(KJson *item);
  /* Delete a KJson entity and all subentities. */
  static void   Delete(KJson *c);

  /* Returns the number of items in an array (or object). */
  static int	  GetArraySize(KJson *array);
  /* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
  static KJson *GetArrayItem(KJson *array,int item);
  /* Get item "string" from object. Case insensitive. */
  static KJson *GetObjectItem(KJson *object,const char *string);

  /* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when Parse() returns 0. 0 when Parse() succeeds. */
  static const char *GetErrorPtr();

  /* These calls create a KJson item of the appropriate type. */
  static KJson *CreateNull();
  static KJson *CreateTrue();
  static KJson *CreateFalse();
  static KJson *CreateBool(int b);
  static KJson *CreateNumber(double num);
  static KJson *CreateString(const char *key);
  static KJson *CreateArray();
  static KJson *CreateObject();

  /* These utilities create an Array of count items. */
  static KJson *CreateIntArray(int *numbers,int count);
  static KJson *CreateFloatArray(float *numbers,int count);
  static KJson *CreateDoubleArray(double *numbers,int count);
  static KJson *CreateStringArray(const char **keys,int count);

  /* Append item to the specified array/object. */
  static void AddItemToArray(KJson *array, KJson *item);
  static void	AddItemToObject(KJson *object,const char *key,KJson *item);
  /* Append reference to item to the specified array/object. Use this when you want to add an existing KJson to a new KJson, but don't want to corrupt your existing KJson. */
  static void AddItemReferenceToArray(KJson *array, KJson *item);
  static void	AddItemReferenceToObject(KJson *object,const char *key,KJson *item);

  /* Remove/Detatch items from Arrays/Objects. */
  static KJson *DetachItemFromArray(KJson *array,int which);
  static void   DeleteItemFromArray(KJson *array,int which);
  static KJson *DetachItemFromObject(KJson *object,const char *key);
  static void   DeleteItemFromObject(KJson *object,const char *key);

    /* Update array items. */
  static void ReplaceItemInArray(KJson *array,int which,KJson *newitem);
  static void ReplaceItemInObject(KJson *object,const char *key,KJson *newitem);

  static void AddNullToObject(KJson* object,const char* name);
  static void AddTrueToObject(KJson* object,const char* name);
  static void AddFalseToObject(KJson* object,const char* name);	
  static void AddNumberToObject(KJson* object,const char* name,double n);
  static void AddStringToObject(KJson* object,const char* name,const char* s);
};


#endif
