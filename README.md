# IOT-Temperature-Monitor

Ce projet est un programme en C qui simule un système de surveillance de température.
Il prend une mesure toutes les X secondes, vérifie si la température dépasse des seuils et enregistre tout dans des fichiers texte.

Ce que le programme fait:
-Génère des températures aléatoires (simulation d’un capteur)
-Compare les valeurs aux seuils minimum et maximum
-Détecte des alertes de niveau 1, 2 ou 3
-Sauvegarde chaque mesure dans journal_mesures.txt
-Peut créer un rapport complet dans rapport_journalier.txt
-Enregistre les températures dans un arbre pour faire des statistiques
-Permet de modifier les paramètres dans config.txt

Fichiers utilisés:

-config.txt : contient les paramètres (seuils, intervalle, etc.)
-journal_mesures.txt : toutes les mesures enregistrées
-rapport_journalier.txt : créé quand on choisit l’option 4
-programme.c : le code du projet

Menu du programme:
-Quand on lance le programme, on a :
 1-Lancer la simulation
 2-Modifier la configuration
 3-Afficher la configuration
 4-Générer un rapport
 5-Quitter
 
Configuration:

Le programme lit automatiquement config.txt.
Si le fichier n’existe pas, il utilise des valeurs par défaut.
On peut changer :
-seuil_min
-seuil_max
-intervalle_mesure
-file_size
-alertes_consecutives

Rapport généré:
 Le rapport contient :
 -min / max / moyenne
 -combien d’alertes il y a eu
 -combien de temps l’état d’alerte a duré
 -statistiques à partir de l’arbre

But du projet:
Le but était de créer une petite application IoT en C qui utilise :
-des structures (file, pile, liste chainée, arbre)
-un fichier de configuration
-un système de journal
-un rapport automatique
