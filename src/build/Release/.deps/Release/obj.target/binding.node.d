cmd_Release/obj.target/binding.node := g++ -shared -pthread -rdynamic -m64  -Wl,-soname=binding.node -o Release/obj.target/binding.node -Wl,--start-group Release/obj.target/binding/png.o Release/obj.target/binding/jpeg.o Release/obj.target/binding/gif.o Release/obj.target/binding/imagesize.o Release/obj.target/binding/binding.o Release/obj.target/http.a Release/obj.target/png.a Release/obj.target/jpeg.a Release/obj.target/zlib.a -Wl,--end-group 
