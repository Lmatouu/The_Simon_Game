////////////////////////////////////////////////////////
//Bibliothèque programmation par graph d'état (Stat Machine)
//Contient les objets :
//Cstat, Ctimer, Cevent, CdigitalOut, Cbistable
//Contient les fonctions :
//transition,when et after
//A DUGAS CPGE - Lycée Livet
////////////////////////////////////////////////////////
#include "obj_stat.h"
#include <Arduino.h>


//***********************************
//member definition Cstat
//***********************************

//Constructeur
Cstat::Cstat(const char * stat_name, int stat_id ) {
   //Variables carractérisant l'état
   _entry = false ; //entry_, do_, exit_ ne peuvent pas être a true simultanément
   _do = false;
   _exit = false;
   _stattime = 0;
   completion_event = false ;
   //Log    
   log = true ;
   _name = String(stat_name) ;
   id = stat_id ;
};

String Cstat::name() {
  return _name;
}

//Variables carractérisant l'état
bool Cstat::entry_(){
  return _entry;
};
bool Cstat::do_(){
  return _do;
};
bool Cstat::exit_(){
  return _exit;
};
bool Cstat::stat(){
  return (_entry||_do);
};
bool Cstat::interne_stat(){
  return _do;
};


//Activation, Désactivation

Cstat* Cstat::activate(){
  _stattime = millis();
  completion_event = false ;
  if (!stat()){
    _entry = true;
    _logln("Activation de l'etat : " + _name + " (Id:" + String(id) + ") a " + String(_stattime) + " ms.", true);
  }
  else{
    _entry = true;
    _exit = true;
    _logln("Re-Activation de l'etat : " + _name + " (Id:" + String(id) + ")", true);
    _logln("  La date d'activation de l etat est maintenant :" + String(_stattime) + " ms.");
  }

  return this; //retourne un pointeur sur l'état actif
};
void Cstat::desactivate(){
  completion_event = false ;
  if (interne_stat()){
    _entry = false;
    _do = false;
    _exit = true;
    _logln("Desactivation de l'etat : " + _name + " (Id:" + String(id) + ") a " + String(millis()) + " ms.", true);
  }
  else{
    _logln("ATTENTION : Tentative de desactivation d'un etat non actif : " + _name + " (Id:" + String(id) + ") a " + String(millis()) + " ms.", true);  
  }
};

//After
bool Cstat::after(unsigned long delay_ms){  //Vaut true apres un délais imposée depuis l'activation de l'état
  //Serial.println(delay_ms);
  if ((millis()-_stattime)<delay_ms)
    return false;
  else  
    return interne_stat();
};

//After resetStattime
void Cstat::resetStattime(){  //Réinitialise _stattime
	_stattime = millis();
};


//Mise à jour
bool Cstat::update(){  //Retourne true si il y a une évolution et false sinon
  if (_entry){
    _entry = false ;
    _do = true ;
		return true ; 
  }
  if (_exit) {
    _exit = false ;
		return true ;
	}
	return false ;
};

//_log
void Cstat::_log(String message, bool titre) { //Equivalent à println  si log=true (ne fait rien sinon)
  if (log)
  {
      if (titre)
        Serial.print(F("Cstat : ")) ;  

      Serial.print(message) ;
  }
};

void Cstat::_logln(String message, bool titre) { //Equivalent à println  si log=true (ne fait rien sinon)
  if (log)
  {
      if (titre)
        Serial.print(F("Cstat : ")) ;  
      Serial.println(message) ;
  }
};

//*************************************************
//Transitions
//*************************************************
boolean transition(Cstat* &pActive, Cstat &from,Cstat &to, boolean event, boolean guard) {//event: éveinement, guard : condition de garde, from : état avant, to : état après, pActive : pointeur sur l'état actif
  if (!from.interne_stat()) //l'état avant n'est pas actif
    return false;
  if (!(event&&guard)) //la condition n'est pas vérifié
    return false;
  //cas de la transition franchissable
  #if LOG_TRANSITION
    Serial.println();Serial.print(F("-> transition de "));Serial.print(from.name());Serial.print(F(" vers "));Serial.println(to.name());
  #endif
  from.desactivate();
  pActive = to.activate();
  return true ;
}

boolean completion(Cstat* &pActive, Cstat &from,Cstat &to,boolean guard) {//event: éveinement, guard : condition de garde, from : état avant, to : état après, pActive : pointeur sur l'état actif
  if (!from.interne_stat()) //l'état avant n'est pas actif
    return false;
  if (!(from.completion_event&&guard)) //la condition n'est pas vérifié
    return false;
  //cas de la transition franchissable
  #if LOG_TRANSITION
    Serial.println();Serial.print(F("-> completion de "));Serial.print(from.name());Serial.print(F(" vers "));Serial.println(to.name());
  #endif
  from.desactivate();
  pActive = to.activate();
  return true ;
}

boolean when(Cstat* &pActive, Cstat &from,Cstat &to, boolean condition) { 
  if (!from.interne_stat()) //l'état avant n'est pas actif
    return false;
  if (!(condition)) //la condition n'est pas vérifié
    return false;
  //cas de la transition franchissable
  #if LOG_TRANSITION
    Serial.println();Serial.print(F("-> when de "));Serial.print(from.name());Serial.print(F(" vers "));Serial.println(to.name());
  #endif
  from.desactivate();
  pActive = to.activate();
  return true ;
}

boolean after(Cstat* &pActive, Cstat &from,Cstat &to, unsigned long delay_ms, boolean guard) { //delay: durée avant le changement d'état, guard : condition de garde, from : état avant, to : état après, pActive : pointeur sur l'état actif
  if (!(from.after(delay_ms)&&guard)) //la condition n'est pas vérifié
    return false;
  //cas de la transition franchissable
  #if LOG_TRANSITION
    Serial.println();Serial.print(F("-> after de "));Serial.print(from.name());Serial.print(F(" vers "));Serial.println(to.name());
  #endif
  from.desactivate();
  pActive = to.activate();
  return true ;
}

//***********************************
//member definition Ctimer
//***********************************
Ctimer::Ctimer(){
  _stattime = millis();
	_stat = false ;
}

void Ctimer::start(){
  _stattime = millis();
	_stat = true ;
}

void Ctimer::stop(){
	_stat = false ;
}

bool Ctimer::stat(){
	return _stat ;
}

bool Ctimer::after(unsigned long delay_ms){
  if (!_stat) //Etat stop
		return false ; 
	
	if ((millis()-_stattime)<delay_ms)
    return false ;
  else{
    return true ;
  }
}

//***********************************
//member definition Cpulse
//***********************************
Cpulse::Cpulse(){
  _every = false ;
  _stattime = millis();
}
  
bool Cpulse::every(unsigned long delay_ms){ 
  if(!_every){
    _every = true ;
    _stattime = millis();
    return false ;    
  }
  else{
    if ((millis()-_stattime)<delay_ms){
      return false ;
    }
    else{
      _every = false ;
      return true ;
    }
  }
}

//***********************************
//member definition Cevent
//***********************************
Cevent::Cevent(const char * name){
  log = false ;
  anti_rebounce_time = ANTI_REBOUNCE_TIME ; //Réglage par défaut
  _stat = LOW ;
  _new_stat = LOW ;
  _rising = false;
  _falling = false;
  _pin = 0 ;
  _stattime = 0 ; //date du derier changement d'état
  _interne_pull_up = false ;
  _name = String(name) ;
}
void Cevent::setPin(uint8_t pin, bool interne_pull_up){
  pinMode(pin,INPUT);
  _pin = pin; 
  _interne_pull_up = interne_pull_up ;
  if (_interne_pull_up)
    digitalWrite(pin, HIGH);
  //initialisation
  _stat = (digitalRead(_pin)^_interne_pull_up) ; //Inversion du résulatat par ou exclusif si la résistance de pull up interne est active
  _stattime = millis() ;
}

bool Cevent::rising(){
  return _rising;
}
bool Cevent::falling(){
  return _falling;
}
bool Cevent::change(){
  return (_falling||_rising);
}

bool Cevent::stat(){
  return _stat ;
}

bool Cevent::update(){ //Retourne false si aucun changement
	_rising = false;
  _falling = false;
	
	if ((millis()-_stattime)<anti_rebounce_time ) //Anti-rebond 
		return false;
		
  _new_stat = (digitalRead(_pin)^_interne_pull_up) ; //Inversion du résulatat par ou exclusif si la résistance de pull up interne est active
  //rising
	if ((_new_stat==HIGH)&&(_stat==LOW)){
     _stat = HIGH ;
     _rising = true ;
     _stattime = millis();
     if (log){
        Serial.print(F("Cevent : "));
        Serial.println(_name + F(" rising at ") + String(_stattime) + F(" ms.")); 
     }
		return true ;
  }
	//falling
  if ((_new_stat==LOW)&&(_stat==HIGH)){
     _stat = LOW ;
     _falling = true ;
     _stattime = millis();
     if (log){
        Serial.print(F("Cevent : "));
        Serial.println(_name + F(" falling at ") + String(_stattime) + F(" ms.")); 
     }
		return true ; 
  }
	return false ; 
}

//***********************************
//member definition Cbutton
//***********************************
Cbutton::Cbutton(const char * name){
  log = false ;
  anti_rebounce_time = ANTI_REBOUNCE_TIME ; //Réglage par défaut
  _stat = LOW ;
  _new_stat = LOW ;
  _rising = false;
  _falling = false;
	_longPress = false ;
	_longPressEvent = false ;
	_click = false ;
	_clickEvent = false ;
  _long_press_time = LONG_PRESS_TIME ; //Réglage par défaut
  _pin = 0 ;
  _stattime = 0 ; //date du derier changement d'état
  _interne_pull_up = false ;
  _name = String(name) ;
}
void Cbutton::setPin(uint8_t pin, bool interne_pull_up){
  pinMode(pin,INPUT);
  _pin = pin; 
  _interne_pull_up = interne_pull_up ;
  if (_interne_pull_up)
    digitalWrite(pin, HIGH);
  //initialisation
  _stat = (digitalRead(_pin)^_interne_pull_up) ; //Inversion du résulatat par ou exclusif si la résistance de pull up interne est active
  _stattime = millis() ;
}
void Cbutton::setLongPressTime(unsigned long delay_ms){  //2 secondes par défaut (cf constructeur)
  _long_press_time = delay_ms ;
}
bool Cbutton::rising(){
  return _rising;
}
bool Cbutton::falling(){
  return _falling;
}
bool Cbutton::change(){
  return (_falling||_rising);
}
bool Cbutton::longPress(){
		return _longPressEvent ;
	}


bool Cbutton::click(){
  return _clickEvent ;
}

bool Cbutton::doubleClick(){
  return _doubleClickEvent ;
}


bool Cbutton::stat(){
  return _stat ;
}

bool Cbutton::update(){
	_rising = false;
  _falling = false;
	_longPressEvent = false ;
	_clickEvent = false ;
  _doubleClickEvent = false ;
	
	if ((millis()-_stattime)<anti_rebounce_time ) //Anti-rebond 
		return false;
		
  _new_stat = (digitalRead(_pin)^_interne_pull_up) ; //Inversion du résulatat par ou exclusif si la résistance de pull up interne est active
  
	
	if (_new_stat==HIGH) {
		//rising
		if (_stat==LOW){
			 _stat = HIGH ;
			 _rising = true ;
			 _stattime = millis();
			 if (log){
					Serial.print(F("Cbutton : "));
					Serial.println(_name + F(" rising at ") + String(_stattime) + F(" ms.")); 
			 }
			return true ;
		}
		
		//longPress
		if (((millis()-_stattime)>_long_press_time)&&!_longPress)	{
			if (log){
				Serial.print(F("Cbutton : "));
				Serial.println(_name + F(" longPress at ") + String(_stattime) + " ms."); 
			}
			_longPress = true ;
			_longPressEvent = true ;
			return true ;
		}
	}
	
	if (_new_stat==LOW) {
		//falling
		if (_stat==HIGH){
			 _stat = LOW ;
			 _falling = true ;
			 _longPress = false ;
			 if ((millis()-_stattime)<CLICK_TIME ) {
					if (_click){
						//doubleClic
						if (log){
							Serial.print(F("Cbutton : "));
							Serial.println(_name + F(" doubleClick at ") + String(_stattime) + F(" ms.")); 
						}
						_doubleClickEvent = true;
						_click = false;
					}
					else 
						_click = true;
			 }
			 _stattime = millis();
			 if (log){
					Serial.print(F("Cbutton : "));
					Serial.println(_name + F(" falling at ") + String(_stattime) + F(" ms.")); 
			 }
		}
		
		//Click
		if ((millis()-_stattime)>CLICK_TIME) {
			if (_click){
				if (log){
					Serial.print(F("Cbutton : "));
					Serial.println(_name + F(" click at ") + String(_stattime) + F(" ms.")); 
				}
				_clickEvent = true ;
				_click = false ;
				return true ;
			}
		}
		return _falling ;
	}
	return false ;	
}

//***********************************
//Cbistable
//***********************************

Cbistable::Cbistable(const char * name1,const char * name2){
  _pin1=0;  
  _pin2=0;
  _name1=String(name1);
  _name2=String(name2);
  log = false ;
  _stat1 = false ;
  _stat2 = false ;
}
void Cbistable::setPin(uint8_t pin1,uint8_t pin2){
  _pin1=pin1;  
  _pin2=pin2;
  pinMode(_pin1,OUTPUT);
  pinMode(_pin2,OUTPUT);  
}
void Cbistable::set1(bool stat){
 if ((stat)&&(digitalRead(_pin2))){
      Serial.print(F("Cbistable : Attention, activation de la sortie 1 sans que la sortie 2 soit désactive"));
      digitalWrite(_pin2,LOW);
      delay(100);
  }
  if (log)
    if (stat)
      Serial.println("-->"+_name1 + F(" ON at ") + String(millis()) + F(" ms.")); 
    else
      Serial.println("-->"+_name1 + F(" OFF at ") + String(millis()) + F(" ms.")); 
  
  digitalWrite(_pin1,stat);
  _stat1 = stat ;
}
void Cbistable::set2(bool stat){
  if ((stat)&&(digitalRead(_pin1))){
      Serial.print(F("Cbistable : Attention, activation de la sortie 2 sans que la sortie 1 soit désactive"));
      digitalWrite(_pin1,LOW);
      delay(100);
  }
  if (log)
    if (stat)
      Serial.println("-->"+_name2 + " ON at " + String(millis()) + F(" ms.")); 
    else
      Serial.println("-->"+_name2 + " OFF at " + String(millis()) + F(" ms.")); 
  
  digitalWrite(_pin2,stat);  
  _stat1 = stat ;
}

bool Cbistable::stat1(){
  return _stat1 ;
}

bool Cbistable::stat2(){
  return _stat2 ;
}

//***********************************
//CdigitalOut
//***********************************

CdigitalOut::CdigitalOut(const char * name){
  _pin=0;
  _name = String(name) ;
  _stattime = 0;
  _stat = false;
  _flash = false ;
  _flash_period = 0 ;
  _delay = 0;
  log = false ; 
}
void CdigitalOut::setPin(uint8_t pin){
  _pin=pin;
  pinMode(_pin,OUTPUT);    
}
void CdigitalOut::on(unsigned long delay_on){
  _delay = delay_on;
  digitalWrite(_pin,LOW);
  _flash = false ;
  _stattime = millis();
  _stat = true ;
  if (log){
    Serial.print(F("CdigitalOut : "));
    if (_delay==0)
      Serial.println(_name + F(" ON at ") + String(_stattime) + F(" ms.")); 
    else
      Serial.println(_name + F(" ON (for ")+String(_delay)+ F(" ms) at ") + String(_stattime) + F(" ms.")); 
  }
}

void CdigitalOut::off(unsigned long delay_off){
  _delay = delay_off;
  digitalWrite(_pin,HIGH);
  _flash = false ;
  _stattime = millis();
  _stat = false ;
  if (log){
    Serial.print(F("CdigitalOut : "));
    if (_delay==0)
      Serial.println(_name + F(" OFF at ") + String(_stattime) + F(" ms.")); 
    else
      Serial.println(_name + F(" OFF (for ") +String(_delay)+ F(" ms) at ") + String(_stattime) + F(" ms.")); 
  }
}

void CdigitalOut::set(bool on_off) //mise à 1 ou à 0
{
  _delay = 0;
  digitalWrite(_pin,on_off);
  _flash = false ;
  _stattime = millis();
  _stat = on_off ;
  if (log){
    Serial.print(F("CdigitalOut : "));
    if (!on_off)
      Serial.println(_name + F(" ON at ") + String(_stattime) + F(" ms.")); 
    else
      Serial.println(_name + F(" OFF at ") + String(_stattime) + F(" ms.")); 
  }  
}

void CdigitalOut::flash(unsigned long flash_period){
  if (log&&!_flash){
    Serial.print(F("Cled : "));
    Serial.println(_name + F(" flash on (Flash period :")+String(flash_period) +F(" ms) at ") + String(_stattime) + F(" ms.")); 
  }
  _flash = true ;
  _flash_period = flash_period ;
  _delay =0 ;
}
 
void CdigitalOut::update(){
  if (_flash){
     if ((millis()-_stattime)>(_flash_period/2)){
      _stat=!_stat ;
      digitalWrite(_pin,_stat);
      _stattime = millis();
    }       
  }
  if (_delay !=0){
    if ((millis()-_stattime)>_delay){
      if (_stat)
        off();
      else
        on() ;
    }
  }
}

bool CdigitalOut::stat(){
  return _stat;
};
