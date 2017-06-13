voodooi2c-client:
	g++ VoodooI2CClient.cc updd.cc -o voodooi2c-client -std=c++11 -ldl "/usr/local/lib/libupddapi.1.0.0.dylib" -framework CoreGraphics -framework CoreFoundation

.PHONY: clean

clean:
	rm -f voodooi2c-client
