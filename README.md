# DynString
Dynamic String in `C`.

## Features

Some features implemented in `DynString` are:

  * fully dynamic string size (with buffer size multple of 256 bytes)
  * `sprintf` like syntax: `dynstr_printf(...)`
  * `puts` like syntax
  * compare between `dynstr` and `dynstr` or `dynstr` and `char*`
  * string splits (with char or string match)
  * string match
  * sub-string
  * string match all
  * char strip
  * iterators with line and col information 
  
In the  future the following feature will be added:

  * string search and replace
  * regex support

## Project import

The simple way to use this library in your project is to add it as `git submodule` or to download a static version as zip file from github.
To add this library as submodule to your git project do the follow:

```bash
cd YOUR_GIT_PROJECT/YOUR_SOURCE_FOLDER/
git submodule add https://github.com/Mandarancio/DynString.git
```

## Example

A simple example of usage is the follow:

```c
#include <stdio.h>
#include "DynString/dynstring.h"

int main(int    argc,
         char **argv)
{
	/**Initial string**/
	dynstr* str = dynstr_from("Hi, how are you?\nFine thanks, and you?\nI'm good!");
	printf("Original size %lu\n", str->size);

	/**Split in lines**/
	size_t n;
	dynstr** lines = dynstr_splitc(str, '\n', &n);
	printf("%lu lines\n", n);
	size_t i;
	for (i = 0; i < n; i++)
	{
    	printf("> %s\n", lines[i]->data);
	}
	/**Strip \n **/
	dynstr_strip(str, '\n');
	printf("%s (%lu)\n", str->data, str->size);
	return 0;
}
```

The output of this simple code is the following:

```
Original size 48
3 lines
> Hi, how are you?
> Fine thanks, and you?
> I'm good!
Hi, how are you?Fine thanks, and you?I'm good! (46)

```
