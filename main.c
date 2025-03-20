

 //  !!!!!!!!!  les GPIO ne sont pas 5V tolerant seulement du 3.3V
 // GPIO O et 21 sont HS

// sendemail -f astroaro2014@gmail.com -t eric.tinlot@gmail.com -u subject -m "message" -s smtp.gmail.com:587 -o tls=yes -xu astroaro2014@gmail.com -xp Etoile1971 



#include <gtk/gtk.h>
#include <gtk/gtkx.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "relay.h"
#include "dht22.h"


#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <sys/socket.h>

#define MAX 80
#define PORT 3492
#define SA struct sockaddr

// define pour_ les labels des buttosns
#define relay_label1  "Monture"
#define relay_label2  "Mini PC "
#define relay_label3  "Camera"
#define relay_label4  "----------" 
#define relay_label5  "----------"
#define relay_label6  "----------"
#define relay_label7  "----------"
#define relay_label8  "Hard Mount"

// lorsque le toit se ferme le 5V apparait sur la pin
#define PIN_ROOF  2
#define ROOF_OPENED 1
#define ROOF_CLOSED 0

#define MOUNT_ON 1
#define MOUNT_OFF 0

 int statut_roof = ROOF_CLOSED;
 int statut_mount = MOUNT_OFF;
 
 float humidity = 0.0;
 float temp_deg = 0.0;
 float temp_far = 0.0;
 
 
 

 int sockfd;
 char buff[MAX];
 float heure_de_fin_de_session = 12.0;
 
 // flag de fin de session mis a un qd on save le temps de fin
 int flag_heure_fin_de_session = FALSE;
 // flag pour heure de fin de session
 int flag_soir1=55; int flag_soir2=55;
 
    GtkBuilder *builder;
   

    // main windows widgets
    GtkWidget *window;
    GtkWidget *fixed1;
    GtkWidget *button_relay_1, *button_relay_2, *button_relay_3 , *button_relay_4;
    GtkWidget *button_relay_5, *button_relay_6, *button_relay_7 , *button_relay_8;
    GtkWidget *button_mount_hardware_on  , *button_mount_software_on , *button_mount_software_off , *button_park, *button_unpark;
    GtkWidget *button_start_session,*button_stop_session,*button_save_time, *button_clear_log;
    GtkWidget *label,*label_mount_status, *label_start,*label_time_saved,*label_time,*label_roof_status,*label_temperature,*label_humidity;
    GtkWidget *textlog;
    GtkWidget *textbuffer1,*entrybuffer1;
    GtkWidget *time_to_stop;

    // init des couleurs des bouton on off 
    GdkRGBA color_on,color_off,color_black,color_red,color_green;





// fonction pour parker a la mise sous tension apres 80 sec
// executer qu'une fois

static gboolean on_timeout_80sec(gpointer user_data){

	writelog ("Command Park Mount after startup requested");
	system("echo \"#:KA#\" | nc -N 192.168.1.122 3492");
	writelog ("System ready to go");
	return G_SOURCE_REMOVE; /* or G_SOURCE_REMOVE when you want to stop */
}


static gboolean on_timeout_button_on_off_mount(gpointer user_data){

	  digitalWrite (Relay_8_GPIOPIN, Relay_Off);
	  writelog("Edges Generation completed");
	
	return G_SOURCE_REMOVE; /* or G_SOURCE_REMOVE when you want to stop */
}


// call back button 
void on_relais1_clicked(GtkButton *b)
{
	
   
   int a = digitalRead (Relay_1_GPIOPIN);
   if (a == 1) {  digitalWrite (Relay_1_GPIOPIN, Relay_On);     writelog("Mount Hardware Power On"); }
   if (a == 0) {  digitalWrite (Relay_1_GPIOPIN, Relay_Off);    writelog("Mount Hardware Power Off"); }

}

void on_relais2_clicked(GtkButton *b)
{
     
   int a = digitalRead (Relay_2_GPIOPIN);
   if (a == 1) {  digitalWrite (Relay_2_GPIOPIN, Relay_On);  writelog("MiniPC On"); }
   if (a == 0) {  digitalWrite (Relay_2_GPIOPIN, Relay_Off); writelog("MiniPC Off");  }

}

void on_relais3_clicked(GtkButton *b)
{
   int a = digitalRead (Relay_3_GPIOPIN);
   if (a == 1) {  digitalWrite (Relay_3_GPIOPIN, Relay_On);   writelog("Camera On"); }
   if (a == 0) {  digitalWrite (Relay_3_GPIOPIN, Relay_Off);  writelog("Camera Off"); }
    

}
void on_relais4_clicked(GtkButton *b)
{
   int a = digitalRead (Relay_4_GPIOPIN);
   if (a == 1) {  digitalWrite (Relay_4_GPIOPIN, Relay_On);  }
   if (a == 0) {  digitalWrite (Relay_4_GPIOPIN, Relay_Off);   }
    

}
void on_relais5_clicked(GtkButton *b)
{
   
       int a = digitalRead (Relay_5_GPIOPIN);
   if (a == 1) {  digitalWrite (Relay_5_GPIOPIN, Relay_On);  }
   if (a == 0) {  digitalWrite (Relay_5_GPIOPIN, Relay_Off);   }
}

void on_relais6_clicked(GtkButton *b)
{        
int a = digitalRead (Relay_6_GPIOPIN);
   if (a == 1) {  digitalWrite (Relay_6_GPIOPIN, Relay_On);  }
   if (a == 0) {  digitalWrite (Relay_6_GPIOPIN, Relay_Off);   }
}

void on_relais7_clicked(GtkButton *b)
{    
    int a = digitalRead (Relay_7_GPIOPIN);
   if (a == 1) {  digitalWrite (Relay_7_GPIOPIN, Relay_On);  }
   if (a == 0) {  digitalWrite (Relay_7_GPIOPIN, Relay_Off);   }
}


//  allumage et extinction de la monture par les relais
void on_relais8_clicked(GtkButton *b){
      writelog("Mount Edge Sarted");

 digitalWrite (Relay_8_GPIOPIN, Relay_On);
     g_timeout_add(3000, on_timeout_button_on_off_mount,button_relay_8);

 if (statut_mount == MOUNT_OFF) 
 {
    
     statut_mount = MOUNT_ON;
     writelog ("commmande ON envoyée wait 1 min");
 }else 
 {
      
     statut_mount = MOUNT_OFF;
     writelog( "commmande OFF envoyée wait 1 min");
 }
}




void writelog(char message[100]){  
   
    FILE* fichier = NULL;
    char mess[200];

    gchar *text;
    

    time_t timestamp = time( NULL );
    struct tm * now = localtime( & timestamp );

   

    sprintf(mess, "%04d/%02d/%02d - %02d:%02d:%02d  %s\n",now->tm_year+1900, now->tm_mon+1, now->tm_mday, now->tm_hour,now->tm_min,now->tm_sec,message);
    text = g_convert (mess, -1, "ISO-8859-1", "UTF-8", NULL, NULL, NULL);
    gtk_text_buffer_insert_at_cursor (textbuffer1, text, strlen(text));

    
    fichier = fopen("mail.txt", "a");
    fprintf(fichier, " %s\n",mess);
    fclose(fichier);

}

// save time
void on_button_save_time_clicked(GtkButton *b)
{
    
    
     const gchar *buffer;
      
     buffer = gtk_entry_get_text(GTK_ENTRY(time_to_stop));
     buffer = g_convert (buffer, -1, "UTF-8", "ISO-8859-1", NULL, NULL, NULL);
   
  
     // test si format est xx:xx
   if (buffer[0]>47 && buffer[0]<58 && buffer[1]>47 && buffer[1]<58 && buffer[3]>47 && buffer[3]<58 && buffer[4]>47 && buffer[4]<58 && buffer[2]==':')
	{
	
	int h = (int)buffer[0] - 48;
	int i = (int)buffer[1] - 48;
	int j = (int)buffer[3] - 48;
	int k = (int)buffer[4] - 48;

	heure_de_fin_de_session =(h*10)+ i + (j+k/10.0)/6.0;
	
 
	flag_heure_fin_de_session = TRUE;
        writelog ("Heure de fin de session enregistrée");
       
	
	
	 buffer = gtk_entry_get_text(GTK_ENTRY(time_to_stop));
	 gtk_label_set_text(GTK_LABEL(label_time_saved),(const gchar*) buffer);
       } 
	else { writelog ("Erreur dans le format de l'heure....  Entrez par ex 23:56 pour 23h56mn");}

}




//  bouton de demarrage de session
void on_start_session_clicked(GtkButton *b)
{
    writelog("Start session on going");	
    writelog("Mount Power ON");
    digitalWrite (Relay_1_GPIOPIN, Relay_On); 
    
    writelog("Power ON - wait 10 sec");  
    // wait 10 sec
    sleep(10); 
    writelog ("Edges Generation - 3 sec started");
    // on genere le creneau pour demarrer la monture
    digitalWrite (Relay_8_GPIOPIN, Relay_On);
    g_timeout_add(3000, on_timeout_button_on_off_mount,button_relay_8);
   
     
    // on demarre le minipc 
    digitalWrite (Relay_2_GPIOPIN, Relay_On);
    
    // on demarre la camera
    digitalWrite (Relay_3_GPIOPIN, Relay_On);
    writelog ("Wait 1 min for mount ready");
    writelog("Start session completed");	
    gtk_label_set_text(GTK_LABEL(label_start),(const gchar*) "session started");
    g_timeout_add(80000, on_timeout_80sec,NULL);
}




//  bouton de fin de session
void on_stop_session_clicked(GtkButton *b)
{
    writelog("Stop session on going");
    
    // on park et on attends 15sec
     writelog("Park Requested - Wait 30 sec");
     system("echo \"#:KA#\" | nc -N 192.168.1.122 3492");	
     sleep(30);
     
     writelog("Mount Hardware Power OFF");
     digitalWrite (Relay_1_GPIOPIN, Relay_Off); 
    
     //on stop la camera
     digitalWrite (Relay_3_GPIOPIN, Relay_Off);
    writelog("Stop session Completed");
    gtk_label_set_text(GTK_LABEL(label_start),(const gchar*) "session stopped");
}



void on_button_clear_log_clicked (GtkButton *b)
{
  
  // on clear la fenetre de log
  gtk_text_buffer_set_text (  textbuffer1,  "" , 0);
	
	
}




// mise a jour de la couleur des relais

static gboolean on_timeout_button_relay1(gpointer user_data){

	int a = digitalRead (Relay_1_GPIOPIN);
	if (a == 1) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_off); }	   
	if (a == 0) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_on); }
	return G_SOURCE_CONTINUE; /* or G_SOURCE_REMOVE when you want to stop */
}

static gboolean on_timeout_button_relay2(gpointer user_data){

	int a = digitalRead (Relay_2_GPIOPIN);
	if (a == 1) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_off); }	   
	if (a == 0) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_on); }
	return G_SOURCE_CONTINUE; /* or G_SOURCE_REMOVE when you want to stop */
}

static gboolean on_timeout_button_relay3(gpointer user_data){

	int a = digitalRead (Relay_3_GPIOPIN);
	if (a == 1) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_off); }	   
	if (a == 0) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_on); }
	return G_SOURCE_CONTINUE; /* or G_SOURCE_REMOVE when you want to stop */
}

static gboolean on_timeout_button_relay4(gpointer user_data){

	int a = digitalRead (Relay_4_GPIOPIN);
	if (a == 1) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_off); }	   
	if (a == 0) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_on); }
	return G_SOURCE_CONTINUE; /* or G_SOURCE_REMOVE when you want to stop */
}

static gboolean on_timeout_button_relay5(gpointer user_data){

	int a = digitalRead (Relay_5_GPIOPIN);
	if (a == 1) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_off); }	   
	if (a == 0) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_on); }
	return G_SOURCE_CONTINUE; /* or G_SOURCE_REMOVE when you want to stop */
}

static gboolean on_timeout_button_relay6(gpointer user_data){

	int a = digitalRead (Relay_6_GPIOPIN);
	if (a == 1) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_off); }	   
	if (a == 0) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_on); }
	return G_SOURCE_CONTINUE; /* or G_SOURCE_REMOVE when you want to stop */
}

static gboolean on_timeout_button_relay7(gpointer user_data){

	int a = digitalRead (Relay_7_GPIOPIN);
	if (a == 1) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_off); }	   
	if (a == 0) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_on); }
	return G_SOURCE_CONTINUE; /* or G_SOURCE_REMOVE when you want to stop */
}

static gboolean on_timeout_button_relay8(gpointer user_data){

	int a = digitalRead (Relay_8_GPIOPIN);
	if (a == 1) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_off); }	   
	if (a == 0) { gtk_widget_override_background_color(GTK_WIDGET(user_data), GTK_STATE_FLAG_NORMAL,&color_on); }
	return G_SOURCE_CONTINUE; /* or G_SOURCE_REMOVE when you want to stop */
}



// mise a jour de l'heure et verif si heure de ferme session

static gboolean on_timeout_updatetime(gpointer user_data){
  
   time_t timestamp = time( NULL );
   struct tm * now = localtime( & timestamp );
   char mess_update[30];
   float heure_actuelle;
   
   timestamp = time( NULL );
   now = localtime( & timestamp );
   
   heure_actuelle=now->tm_hour+now->tm_min/60.0+now->tm_sec/3600.0;
   
   sprintf(mess_update, "%02d:%02d:%02d", now->tm_hour,now->tm_min,now->tm_sec);
   gtk_label_set_text(label_time,mess_update);
   

   heure_actuelle = now->tm_hour+now->tm_min/60.0;
  
   if (flag_heure_fin_de_session == TRUE) 
   { 
	// fprintf(stderr,"\n avant test heure actuelle %f , heure session %f",heure_actuelle,heure_de_fin_de_session);
	// si debut de soiree
	if (heure_actuelle > 12.00) { flag_soir1=1;}  	else   { flag_soir1=0;}
	if (heure_de_fin_de_session > 12.00) { flag_soir2=1;}  	else   { flag_soir2=0;}
	
	 // si les deux heures sont le soir
	if (flag_soir1==1 && flag_soir2==1 && heure_actuelle>=heure_de_fin_de_session)
	{  
		  gtk_label_set_text(label_time_saved, "00:00");
		 flag_heure_fin_de_session=FALSE; 
	 Time_out_session();
	  
	}
	if (flag_soir1==0 && flag_soir2==0 && heure_actuelle>=heure_de_fin_de_session)
	{  
		  gtk_label_set_text(label_time_saved, "00:00");
		 flag_heure_fin_de_session=FALSE; 
	 Time_out_session();
	 
	}
	 
   }
   return G_SOURCE_CONTINUE; /* or G_SOURCE_REMOVE when you want to stop */
   
}

void func(int sockfd)
{
   
     buff[0]='#';
   
     buff[1]=':';
     buff[2]='G';
     buff[3]='s';
     buff[4]='t';
     buff[5]='a';
     buff[6]='t';
     buff[7]='#';
     buff[8]='\n';
     
  printf("\n From moi : %s", buff);
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff));
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);
      
    
}



void Time_out_session(){
	
     // cette fonction est appelée qd l'heure de tout fermee est arrivée
	
	
	 
    // on regarde si la monture est allumée, si oui on park la monture 
    // si eteint ca bloque le soft donc on ne fait rien
    int a = digitalRead (Relay_1_GPIOPIN);
    if (a == 0) 
	{         
		writelog ("Command Park Mount requested");
		system("echo \"#:KA#\" | nc -N 192.168.1.122 3492");
		writelog ("Command Park Mount CWD ongoing");
		sleep(15);
        }
     // on coupe la monture  
     digitalWrite (Relay_1_GPIOPIN, Relay_Off);
     writelog("Mount Hardware Power Off");
	
     // on coupe la camera et focuser
     digitalWrite (Relay_3_GPIOPIN, Relay_Off);
     writelog ("Camera Off");
     writelog ("session completed");
     system("ssmtp astroaro2024@Gmail.com  < mail.txt ");
}










// menu exit
void on_menu_exit_activate(GtkMenu *m) {
   gtk_main_quit();
}

	

// timer 5 sec utilise pour la gestion du toit 
static gboolean on_timeout_5000ms(gpointer user_data){
        // P21 en entree pour lecture status toît
    	  
		
	int a = digitalRead (PIN_ROOF);
//	fprintf(stderr,"\n valeur de a %d",a);
	//lorsque le toit se ferme
	
	if (statut_roof == ROOF_CLOSED) 
	{  
		gtk_label_set_text(GTK_LABEL(label_roof_status),(const gchar*) "Roof Closed"); 
		gtk_widget_override_color (label_roof_status, GTK_STATE_NORMAL, &color_red);
	}	
	
	
	if (statut_roof == ROOF_OPENED && a==ROOF_CLOSED){ 
		statut_roof =  ROOF_CLOSED;
		// action park monture
	//	system("echo \"#:KA#\" | nc -N 192.168.1.122 3492"); 
		gtk_label_set_text(GTK_LABEL(label_roof_status),(const gchar*) "Roof Closed");
		 gtk_widget_override_color (label_roof_status, GTK_STATE_NORMAL, &color_red);
	writelog("Roof Closed"); 
	system("ssmtp astroaro2024@Gmail.com  < roofclosed.txt ");
		
	}
	else if (statut_roof == ROOF_CLOSED && a==ROOF_OPENED){ 
		statut_roof =  ROOF_OPENED; 
		writelog("Roof Opened ---> no action"); 
		gtk_label_set_text(GTK_LABEL(label_roof_status),(const gchar*) "Roof Opened");
		gtk_widget_override_color (label_roof_status, GTK_STATE_NORMAL, &color_green);
		system("ssmtp astroaro2024@Gmail.com  < roofopen.txt ");
	}
	
	
	
	
	
	 char mess[30];
     	
   read_dht_data(DHT_PIN_INT,&temp_deg,&temp_far,&humidity);
 
   if (temp_deg !=0.0 && humidity!=0.0) 
   {
        sprintf(mess,"Temperature : %-3.1f°c",temp_deg);
        gtk_label_set_text(label_temperature,mess);

        sprintf(mess,"Humidity    : %-3.1f%%",humidity);
        gtk_label_set_text(label_humidity,mess); 
   }
   

	
	
	return G_SOURCE_CONTINUE;
	} 


static gboolean on_timeout_relay8(gpointer user_data){
       digitalWrite (Relay_8_GPIOPIN, Relay_Off);
	return G_SOURCE_REMOVE;
	} 




//  allumage et extinction de la monture par le reseau
void button_mount_software_power_on_clicked_cb(GtkButton *b)
    
{
     gtk_label_set_text(GTK_LABEL(label),(const gchar*) "Mount On");
     system("sudo etherwake -b 00:C0:08:AA:C8:68");
}


void button_mount_software_power_off_clicked_cb(GtkButton *b)
{
     writelog("Mount power off via network");
     system("echo \" #:shutdown# \" | nc -N 192.168.1.122 3492");
}
  
//  Park et unpark de la monture
void button_park_clicked_cb(GtkButton *b)
{
     writelog("Park Requested");
     system("echo \"#:KA#\" | nc -N 192.168.1.122 3492");
}

 

void button_unpark_clicked_cb(GtkButton *b)
{
     writelog("Unpark requested");
     system("echo \"#:PO#\" | nc -N 192.168.1.122 3492");
}

void logfile_init()
{
     // on cree un nouveau fichier
     remove("mail.txt");
     
     FILE* fichier = NULL;
     
     time_t timestamp = time( NULL );
     struct tm * now = localtime( & timestamp );
     
     fichier = fopen("mail.txt", "a");
     fprintf(fichier, "To: astroaro2024@gmail.com  \nFrom: eric.tinlot@gmx.com \nSubject: Astro Aro %04d/%02d/%02d",now->tm_year+1900, now->tm_mon+1, now->tm_mday);
     fprintf(fichier, "\n");
     // on ferme le fichier 
     fclose(fichier);
	
}


int main(int argc, char* argv[])
{


    relay_init();
    logfile_init();
  
    
     
    gtk_init(&argc, &argv);

    // Create a builder object that will load the file.
    builder = gtk_builder_new_from_file("gm2000.ui");
    window = GTK_WIDGET(gtk_builder_get_object(builder,"window"));

	



   g_signal_connect(window,"destroy",G_CALLBACK(gtk_main_quit),NULL);
   gtk_builder_connect_signals(builder,NULL);


   fixed1   = GTK_WIDGET(gtk_builder_get_object(builder,"fixed"));
   
   // creation timer de 1000 ms et de la call back associee
   g_timeout_add(2000, on_timeout_5000ms,fixed1);
   
  // init des couleurs des bouton on off 
 
  
  gdk_rgba_parse(&color_on,"#80FF80");
  gdk_rgba_parse(&color_off,"#FF8080");
  gdk_rgba_parse(&color_black,"#000000");
  gdk_rgba_parse(&color_red,"#FF8080");
  gdk_rgba_parse(&color_green,"#80FF80");
  
	
  button_relay_1 = GTK_WIDGET(gtk_builder_get_object(builder,"relais1"));
  button_relay_2 = GTK_WIDGET(gtk_builder_get_object(builder,"relais2"));
  button_relay_3 = GTK_WIDGET(gtk_builder_get_object(builder,"relais3"));
  button_relay_4 = GTK_WIDGET(gtk_builder_get_object(builder,"relais4"));
  button_relay_5 = GTK_WIDGET(gtk_builder_get_object(builder,"relais5"));
  button_relay_6 = GTK_WIDGET(gtk_builder_get_object(builder,"relais6"));
  button_relay_7 = GTK_WIDGET(gtk_builder_get_object(builder,"relais7"));
  button_relay_8 = GTK_WIDGET(gtk_builder_get_object(builder,"relais8"));
    
    
 // init des titre des boutons  des relays 
 gtk_button_set_label(button_relay_1,relay_label1);   
 gtk_widget_override_color (button_relay_1, GTK_STATE_NORMAL, &color_black);  
   
 gtk_button_set_label(button_relay_2,relay_label2);   
 gtk_widget_override_color (button_relay_2, GTK_STATE_NORMAL, &color_black);  
   
 gtk_button_set_label(button_relay_3,relay_label3);   
 gtk_widget_override_color (button_relay_3, GTK_STATE_NORMAL, &color_black);   
    
  
 gtk_button_set_label(button_relay_4,relay_label4);   
 gtk_widget_override_color (button_relay_4, GTK_STATE_NORMAL, &color_black);  
 
 gtk_button_set_label(button_relay_5,relay_label5);   
 gtk_widget_override_color (button_relay_5, GTK_STATE_NORMAL, &color_black);  
   
 gtk_button_set_label(button_relay_6,relay_label6);   
 gtk_widget_override_color (button_relay_6, GTK_STATE_NORMAL, &color_black);  
   
 gtk_button_set_label(button_relay_7,relay_label7);   
 gtk_widget_override_color (button_relay_7, GTK_STATE_NORMAL, &color_black);   
    
  
 gtk_button_set_label(button_relay_8,relay_label8);   
 gtk_widget_override_color (button_relay_8, GTK_STATE_NORMAL, &color_black);  
  
  
  // init des couleurs des boutons
  
 gtk_widget_override_background_color(GTK_WIDGET(button_relay_1), GTK_STATE_FLAG_NORMAL,&color_off); 
 gtk_widget_override_background_color(GTK_WIDGET(button_relay_2), GTK_STATE_FLAG_NORMAL,&color_off); 
 gtk_widget_override_background_color(GTK_WIDGET(button_relay_3), GTK_STATE_FLAG_NORMAL,&color_off); 
 gtk_widget_override_background_color(GTK_WIDGET(button_relay_4), GTK_STATE_FLAG_NORMAL,&color_off); 
     

 // timeout pour l'update des statuts des relais (couleur des boutons)
 g_timeout_add (250 /* milliseconds */, on_timeout_button_relay1, button_relay_1);
 g_timeout_add (250 /* milliseconds */, on_timeout_button_relay2, button_relay_2);   
 g_timeout_add (250 /* milliseconds */, on_timeout_button_relay3, button_relay_3);   
 g_timeout_add (250 /* milliseconds */, on_timeout_button_relay4, button_relay_4);    
 g_timeout_add (250 /* milliseconds */, on_timeout_button_relay5, button_relay_5);
 g_timeout_add (250 /* milliseconds */, on_timeout_button_relay6, button_relay_6);   
 g_timeout_add (250 /* milliseconds */, on_timeout_button_relay7, button_relay_7);    
 g_timeout_add (250 /* milliseconds */, on_timeout_button_relay8, button_relay_8);         
   
   
 // update de l' heure toute les secondes
 g_timeout_add (1000 /* milliseconds */, on_timeout_updatetime,label_time);

       
   
   button_mount_software_on  = GTK_WIDGET(gtk_builder_get_object(builder,"button_mount_software_power_on"));
   button_mount_software_off = GTK_WIDGET(gtk_builder_get_object(builder,"button_mount_software_power_off"));
   
   
   button_park  = GTK_WIDGET(gtk_builder_get_object(builder,"button_park"));
   button_unpark = GTK_WIDGET(gtk_builder_get_object(builder,"button_unpark"));
   button_start_session = GTK_WIDGET(gtk_builder_get_object(builder,"button_start_session"));
   button_stop_session = GTK_WIDGET(gtk_builder_get_object(builder,"button_stop_session"));
   button_save_time = GTK_WIDGET(gtk_builder_get_object(builder,"button_save_time"));
   button_clear_log = GTK_WIDGET(gtk_builder_get_object(builder,"button_clear_log"));


   label   = GTK_WIDGET(gtk_builder_get_object(builder,"label"));
   label_mount_status = GTK_WIDGET(gtk_builder_get_object(builder,"label_mount_status"));
   label_start  = GTK_WIDGET(gtk_builder_get_object(builder,"label_start"));
   label_time  = GTK_WIDGET(gtk_builder_get_object(builder,"label_time"));
   
   label_temperature  = GTK_WIDGET(gtk_builder_get_object(builder,"label_temperature"));
   label_humidity  = GTK_WIDGET(gtk_builder_get_object(builder,"label_humidity"));
   
   gtk_label_set_text(GTK_LABEL(label_start),(const gchar*) "session not started");
   label   = GTK_WIDGET(gtk_builder_get_object(builder,"label"));
  
   label_roof_status = GTK_WIDGET(gtk_builder_get_object(builder,"label_roof_status"));
   
   
   
   textlog     = GTK_WIDGET(gtk_builder_get_object(builder,"textlog"));
   textbuffer1 = GTK_WIDGET(gtk_builder_get_object(builder,"textbuffer1"));
   entrybuffer1 = GTK_WIDGET(gtk_builder_get_object(builder,"entrybuffer1"));
 
   // init du text entry pour l'heure de fin, du bouton de sauvegarde et du label de display
   time_to_stop = GTK_WIDGET(gtk_builder_get_object(builder,"time_to_stop"));
   label_time_saved   = GTK_WIDGET(gtk_builder_get_object(builder,"label_time_saved"));
   gtk_label_set_text(GTK_LABEL(label_time_saved),(const gchar*) "XX:XX");
   
   writelog("                       ");
   writelog("Astro Aro RPI4 Started");

    gtk_widget_show(window);


    // Main loop.
    gtk_main();

    return EXIT_SUCCESS;
}


