#include <MIDI.h>
#include <Key.h>
#include <Keypad.h>
#include <TMRpcm.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>
#define MAXSTRING 20

LiquidCrystal_I2C lcd(0x27,20,4);

struct HairlessMidiSettings : public midi::DefaultSettings
{
   static const bool UseRunningStatus = false;
   static const long BaudRate = 115200;
};


MIDI_CREATE_CUSTOM_INSTANCE(HardwareSerial, Serial, MIDI, HairlessMidiSettings);

int inp,prova;
char ch,  mod;
char Keys[4][4]={
{'1','2','3','A'},
{'4','5','6','B'},
{'7','8','9','C'},
{'*','0','#','D'}
};

const byte password[]={4,5,8,7};  //immettere una password a piacere
const byte colPins[4]={41,39,37,35}, rowPins[4]={49,47,45,43}, led[4]={29,27,25,23}, f_switch[6]={32,30,28,26,24,22}, potenziometro[]={6,2,4,0}, Midi[6]={50,51,52,53,54,55}, Midi_vol[4]={40,41,42,43};
int inp_fin[6]={0,0,0,0,0,0},powa[6],pot_value,finpot_value[4], lastwait, wait=0,bpm, first,accent, mid_value;
char *mode[6]={"Effetto precedente", "Metronomo","Accordatore", "Skreamer", "Delay", "Effetto Successivo"}; //modificare solamente i 4 centrali (2,3,4,5) sono per la mod 'B'. Modificare i 2 centrali (3,4) per la mod 'A'.
char key=NULL, *volume[]={"Master: ","Metronomo: ","Mixer: ","Gain: "}; //modificare in base ai volumi scelti nel Guitar Rig

Keypad tastiera= Keypad(makeKeymap(Keys),rowPins,colPins,4,4);
TMRpcm player;

void setup() {
  MIDI.begin(MIDI_CHANNEL_OFF);
  lcd.init();
  lcd.backlight();
  beg_sd();
  intro();
  for (int x=0; x<6;x++){
    pinMode(f_switch[x], INPUT);
      if (x<5)
        pinMode(led[x], OUTPUT);
                        }
  pinMode(53,OUTPUT);
  player.speakerPin=5;
  delay(2000);
  lcd.clear();
  get_password();
  get_mod();
  lcd.clear();
  lcd.noBacklight();
  reset();
}


void loop() {
 mod_a();
 mod_b();
 mod_c();
 mod_d();
}


void beg_sd(){
  File fi = SD.open("README.txt", FILE_WRITE);
  fi.println("Grazie mille per l'utilizzo di Ryu! \n Per usufruire della modalità 'D' bisogna inserire un file wav in 8 bit e 8KHz chiamato 'Ryu.wav'. \n Se si dispone di un file mp3, basta cliccare sul seguente link: https://audio.online-convert.com/convert-to-wav . \n Buon divertimento! \n\n Stefano Morano");
  fi.close();
}
 
 
void reset(){
  for (int x=0; x<6; x++){
    powa[x]=0;
    inp_fin[x]=digitalRead(f_switch[x]);
      if (x<4)
         digitalWrite(led[x],0);
                         }  
            }
   
   
void mod_a(){
  int x;
  while (mod=='A'){
   for (x=0;x<4; x+=3){
        inp=digitalRead(f_switch[x]);
            if (inp!=inp_fin[x]){
                lcd.clear();
                lcd.backlight(); 
                inp_fin[x]=!inp_fin[x];
                MIDI.sendControlChange((x==0) ? Midi[0] : Midi[5],127,1);
                lcd.print((x==0)? mode[0] : mode[5]);
                lampeggia(x,2);
                lastwait=wait;
                               }
                        }
     for (x=1;x<3;x++){
        inp=digitalRead(f_switch[x]);
           if (inp!=inp_fin[x]){
                lcd.clear();
                lcd.backlight();
                inp_fin[x]=!inp_fin[x];
                powa[x]=!powa[x];
                digitalWrite(led[x], powa[x]);
                MIDI.sendControlChange(Midi[x+1], powa[x] ? 127:1, 1);
                lcd.print(mode[x+1]);
                lcd.print(powa[x] ? ": on" : ": off");
                lastwait=wait;
                          }
                }
     for (x=0; x<4; x++){
         pot_value=analogRead(potenziometro[x])/10.23;
         mid_value=analogRead(potenziometro[x])/8;
            if (pot_value > finpot_value[x]+1 || pot_value < finpot_value[x]-1 ){
               finpot_value[x]=pot_value;
               lcd.clear();
               lcd.backlight();
               MIDI.sendControlChange(Midi_vol[x],mid_value,1);
               lcd.print(volume[x]);
               lcd.print(pot_value+1);
               lcd.setCursor(19,0);
               lcd.print("%");
               lastwait=wait;
                                                                                }
                        } 
    wait=millis();
      if ((wait-lastwait)>4000){
            lastwait=wait;
            lcd.clear();
            lcd.noBacklight();
                                }
   char key=tastiera.getKey();
      if (key=='B' || key=='C' ||  key=='D')  {
            lcd.clear();
            lcd.backlight();
            mod=key;
            reset();
            lcd.print("Attivazione mod: ");
            lcd.print(mod);
            delay(2000);
            lcd.clear();
            lcd.noBacklight();
                                              }
          }                  
}


void mod_b(){
  int x;
  while (mod=='B'){
    for (x=4;x<6; x++){
     inp=digitalRead(f_switch[x]);
        if (inp!=inp_fin[x]){
           lcd.clear();
           lcd.backlight();
           inp_fin[x]=!inp_fin[x];
           MIDI.sendControlChange((x==4)? Midi[0] : Midi[5],127,1);
           lcd.print((x==4)? mode[0] : mode[5]);
           lastwait=wait;
                               }
                        }
   for (x=0;x<4;x++){
    inp=digitalRead(f_switch[x]);
      if (inp!=inp_fin[x]){
        lcd.clear();
        lcd.backlight();
        inp_fin[x]=!inp_fin[x];
        powa[x]=!powa[x];
        digitalWrite(led[x], powa[x]);
        MIDI.sendControlChange(Midi[x+1], powa[x] ? 127:1, 1);
        lcd.print(mode[x+1]);
        lcd.print(powa[x] ? ": on" : ": off");
        lastwait=wait;
                          }
                    }
  for (x=0; x<4; x++){
   pot_value=analogRead(potenziometro[x])/10.23;
   mid_value=analogRead(potenziometro[x])/8;
    if (pot_value > finpot_value[x]+1 || pot_value < finpot_value[x]-1 ){
        finpot_value[x]=pot_value;
        lcd.clear();
        lcd.backlight();
        MIDI.sendControlChange(Midi_vol[x],mid_value,1);
        lcd.print(volume[x]);
        lcd.print(pot_value+1);
        lcd.setCursor(19,0);
        lcd.print("%");
        lastwait=wait;
                                                                        }
                    }
  wait=millis();
      if ((wait-lastwait)>4000){
            lastwait=wait;
            lcd.clear();
            lcd.noBacklight();
                              }
  char key=tastiera.getKey();
    if (key=='A'|| key=='C' ||  key=='D')  {
        lcd.clear();
        lcd.backlight();
        mod=key;
        reset();
        lcd.print("Attivazione mod: ");
        lcd.print(mod);
        delay(2000);
        lcd.clear();
        lcd.noBacklight();
                                          }    
    }       
}


void mod_c(){
  if (mod=='C'){
  lcd.backlight();
  lcd.setCursor(6,0);
  lcd.print("bpm");
  bpm=0;
  first=1;
  }
  while (mod=='C'){
      key=tastiera.getKey();
        if (key!='*' && key!='#' && key){
          if (!first){
            bpm*=10;
            bpm+= atoi(&key);
            lcd.clear();
            lcd.print(bpm);
            lcd.setCursor(6,0);
            lcd.print("bpm");
                    }
          else {
            bpm+= atoi(&key);
            first=0;
            lcd.clear();
            lcd.print(bpm);
            lcd.setCursor(6,0);
            lcd.print("bpm");
               }
    if (bpm>500){
        bpm=0;
        first=1;
        lcd.clear();
        lcd.setCursor(6,0);
        lcd.print("bpm");
              }
                                   }
    if (key=='C'){
      bpm=0;
      first=1;
      lcd.clear();
      lcd.setCursor(6,0);
      lcd.print("bpm");
    }
   metronomo(bpm,key);
   if (key=='A' || key=='B' || key=='D'){
      lcd.clear();
      mod=key;
      reset();
      lcd.print("Attivazione mod: ");
      lcd.print(mod);
      delay(2000);
      lcd.clear();
      lcd.noBacklight();
                                      }
      }
}

void mod_d(){
  if (mod=='D')
    lcd.backlight();
  int pronto=0;
  char key;
  while (mod=='D'){
  while (!pronto){
   key=tastiera.getKey();
    if(SD.begin())
      {
        pronto=1;
      } else{
        lcd.clear(); 
      lcd.print("Inserire scheda SD");
      delay(2);
      }
      if (key=='A' || key=='B' || key=='C'){
      lcd.clear();
      mod=key;
      reset();
      lcd.print("Attivazione mod: ");
      lcd.print(mod);
      delay(2000);
      lcd.clear();
      lcd.noBacklight();
      break;
                                      }
             }
             
if (!SD.exists("Ryu.wav")){
  lcd.clear();
  lcd.print("File Ryu.wav");
  lcd.setCursor(0,1);
  lcd.print("inesistente");
  delay(1000);
}
else{
  key=tastiera.getKey();
  player.setVolume(7);
 if (key=='*'){
  lcd.clear();
  lcd.print("Play");
  player.play("Ryu.wav");
  while (key!='#'){
    key=tastiera.getKey();
  }
   
  }
 if (key=='#'){
  lcd.clear();
  lcd.print("Stop");
  player.pause();
 }
}
  key=tastiera.getKey();
if (key=='A' || key=='B' || key=='C'){
      lcd.clear();
      mod=key;
      reset();
      lcd.print("Attivazione mod: ");
      lcd.print(mod);
      delay(2000);
      lcd.clear();
      lcd.noBacklight();
      pronto=0;
                                      }

  }
}

void lampeggia(int x, int volte){
  int luce;
  for (int y=0; y<volte; y++){
  digitalWrite(led[x],1);
  delay(200);
   digitalWrite(led[x],0);
  delay(200);      
        }
}


void get_password(){
  int accesso=0;
  lcd.backlight();
     while (!accesso){   
        lcd.print("Inserisci Password: ");
        lcd.setCursor(0,1);
        int right=1;
          for (int x=0; x<4; x++){
              char key=NULL;
                while (key==NULL){
                  key = tastiera.getKey();
                                  }
                if (key){
                  lcd.print("*");
                    if (atoi(&key)!=password[x])
                      right=0;
                        }
                                 }
    lcd.clear();
     if (right){
      lcd.print("Password corretta");
      accesso=1;
              }
     else lcd.print("Password errata");
    delay(3000);
    lcd.clear();
   }
}


void get_mod(){
mod=NULL;
  lcd.print("Seleziona una mod");
    while (mod==NULL){
      lcd.setCursor(0,1);
      char key=tastiera.getKey();
        if (key=='A' || key=='B' ||key=='C' ||key=='D'){
          lcd.clear();
          lcd.print("Attivazione mod ");
          lcd.print(key);
          mod=key;
          delay(2000);
          lcd.clear();
                                                       }
                  }
}

void intro(){
  lcd.print("Ryu");
  lcd.setCursor(0,2);
  lcd.print("Made by S.Morano");
}


void metronomo(int bpm, char key){
  if (key=='*' && bpm!=0){
    lcd.setCursor(0,2);
    lcd.print("Play");
  while (key!='#'){
    wait = millis();
      if ((wait - lastwait) > (60000/bpm) ) {
         accent++;
         tone(5,(accent==1)? 1500:1000,100);
         delay(100);
         noTone(5);
         lastwait=wait;
                                            }  
      if (accent==4)
        accent=0;
      key=tastiera.getKey();
                  } 
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(bpm);
  lcd.setCursor(6,0);
  lcd.print("bpm");
  lcd.setCursor(0,2);
  lcd.print("Stop");
                        }
}
