#include <gtk-3.0/gtk/gtk.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <time.h>  
#include <curl/curl.h>

#include "relay.h"





void relay_init(void){


        FILE* fichier = NULL;
	wiringPiSetup () ;

        if (setuid(getuid()) < 0)  {  	perror("Dropping privileges failed\n");	}

	pinMode (Relay_1_GPIOPIN, OUTPUT) ;
	pinMode (Relay_2_GPIOPIN, OUTPUT) ;
	pinMode (Relay_3_GPIOPIN, OUTPUT) ;
	pinMode (Relay_4_GPIOPIN, OUTPUT) ;
	pinMode (Relay_5_GPIOPIN, OUTPUT) ;
	pinMode (Relay_6_GPIOPIN, OUTPUT) ;
	pinMode (Relay_7_GPIOPIN, OUTPUT) ;
	pinMode (Relay_8_GPIOPIN, OUTPUT) ;
	
	
	// au boot le fichier boot.txt est creer ds le fichier etc/rc.local par execution du script coldboot.sh
     // si boot.txt present on eteint tout sauf mele
	fichier = fopen("boot.txt", "r");
	if (fichier != NULL)
	{
	
	// on eteint tout sauf  mele
	digitalWrite (Relay_1_GPIOPIN, Relay_Off);
	digitalWrite (Relay_2_GPIOPIN, Relay_On);
	digitalWrite (Relay_3_GPIOPIN, Relay_Off);
	digitalWrite (Relay_4_GPIOPIN, Relay_Off);

	digitalWrite (Relay_5_GPIOPIN, Relay_Off);
	digitalWrite (Relay_6_GPIOPIN, Relay_Off);
	digitalWrite (Relay_7_GPIOPIN, Relay_Off);
	digitalWrite (Relay_8_GPIOPIN, Relay_Off);
	
	// on delete le fichier 
	fclose(fichier);
	remove("boot.txt");
	 
	}
	else 
	{
	}

 
      
}

