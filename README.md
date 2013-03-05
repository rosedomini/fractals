Copyright 2011 Dominic Rose, Minh Huy Nguyen
Projet différencié 2011-2012 - Fractales | Ecole Internationale des Sciences du Traitement de l'Information

___________________________________________________________
Comment compiler :

Il faut la librairie SDL, après il suffit de se placé dans 
le répertoire 'src' et utiliser la commande `make all`.

`make all` permet de compiler le programme et générer la 
documentation technique, avant de proposer un test du programme.

`make very-clean` permet de nettoyer les fichiers et 
dossiers créés par make.

___________________________________________________________
Tester le programme :

`make test` (aussi effectué par `make all`) permet entre 
autre de lancer le programme avec les fichiers du dossier 
de configuration conf déjà existants. Ceci exécute un 
script bash qui ouvre le menu suivant :

1 - Voir comment commander le programme (clavier, souris)
2 - Afficher l'ensemble de Mandelbrot et des ensembles de
 Julia
3 - Afficher des fractales de type IFS, L-systèmes
4 - Afficher les mêmes fractales en utilisant l'algorithme
 de flammes
5 - Afficher des fractales de type flamme (48 variations)
6 - Afficher des fractales de type flamme complexes (1 ou 
plusieurs algorithmes choisis aléatoirement)
7 - Créer un fichier de configuration de fractale(s)
8 - Afficher la documentation technique

----------- COMMANDE DU PROGRAMME -------------------------

FLÈCHE GAUCHE/DROITE : fractale précédente/suivante
ESPACE, ENTRÉE, F5 : génère de nouvelles valeurs pour les
 fractales aléatoires (flammes)
ECHAP : quitter

    Pour les ensembles de Mandelbrot et de Julia, les 
    cliques gauche et droit permettent de passer de 
    l'ensemble de Mandelbrot à l'ensemble de Julia et inversement.

CLIQUE DROIT/GAUCHE : fractale précédente/suivante (sauf 
Mandelbrot/Julia)
CLIQUE DU MILIEU : génère de nouvelles valeurs pour les 
fractales aléatoires (flammes)
ROULETTE : zoom avant/arrière

----------- CONFIGURATION ----------------------------------

Charger un fichier de configuration :
./fractal fichier_de_conf

Vous pouvez créer ce fichier avec un éditeur de texte, 
avec l'aide de la page Configuration.html du répertoire bin.
Ce script tentera d'ouvrir cette page avec Firefox (dans 
un nouvel onglet).

____________________________________________________________
Exécuter le programme :

Le programme nécessite d'être exécuté avec comme paramètre 
l'emplacement d'un fichier de configuration compatible.

$ ./fractal
Syntaxe : ./fractal fichier_de_config
$ ./fractal ../conf/nom_fichier


== Copying

This project is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This project is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Foobar.  If not, see <http://www.gnu.org/licenses/>.