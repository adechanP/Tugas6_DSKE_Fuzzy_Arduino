#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
//LCD
LiquidCrystal_I2C lcd(0x20, 16, 2);

float hMember[5];
float wMember[5]; 
float u[4]; 
float c[4];
float healthDecision;
String healthDecisionString;

//Fuzzy Points
int vShort    = 115; 
int vSlim     = 65;  
int Short_1   = 126; 
int Slim_1    = 74;  
int Short_2   = 149; 
int Slim_2    = 90;  
int hMedium_1 = 163; 
int wMedium_1 = 97;  
int hMedium_2 = 182; 
int wMedium_2 = 113;
int Tall_1    = 195; 
int Heavy_1   = 122;
int Tall_2    = 218; 
int Heavy_2   = 140;
int vTall     = 228; 
int vHeavy    = 148;

float U  = 0.2;
float LH = 0.4;
float SH = 0.6;
float H  = 0.8;

void heightFuzzy(float x){
  for (int i=0; i<5; i++) {
    hMember[i]=0;
  }
  
  if (x<=vShort){
    hMember[0]=1;
  }
  else if (x>vShort&&x<=Short_1){
    hMember[0]=(Short_1-x)/abs(Short_1-vShort);      
    hMember[1]=(x-vShort)/abs(Short_1-vShort);
  }
  else if (x<=Short_2){
    hMember[1]=1;
  }
  else if (x>Short_2 && x<=hMedium_1){
    hMember[1]=(hMedium_1-x)/abs(hMedium_1-Short_2); 
    hMember[2]=(x-hMedium_1)/abs(hMedium_1-Short_2);
  }
  else if (x<=hMedium_2){
    hMember[2]=1;
  }
  else if (x>hMedium_2 && x<=Tall_1){
    hMember[2]=(Tall_1-x)/abs(Tall_1-hMedium_2);     
    hMember[3]=(x-Tall_1)/abs(Tall_1-hMedium_2);
  }
  else if (x<=Tall_2){
    hMember[3]=1;
  }
  else if (x> Tall_2 && x<=vTall){
    hMember[3]=(vTall-x)/abs(vTall-Tall_2);          
    hMember[4]=(x-vTall)/abs(vTall-Tall_2);
  }
  else if (x>vTall){
    hMember[4]=1;
  }
}

void weightFuzzy(float x){
  for (int i=0; i<5; i++){
    wMember[i]=0;
  }
  if (x<vSlim){
    wMember[0]=1;
  }
  else if (x>vSlim && x<=Slim_1){
    wMember[0]=(Slim_1-x)/abs(Slim_1-vSlim);
    wMember[1]=(x-vSlim)/abs(Slim_1-vSlim);
  }
  else if (x<=Slim_2){
    wMember[1]=1;
  }
  else if (x>Slim_2 && x<=wMedium_1){
    wMember[1]=(wMedium_1-x)/abs(wMedium_1-Slim_2); 
    wMember[2]=(x-wMedium_1)/abs(wMedium_1-Slim_2);
  }
  else if (x<=wMedium_2){
    wMember[2]=1;
  }
  else if (x>wMedium_2 && x<=Heavy_1){
    wMember[2]=(Heavy_1-x)/abs(Heavy_1-wMedium_2);  
    wMember[3]=(x-Heavy_1)/abs(Heavy_1-wMedium_2);
  }
  else if (x<=Heavy_2){
    wMember[3]=1;
  }
  else if (x>Heavy_2 && x<=vHeavy){
    wMember[3]=(vHeavy-x)/abs(vHeavy-Heavy_2);      
    wMember[4]=(x-vHeavy)/abs(vHeavy-Heavy_2);
  }
  else if (x>vHeavy){
    wMember[4]=1;
  }
}

int rules[5][5]={{3,2,1,0,0},
                 {2,3,2,1,0},
                 {1,3,3,1,0},
                 {0,2,3,2,0},
                 {0,1,3,2,1}};

void rulesOperation(){
  int k;
  for (int i=0; i<5; i++) {u[i]=0;}
  for(int i=0; i<5; i++){
     for(int j=0; j<5; j++){
         k=rules[i][j];
         u[k]=max(u[k],min(hMember[j],wMember[i]));
     }
   }
}

void defuzzy(){
  float num = 0;
  float denum = 0;
  float c[4]={U,LH,SH,H};
  for (int i=0; i<4; i++){
    num += c[i]*u[i];
    denum += u[i];
  }
  healthDecision = num/denum;
  if (healthDecision==U){
    healthDecisionString = "U";
  }
  else if (healthDecision==LH){
    healthDecisionString = "LH";
  }
  else if (healthDecision==SH){
    healthDecisionString = "SH";
  }
  else if (healthDecision==H){
    healthDecisionString = "H";
  }
}

void setup() {
  lcd.begin(16,2);
  lcd.backlight();

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

void loop() {
  lcd.clear();

  int height = analogRead(A0);
  int weight = analogRead(A1);

  height = map(height, 0, 1023, 100, 240);
  weight = map(weight, 0, 1023, 60, 160);
  lcd.setCursor(0, 0); lcd.print("H: ");
  lcd.setCursor(2, 0); lcd.print(height);
  lcd.setCursor(0, 1); lcd.print("W: ");
  lcd.setCursor(2, 1); lcd.print(weight);

  heightFuzzy(height);
  weightFuzzy(weight);
  rulesOperation();
  defuzzy();

  lcd.setCursor(6, 0); lcd.print("D: ");
  lcd.setCursor(8, 0); lcd.print(healthDecision);

  delay(50);
}