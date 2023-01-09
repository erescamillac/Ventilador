//--INI: librerias WIFI
//Include the needed library, we will use softer serial communication with the ESP8266
#include <SoftwareSerial.h>
#include <avr/power.h>

//LCD config
#include <Wire.h> 
// #include <LiquidCrystal_I2C.h>
// LiquidCrystal_I2C lcd(0x27,20,4);  //sometimes the LCD adress is not 0x3f. Change to 0x27 if it dosn't work.


//Define the used
#define ESP8266_RX 10  //Connect the TX pin from the ESP to this RX pin of the Arduino
#define ESP8266_TX 11  //Connect the TX pin from the Arduino to the RX pin of ESP
//++FIN: librerias WIFI

//--INI: vars WIFI
/*int LED1 = 2;
int LED2 = 3;
int LED3 = 4;
int LED4 = 5;
int LED5 = 6;*/

int Potentiometer_1 = A0;
int Potentiometer_2 = A1;
int Potentiometer_3 = A2;
int Potentiometer_4 = A3;
int switch1 = 7;
int switch2 = 8;
int switch3 = 9;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////Variables you must change according to your values/////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Add your data: SSID + KEY + host + location + id + password
//////////////////////////////////////////////
// const char SSID_ESP[] = "MiFibra-4132";         //Give EXACT name of your WIFI
// const char SSID_KEY[] = "vzP5anY5";             //Add the password of that WIFI connection

const char SSID_ESP[] = "Totalplay-2EA8";         //Give EXACT name of your WIFI
const char SSID_KEY[] = "2EA8659EXKHY9c85";             //Add the password of that WIFI connection

// const char* host = "noobix.000webhosapp.com";  //Add the host without "www" Example: electronoobs.com
// pruebasoooo.000webhostapp.com
const char* host = "pruebasoooo.000webhostapp.com";  //Add the host without "www" Example: electronoobs.com
String NOOBIX_id = "99999";                     //This is the ID you have on your database, I've used 99999 becuase there is a maximum of 5 characters
String NOOBIX_password = "12345";               //Add the password from the database, also maximum 5 characters and only numerical values
String location_url = "/TX.php?id=";            //location of your PHP file on the server. In this case the TX.php is directly on the first folder of the server
                                                //If you have the files in a different folder, add thas as well, Example: "/ESP/TX.php?id="     Where the folder is ESP 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Used variables in the code
String url = "";
String URL_withPacket = "";    
unsigned long multiplier[] = {1,10,100,1000,10000,100000,1000000,10000000,100000000,1000000000};
//MODES for the ESP
const char CWMODE = '1';//CWMODE 1=STATION, 2=APMODE, 3=BOTH
const char CIPMUX = '1';//CWMODE 0=Single Connection, 1=Multiple Connections


//Define the used functions later in the code, thanks to Kevin Darrah, YT channel:  https://www.youtube.com/user/kdarrah1234
boolean setup_ESP();
boolean read_until_ESP(const char keyword1[], int key_size, int timeout_val, byte mode);
void timeout_start();
boolean timeout_check(int timeout_ms);
void serial_dump_ESP();
boolean connect_ESP();
void connect_webhost();
unsigned long timeout_start_val;
char scratch_data_from_ESP[20];//first byte is the length of bytes
char payload[200];
byte payload_size=0, counter=0;
char ip_address[16];


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//Variable to SEND to the DATABASE
bool sent_bool_1 = 0;
bool sent_bool_2 = 0;
bool sent_bool_3 = 0;
int  sent_nr_1 = 0;
int  sent_nr_2 = 0;
int  sent_nr_3 = 0;
int  sent_nr_4 = 0;
int  sent_nr_5 = 0;

//Variable RECEIVED from the DATABASE
bool received_bool_1 = 0;
bool received_bool_2 = 0;
bool received_bool_3 = 0;
bool received_bool_4 = 0; // Control On/Off de Ventilador
bool received_bool_5 = 0;
int  received_nr_1 = 0;
int  received_nr_2 = 0;
int  received_nr_3 = 0;
int  received_nr_4 = 0;
int  received_nr_5 = 0;
String received_text = "";


/////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////

//Store received chars in this variables
char t1_from_ESP[5];  //For time from web
char d1_from_ESP[2];  //For received_bool_2
char d2_from_ESP[2];  //For received_bool_2
char d3_from_ESP[2];  //For received_bool_3
char d4_from_ESP[2];  //For received_bool_4
char d5_from_ESP[2];  //For received_bool_5
char d9_from_ESP[6];  //For received_nr_1
char d10_from_ESP[6]; //For received_nr_2
char d11_from_ESP[6]; //For received_nr_3
char d12_from_ESP[6]; //For received_nr_4
char d13_from_ESP[6]; //For received_nr_5
char d14_from_ESP[300]; //For received_text 


//DEFINE KEYWORDS HERE
const char keyword_OK[] = "OK";
const char keyword_Ready[] = "Ready";
const char keyword_no_change[] = "no change";
const char keyword_blank[] = "#&";
const char keyword_ip[] = "192.";
const char keyword_rn[] = "\r\n";
const char keyword_quote[] = "\"";
const char keyword_carrot[] = ">";
const char keyword_sendok[] = "SEND OK";
const char keyword_linkdisc[] = "Unlink";

const char keyword_t1[] = "t1";
const char keyword_b1[] = "b1";
const char keyword_b2[] = "b2";
const char keyword_b3[] = "b3";
const char keyword_b4[] = "b4";
const char keyword_b5[] = "b5";
const char keyword_n1[] = "n1";
const char keyword_n2[] = "n2";
const char keyword_n3[] = "n3";
const char keyword_n4[] = "n4";
const char keyword_n5[] = "n5";
const char keyword_n6[] = "n6";
const char keyword_doublehash[] = "##";


SoftwareSerial ESP8266(ESP8266_RX, ESP8266_TX);// rx tx

//++FIN: vars WIFI

int lecturaADC = 0;
double voltajeLM35 = 0.0;
double TemperaturaLM35 = 0.0;

//Reles
const int RELE1 = 4; // Activar RELE1 (Velocidad 1) :: con LOW
const int RELE2 = 5;
const int RELE3 = 6;

void setup() {
  // put your setup code here, to run once:
  pinMode(12, OUTPUT); //Sali a Alarma

  //--INI: pinmode WIFI
  //Pin Modes for ESP TX/RX
  pinMode(ESP8266_RX, INPUT);
  pinMode(ESP8266_TX, OUTPUT);
  
  /*
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);

  pinMode(Potentiometer_1, INPUT);
  pinMode(Potentiometer_2, INPUT);
  pinMode(Potentiometer_3, INPUT);
  pinMode(Potentiometer_4, INPUT);

  pinMode(switch1, INPUT);
  pinMode(switch2, INPUT);
  pinMode(switch3, INPUT);
  */
  
  //++FIN: pinmode WIFI

  Serial.begin(9600);  
  //Iniicalisamos los leds
  pinMode(RELE1, OUTPUT);
  pinMode(RELE2, OUTPUT);
  pinMode(RELE3, OUTPUT);

  digitalWrite(RELE1, HIGH);
  digitalWrite(RELE2, HIGH);
  digitalWrite(RELE3, HIGH);

  //INI: init ESP wifi module
  ESP8266.begin(9600);//default baudrate for ESP
  ESP8266.listen();//not needed unless using other software serial instances
  // Serial.begin(9600); //for status and debug
  
  delay(2000);//delay before kicking things off
  Serial.println( "##--INI :: Configurando ESP()..." );
  setup_ESP();//go setup the ESP 
  Serial.println( "##++FIN :: Configuracion ESP()+++" );
  //FIN: init ESP wifi module

}

void loop() {
  // put your main code here, to run repeatedly:
  lecturaADC = analogRead(A0);
  voltajeLM35 = ((double)lecturaADC/1023)*5;
  //Serial.print("\nEl valor en voltaje de salida en el sensor es: ");
  //Serial.print(voltajeLM35,4); //Imprime la variable double con 4 decimales

  TemperaturaLM35 = voltajeLM35/0.01;
  //Serial.print("\nEl valor en temperatura de salida en el sensor es: ");
  //Serial.print(TemperaturaLM35,2); //Imprime la variable double con 2 decimales
  //Serial.print(" °C");
  //Serial.print("\n\n");

  TemperaturaLM35 = ((lecturaADC*5000.0)/1023)/10;
  Serial.print("\n\n");
  Serial.print(TemperaturaLM35);

  if (TemperaturaLM35 > 30.00){

    Serial.print("\n3 Velocidad Prendida"); 
    digitalWrite(RELE1, HIGH);
    digitalWrite(RELE2, HIGH);
    digitalWrite(RELE3, LOW);

  }else if(TemperaturaLM35 > 25.00 && TemperaturaLM35 <= 30.00){

    Serial.print("\n2 Velocidad Prendida");
    digitalWrite(RELE1, HIGH);
    digitalWrite(RELE2, LOW);
    digitalWrite(RELE3, HIGH);
    
  }else if(TemperaturaLM35 > 20.00 && TemperaturaLM35 <= 25.00){

    Serial.print("\n1 Velocidad Prendida");
    digitalWrite(RELE1, LOW);
    digitalWrite(RELE2, HIGH);
    digitalWrite(RELE3, HIGH);
    
  }else if(TemperaturaLM35 <= 20.00){
    
    Serial.print("\nVentilador Apagado");
    digitalWrite(RELE1, HIGH);
    digitalWrite(RELE2, HIGH);
    digitalWrite(RELE3, HIGH);
  }

  send_to_server_1();

Serial.println( "[received_bool_4]received_bool_4" );
Serial.println( received_bool_4 );
Serial.println( "[received_bool_4]received_bool_4" );

  if( received_bool_4 == 1 ){
    Serial.println( "Ventilador ENCENDIDO desde la App..." );
    digitalWrite(RELE1, LOW);
    digitalWrite(RELE2, HIGH);
    digitalWrite(RELE3, HIGH);
  }else{
    Serial.println( "Ventilador APAGADO desde la App..." );
    digitalWrite(RELE1, HIGH);
    digitalWrite(RELE2, HIGH);
    digitalWrite(RELE3, HIGH);
  }
  
//Retardo de 3 segundos entre medicion.
delay(10000);
}