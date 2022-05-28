
# Gaffophone LPC17xx


Le gaffophone est une application développer pour le micro contrôleur LPC17xx
Son but et également d'apprendre un morceau de musique


## Lien

 - [Démonstration vidéo Youtube](https://youtu.be/6VjBuNxQowo)




## Features

- Clavier musical de 4 notes
- Sauvegarde du nombre de notes composé (afficher a l'écran) dans une mémoire flash
- Chargement du nombre de notes composées depuis la mémoire flash
- Affichage du nombre de notes composées
- Réinitialisation à 0 du nombre de notes composé
- Lecture d'une petite musique (au clair de la lune)
- Vérification et affichage de la validité d'un morceau composé par l'utilisateur s'il correspond ou non au morceau de musique jouée par l'application

## Utilisation

Le principe de fonctionnement est simple, appuyer sur les différentes touches pour faire de la musique

À chaque touche appuyé le compteur en bas de l'écran s'incrémente et vous permet de compter facilement le nombre de touches joué

Vous pouvez sauvegarder le nombre affiché dans la mémoire flash en appuyant sur la disquette violette en haut à gauche de l'écran

Vous pouvez charger le nombre de touches enclenché qui est sauvegarder en mémoire en appuyant sur la disquette jaune en haut à droite de l'écran

Le bouton play en haut au centre de l'écran va jouer "au clair de la lune" et prendre le total contrôle, vous ne pouvez plus interagir durant la musique

Essayer de rejouer ce morceau de musique, la prochaine fois que vous appuierez sur le bouton play, l'application vérifiera la suite de touche enclenché et vous affichera si vous avez réussi à refaire "au clair de la lune"

## Documentation

### Piste d'amélioration

La séparation entre plusieurs fichiers c a été entamé, le programme fonctionnel est disposé dans un fichier main.c unique ce qui évite les erreurs d'inclusion de fichiers même si ce n'est pas une bonne pratique de développement, j'ai commencé à découper ce fichier en plusieurs parties pour les différents périphériques mais cela ne fonctionne pas très bien cette tentative reste disponible dans un fichier zip à la racine du projet

### Périphérique utilisé

Timer

Un timer est utilisé, uniquement pour lancer des interruptions répétées afin de modifier l'état du GPIO et donc faire varier le buzzer

Mémoire flash

Une mémoire flash permet de stocker le nombre de touches enclenchées, à chaque fois que l'utilisateur relève le doigt cette variable est mise à jour et afficher, l'enregistrement dans le noir Flash avec la communication I2C s'effectue quand on appuie sur la disquette violette, le chargement de la variable enregistrée dans la mémoire flash est effectué quand on appuie sur la disquette jaune

### Fonction dans le code

Fonction initPinConnectBloc

- Cette fonction initialise le pin connector block pour les périphériques

Fonction initTimer

- Cette fonction initialise le timer

Fonction TIMER0_IRQHandler

- Cette fonction est appelée à chaque interruption du timer, et l'inverse la valeur du GPIO du buzzer et incrémente un compteur dans une variable générale qui va servir à maintenir la note pendant un temps donné (cela évite d'utiliser un deuxième timer)

Fonction init_i2c

- La fonction initialise la communication I2C

Fonction i2c_eeprom_read

- Cette fonction permet de lire dans la mémoire flash avec communication I2C

Fonction i2c_eeprom_write

- Cette fonction permet d'écrire dans la mémoire flash avec une communication I2C

Fonction uneMusique

- Cette fonction prend en paramètre le temps pendant lequel maintenir une note et la note à enclencher, cette fonction est totalement bloquante pour que l'utilisateur ne puisse pas interagir, la variable prise en paramètre pour le temps de maintien de la note et comparer avec une variable globale qui va s'incrémenter à chaque interruption du timer et permet donc de maintenir une touche enclencher par exemple pendant une seconde

### Fonction main, initialisation

La fonction main fais beaucoup de choses mais tourne surtout sur la gestion de l'écran, elle comporte beaucoup d'instruction lourdes et bloquante mais cela ne pose pas de problème puisque le son est géré par le timer

Dans un premier temps la fonction main initialise toutes les variables qui vont être utilisées

Ensuite les différentes fonctions d'initialisation sont appelés
Initialisation du Pin connect block initialisation du timer initialisation de l'i2c initialisation de l'écran et du tactile de l'écran
Affichage de tous les éléments graphiques
extinction de la LED numéro 0 et production d'un petit bip pour signaler la fin de l'initialisation

### Fonction main, la boucle principale

La boucle principale tourne autour de la lecture du tactile à chaque tour de boucle on regarde quelles sont les valeurs pour savoir si l'utilisateur cliquez à un endroit

Si l'utilisateur clique sur la touche DO par exemple, cela va directement être détecté, un compteur spécifique à la touche DO va être incrémenté à chaque tour de boucle, au bout de 10 tours de boucle on sait que l'utilisateur a bien appuyé sur la touche DO et ce n'est pas un bug du tactile, on vérifie également si la touche DO n'est pas déjà enclenché avec la variable selectTactile qui par exemple pour la touche DO va prendre la valeur 10, si cette valeur n'est pas égal à 10 on sait que l'on a donc pas activer la touche DO.

Par la suite pour lancer la note de musique, on active le timer, on le reset pour que le compteur du timer soit à zéro et on change le match value du timer afin d'attendre plus ou moins longtemps entre chaque interruption

Pour changer la fréquence du buzzer
La réinitialisation du timer est très importante car sinon le compteur dans le timer pourrait se retrouver supérieur à une match value modifier à la volée ce qui entraînerait un blocage du timer !

À ce moment-là on modifie également la couleur de la touche pour donner un feedback à l'utilisateur.

Si l'utilisateur ne touche rien du tout et que la variable selectTactile n'est pas égal à 50, on sait que l'utilisateur vient de lever le doigt, et donc on passe selectTactile à 50 pour avoir une condition qui s'exécute uniquement quand l'utilisateur lève le doigt.
Avant de passer selectTactil a 50, on regarde sa valeur pour savoir quelle était la touche appuyer, en fonction de ça on affiche les couleurs normales du clavier et on enregistre dans un tableau la touche appuyée pour vérifier si l'utilisateur a réussi à jouer "au clair de la lune"

Lorsque l'utilisateur lève son doigt d'une touche on va également mettre à jour l'affichage du petit compteur, désactiver le timer, et réinitialiser toutes les variables qui en ont besoin

Les fonctions des quatre premières conditions pour les quatre touches du clavier do ré mi fa sont très similaires

Les deux conditions suivantes correspondent au clic sur les disquettes pour charger ou sauvegarder le compteur de frappe

Une condition sert à détecter si l'utilisateur clique sur le texte du compteur en bas de l'écran afin de le réinitialiser

Et la dernière condition sert à détecter si utilisateur clique sur le bouton play.
À ce moment-là on regarde si les frappes de l'utilisateur sont égales à la séquence de frappe de la musique "au clair de la lune" si oui on affiche le texte valide sinon on affiche le texte non valide.
Suite à cela on met la LED à un pour montrer à l'utilisateur que l'on ne peut pas interagir avec l'écran, et on va appeler la fonction uneMusique pour jouer une note pendant un certain temps (ici on joue au clair de la lune)
Et on réinitialise le tableau pour stocker les frappes utilisateurs sur le clavier musical afin de les recevoir, les 11 prochaines frappes seront donc enregistrées et comparer lors du prochain lancement sur le bouton play





