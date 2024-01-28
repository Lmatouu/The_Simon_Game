// Les bibliothèques disponibles
#include <LiquidCrystal_I2C.h>    // bibliothèque de pilotage de l'écran lcd via protocole i2c
#include <Key.h>                  // bibliothèque de fonctionnalités du clavier
#include <Keypad.h>               // bibliothèque de pilotage du clavier
#include "obj_stat.h"             // bibliothèque locale de programmation de la machine à états

// Les constantes
const byte ROWS = 4;    //4 rows
const byte COLS = 4;    //4 columns
const char keys[ROWS][COLS] = {
    {'1','2','3','4'},        // déclaration des touches seulement les touches de la première ligne nous intéresse 1 à 4
    {'1','2','3','4'},
    {'1','2','3','4'},
    {'1','2','3','4'}
};
const byte rowPins[ROWS] = {2, 3, 4, 5}; //connect to the row pinouts of the keypad
const byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad

// Les variables globales
int digit[10];  // liste vide de 10 éléments correspondant aux touches appuyées 
int sequence[10]; // liste vide de 10 éléments correspondant à la séquence de led fournit aléatoirement
byte index = 0;       // compteur d'appuis
bool ok = false;      // varibale comparaison liste digit et sequence
int niveau = 1; // variable associée au niveau 
int randNumber; // variable associée à un nombre aléatoire compris entre 1 et 4
int son [] = {261 ,360, 392, 503}; // liste associée note de musique
              /// do mi sol do
int newkey;

// Les objets
LiquidCrystal_I2C lcd(0x27,16,2);   // écran de 16 colonnes et 2 lignes
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define BUZ A0          // Câblage buzzer sur la pin A0

// Les événements sur bouton poussoir
Cevent bpS4, bpS3 ; // bouton poussoir

// Les actions 
CdigitalOut led1 ;  // LED1
CdigitalOut led2 ;  // LED2
CdigitalOut led3 ;  // LED3
CdigitalOut led4 ;  // LED4
CdigitalOut ledok ;  // LED3
CdigitalOut ledv ;  // LED4



// Les états
Cstat ACCUEIL("Etat ACCUEIL",0);
Cstat AFFICHAGE("Etat AFFICHAGE",1);
Cstat SAISIE("Etat SAISIE",2);
Cstat VERIFICATION("Etat VERIFICATION",3);
Cstat ERREUR("Etat ERREUR",4);
Cstat CODEBON("Etat CODEBON",5);
Cstat* pActif;




 void setup() {
  bpS4.setPin(13); // Association de la pin 13 au BPS4
  bpS3.setPin(12); // Association de la pin 12 au BPS3
  led1.setPin(14); // Association de la pin 14 à led1
  led2.setPin(15); // Association de la pin 15 à led2
  led3.setPin(16); // Association de la pin 16 à led3
  led4.setPin(17); // Association de la pin 17 à led4
  ledok.setPin(19); // Association de la pin 16 à led3
  ledv.setPin(18); // Association de la pin 17 à led4

  pinMode(BUZ,OUTPUT);  // buzzer sur pin A0
  Serial.begin(9600);
  randomSeed(analogRead(1)); // random sur pin A1
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Jeu Simon");
  lcd.setCursor(2,1);
  lcd.print("Start");
  pActif = ACCUEIL.activate();

}


//Fonction qui permet d'allumer une led   
void jouerNote(int numero)
{
  switch(numero)
  {
    case 1:
        led1.on();
      break;
    case 2:
      led2.on();
      break;
      
    case 3:
      led3.on();
      break;
      
    case 4:
      led4.on();
      break;
  }
  tone(BUZ, son[numero-1]); //Le tableau commence à 0 (non à 1)
  delay(1200); //temps d'affichage led (1,2secondes)
  led1.off();
  led2.off();
  led3.off();
  led4.off();
  ledok.off();
  ledv.off();
  noTone(BUZ);
  delay(200);
}
    


//Fonction qui permet de parcourir la liste sequence et d'allumer une led par une led
void jouerSequence(int niveau)
{
  for(int i=0; i<=niveau; i++)
  {
    jouerNote(sequence[i]);
  }
}


//Fonction qui permet de comparer les deux listes sequences et digits
//Si les deux listes sont identiques la fonction renvoie ok=True sinon False
void comparer_liste (int liste1[],int liste2[])
{
ok=true;
int i = 9;
for (int k=0; k<i+1;k=k+1)
{
  Serial.print('s');
  Serial.print(liste1[k]);
  Serial.print('d');
  Serial.print(liste2[k]);
  if (liste1[k]!=liste2[k])
  {
    ok=false;
  }
}
}


 //Fonction qui permet d'ajouter une note aléatoire à la suite sequence
void nouvelleNote(int niveau)
{
  randNumber=random(1,5);
  Serial.print(randNumber);
  sequence[niveau-1] = randNumber;
  
}




//Fonction qui permet de jouer une musique à chaque lancement du jeu Simon 
void melodieLancement()
{
  led1.on();
  led2.on();
  led3.on();
  led4.on();
  ledok.off();
  ledv.off();

  tone(BUZ, son[0]);
  delay(300);
  tone(BUZ, son[1]);
  delay(300);
  tone(BUZ, son[2]);
  delay(300);
  tone(BUZ, son[3]);
  delay(300);
  led1.off();
  led2.off();
  led3.off();
  led4.off();
  ledok.off();
  ledv.off();
  noTone(BUZ);
}

void loop(){
  //Description des transitions
      // un appui sur un BP est détecté par la méthode .falling()
  transition(pActif,ACCUEIL,AFFICHAGE,bpS3.falling());
  transition(pActif,VERIFICATION,ERREUR,bpS4.falling(),ok==false);
  transition(pActif,VERIFICATION,CODEBON,bpS4.falling(),ok==true);
  transition(pActif,ERREUR,ACCUEIL,bpS3.falling());
  transition(pActif,CODEBON,AFFICHAGE,bpS3.falling(),ok==true);

 if (ACCUEIL.entry_())
  {
    melodieLancement();
    
    ok = false;
  } 


if (AFFICHAGE.do_())
  {
    lcd.clear();
    lcd.print("AFFICHAGE LED:");
    nouvelleNote(niveau);
    jouerSequence(niveau);
    transition(pActif,AFFICHAGE,SAISIE,true);
  }

  




 if (SAISIE.do_())
  {
    char key = keypad.getKey();
    if (key and index<niveau) 
    {
        lcd.setCursor(2,1);
        lcd.print(key);
        digit[index]=(key-48);
        newkey=(key-48);
        Serial.print(newkey);
        index++;
        
     switch (index)
        {
 
          case 1:
            jouerNote(newkey);
            break;
          case 2:
            jouerNote(newkey);
            break;
          case 3:
            jouerNote(newkey);
            break;
          case 4:
            jouerNote(newkey);
            break;
          case 5:
            jouerNote(newkey);
            break;
          case 6:
            jouerNote(newkey);
            break;
          case 7:
            jouerNote(newkey);
            break;
          case 8:
            jouerNote(newkey);
            break;
          case 9:
            jouerNote(newkey);
            break;
          case 10:
            jouerNote(newkey);
            break;
            
        }      
    }
    if (index==niveau)
     {
      lcd.setCursor(2,1);
      lcd.print("verification");
      index = 0;
      comparer_liste (sequence,digit);
      transition(pActif,SAISIE,VERIFICATION,true);
      
     }
            
}

  if (VERIFICATION.entry_())
  {
    
    led1.flash(150);
    led2.flash(200);
    led3.flash(250);
    led4.flash(300);
  }

  if(transition(pActif,VERIFICATION,CODEBON,bpS4.falling(),ok==true)) 
  {
    led1.off();
    led2.off();
    led3.off();
    led4.off();
  }

  if (ERREUR.entry_())
  {
    lcd.clear();
    lcd.setCursor(3,0);
    lcd.print("SERIE");
    lcd.setCursor(6,1);
    lcd.print("FAUX");
    niveau=1;
    int digit[10];
    int sequence[10];
    transition(pActif,ERREUR,ACCUEIL,true);
  }
  if (ERREUR.do_())
  {
    led1.flash(150);
    led2.flash(200);
    led3.flash(250);
    led4.flash(300);
    
  }

  if (ERREUR.exit_())
  {
    led1.off();
    led2.off();
    led3.off();
    led4.off();
    
    
  }
  
  if (CODEBON.entry_())
  {
    tone(BUZ,523,2500);
    tone(BUZ,14000,2500);
    lcd.clear();
    lcd.print("Niveau suivant");
    delay(1000);
    niveau=niveau+1;
    transition(pActif,CODEBON,AFFICHAGE,true);
    
    
    
  }


  // Mises à jour des états
  bpS4.update();
  bpS3.update();
  ledv.update();
  led1.update();
  led2.update();
  led3.update();
  led4.update();
  ACCUEIL.update();
  VERIFICATION.update();
  SAISIE.update();
  ERREUR.update();
  CODEBON.update();
  AFFICHAGE.update();


  
}
