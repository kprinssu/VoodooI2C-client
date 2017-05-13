voodooi2c-client:
	gcc VoodooI2CClient.c updd.c mouse.c -o voodooi2c-client -ldl "/usr/local/lib/libupddapi.1.0.0.dylib" -framework CoreGraphics -framework CoreFoundation

.PHONY: clean

clean:
	rm -f voodooi2c-client