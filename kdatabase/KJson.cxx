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
*/

/* KJson */
/* JSON parser in C. */


#include "KJson.h"

static const char *ep;

const char *KJson::GetErrorPtr() {return ep;}

static int KJson_strcasecmp(const char *s1,const char *s2)
{
	if (!s1) return (s1==s2)?0:1;if (!s2) return 1;
	for(; tolower(*s1) == tolower(*s2); ++s1, ++s2)	if(*s1 == 0)	return 0;
	return tolower(*(const unsigned char *)s1) - tolower(*(const unsigned char *)s2);
}

static void *(*KJson_malloc)(size_t sz) = malloc;
static void (*KJson_free)(void *ptr) = free;

static char* KJson_strdup(const char* str)
{
      size_t len;
      char* copy;

      len = strlen(str) + 1;
      if (!(copy = (char*)KJson_malloc(len))) return 0;
      memcpy(copy,str,len);
      return copy;
}

void KJson::InitHooks(Hooks* hooks)
{
    if (!hooks) { /* Reset hooks */
        KJson_malloc = malloc;
        KJson_free = free;
        return;
    }

	KJson_malloc = (hooks->malloc_fn)?hooks->malloc_fn:malloc;
	KJson_free	 = (hooks->free_fn)?hooks->free_fn:free;
}

/* Internal constructor. */
static KJson *KJson_New_Item()
{
	KJson* node = (KJson*)KJson_malloc(sizeof(KJson));
	if (node) memset(node,0,sizeof(KJson));
	return node;
}

/* Delete a KJson structure. */
void KJson::Delete(KJson *c)
{
	KJson *next;
	while (c)
	{
		next=c->next;
		if (!(c->type&KJson_IsReference) && c->child) Delete(c->child);
		if (!(c->type&KJson_IsReference) && c->valuestring) KJson_free(c->valuestring);
		if (c->key) KJson_free(c->key);
		KJson_free(c);
		c=next;
	}
}

/* Parse the input text to generate a number, and populate the result into item. */
static const char *parse_number(KJson *item,const char *num)
{
	double n=0,sign=1,scale=0;int subscale=0,signsubscale=1;

	/* Could use sscanf for this? */
	if (*num=='-') sign=-1,num++;	/* Has sign? */
	if (*num=='0') num++;			/* is zero */
	if (*num>='1' && *num<='9')	do	n=(n*10.0)+(*num++ -'0');	while (*num>='0' && *num<='9');	/* Number? */
	if (*num=='.') {num++;		do	n=(n*10.0)+(*num++ -'0'),scale--; while (*num>='0' && *num<='9');}	/* Fractional part? */
	if (*num=='e' || *num=='E')		/* Exponent? */
	{	num++;if (*num=='+') num++;	else if (*num=='-') signsubscale=-1,num++;		/* With sign? */
		while (*num>='0' && *num<='9') subscale=(subscale*10)+(*num++ - '0');	/* Number? */
	}

	n=sign*n*pow(10.0,(scale+subscale*signsubscale));	/* number = +/- number.fraction * 10^+/- exponent */
	
	item->valuedouble=n;
	item->valueint=(int)n;
	item->type=KJson_Number;
	return num;
}

/* Render the number nicely from the given item into a string. */
static char *print_number(KJson *item)
{
	char *str;
	double d=item->valuedouble;
	if (fabs(((double)item->valueint)-d)<=DBL_EPSILON && d<=INT_MAX && d>=INT_MIN)
	{
		str=(char*)KJson_malloc(21);	/* 2^64+1 can be represented in 21 chars. */
		if (str) sprintf(str,"%d",item->valueint);
	}
	else
	{
		str=(char*)KJson_malloc(64);	/* This is a nice tradeoff. */
		if (str)
		{
			if (fabs(floor(d)-d)<=DBL_EPSILON)			sprintf(str,"%.0f",d);
			else if (fabs(d)<1.0e-6 || fabs(d)>1.0e9)	sprintf(str,"%e",d);
			else										sprintf(str,"%f",d);
		}
	}
	return str;
}

/* Parse the input text into an unescaped cstring, and populate item. */
static const unsigned char firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };
static const char *parse_string(KJson *item,const char *str)
{
	const char *ptr=str+1;char *ptr2;char *out;int len=0;unsigned uc;
	if (*str!='\"') {ep=str;return 0;}	/* not a string! */
	
	while (*ptr!='\"' && *ptr && ++len) if (*ptr++ == '\\') ptr++;	/* Skip escaped quotes. */
	
	out=(char*)KJson_malloc(len+1);	/* This is how long we need for the string, roughly. */
	if (!out) return 0;
	
	ptr=str+1;ptr2=out;
	while (*ptr!='\"' && *ptr)
	{
		if (*ptr!='\\') *ptr2++=*ptr++;
		else
		{
			ptr++;
			switch (*ptr)
			{
				case 'b': *ptr2++='\b';	break;
				case 'f': *ptr2++='\f';	break;
				case 'n': *ptr2++='\n';	break;
				case 'r': *ptr2++='\r';	break;
				case 't': *ptr2++='\t';	break;
				case 'u':	 /* transcode utf16 to utf8. DOES NOT SUPPORT SURROGATE PAIRS CORRECTLY. */
					sscanf(ptr+1,"%4x",&uc);	/* get the unicode char. */
					len=3;if (uc<0x80) len=1;else if (uc<0x800) len=2;ptr2+=len;
					
					switch (len) {
						case 3: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 2: *--ptr2 =((uc | 0x80) & 0xBF); uc >>= 6;
						case 1: *--ptr2 =(uc | firstByteMark[len]);
					}
					ptr2+=len;ptr+=4;
					break;
				default:  *ptr2++=*ptr; break;
			}
			ptr++;
		}
	}
	*ptr2=0;
	if (*ptr=='\"') ptr++;
	item->valuestring=out;
	item->type=KJson_String;
	return ptr;
}

/* Render the cstring provided to an escaped version that can be printed. */
static char *print_string_ptr(const char *str)
{
	const char *ptr;char *ptr2,*out;int len=0;unsigned char token;
	
	if (!str) return KJson_strdup("");
	ptr=str;while ((token=*ptr) && ++len) {if (strchr("\"\\\b\f\n\r\t",token)) len++; else if (token<32) len+=5;ptr++;}
	
	out=(char*)KJson_malloc(len+3);
	if (!out) return 0;

	ptr2=out;ptr=str;
	*ptr2++='\"';
	while (*ptr)
	{
		if ((unsigned char)*ptr>31 && *ptr!='\"' && *ptr!='\\') *ptr2++=*ptr++;
		else
		{
			*ptr2++='\\';
			switch (token=*ptr++)
			{
				case '\\':	*ptr2++='\\';	break;
				case '\"':	*ptr2++='\"';	break;
				case '\b':	*ptr2++='b';	break;
				case '\f':	*ptr2++='f';	break;
				case '\n':	*ptr2++='n';	break;
				case '\r':	*ptr2++='r';	break;
				case '\t':	*ptr2++='t';	break;
				default: sprintf(ptr2,"u%04x",token);ptr2+=5;	break;	/* escape and print */
			}
		}
	}
	*ptr2++='\"';*ptr2++=0;
	return out;
}
/* Invote print_string_ptr (which is useful) on an item. */
static char *print_string(KJson *item)	{return print_string_ptr(item->valuestring);}

/* Predeclare these prototypes. */
static const char *parse_value(KJson *item,const char *value);
static char *print_value(KJson *item,int depth,int fmt);
static const char *parse_array(KJson *item,const char *value);
static char *print_array(KJson *item,int depth,int fmt);
static const char *parse_object(KJson *item,const char *value);
static char *print_object(KJson *item,int depth,int fmt);

/* Utility to jump whitespace and cr/lf */
static const char *skip(const char *in) {while (in && *in && (unsigned char)*in<=32) in++; return in;}

/* Parser core - when encountering text, process appropriately. */
static const char *parse_value(KJson *item,const char *value)
{
	if (!value)						return 0;	/* Fail on null. */
	if (!strncmp(value,"null",4))	{ item->type=KJson_NULL;  return value+4; }
	if (!strncmp(value,"false",5))	{ item->type=KJson_False; return value+5; }
	if (!strncmp(value,"true",4))	{ item->type=KJson_True; item->valueint=1;	return value+4; }
	if (*value=='\"')				{ return parse_string(item,value); }
	if (*value=='-' || (*value>='0' && *value<='9'))	{ return parse_number(item,value); }
	if (*value=='[')				{ return parse_array(item,value); }
	if (*value=='{')				{ return parse_object(item,value); }

	ep=value;return 0;	/* failure. */
}

/* Render a value to text. */
static char *print_value(KJson *item,int depth,int fmt)
{
	char *out=0;
	if (!item) return 0;
	switch ((item->type)&255)
	{
		case KJson_NULL:	out=KJson_strdup("null");	break;
		case KJson_False:	out=KJson_strdup("false");break;
		case KJson_True:	out=KJson_strdup("true"); break;
		case KJson_Number:	out=print_number(item);break;
		case KJson_String:	out=print_string(item);break;
		case KJson_Array:	out=print_array(item,depth,fmt);break;
		case KJson_Object:	out=print_object(item,depth,fmt);break;
	}
	return out;
}

/* Build an array from input text. */
static const char *parse_array(KJson *item,const char *value)
{
	KJson *child;
	if (*value!='[')	{ep=value;return 0;}	/* not an array! */

	item->type=KJson_Array;
	value=skip(value+1);
	if (*value==']') return value+1;	/* empty array. */

	item->child=child=KJson_New_Item();
	if (!item->child) return 0;		 /* memory fail */
	value=skip(parse_value(child,skip(value)));	/* skip any spacing, get the value. */
	if (!value) return 0;

	while (*value==',')
	{
		KJson *new_item;
		if (!(new_item=KJson_New_Item())) return 0; 	/* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_value(child,skip(value+1)));
		if (!value) return 0;	/* memory fail */
	}

	if (*value==']') return value+1;	/* end of array */
	ep=value;return 0;	/* malformed. */
}

/* Render an array to text */
static char *print_array(KJson *item,int depth,int fmt)
{
	char **entries;
	char *out=0,*ptr,*ret;int len=5;
	KJson *child=item->child;
	int numentries=0,i=0,fail=0;
	
	/* How many entries in the array? */
	while (child) numentries++,child=child->next;
	/* Allocate an array to hold the values for each */
	entries=(char**)KJson_malloc(numentries*sizeof(char*));
	if (!entries) return 0;
	memset(entries,0,numentries*sizeof(char*));
	/* Retrieve all the results: */
	child=item->child;
	while (child && !fail)
	{
		ret=print_value(child,depth+1,fmt);
		entries[i++]=ret;
		if (ret) len+=strlen(ret)+2+(fmt?1:0); else fail=1;
		child=child->next;
	}
	
	/* If we didn't fail, try to malloc the output string */
	if (!fail) out=(char*)KJson_malloc(len);
	/* If that fails, we fail. */
	if (!out) fail=1;

	/* Handle failure. */
	if (fail)
	{
		for (i=0;i<numentries;i++) if (entries[i]) KJson_free(entries[i]);
		KJson_free(entries);
		return 0;
	}
	
	/* Compose the output array. */
	*out='[';
	ptr=out+1;*ptr=0;
	for (i=0;i<numentries;i++)
	{
		strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
		if (i!=numentries-1) {*ptr++=',';if(fmt)*ptr++=' ';*ptr=0;}
		KJson_free(entries[i]);
	}
	KJson_free(entries);
	*ptr++=']';*ptr++=0;
	return out;	
}

/* Build an object from the text. */
static const char *parse_object(KJson *item,const char *value)
{
	KJson *child;
	if (*value!='{')	{ep=value;return 0;}	/* not an object! */
	
	item->type=KJson_Object;
	value=skip(value+1);
	if (*value=='}') return value+1;	/* empty array. */
	
	item->child=child=KJson_New_Item();
	if (!item->child) return 0;
	value=skip(parse_string(child,skip(value)));
	if (!value) return 0;
	child->key=child->valuestring;child->valuestring=0;
	if (*value!=':') {ep=value;return 0;}	/* fail! */
	value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
	if (!value) return 0;
	
	while (*value==',')
	{
		KJson *new_item;
		if (!(new_item=KJson_New_Item()))	return 0; /* memory fail */
		child->next=new_item;new_item->prev=child;child=new_item;
		value=skip(parse_string(child,skip(value+1)));
		if (!value) return 0;
		child->key=child->valuestring;child->valuestring=0;
		if (*value!=':') {ep=value;return 0;}	/* fail! */
		value=skip(parse_value(child,skip(value+1)));	/* skip any spacing, get the value. */
		if (!value) return 0;
	}
	
	if (*value=='}') return value+1;	/* end of array */
	ep=value;return 0;	/* malformed. */
}

/* Render an object to text. */
static char *print_object(KJson *item,int depth,int fmt)
{
	char **entries=0,**names=0;
	char *out=0,*ptr,*ret,*str;int len=7,i=0,j;
	KJson *child=item->child;
	int numentries=0,fail=0;
	/* Count the number of entries. */
	while (child) numentries++,child=child->next;
	/* Allocate space for the names and the objects */
	entries=(char**)KJson_malloc(numentries*sizeof(char*));
	if (!entries) return 0;
	names=(char**)KJson_malloc(numentries*sizeof(char*));
	if (!names) {KJson_free(entries);return 0;}
	memset(entries,0,sizeof(char*)*numentries);
	memset(names,0,sizeof(char*)*numentries);

	/* Collect all the results into our arrays: */
	child=item->child;depth++;if (fmt) len+=depth;
	while (child)
	{
		names[i]=str=print_string_ptr(child->key);
		entries[i++]=ret=print_value(child,depth,fmt);
		if (str && ret) len+=strlen(ret)+strlen(str)+2+(fmt?2+depth:0); else fail=1;
		child=child->next;
	}
	
	/* Try to allocate the output string */
	if (!fail) out=(char*)KJson_malloc(len);
	if (!out) fail=1;

	/* Handle failure */
	if (fail)
	{
		for (i=0;i<numentries;i++) {if (names[i]) KJson_free(names[i]);if (entries[i]) KJson_free(entries[i]);}
		KJson_free(names);KJson_free(entries);
		return 0;
	}
	
	/* Compose the output: */
	*out='{';ptr=out+1;if (fmt)*ptr++='\n';*ptr=0;
	for (i=0;i<numentries;i++)
	{
		if (fmt) for (j=0;j<depth;j++) *ptr++='\t';
		strcpy(ptr,names[i]);ptr+=strlen(names[i]);
		*ptr++=':';if (fmt) *ptr++='\t';
		strcpy(ptr,entries[i]);ptr+=strlen(entries[i]);
		if (i!=numentries-1) *ptr++=',';
		if (fmt) *ptr++='\n';*ptr=0;
		KJson_free(names[i]);KJson_free(entries[i]);
	}
	
	KJson_free(names);KJson_free(entries);
	if (fmt) for (i=0;i<depth-1;i++) *ptr++='\t';
	*ptr++='}';*ptr++=0;
	return out;	
}

/* Utility for array list handling. */
static void suffix_object(KJson *prev,KJson *item) {prev->next=item;item->prev=prev;}
/* Utility for handling references. */
static KJson *create_reference(KJson *item) {KJson *ref=KJson_New_Item();if (!ref) return 0;memcpy(ref,item,sizeof(KJson));ref->key=0;ref->type|=KJson_IsReference;ref->next=ref->prev=0;return ref;}


/* Parse an object - create a new root, and populate. */
KJson *KJson::Parse(const char *value)
{
	KJson *c=KJson_New_Item();
	ep=0;
	if (!c) return 0;       /* memory fail */

	if (!parse_value(c,skip(value))) {Delete(c);return 0;}
	return c;
}

/* Render a KJson item/entity/structure to text. */
char *KJson::Print(KJson *item)				{return print_value(item,0,1);}
char *KJson::PrintUnformatted(KJson *item)	{return print_value(item,0,0);}


/* Get Array size/item / object item. */
int    KJson::GetArraySize(KJson *array)							{KJson *c=array->child;int i=0;while(c)i++,c=c->next;return i;}
KJson *KJson::GetArrayItem(KJson *array,int item)				{KJson *c=array->child;  while (c && item>0) item--,c=c->next; return c;}
KJson *KJson::GetObjectItem(KJson *object,const char *akey)	{KJson *c=object->child; while (c && KJson_strcasecmp(c->key,akey)) c=c->next; return c;}

/* Add item to array/object. */
void   KJson::AddItemToArray(KJson *array, KJson *item)						{KJson *c=array->child;if (!item) return; if (!c) {array->child=item;} else {while (c && c->next) c=c->next; suffix_object(c,item);}}
void   KJson::AddItemToObject(KJson *object,const char *akey,KJson *item)	{if (!item) return; if (item->key) KJson_free(item->key);item->key=KJson_strdup(akey);AddItemToArray(object,item);}
void	KJson::AddItemReferenceToArray(KJson *array, KJson *item)						{AddItemToArray(array,create_reference(item));}
void	KJson::AddItemReferenceToObject(KJson *object,const char *akey,KJson *item)	{AddItemToObject(object,akey,create_reference(item));}

KJson *KJson::DetachItemFromArray(KJson *array,int which)			{KJson *c=array->child;while (c && which>0) c=c->next,which--;if (!c) return 0;
	if (c->prev) c->prev->next=c->next;if (c->next) c->next->prev=c->prev;if (c==array->child) array->child=c->next;c->prev=c->next=0;return c;}
void   KJson::DeleteItemFromArray(KJson *array,int which)			{Delete(DetachItemFromArray(array,which));}
KJson *KJson::DetachItemFromObject(KJson *object,const char *akey) {int i=0;KJson *c=object->child;while (c && KJson_strcasecmp(c->key,akey)) i++,c=c->next;if (c) return DetachItemFromArray(object,i);return 0;}
void   KJson::DeleteItemFromObject(KJson *object,const char *akey) {Delete(DetachItemFromObject(object,akey));}

/* Replace array/object items with new ones. */
void   KJson::ReplaceItemInArray(KJson *array,int which,KJson *newitem)		{KJson *c=array->child;while (c && which>0) c=c->next,which--;if (!c) return;
	newitem->next=c->next;newitem->prev=c->prev;if (newitem->next) newitem->next->prev=newitem;
	if (c==array->child) array->child=newitem; else newitem->prev->next=newitem;c->next=c->prev=0;Delete(c);}
void   KJson::ReplaceItemInObject(KJson *object,const char *akey,KJson *newitem){int i=0;KJson *c=object->child;while(c && KJson_strcasecmp(c->key,akey))i++,c=c->next;if(c){newitem->key=KJson_strdup(akey);ReplaceItemInArray(object,i,newitem);}}

/* Create basic types: */
KJson *KJson::CreateNull()						{KJson *item=KJson_New_Item();if(item)item->type=KJson_NULL;return item;}
KJson *KJson::CreateTrue()						{KJson *item=KJson_New_Item();if(item)item->type=KJson_True;return item;}
KJson *KJson::CreateFalse()						{KJson *item=KJson_New_Item();if(item)item->type=KJson_False;return item;}
KJson *KJson::CreateBool(int b)					{KJson *item=KJson_New_Item();if(item)item->type=b?KJson_True:KJson_False;return item;}
KJson *KJson::CreateNumber(double num)			{KJson *item=KJson_New_Item();if(item){item->type=KJson_Number;item->valuedouble=num;item->valueint=(int)num;}return item;}
KJson *KJson::CreateString(const char *akey)	{KJson *item=KJson_New_Item();if(item){item->type=KJson_String;item->valuestring=KJson_strdup(akey);}return item;}
KJson *KJson::CreateArray()						{KJson *item=KJson_New_Item();if(item)item->type=KJson_Array;return item;}
KJson *KJson::CreateObject()						{KJson *item=KJson_New_Item();if(item)item->type=KJson_Object;return item;}

/* Create Arrays: */
KJson *KJson::CreateIntArray(int *numbers,int count)				{int i;KJson *n=0,*p=0,*a=CreateArray();for(i=0;a && i<count;i++){n=CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
KJson *KJson::CreateFloatArray(float *numbers,int count)			{int i;KJson *n=0,*p=0,*a=CreateArray();for(i=0;a && i<count;i++){n=CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
KJson *KJson::CreateDoubleArray(double *numbers,int count)		{int i;KJson *n=0,*p=0,*a=CreateArray();for(i=0;a && i<count;i++){n=CreateNumber(numbers[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}
KJson *KJson::CreateStringArray(const char **strings,int count)	{int i;KJson *n=0,*p=0,*a=CreateArray();for(i=0;a && i<count;i++){n=CreateString(strings[i]);if(!i)a->child=n;else suffix_object(p,n);p=n;}return a;}

void KJson::AddNullToObject(KJson* object,const char* name){return AddItemToObject(object, name, CreateNull());}	
void KJson::AddTrueToObject(KJson* object,const char* name){  return AddItemToObject(object, name, CreateTrue());}	
void KJson::AddFalseToObject(KJson* object,const char* name){return AddItemToObject(object, name, CreateFalse());}		
void KJson::AddNumberToObject(KJson* object,const char* name,double n){return AddItemToObject(object, name, CreateNumber(n)); }
void KJson::AddStringToObject(KJson* object,const char* name,const char* s){  return AddItemToObject(object, name, CreateString(s));}	
