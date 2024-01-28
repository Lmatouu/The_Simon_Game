////////////////////////////////////////////////////////
//Bibliothèque programmation par graph d'état (Stat Machine)
//Contient les objets :
//Cstat, Ctimer, Cevent, CdigitalOut, Cbistable
//Contient les fonctions :
//transition, when et after
//Version 1.5.1 du 04 06 2020, A DUGAS CPGE - Lycée Livet
////////////////////////////////////////////////////////
//Nouveauté version 1.3 : Important
    //La fonction stat() permet de savoir si l'état est actif. Attention, elle ne doit jamais être utilisée dans la transition immédiatement suivante au risque de rendre le systeme instable.
    //En effet, dans ce cas l'activité _entry risque de ne jamais être exécutée (Cas ou dans l'activité _entry on inititalise une variable utilisée dans la transition suivant).
    //La fonction _interne_stat() doit être utilisée à la place, _interne_stat() revoie true une fois _entry exécutée. 
//Nouveauté version 1.3.1 :
		//Ajout de la méthode resetStattime dans Cstat
//Nouveauté version 1.4.0 : Importante
    //Simplification de la class Cevent (On ne garde que .rising et .falling)
		//Ajout de la classe Cbutton qui reprend Cevent avec en plus :
				//.click, .doubleClick et .longPress
		//Refonte de la classe Ctimer (Suppression de Every)
		//Ajout de la classe Cpulse pour gérer le every
//Correction de bug version 1.4.1 :
    //Correction de Cevent::SetPin et Cbutton::SetPin
    //Lecture de l'entrée pour mettre à jour l'état après la définition de la pin d'entrée
//Nouveauté version 1.4.3
		//Modification de la méthode update de CStat
		//Modification de la méthode update de CEvent
		//Modification de la méthode update de CButton	
		//Optimisation de Cbutton::update
//Nouveauté version 1.4.4 du 31/05/2018
    //Correction bug affichage dans Serial sur CButton
//Nouveauté version 1.4.5 du 29/05/2019
		//Modification de la méthode update de CEvent et CButton
			//ANTI_REBOUNCE_TIME devient la valeur par défaut qui peut être modifié avec la propriété : anti_rebounce_time (CEvent et CButton)
		//Modification de la méthode constructeur de CButton
			//Le long_press_time peut être défini en début de programme par LONG_PRESS_TIME
 //Nouveauté version 1.5.0 du 05/05/2020
    //Nouvelle fonctionalité
      //Ajout de la fonction when pour les transitions booleenes
 //Nouveauté version 1.5.1 du 04/06/2020
    //Nouvelle fonctionalité
      //Ajout de la fonction completion pour les transition implicites 
      //Ajout des log sur les transitions (activé par défaut)     
////////////////////////////////////////////////////////


#ifndef obj_stat_h
#define obj_stat_h

#include <Arduino.h>

//Class CButton et Cevent
#define ANTI_REBOUNCE_TIME 10 //Valeur par défaut en ms permettant de neutraliser les rebons dans Cevent et Cbutton
#define CLICK_TIME 500 //Valeur maxi en ms permettant de considerer un créneau comme un clic
//Class CButton
#define LONG_PRESS_TIME 2000 //Réglage de long press pour CButton

#define LOG_TRANSITION true  //Permet de mettre un log lors du franchissement d'une transition

//***********************************
//Cstat
//***********************************
class Cstat
{
  public:
    Cstat(const char * stat_name ="", int stat_id = 0); //Constructeur 
    //Log
    bool log ;     //Permet d'afficher les log (par défaut true)
    int id ;       //Permet de donner un numéro d'identifiant à l'état
    String name(); //Renvoie le nom de l'état
    //Etat
    bool entry_(); //Vrai lorsque l'on rentre dans l'état
    bool do_();    //Vrai lorsque l'on est dans l'état
    bool exit_();  //Vrai lorsque l'on sort de l'état
    bool stat();   //stat = _entry||_do 
    bool completion_event;  //à mettre à true lorsque l'état est consdéré comme terminé (permet de franchir une transition de type completion)
    
    //La fonction stat() permet de savoir si l'état est actif. Attention, elle ne doit jamais être utilisée dans la transition immédiatement suivante au risque de rendre le systeme instable.
    //En effet, dans ce cas l'activité _entry risque de ne jamais être exécutée (Cas ou dans l'activité _entry on inititalise une variable utilisée dans la transition suivant).
    //La fonction _interne_stat() doit être utilisée à la place, _interne_stat() revoie true une fois _entry exécutée.  
    
    bool interne_stat(); //interne_stat = _do
    //Activation, Désactivation
    Cstat* activate() ;  //Active l'état et renvoie un pointeur sur l'état courant, si p_active <> 0 alors l'état pointé est automatiquement désactivé (Sauf si p_active pointe vers lui même (cas d'un réactivation))
    void desactivate() ; //Désactive l'état courant
    //After
    bool after(unsigned long delay_ms) ; //Vaut true apres un délais imposée depuis l'activation de l'état ou le dernier resetStattime
    void resetStattime() ; //Permet de remettre à la date courrante la date d'entrée dans l'état
    //Mise à jour
    bool update() ; //A placer a la fin de la fonction loop OBLIGATOIREMENT   
										//Renvoie true si il y a une évolution et false sinon V1.4.3
  private:
  //public:
    String _name ;            //Nom de l'état à afficher dans les logs
    //Variables carractérisant l'état
    bool _entry ;             //_entry, _do, _exit ne peuvent pas être à true simultanément
    bool _do ;
    bool _exit ;
    unsigned long _stattime ; //Date en ms de la dernière activation ou réactivation de l'état

    //Log 
    void _log(String  message,  bool titre = false) ;   //Equivalent à print  si log=true (ne fait rien sinon), si titre = true alors on affiche le nom de la classe avant
    void _logln(String  message,  bool titre = false) ; //Equivalent à println  si log=true (ne fait rien sinon), si titre = true alors on affiche le nom de la classe avant
};

boolean transition(Cstat* &pActive, Cstat &from,Cstat &to, boolean event, boolean guard = true); //event: évenement, guard : condition de garde, from : état avant, to : état après, pActive : pointeur sur l'état actif
boolean completion(Cstat* &pActive, Cstat &from,Cstat &to, boolean guard = true); //guard : condition de garde, from : état avant, to : état après, pActive : pointeur sur l'état actif
boolean when(Cstat* &pActive, Cstat &from,Cstat &to, boolean condition); //condition: condition booleene de franchissement, from : état avant, to : état après, pActive : pointeur sur l'état actif
boolean after(Cstat* &pActive, Cstat &from,Cstat &to, unsigned long delay_ms, boolean guard = true); //delay: durée avant le changement d'état, guard : condition de garde, from : état avant, to : état après, pActive : pointeur sur l'état actif

//***********************************
//Ctimer
//***********************************
class Ctimer
{
  public:
    Ctimer();
    void start(); //Pour lancer le timer ou le relancer
		void stop(); //Pour stopper le timer
    bool after(unsigned long delay_ms); //Vaut true apres un délais imposée
		bool stat() ;
	private:
		bool _stat ; //Start ou stop
    unsigned long _stattime ;
};

//***********************************
//Cpulse
//***********************************
class Cpulse
{
  public:
    Cpulse();
    bool every(unsigned long delay_ms); //Retourne une impulsion tous le x ms
  private:
    bool _every ; //Permet de gérer le basculement de every
    unsigned long _stattime ;
};


//***********************************
//Cevent
//***********************************
class Cevent
{
  public:
    Cevent(const char * name="");
    bool log ; //Permet d'afficher les log (par défaut false)
    void setPin(uint8_t pin, bool interne_pull_up = false); //Spécifie si on active ou non la résistance de pull up, si oui, stat, rising et falling sont automatiquement inversés
    bool rising(); //Front montant
    bool falling(); //Front descendant
    bool change(); //Front montant ou descendant
    bool stat(); //Etat
	unsigned long anti_rebounce_time ; //Valeur en ms permettant de neutraliser les rebons (réglé par défaut à ANTI_REBOUNCE_TIME)
    bool update() ; //Mise a jour de l'évenement. A placer a la fin de la fonction loop OBLIGATOIREMENT   
										//Renvoie true si il y a une évolution et false sinon V1.4.3
  private:
    String _name ; //Nom de l'évenement à afficher dans les logs
    bool _stat ;
    bool _new_stat ;
    bool _rising;
    bool _falling;
    uint8_t _pin ;
    bool _interne_pull_up ;
    unsigned long _stattime ;
};

//***********************************
//Cbutton
//***********************************
class Cbutton
{
  public:
    Cbutton(const char * name="");
    bool log ; //Permet d'afficher les log (par défaut false)
    void setPin(uint8_t pin, bool interne_pull_up = false); //Spécifie si on active ou non la résistance de pull up, si oui, stat, rising et falling sont automatiquement inversés
    void setLongPressTime(unsigned long delay_ms); //2 secondes par défaut (cf constructeur)
    bool rising(); //Front montant
    bool falling(); //Front descendant
    bool change(); //Front montant ou descendant
    bool longPress(); //Appui long (Passe à true après un délais défini par setLongPressTime
	  bool click() ; //Clic inférieur à 500 ms
    bool doubleClick() ; //Clic inférieur à 500 ms
    bool stat(); //Etat
	unsigned long anti_rebounce_time ; //Valeur en ms permettant de neutraliser les rebons (réglé par défaut à ANTI_REBOUNCE_TIME)	
    bool update() ; //Mise a jour de l'évenement. A placer a la fin de la fonction loop OBLIGATOIREMENT 
										//Renvoie true si il y a une évolution et false sinon V1.4.3		
  private:
    String _name ; //Nom de l'évenement à afficher dans les logs
    bool _stat ;
    bool _new_stat ;
    bool _rising;
    bool _falling;
	  bool _longPress;
	  bool _longPressEvent ;
	  bool _click;
	  bool _clickEvent;
	  bool _doubleClickEvent ;
    unsigned long _long_press_time ;
    uint8_t _pin ;
    bool _interne_pull_up ;
    unsigned long _stattime ;
};


//***********************************
//Cbistable
//***********************************
class Cbistable
{
  public:
    Cbistable(const char * name1="",const char * name2="");
    bool log ; //Permet d'afficher les log (par défaut false)
    void setPin(uint8_t pin1,uint8_t pin2);
    void set1(bool stat);
    void set2(bool stat);
    bool stat1(); //Etat
    bool stat2(); //Etat
  private:
    uint8_t _pin1 ;
    uint8_t _pin2 ;
    bool _stat1 ;
    bool _stat2 ;
    String _name1 ; //Nom à afficher dans les logs
    String _name2 ; //Nom à afficher dans les logs
};
//***********************************
//CdigitalOut
//***********************************
class CdigitalOut
{
  public:
    CdigitalOut(const char * name="");
    bool log ; //Permet d'afficher les log (par défaut false)
    void setPin(uint8_t pin);
    void on(unsigned long delay_on = 0); //delay = 0 => ON permanent, delay = valeur (ms) => ON pendant valeur puis OFF
    void off(unsigned long delay_off = 0); //delay = 0 => OFF permanent, delay = valeur (ms) => ON pendant valeur puis OFF
    void set(bool on_off) ; //mise à 1 ou à 0
    void flash(unsigned long flash_period = 500); //Permet de faire clignotter la led à la période demandé en ms)
    bool stat(); //Etat
    void update() ; //Mise a jour de l'objet

  private:
    uint8_t _pin ;
    String _name ; //Nom à afficher dans les logs
    bool _stat ;
    bool _flash ; //Permet de gérer le clignotement
    unsigned long _flash_period ; //Période de clignotement
    unsigned long _stattime ;
    unsigned long _delay; //Mémorise la durée avant changement d'état

};

#endif
