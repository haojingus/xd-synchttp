cmd_Release/http.a := rm -f Release/http.a && ./gyp-mac-tool filter-libtool libtool  -static -o Release/http.a Release/obj.target/libhttp/http.o
