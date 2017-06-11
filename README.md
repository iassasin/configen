# Configen
Preprocess your own configs or sources and translate them to target text or code

## Simple
Configen use simple syntax. It's looks like a bash or other shell scripts, but more predictable and simple exactly for preprocessing.

For example, you need to out a string, so just write it:
```
"My awesome string
"New line
```
There is no trailing `"`, so new line will be added automatically

### Functions

Need functions and variables? Just use it:
```
write | arg1 arg2 {
	"I wrote $arg1 $arg2
}

write hello, world
```
Code above will produce a string `I wrote hello, world`. Spaces is delimeter for arguments. To pass arguments with spaces just use single or double qoutes

### Custom syntax
Use regular expressions to provide your intuitive syntax
```
site |~ (https?)://([^/]*)(/.*) {
	"Protocol: $1
	"Host: $2
	"Path: $3
}

site http://example.com/
```

# Requirements
Developed on gcc 5.4 using C++14. Any compiler with C++14 standart support should work.  
Configen uses standart library only.

# Build
To build configen clone repository then use this commands in terminal:
```
cd configen
mkdir Release
cd Release
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

To speed up the build process use `make -j4`, where `4` - your CPU cores count.
