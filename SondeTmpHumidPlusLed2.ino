// --- Programme Arduino ---

// Auteur du Programme : Justin Briard -
// Programme écrit le : 02/01/2013.

// ------- Licence du code de ce programme -----
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License,
//  or any later version.
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

// ////////////////////  PRESENTATION DU PROGRAMME ////////////////////

// -------- Que fait ce programme ? ---------
 /* Ce programme


*/

// --- Fonctionnalités utilisées ---


// -------- Circuit à réaliser ---------



// --- Déclaration des constantes ---

// --- Inclusion des librairies ---

#include "DHT.h"
#include <Ethernet.h>    /* Pour la shield ethernet */
#include <SPI.h>         /* Pour la communication bas niveau (carte SD et ethernet) */
#include <EthernetUdp.h> /* Pour la communication UDP */
// --- Déclaration des constantes utiles ---

const int R=1;
const int V=1;
const int B=1;



#define DHTPIN 2     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)


/** Taille du buffer pour la lecture du fichier texte et l'envoi de la requete HTTP */
static const byte BUFFER_SIZE = 50;

/** Adresse MAC de l'ethernet shield */
static byte localMacAddress[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xEF };

/** Port d'écoute local */
static const unsigned int listenPort = 8888;

/** Adresse IP serveur perso PHP */
//static IPAddress serverIpAddress(192, 168, 0, 101);
char URLAddress[]="Sonde-Terrario.local";
/** Instance de EthernetUDP permettant l'échange de données via UDP */
static EthernetUDP Udp;

// --- Déclaration des constantes des broches E/S numériques ---

const int ledRouge=3; // Constante pour la broche 3
const int ledVert=5; // Constante pour la broche 5
const int ledBleu=6; // Constante pour la broche 6

// --- Déclaration des constantes des broches analogiques ---


// --- Déclaration des variables globales ---


// --- Déclaration des objets utiles pour les fonctionnalités utilisées ---
DHT dht(DHTPIN, DHTTYPE);


// ////////////////////////// 2. FONCTION SETUP = Code d'initialisation //////////////////////////
// La fonction setup() est exécutée en premier et 1 seule fois, au démarrage du programme

void setup()   { // debut de la fonction setup()
Serial.begin(9600); 

dht.begin();

// --- ici instructions à exécuter 1 seule fois au démarrage du programme ---

// ------- Initialisation fonctionnalités utilisées -------  


// ------- Broches en sorties numériques -------  
 pinMode (ledVert,OUTPUT); // Broche ledVert configurée en sortie
 pinMode (ledRouge,OUTPUT); // Broche ledRouge configurée en sortie
 pinMode (ledBleu,OUTPUT); // Broche ledBleu configurée en sortie

// ------- Broches en entrées numériques -------  

// ------- Activation si besoin du rappel au + (pullup) des broches en entrées numériques -------  

// ------- Initialisation des variables utilisées -------  

// ------- Codes d'initialisation utile -------  

//Initialisation
Serial.println("Test LED");
digitalWrite(ledBleu,LOW); // allume la couleur voulue
delay(500); // pause
digitalWrite(ledBleu,HIGH);
digitalWrite(ledRouge,LOW); // allume la couleur voulue
delay(500); // pause
digitalWrite(ledRouge,HIGH);
Blinkled(2,4);
digitalWrite(ledVert,LOW); // allume la couleur voulue
delay(700); // pause
Serial.println("Ready");


    // start Ethernet and UDP
  if (Ethernet.begin(localMacAddress) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
        Blinkled(1,4);
    for(;;)
      ;
  }
  Udp.begin(listenPort);
  
  

} // fin de la fonction setup()
// ********************************************************************************
int NbrBrumisation;
float BoucleLecture = 100;

////////////////////////////////// 3. FONCTION LOOP = Boucle sans fin = coeur du programme //////////////////
// la fonction loop() s'exécute sans fin en boucle aussi longtemps que l'Arduino est sous tension

void loop(){ // debut de la fonction loop()
float Temperature = 0;
float Humidite = 0;
float TemperatureT = 0;
float HumiditeT = 0;
int CptBrumisation = 0;

// boucle lecture temperature humidite
for (int i=1; i <= BoucleLecture; i++){ 
  float h = dht.readHumidity();
  float t = dht.readTemperature();
   if (isnan(t) || isnan(h)) {
    Serial.println("Failed to read from DHT");
    Blinkled(1,4);
  } else {
 Temperature = t + Temperature;
 Humidite = h + Humidite;
  }
delay(500); 
  } // fin de la boucle for
  
  // moyenne des valeurs
TemperatureT = Temperature / BoucleLecture + 1;
HumiditeT = Humidite / BoucleLecture;
  // Affichage Valeurs
    Serial.println("##############################"); 
    Serial.print("Humidity: "); 
    Serial.print(HumiditeT);
    Serial.print(" %\t");
    Serial.print("Temperature: "); 
    Serial.print(TemperatureT);
    Serial.println(" *C");
    Serial.println("##############################"); 
    // Alerte temperature humidite led 
if (TemperatureT < 20 || HumiditeT < 80 || TemperatureT > 32)
{
Blinkled(1,2);
digitalWrite(ledRouge,LOW);
}
else
{
digitalWrite(ledVert,LOW); // allume la couleur voulue
}

// action pulverisation
if (HumiditeT < 80)
{
  if (NbrBrumisation > 5)
  {
Serial.print("Trop de declenchement de la Brumisation : ");
Serial.println(NbrBrumisation);
Blinkled(3,2);
Blinkled(1,5);
digitalWrite(ledRouge,LOW);
  }
  else
  {
Brumisation(1000);
CptBrumisation = 1;
NbrBrumisation++;
  }
}
  
if (sendData(TemperatureT, HumiditeT, CptBrumisation)) { /* Réussite */
    	/* Message de debug */
    Serial.println(F("Envoi ok"));
	/* Tentative d'envoi des rapports en attente */
	 } else { /* Echec */
	/* Message de debug */
    Serial.println(F("Echec envoi"));
    Blinkled(1,6);
digitalWrite(ledRouge,LOW);
  }

//while(1); // stop loop

} // fin de la fonction loop() - le programme recommence au début de la fonction loop sans fin
// ********************************************************************************


// ////////////////////////// FONCTIONS DE GESTION DES INTERRUPTIONS ////////////////////





// ////////////////////////// AUTRES FONCTIONS DU PROGRAMME ////////////////////



void Blinkled(int Color, int Blink) {
int varB =0;
while(varB < Blink){ 
if (Color==1) digitalWrite(ledRouge,LOW); delay(200); digitalWrite(ledRouge,HIGH);// pause
if (Color==2) digitalWrite(ledVert,LOW); delay(200); digitalWrite(ledVert,HIGH);// pause
if (Color==3) digitalWrite(ledBleu,LOW); delay(200); digitalWrite(ledBleu,HIGH);// pause
varB++; // incrémente la variable 
}
}


void Brumisation(int Time) {
// code lancement pulverisation
 Serial.println("Debut de la Brumisation");
 delay(Time); 
 Serial.println("Fin de la Brumisation");
}




/** Fonction d'envoi au serveur PHP */
boolean sendData(float TemperatureT, float HumiditeT, int CptBrumisation) {
  
  /* Buffer contenant une partie de la requete HTTP */
  char buffer[BUFFER_SIZE];
  
  /* Ouverture de la connexion TCP */
  EthernetClient client;
  Serial.print(F("Connexion au serveur ... "));
  if (!client.connect(URLAddress, 80)) {
  
    /* Erreur */
    Serial.println(F("Echec !"));
    return false;
  }
  Serial.println(F("Ok !"));

  /* Envoi de la requete HTTP */
  client.print(F("GET /arduino/registerData.php?temperature="));
  client.print(TemperatureT);
  client.print("&humidite=");
  client.print(HumiditeT);
  client.print("&brumisation=");
  client.print(CptBrumisation);
  client.println(F(" HTTP/1.0"));
  client.println();

  /* Fin de la requete HTTP */
  Serial.println(F("Fin envoi ..."));
  delay(50); /* Permet au serveur distant de recevoir les données avant de fermer la connexion */
  client.stop();
  
  /* Pas d'erreur */
  return true;
}
// ////////////////////////// Fin du programme ////////////////////

