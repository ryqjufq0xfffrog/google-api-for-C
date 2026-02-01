# google-api-for-C
Google OAuth2 client written in C. 

# Dependencies: 
This program requires [json-c](https://github.com/json-c/json-c) and [libcurl](https://curl.se/libcurl/).
Install them, and if you get some linking errors, you might want to edit `INCDIR` in Makefile.

# Building:
run
```
autoreconf -i
mkdir build
cd build
../configure
make
```
at the top dir.

# Running sample(s)
After building, a sample will be created at `/build/src/samples/sample`.
Run it with `sample -c credentials.json`.
You need a `credentials.json` with type "web".
This tutorial ([Create authorization credentials](https://developers.google.com/identity/protocols/oauth2/web-server#creatingcred)) may help you creating credentials.
