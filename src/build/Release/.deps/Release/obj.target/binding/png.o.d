cmd_Release/obj.target/binding/png.o := cc '-DNODE_GYP_MODULE_NAME=binding' '-DUSING_UV_SHARED=1' '-DUSING_V8_SHARED=1' '-DV8_DEPRECATION_WARNINGS=1' '-D_LARGEFILE_SOURCE' '-D_FILE_OFFSET_BITS=64' '-DBUILDING_NODE_EXTENSION' -I/home/haojing/.node-gyp/10.0.0/include/node -I/home/haojing/.node-gyp/10.0.0/src -I/home/haojing/.node-gyp/10.0.0/deps/uv/include -I/home/haojing/.node-gyp/10.0.0/deps/v8/include -I../third-party/libpng -I../third-party/jpeg  -fPIC -pthread -Wall -Wextra -Wno-unused-parameter -m64 -O3 -fno-omit-frame-pointer  -MMD -MF ./Release/.deps/Release/obj.target/binding/png.o.d.raw   -c -o Release/obj.target/binding/png.o ../png.c
Release/obj.target/binding/png.o: ../png.c ../png.h \
 ../third-party/libpng/png.h ../third-party/libpng/pnglibconf.h \
 ../third-party/libpng/pngconf.h ../image.h ../http.h
../png.c:
../png.h:
../third-party/libpng/png.h:
../third-party/libpng/pnglibconf.h:
../third-party/libpng/pngconf.h:
../image.h:
../http.h:
