#include "constantes.h" // fichier contenant toutes les constantes du projet
#include <stdint.h>
#include "lpc17xx_i2c.h"

// mettez ici toutes les déclarations de variables globales

char chaine[30]; // buffer pour l'affichage sur le LCD
uint16_t touch_x, touch_y ;

//pas propre de les metre en var global mais quand on initialise dans le main cela fais une erreur "ne peut pas localiser les fichier ... lors du transfert sur la carte)


