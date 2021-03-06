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

//April 28, 2011.-----------------------------------------

This code/document was modified by Adam Cox on April 28 to incorporate 
this library into KData. All of the names, cJSON were changed to KJson in
order to make this easily integrate into the KData build system and to "naturally"
fit into the rest of the KData framework. All strings found in the code and 
in this documentation that are "KJson", were originally "cJSON".

If you want to compile a program that uses KJson, 
you don't need to do anything special. Just include the the header as normal.

Read below for the documentation on using KJson/cJSON. Also, check out the .h file
for a list of the available functions
https://edwdev-ik.fzk.de/kdata_dev_ref/src/KJson.h.html


//--------------------------------------------------------




Welcome to KJson. (AKA cJSON)

KJson aims to be the dumbest possible parser that you can get your job done with.
It's a single file of C, and a single header file.

JSON is described best here: http://www.json.org/
It's like XML, but fat-free. You use it to move data around, store things, or just
generally represent your program's state.

As a library, KJson exists to take away as much legwork as it can, but not get in your way.
As a point of pragmatism (i.e. ignoring the truth), I'm going to say that you can use it
in one of two modes: Auto and Manual. Let's have a quick run-through.


I lifted some JSON from this page: http://www.json.org/fatfree.html
That page inspired me to write KJson, which is a parser that tries to share the same
philosophy as JSON itself. Simple, dumb, out of the way.

Some JSON:
{
    "name": "Jack (\"Bee\") Nimble", 
    "format": {
        "type":       "rect", 
        "width":      1920, 
        "height":     1080, 
        "interlace":  false, 
        "frame rate": 24
    }
}

Assume that you got this from a file, a webserver, or magic JSON elves, whatever,
you have a char * to it. Everything is a KJson struct.
Get it parsed:
	KJson *root = KJson::Parse(my_json_string);

This is an object. We're in C. We don't have objects. But we do have structs.
What's the framerate?

	KJson *format = KJson::GetObjectItem(root,"format");
	int framerate = KJson::GetObjectItem(format,"frame rate")->valueint;


Want to change the framerate?
	KJson::GetObjectItem(format,"frame rate")->valueint=25;
	
Back to disk?
	char *rendered=KJson::Print(root);

Finished? Delete the root (this takes care of everything else).
	KJson::Delete(root);

That's AUTO mode. If you're going to use Auto mode, you really ought to check pointers
before you dereference them. If you want to see how you'd build this struct in code?
	KJson *root,*fmt;
	root=KJson::CreateObject();	
	KJson::AddItemToObject(root, "name", KJson::CreateString("Jack (\"Bee\") Nimble"));
	KJson::AddItemToObject(root, "format", fmt=KJson::CreateObject());
  KJson::AddItemToObject(fmt, "type", KJson::CreateString("type") );
  KJson::AddItemToObject(fmt, "width", KJson::CreateNumber(1920) );
  KJson::AddItemToObject(fmt, "height", KJson::CreateNumber(1080) );
  KJson::AddItemToObject(fmt, "interlace", KJson::CreateFalse() );
  KJson::AddItemToObject(fmt, "frame rate", KJson::CreateNumber(24) );
 

Hopefully we can agree that's not a lot of code? There's no overhead, no unnecessary setup.
Look at test.c for a bunch of nice examples, mostly all ripped off the json.org site, and
a few from elsewhere.

What about manual mode? First up you need some detail.
Let's cover how the KJson objects represent the JSON data.
KJson doesn't distinguish arrays from objects in handling; just type.
Each KJson has, potentially, a child, siblings, value, a name.

The root object has: Object Type and a Child
The Child has name "name", with value "Jack ("Bee") Nimble", and a sibling:
Sibling has type Object, name "format", and a child.
That child has type String, name "type", value "rect", and a sibling:
Sibling has type Number, name "width", value 1920, and a sibling:
Sibling has type Number, name "height", value 1080, and a sibling:
Sibling hs type False, name "interlace", and a sibling:
Sibling has type Number, name "frame rate", value 24

Here's the structure:
typedef struct KJson {
	struct KJson *next,*prev;
	struct KJson *child;

	int type;

	char *valuestring;
	int valueint;
	double valuedouble;

	char *key;
} KJson;

By default all values are 0 unless set by virtue of being meaningful.

next/prev is a doubly linked list of siblings. next takes you to your sibling,
prev takes you back from your sibling to you.
Only objects and arrays have a "child", and it's the head of the doubly linked list.
A "child" entry will have prev==0, but next potentially points on. The last sibling has next=0.
The type expresses Null/True/False/Number/String/Array/Object, all of which are #defined in
KJson.h

A Number has valueint and valuedouble. If you're expecting an int, read valueint, if not read
valuedouble.

Any entry which is in the linked list which is the child of an object will have a "key"
which is the "name" of the entry. When I said "name" in the above example, that's "key".
"key" is the JSON name for the 'variable name' if you will.

Now you can trivially walk the lists, recursively, and parse as you please.
You can invoke KJson::Parse to get KJson to parse for you, and then you can take
the root object, and traverse the structure (which is, formally, an N-tree),
and tokenise as you please. If you wanted to build a callback style parser, this is how
you'd do it (just an example, since these things are very specific):

void parse_and_callback(KJson *item,const char *prefix)
{
	while (item)
	{
		char *newprefix=malloc(strlen(prefix)+strlen(item->name)+2);
		sprintf(newprefix,"%s/%s",prefix,item->name);
		int dorecurse=callback(newprefix, item->type, item);
		if (item->child && dorecurse) parse_and_callback(item->child,newprefix);
		item=item->next;
		free(newprefix);
	}
}

The prefix process will build you a separated list, to simplify your callback handling.
The 'dorecurse' flag would let the callback decide to handle sub-arrays on it's own, or
let you invoke it per-item. For the item above, your callback might look like this:

int callback(const char *name,int type,KJson *item)
{
	if (!strcmp(name,"name"))	{ /* populate name */ }
	else if (!strcmp(name,"format/type")	{ /* handle "rect" */ }
	else if (!strcmp(name,"format/width")	{ /* 800 */ }
	else if (!strcmp(name,"format/height")	{ /* 600 */ }
	else if (!strcmp(name,"format/interlace")	{ /* false */ }
	else if (!strcmp(name,"format/frame rate")	{ /* 24 */ }
	return 1;
}

Alternatively, you might like to parse iteratively.
You'd use:

void parse_object(KJson *item)
{
	int i; for (i=0;i<KJson::GetArraySize(item);i++)
	{
		KJson *subitem=KJson::GetArrayItem(item,i);
		// handle subitem.	
	}
}

Or, for PROPER manual mode:

void parse_object(KJson *item)
{
	KJson *subitem=item->child;
	while (subitem)
	{
		// handle subitem
		if (subitem->child) parse_object(subitem->child);
		
		subitem=subitem->next;
	}
}

Of course, this should look familiar, since this is just a stripped-down version
of the callback-parser.

This should cover most uses you'll find for parsing. The rest should be possible
to infer.. and if in doubt, read the source! There's not a lot of it! ;)


In terms of constructing JSON data, the example code above is the right way to do it.
You can, of course, hand your sub-objects to other functions to populate.
Also, if you find a use for it, you can manually build the objects.
For instance, suppose you wanted to build an array of objects?

KJson *objects[24];

KJson *Create_array_of_anything(KJson **items,int num)
{
	int i;KJson *prev, *root=KJson::CreateArray();
	for (i=0;i<24;i++)
	{
		if (!i)	root->child=objects[i];
		else	prev->next=objects[i], objects[i]->prev=prev;
		prev=objects[i];
	}
	return root;
}
	
and simply: Create_array_of_anything(objects,24);

KJson doesn't make any assumptions about what order you create things in.
You can attach the objects, as above, and later add children to each
of those objects.

As soon as you call KJson::Print, it renders the structure to text.



Enjoy KJson!


- Dave Gamble, Aug 2009
