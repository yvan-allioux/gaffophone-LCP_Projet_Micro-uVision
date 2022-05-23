//===========================================================//
// Projet Micro - INFO1 - ENSSAT - S2 2022      						 //
//===========================================================//
// File                : Programme de départ
// Hardware Environment: Open1768	
// Build Environment   : Keil µVision
/*
TO DO


NOTE POUR LORAL
- note de misique non bloquante grace a l'interuption
*/
//===========================================================//
/**
 *	@author yvan
 */
 
#include "lpc17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_timer.h"
#include "touch\ili_lcd_general.h"
#include "touch\lcd_api.h"
#include "affichagelcd.h"
#include "touch\touch_panel.h"
#include "lpc17xx_i2c.h"


#include "globaldec.h" // fichier contenant toutes les déclarations de variables globales
#include <stdio.h>

//TIMER DEFFINE
#define precision 50 // précion des temps en microsecondes
#define demie_periode 10 // duree de la demie-periode du signal en ms, plus c'est eleve plus c'est grave
#define countTactilCouleur 10
//i2c
#include "lpc17xx_i2c.h"


//INITIALISATION DU PIN CONECTEUR BLOCK (gpip pinsel ...)
void initPinConnectBloc()//----------------------------PIN CONNECT----
{
	/**
	 * Déclaration des structures
	 */
	// declaration des structures de configuration
	PINSEL_CFG_Type pinLed; //initialisation d'une structure cette var a le nom pin
	PINSEL_CFG_Type pinBuz;
		
	/**
	 * 	Initialisation des informations de la Pin0.0 GPIO
		*	PINSEL_CFG_Type a des valeur 
	 */

	pinLed.Portnum = 0;
	pinLed.Pinnum = 0;
	pinLed.Funcnum = 0;
	pinLed.Pinmode = 0;
	pinLed.OpenDrain = 0;

	LPC_PINCON->PINSEL0 = 0; //?
	LPC_PINCON->PINMODE0 = 0;
	
	/**
	 * 	Initialisation des informations de la Pin1.9 GPIO
		*	PINSEL_CFG_Type a des valeur 
	 */

	pinLed.Portnum = 1;
	pinLed.Pinnum = 9;
	pinLed.Funcnum = 0;
	pinLed.Pinmode = 0;
	pinLed.OpenDrain = 0;

	LPC_PINCON->PINSEL2 = 0;
	LPC_PINCON->PINMODE0 = 0;

	/*
	 * Initialisation des registres
	 */
	 
	PINSEL_ConfigPin(&pinLed);
	
	PINSEL_ConfigPin(&pinLed);
	
	// Configuration du PIN Connect Block pour la mémoire
	LPC_PINCON->PINSEL1 |= (1 << 22);
	LPC_PINCON->PINSEL1 |= (1 << 24);
	
	// Led en sortie du GPIO
	GPIO_SetDir(0, (1<<0), 1);
	// buz en sortie du GPIO
	GPIO_SetDir(1, (1<<9), 1);
	
	return;
}


//INITIALISATION DU TIMER
void initTimer()
{
	/**
	 * Déclaration des structures
	 */

	// TP : Timer Precision, pour timer mode et precision
	TIM_TIMERCFG_Type timer;

	// TM : Timer Match, pour timer match
	TIM_MATCHCFG_Type match;
	
	
	/**
	 * Initialisation des du mode timer et de la précision du timer
	 */
	
	// 0 ou 1 (type enum)
	timer.PrescaleOption = TIM_PRESCALE_USVAL; 
	
	// valeur donnee en micro-seconde, précion du temps en microsecondes
	timer.PrescaleValue = precision ;

	// Initialisation des registres
	TIM_Init(LPC_TIM0,TIM_TIMER_MODE,&timer);
	
	// Prescal Register : PR = 250000 c'est la fréquence 
	//LPC_TIM0->PR = 250000; // précision 10ms   suprimer car on a PrescaleValue qui fais la meme chose
	
	/**
	 * Initialisation des actions à suivre lors d'un match
	 */
	
	// utilisation du MR0
	match.MatchChannel = 0;
	
	// temps d'une demi-période
	//match.MatchValue = 50; // 50*10ms = 500ms = 0.5s, 50 pour la led
	match.MatchValue = demie_periode;
	
	// interruption à chaque match = actif
	match.IntOnMatch = ENABLE;
	
	// arrêt à chaque match : inactif
	match.StopOnMatch = DISABLE;
	
	// reset TC à chaque match : actif
	match.ResetOnMatch = ENABLE;
	
	// inversion de l'état à chaque match
	match.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	//match.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	
	
	/**
	 * Application de la configuration du match
	 */
	 
	TIM_ConfigMatch(LPC_TIM0, &match);
	
	// Autorisation NVIC d'interruption du traitant "TIMER0_IRQn"
	NVIC_EnableIRQ(TIMER0_IRQn);
	
	/**
	 * Lancement du timer
	 */
	 
	TIM_Cmd(LPC_TIM0, ENABLE);
	
	return;
}



//INITIALISATION DU TIMER POUR LA MUSIQUE
void initTimerMusique()
{
	/*
	TIM_TIMERCFG_Type timerMusique;
	TIM_MATCHCFG_Type matchMusique;
	timerMusique.PrescaleOption = TIM_PRESCALE_USVAL; 
	timerMusique.PrescaleValue = precision ;
	TIM_Init(LPC_TIM1,TIM_TIMER_MODE,&timerMusique);
	matchMusique.MatchChannel = 0;
	matchMusique.MatchValue = demie_periode;
	matchMusique.IntOnMatch = ENABLE;
	matchMusique.StopOnMatch = DISABLE;
	matchMusique.ResetOnMatch = ENABLE;
	matchMusique.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	TIM_ConfigMatch(LPC_TIM1, &matchMusique);
	NVIC_EnableIRQ(TIMER0_IRQn);
	TIM_Cmd(LPC_TIM1, ENABLE);
	*/
	return;
}

//FONCTION APELE LOR DUNE INTERUPTION
	void TIMER0_IRQHandler ()
	{
		if ( (GPIO_ReadValue(1)& (1<<9)) == 0 ){
				GPIO_SetValue(1, (1<<9));
		}else{
				GPIO_ClearValue(1, (1<<9));
		}
		TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}


//INITIALISATION I2C
void init_i2c(void){
	//I2C_Init(LPC_I2C0, 250000); // freq a 250 khz
	I2C_Init(LPC_I2C0, 500000); // freq a 500 khz
	I2C_Cmd(LPC_I2C0,ENABLE); // aprouve les operation sur l I2C
}


//LECTURE DE LA MEMOIRE
void i2c_eeprom_read(uint16_t addr, uint8_t* data, int length)
{
	
	I2C_M_SETUP_Type config; // config I2C
	uint8_t addr_ligne = addr & 0xFF;
	config.sl_addr7bit = 0x50 | (addr >> 8); // 0101 | addr >> 8 pour ne récupérer que les 3 bits de poids fort de l'adresse de mémoire
		
	config.tx_data = &addr_ligne;
	config.tx_count = 0;
	config.tx_length = 1;
	
	config.rx_data = data;
	config.rx_length = length;

	// Nombre de retransmission à zéro de base et maximum 3
	config.retransmissions_max = 3;
	config.retransmissions_count = 0;
	config.status = 0;
	
	I2C_MasterTransferData(LPC_I2C0, &config, I2C_TRANSFER_POLLING);
	
}


//ECRITURE DANS LA MEMOIRE
uint16_t i2c_eeprom_write(uint16_t addr, uint8_t* data, int length)
{
	int i;
	uint8_t data_to_write[255];
	I2C_M_SETUP_Type config; // config I2C
	
	// 0101 | addr >> 8 pour ne récupérer que les 3 bits de poids fort de l'adresse de mémoire
	config.sl_addr7bit = 0x50 | (addr >> 8); 

	data_to_write[0] = ((addr) & (0xFF)); // Récupération des lignes de la mémoire à transmettre dans les données
	for(i=0; i < length; i++)
	{
		data_to_write[i+1] = data[i]; // Récupération des données à écrire
	}
	
	config.tx_data = data_to_write; // Données à écrire
	config.tx_count = 0; // Compteur des données à transmettre à 0
	config.tx_length = length + 1; // Taille de l'adresse + les données
	// Nombre de retransmission à zéro de base et maximum 3
	config.retransmissions_max = 3;
	config.retransmissions_count = 0;
	config.status = 0;
	
	I2C_MasterTransferData(LPC_I2C0, &config,I2C_TRANSFER_POLLING);
	
}
//JOUER UNE MUSIQUE
void uneMusique(){
	int n;
	//desactivation touch_read
	//pas besoin de desactivation si on apelle la fonction quand on clic sur un endrois
	
	TIM_ResetCounter(LPC_TIM0);
	TIM_UpdateMatchValue(LPC_TIM0,0,22);//DO
	dessiner_rect(10,60,110,110,2,1,Black,Blue2);
	n=sprintf(chaine,"DO");
	LCD_write_english_string (60,110,chaine,Black,Blue2);
	//timer 0.5 s ... utiliser un 2eme timer ???
	dessiner_rect(10,60,110,110,2,1,Black,Cyan);//DO
	n=sprintf(chaine,"DO");
	LCD_write_english_string (60,110,chaine,Black,Cyan);
	
	
	TIM_ResetCounter(LPC_TIM0);
	TIM_UpdateMatchValue(LPC_TIM0,0,21);//RE
	dessiner_rect(120,60,110,110,2,1,Black,Blue2);//RE
	n=sprintf(chaine,"RE");
	LCD_write_english_string (170,110,chaine,Black,Blue2);
	//timer 0.5 s ... utiliser un 2eme timer ???
	dessiner_rect(120,60,110,110,2,1,Black,Cyan);//RE
	n=sprintf(chaine,"RE");
	LCD_write_english_string (170,110,chaine,Black,Cyan);
	
}

//===========================================================//
// Function: Main
//===========================================================//
int main(void)
{	  
	int n;
	uint8_t varMemoire = 42;
	uint8_t varMemoireLecture = 0;

	
	//variable pour coriger et controler la modification du match avec le tactil
	//BUG DU MATCH VALUE : quand on change le match value parfois cela bloque le timer, aprèes de longue investigation il ce pourais que ce sois le passag a match value plus petit que actuel
	
	demiPeriodeGlobale = 0;//var pour n'avoir que 1 etat par interaction dans les boucle

	countTactilJaune = 0;
	countTactilVert = 0;
	countTactilBleu = 0;
	countTactilRouge = 0;
	

	initPinConnectBloc();
	initTimer();
	

	init_i2c();
	i2c_eeprom_write(0, &varMemoire, sizeof(varMemoire));
	i2c_eeprom_read(0, &varMemoireLecture, sizeof(varMemoireLecture));

	
	TIM_UpdateMatchValue(LPC_TIM0,0,30);
	  // Init(); // init variables globales et pinsel pour IR => à faire 
	  lcd_Initializtion(); // init pinsel ecran et init LCD
		// affichage sur l'écran d'un exemple de 4 carrés de couleur et d'une chaine de caractère
	  n=sprintf(chaine,"gaffophone V0.47 yvan");
	  LCD_write_english_string (32,30,chaine,White,Blue);
		
		//carée
	  dessiner_rect(10,60,110,110,2,1,Black,Cyan);//DO
		n=sprintf(chaine,"DO");
	  LCD_write_english_string (60,110,chaine,Black,Cyan);
		
		dessiner_rect(120,60,110,110,2,1,Black,Cyan);//RE
		n=sprintf(chaine,"RE");
	  LCD_write_english_string (170,110,chaine,Black,Cyan);
		
	  dessiner_rect(10,170,110,110,2,1,Black,Cyan);//MI
		n=sprintf(chaine,"MI");
	  LCD_write_english_string (60,210,chaine,Black,Cyan);
		
	  dessiner_rect(120,170,110,110,2,1,Black,Cyan);//FA
		n=sprintf(chaine,"FA");
	  LCD_write_english_string (170,210,chaine,Black,Cyan);
		
		//dessiner_rect(60,100,4,4,0,1,Black,Black);//TEST A SUPRIMER
		//dessiner_rect(65,100,4,4,0,1,Red,Red);//TEST A SUPRIMER

	  touch_init(); // init pinsel tactile et init tactile; à ne laisser que si vous utilisez le tactile
		
		//on stop le bruir en metant une periode très longue (pas propre comme methode mais bon ...)
		TIM_ResetCounter(LPC_TIM0);
		TIM_UpdateMatchValue(LPC_TIM0,0,100000000);
		
	  GPIO_SetDir(0, (1<<0), 0); //la led est eteinte quand linitialisation est terminer
		
    while(1){

			
			touch_read();
			if(((touch_x > 600) && (touch_x < 2000)) && ((touch_y > 2000) && (touch_y < 3000))){//jaune
				countTactilJaune++;
				if(demiPeriodeGlobale != 10 && countTactilJaune > countTactilCouleur){
					
					demiPeriodeGlobale = 10;
					TIM_ResetCounter(LPC_TIM0);
					TIM_UpdateMatchValue(LPC_TIM0,0,22);//DO
					GPIO_SetDir(0, (1<<0), 1);
					countTactilJaune = 0;
					
					dessiner_rect(10,60,110,110,2,1,Black,Blue2);
					n=sprintf(chaine,"DO");
					LCD_write_english_string (60,110,chaine,Black,Blue2);
				}
			}else if(((touch_x > 2100) && (touch_x < 3600)) && ((touch_y > 2000) && (touch_y < 3000))){//vert
				countTactilVert++;
				if(demiPeriodeGlobale != 20 && countTactilVert > countTactilCouleur){
					
					demiPeriodeGlobale = 20;
					TIM_ResetCounter(LPC_TIM0);
					TIM_UpdateMatchValue(LPC_TIM0,0,21);//RE
					countTactilVert = 0;
					
					dessiner_rect(120,60,110,110,2,1,Black,Blue2);//RE
					n=sprintf(chaine,"RE");
					LCD_write_english_string (170,110,chaine,Black,Blue2);
				}
			}else if(((touch_x > 600) && (touch_x < 2000)) && ((touch_y > 700) && (touch_y < 1800))){//bleu
				countTactilBleu++;
				if(demiPeriodeGlobale != 30 && countTactilBleu > countTactilCouleur){
					
					demiPeriodeGlobale = 30;
					TIM_ResetCounter(LPC_TIM0);
					TIM_UpdateMatchValue(LPC_TIM0,0,20);//MI
					countTactilBleu = 0;
					
					dessiner_rect(10,170,110,110,2,1,Black,Blue2);//MI
					n=sprintf(chaine,"MI");
					LCD_write_english_string (60,210,chaine,Black,Blue2);
				}
			}else if(((touch_x > 2100) && (touch_x < 3600)) && ((touch_y > 700) && (touch_y < 1800))){//rouge
				countTactilRouge++;
				if(demiPeriodeGlobale != 40 && countTactilRouge > countTactilCouleur){
					
					demiPeriodeGlobale = 40;
					TIM_ResetCounter(LPC_TIM0);
					TIM_UpdateMatchValue(LPC_TIM0,0,18);//FA
					countTactilRouge = 0;
					
					dessiner_rect(120,170,110,110,2,1,Black,Blue2);//FA
					n=sprintf(chaine,"FA");
					LCD_write_english_string (170,210,chaine,Black,Blue2);
				}
			}else{
				if(demiPeriodeGlobale != 50){//pour ne passer que 1 fois ici quand on ne touche pas l'ecrant (detecte le levée de doit en gros)
					switch(demiPeriodeGlobale)//pour ne reaficher la couleur originale que d'un carée
					{
					case 10:
						dessiner_rect(10,60,110,110,2,1,Black,Cyan);//DO
						n=sprintf(chaine,"DO");
						LCD_write_english_string (60,110,chaine,Black,Cyan);
						break;
					case 20:
						dessiner_rect(120,60,110,110,2,1,Black,Cyan);//RE
						n=sprintf(chaine,"RE");
						LCD_write_english_string (170,110,chaine,Black,Cyan);
						break;
					case 30:
						dessiner_rect(10,170,110,110,2,1,Black,Cyan);//MI
						n=sprintf(chaine,"MI");
						LCD_write_english_string (60,210,chaine,Black,Cyan);
						break;
					case 40:
						dessiner_rect(120,170,110,110,2,1,Black,Cyan);//FA
						n=sprintf(chaine,"FA");
						LCD_write_english_string (170,210,chaine,Black,Cyan);
						break;
					default:
						//rien
						break;
					}
					
					//i2c_eeprom_read(0, &varMemoireLecture, sizeof(varMemoire));
					n=sprintf(chaine,"nb de note %d", varMemoireLecture);
					LCD_write_english_string (60,290,chaine,Black,Blue);
					
					TIM_ResetCounter(LPC_TIM0);
					TIM_UpdateMatchValue(LPC_TIM0,0,100000000);
					demiPeriodeGlobale = 0;
					countTactilJaune = 0;
					countTactilVert = 0;
					countTactilBleu = 0;
					countTactilRouge = 0;
					demiPeriodeGlobale = 50;
				}//fin detection lever doit de lecrant
				
				
			}
			
		}//fin while
	 
		
		
		
		
	}

//---------------------------------------------------------------------------------------------	
#ifdef  DEBUG
void check_failed(uint8_t *file, uint32_t line) {while(1);}
#endif
