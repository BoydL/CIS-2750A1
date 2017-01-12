Lucas Boyd             0795339
CIS 2750              Assignment One

***************
Program summary:
***************
This Library creates a hash table which stores Parameters entered by the user. The program stores
these values by using the function PM_manage. this function takes in the hash table as an argument
as well as the perameter name, type, and weather or not it is required. This will then store that
parameter in the hashtable at a location determined by the hash value of the pname. Once all parameters
are entered, the library can initialize values by reading in a file with parameter name and what data
will be going in that particular name. The user then can use the getvalue and has value functions to 
see and retrieve values from a particullar parameter. the adata that can be entered can be of type
LIST, BOOLEAN, STRING, INT, and FLOAT. Collision handling for the hashtable is delt with through
chaining.

*************
compiling the program
*************
Got to appropriate directroy.
type make or make ParameterManager

*************
running the program
*************
Create an appropriate main and add my library.


*********
known limitations
*********
The program does not quite free all memory yet. I am getting a memory leak of 16Bytes for 1 free 
which I cannot fined which occurs when I make a list.

**********
Test file for example input
**********
#
# Sample configuration file for a1example.c
#

name   = "Bill Jones"; # whitespace is irrelavent outside quoted strings
id     =               # this comment is spurious, but is legal
         123;
regstat=true;          # no spaces here, but shouldn't matter

marks  = { "60",
           "70",
           "80",
           "75" };

# average is absent in this file - optional parameter in example

**********
Example output
**********
Parameters:
name        = Bill Jones
id          = 123
registered? = yes
marks       = 
60 70 80 75 
average     = not entered
1

