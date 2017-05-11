voodooi2c-client:
	gcc VoodooI2CClient.c updd.c -o voodooi2c-client -ldl "/usr/local/lib/libupddapi.1.0.0.dylib"

.PHONY: clean

clean:
	rm -f voodooi2c-client