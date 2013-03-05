#!/bin/bash

for i in {1..40}; do

echo "
---------- MENU ----------

1 - Voir comment commander le programme (clavier, souris)
2 - Afficher l'ensemble de Mandelbrot et des ensembles de Julia
3 - Afficher des fractales de type IFS, L-systèmes
4 - Afficher les mêmes fractales en utilisant l'algorithme de flammes
5 - Afficher des fractales de type flamme (48 variations)
6 - Afficher des fractales de type flamme complexes (1 ou plusieurs algorithmes choisis aléatoirement)
7 - Créer un fichier de configuration de fractale(s)
8 - Afficher la documentation technique

Votre choix (ou 0 pour quitter) :"
read choice

if [ $choice -eq 0 ]; then break; fi
if [ $choice -eq 1 ]; then
	echo "
-------- COMMANDE DU PROGRAMME -----------

FLÈCHE GAUCHE/DROITE : fractale précédente/suivante
ESPACE, ENTRÉE, F5 : génère de nouvelles valeurs pour les fractales aléatoires (flammes)
ECHAP : quitter

    Pour les ensembles de Mandelbrot et de Julia, les cliques gauche et droit permettent de passer de l'ensemble de Mandelbrot à l'ensemble de Julia et inversement.

CLIQUE DROIT/GAUCHE : fractale précédente/suivante (sauf Mandelbrot/Julia)
CLIQUE DU MILIEU : génère de nouvelles valeurs pour les fractales aléatoires (flammes)
ROULETTE : zoom avant/arrière

Appuyer sur une touche pour continue...
"
	read next
fi
if [ $choice -eq 2 ]; then
	../bin/fractal ../conf/series.escape-time.fractal
fi
if [ $choice -eq 3 ]; then
	../bin/fractal ../conf/series.IFS.fractal
fi
if [ $choice -eq 4 ]; then
	../bin/fractal ../conf/series.IFS.flame.fractal
fi
if [ $choice -eq 5 ]; then
	../bin/fractal ../conf/series.flame.single.fractal
fi
if [ $choice -eq 6 ]; then
	../bin/fractal ../conf/series.flame.complex.fractal
fi
if [ $choice -eq 7 ]; then
	echo "
------ CONFIGURATION --------

Charger un fichier de configuration : ./fractal fichier_de_conf
Vous pouvez créer ce fichier avec un éditeur de texte, avec l'aide de la page Configuration.html du répertoire bin.
Cette page va s'ouvrir si possible dans une nouvelle fenêtre du navigateur Mozilla Firefox.
Appuyer sur entrée pour continuer...
"
	read next
	firefox --new-window "file://`pwd`/../bin/Configuration.html"&
fi
if [ $choice -eq 8 ]; then
	echo "
------ DOCUMENTATION TECHNIQUE --------

La documentation technique se trouve dans le dossier doc du projet.
La page d'accueil de la version HTML de la documentation va s'ouvrir si possible dans une nouvelle fenêtre du navigateur Mozilla Firefox.
Appuyer sur entrée pour continuer...
"
	read next
	firefox --new-window "file://`pwd`/../doc/html/index.html"&
fi

done



















