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
*/

#ifndef KJson__h
#define KJson__h

#ifdef __cplusplus
extern "C"
{
#endif

enum kjson_types
{KJson_False=0,
   KJson_True,
   KJson_NULL,
   KJson_Number,
   KJson_String,
   KJson_Array, 
   KJson_Object, 
   KJson_IsReference= 256
 };
/* KJson Types: */
//#define KJson_False 0
//#define KJson_True 1
//#define KJson_NULL 2
//#define KJson_Number 3
//#define KJson_String 4
//#define KJson_Array 5
//#define KJson_Object 6
	
//#define KJson_IsReference 256

/* The KJson structure: */
typedef struct KJson {
	struct KJson *next,*prev;	/* next/prev allow you to walk array/object chains. Alternatively, use GetArraySize/GetArrayItem/GetObjectItem */
	struct KJson *child;		/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */

	int type;					/* The type of the item, as above. */

	char *valuestring;			/* The item's string, if type==KJson_String */
	int valueint;				/* The item's number, if type==KJson_Number */
	double valuedouble;			/* The item's number, if type==KJson_Number */

	char *key;				/* The item's name string, if this item is the child of, or is in the list of subitems of an object. */
} KJson;

typedef struct KJson_Hooks {
      void *(*malloc_fn)(size_t sz);
      void (*free_fn)(void *ptr);
} KJson_Hooks;

/* Supply malloc, realloc and free functions to KJson */
extern void KJson_InitHooks(KJson_Hooks* hooks);


/* Supply a block of JSON, and this returns a KJson object you can interrogate. Call KJson_Delete when finished. */
extern KJson *KJson_Parse(const char *value);
/* Render a KJson entity to text for transfer/storage. Free the char* when finished. */
extern char  *KJson_Print(KJson *item);
/* Render a KJson entity to text for transfer/storage without any formatting. Free the char* when finished. */
extern char  *KJson_PrintUnformatted(KJson *item);
/* Delete a KJson entity and all subentities. */
extern void   KJson_Delete(KJson *c);

/* Returns the number of items in an array (or object). */
extern int	  KJson_GetArraySize(KJson *array);
/* Retrieve item number "item" from array "array". Returns NULL if unsuccessful. */
extern KJson *KJson_GetArrayItem(KJson *array,int item);
/* Get item "string" from object. Case insensitive. */
extern KJson *KJson_GetObjectItem(KJson *object,const char *string);

/* For analysing failed parses. This returns a pointer to the parse error. You'll probably need to look a few chars back to make sense of it. Defined when KJson_Parse() returns 0. 0 when KJson_Parse() succeeds. */
extern const char *KJson_GetErrorPtr();
	
/* These calls create a KJson item of the appropriate type. */
extern KJson *KJson_CreateNull();
extern KJson *KJson_CreateTrue();
extern KJson *KJson_CreateFalse();
extern KJson *KJson_CreateBool(int b);
extern KJson *KJson_CreateNumber(double num);
extern KJson *KJson_CreateString(const char *string);
extern KJson *KJson_CreateArray();
extern KJson *KJson_CreateObject();

/* These utilities create an Array of count items. */
extern KJson *KJson_CreateIntArray(int *numbers,int count);
extern KJson *KJson_CreateFloatArray(float *numbers,int count);
extern KJson *KJson_CreateDoubleArray(double *numbers,int count);
extern KJson *KJson_CreateStringArray(const char **strings,int count);

/* Append item to the specified array/object. */
extern void KJson_AddItemToArray(KJson *array, KJson *item);
extern void	KJson_AddItemToObject(KJson *object,const char *string,KJson *item);
/* Append reference to item to the specified array/object. Use this when you want to add an existing KJson to a new KJson, but don't want to corrupt your existing KJson. */
extern void KJson_AddItemReferenceToArray(KJson *array, KJson *item);
extern void	KJson_AddItemReferenceToObject(KJson *object,const char *string,KJson *item);

/* Remove/Detatch items from Arrays/Objects. */
extern KJson *KJson_DetachItemFromArray(KJson *array,int which);
extern void   KJson_DeleteItemFromArray(KJson *array,int which);
extern KJson *KJson_DetachItemFromObject(KJson *object,const char *string);
extern void   KJson_DeleteItemFromObject(KJson *object,const char *string);
	
  /* Update array items. */
extern void KJson_ReplaceItemInArray(KJson *array,int which,KJson *newitem);
extern void KJson_ReplaceItemInObject(KJson *object,const char *string,KJson *newitem);
  
extern void KJson_AddNullToObject(KJson* object,const char* name);
extern void KJson_AddTrueToObject(KJson* object,const char* name);
extern void KJson_AddFalseToObject(KJson* object,const char* name);	
extern void KJson_AddNumberToObject(KJson* object,const char* name,double n);
extern void KJson_AddStringToObject(KJson* object,const char* name,const char* s);
  
#ifdef __cplusplus
}
#endif

#endif
