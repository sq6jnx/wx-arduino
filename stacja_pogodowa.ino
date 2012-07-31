/*
  Wstęp do automatycznej stacji pogodowej 
  
  This example code is in the public domain.
 */

/*
   pomysły na usprawnienia:

   funkcja wyczyść i napisz: czyści ekran i wypisuje łańcuch -- pytanie, czy
   zaoszczędzimy trochę miejsca

   konwersja z double na napis -- może uda się bez tego dziwa?

*/

#define ArgArg(x, y)          x##y
#define ArgText(x)            x##TEXT
#define TextArg(x)            TEXT##x
#define TextText              TEXT##text
#define Jitter                1
#define bug                   2
#define Jitterbug             3
/*
Invocation 	Result of Macro Expansion
ArgArg(lady, bug) 	"ladybug"
ArgText(con) 	"conTEXT"
TextArg(book) 	"TEXTbook"
TextText 	"TEXTtext"
ArgArg(Jitter, bug) 	3

http://publib.boulder.ibm.com/infocenter/comphelp/v8v101/index.jsp?topic=%2Fcom.ibm.xlcpp8a.doc%2Flanguage%2Fref%2Finclude_next.htm

*/
#include <LiquidCrystal.h>
#include "Wire.h"
#include "Adafruit_BMP085.h"
Adafruit_BMP085 bmp;

LiquidCrystal lcd(12, 11, 5, 4, 3, 2);


int petla=0;
int duza_petla=0;


double temp=0;
double temp_max=-3000;
double temp_min= 3000;

double cisn=0;
double cisn_max=-3000;
double cisn_min= 3000;
double d2h;

double cisn_d2h[24];
int cisn_d2h_index=-1;

char s[32]; // bufor dla dtostrf()

void setup() {      
  /* inicalizacja wyświetlacza */
  lcd.begin(16, 2); 

  /* inicjalizacja portu szeregowego */
  Serial.begin(9600);
  
  /* inicjalizacja czujnika BMP085*/
  bmp.begin();

  cisnienie();
  for(int i=0;i<24;i++) cisn_d2h[i]=cisn;
  cisn_d2h_index=0;
  d2h=0;
}

void temperatura() {
  lcd.clear();
  lcd.print("Temperatura:"); 
  
  temp=bmp.readTemperature();
  
  if(temp<0 || temp>10) {
    lcd.setCursor(6, 1);
    lcd.write(dtostrf(temp,4,1,s));
  } else {
    lcd.setCursor(7, 1);
    lcd.write(dtostrf(temp,5,1,s));    
  }
  
  //lcd.setCursor(10, 1);
  lcd.print(" st. C");
  
  if(temp_max<temp) temp_max=temp;
  if(temp_min>temp) temp_min=temp;
}

void temperatura_min() {
  lcd.clear();
  lcd.print("Temperatura min.:"); 
  
  if(temp_min<-10 || temp_min>10) {
    lcd.setCursor(6, 1);
    lcd.write(dtostrf(temp_min,4,1,s));
  } else {
    lcd.setCursor(7, 1);
    lcd.write(dtostrf(temp_min,5,1,s));    
  }
  
  //lcd.setCursor(10, 1);
  lcd.print(" st. C");

}

void temperatura_max() {
  lcd.clear();
  lcd.print("Temperatura max.:"); 
  
  if(temp_max<-10 || temp_max>10) {
    lcd.setCursor(6, 1);
    lcd.write(dtostrf(temp_max,4,1,s));
  } else {
    lcd.setCursor(7, 1);
    lcd.write(dtostrf(temp_max,5,1,s));    
  }
  
  //lcd.setCursor(10, 1);
  lcd.print(" st. C");
}


void cisnienie() {
  lcd.clear();
  lcd.print("Cisnienie:"); 
  
  cisn=bmp.readPressure()/100;
  
  if(cisn<1000) {
    lcd.setCursor(4, 1);
    lcd.write(dtostrf(cisn,8,1,s));
  } else {
    lcd.setCursor(3, 1);
    lcd.write(dtostrf(cisn,9,1,s));    
  }
  
  //lcd.setCursor(12, 1);
  lcd.print(" hPa");
  
  if(cisn_max<cisn) cisn_max=cisn;
  if(cisn_min>cisn) cisn_min=cisn;
}

void cisnienie_min() {
  lcd.clear();
  lcd.print("Cisnienie min.:"); 
  
  if(cisn_min<1000) {
    lcd.setCursor(4, 1);
    lcd.write(dtostrf(cisn_min,8,1,s));
  } else {
    lcd.setCursor(3, 1);
    lcd.write(dtostrf(cisn_min,9,1,s));    
  }
  
  //lcd.setCursor(12, 1);
  lcd.print(" hPa");
}

void cisnienie_max() {
  lcd.clear();
  lcd.print("Cisnienie max.:"); 
  
  if(cisn_max<1000) {
    lcd.setCursor(4, 1);
    lcd.write(dtostrf(cisn_max,8,1,s));
  } else {
    lcd.setCursor(3, 1);
    lcd.write(dtostrf(cisn_max,9,1,s));    
  }
  
  //lcd.setCursor(12, 1);
  lcd.print(" hPa");
}


void cisnienie_d2h() {
  cisn_d2h_index=(cisn_d2h_index++)%24;
  cisn_d2h[cisn_d2h_index]=cisn;
  d2h=cisn-cisn_d2h[(cisn_d2h_index+25)%24]; // 25=24+1, czyli jeden dalej niż
                                             //jesteśmy teraz --> najstarszy
                                             //posiadany odczyt
}


void pokaz_cisnienie_d2h() {
  lcd.clear();
  lcd.print("Zmiana cisn.(2h):"); 
  
  if(d2h<1000) {
    lcd.setCursor(4, 1);
    lcd.write(dtostrf(d2h,8,1,s));
  } else {
    lcd.setCursor(3, 1);
    lcd.write(dtostrf(d2h,9,1,s));    
  }
  
  //lcd.setCursor(12, 1);
  lcd.print(" hPa");
}


void raportuj_serial() {
    Serial.print(temp);
    Serial.print(";");
    Serial.print(cisn);
    Serial.print(";");
    Serial.println(cisn-cisn_d2h[cisn_d2h_index]);
}

void loop() {
  if(petla==0) { 
    duza_petla=(duza_petla++)%24;
    if(duza_petla==0) cisnienie_d2h();
  }

  switch(petla) {
    case 0: temperatura(); break;
    case 1: temperatura_min(); break;
    case 2: temperatura_max(); break;
    case 3: cisnienie(); break;
    case 4: cisnienie_min(); break;
    case 5: cisnienie_max(); break;
    case 6: pokaz_cisnienie_d2h();
            raportuj_serial(); break;    
    default: petla=0;
  }
  
  delay(2000);
  petla=(petla++)%6;
}

