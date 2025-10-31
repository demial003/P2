Hello, our spell checker has been broken down into a few key functions. 
The first major part of our project looks at the type of inputs we are receiving. 
It judges whether we are looking at a file or a directory, and after doing so, it begins to read in the files. 
Then it reads from the dictonary file and puts all the words in the dictionary.txt into our words char array, 
which we use to relation to key functions like Bsearch, and qsort. 
We then filter the words from our test file to grab only valid inputs, which we further test, checking for capitalization, trailing special characters, etc. 
In doing so, we check to see if the word exists within our dictonary and confirming the case sensitive part of the capitalization portion of the project. 
Our tests consists of two important tests, the first being a song by Don Mclean called 
Vincent where we have deliberately placed words that would cause issues. 
Those issues are with capitalization, words that contain digits but are not in the dictonary, 
words inside of special characters, and strings of letters with special characters inside of them.

test command : ./spell -s .bar testing/dict.txt testing/files/ 