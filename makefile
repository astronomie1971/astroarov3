gm2000: main.c  relay.c dht22.c
	gcc  `pkg-config --cflags gtk+-3.0`    -rdynamic  -I/usr/local/include -L/usr/local/lib   -lwiringPi    main.c   relay.c  dht22.c   `pkg-config   --libs gtk+-3.0`  -o astroaro  -lcurl
