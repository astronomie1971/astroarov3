
// definition des GPIO utlisées pour les 3 relais et les niveaux on/off
//#define Relay_1_GPIOPIN 25
//#define Relay_2_GPIOPIN 28
//#define Relay_3_GPIOPIN 29


#define Relay_1_GPIOPIN 25
#define Relay_2_GPIOPIN 29
#define Relay_3_GPIOPIN 28
#define Relay_4_GPIOPIN 24
#define Relay_5_GPIOPIN 22
#define Relay_6_GPIOPIN 27
#define Relay_7_GPIOPIN 26
#define Relay_8_GPIOPIN 23 

#define Relay_On  0x00
#define Relay_Off 0x01


// function prototypes
void relay_init(void);
