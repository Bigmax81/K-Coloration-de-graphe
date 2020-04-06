// K-Coloration d'un graphe avec l'API lp_solve

// Compilation et lancement du programme : g++ 21700336.cpp -llpsolve55 -lcolamd -ldl -o dm && ./dm graph.dimacs

// DM rédigé et complété par Emmanuel Garreau, 21700336, L2 Informatique, Groupe 1B

/* Partie réponse : modélisation mathématique

Rappel sur le problème de k-coloration d'un graphe : on cherche, pour un graphe donné G = (E, A) avec E, l'ensemble des sommets, et A,
l'ensemble des arrêtes, à obtenir le k minimum tel que l'on puisse colorier l'ensemble des sommets du graphe avec k couleurs et que deux
sommets adjacents n'aient pas la même couleur.

On cherche donc à obtenir k tel que 1 <= k <= k_max avec k_max = card(E). En effet, dans le pire des cas, on aura une couleur par sommet.
On note n = Card(E).


a) Il y a deux séries de variables :

      - 1) k_max * n variables : chaque sommet doit avoir k_max variables tel que si S_c = 1, la couleur c est utilisée, sinon S_c = 0, avec
                                 c appartient à [1, k_max] et S appartient à E.

      - 2) k_max variables : on attribue une variable pour chaque couleur telle que si la couleur est utilisée, Couleur_c = 1, sinon
                             Couleur_c = 0 avec C appartient à [1, k_max].
    
    On a donc un total de ( (k_max * n) + k_max ) variables.


b) Il y a trois types de contraintes :

      - 1) Toutes les variables doivent appartenir à l'ensemble {0, 1}, ce sont des variables booléennes :
           pour tout v appartenant à E, pour tout c appartenant à [1, k_max], 0 <= v_c <= 1, avec v_c un Int
           et
           pour tout c appartenant à [1, k_max], 0 <= Couleur_c <= 1, avec Couleur_c un Int.

      - 2) La somme des variables d'un unique sommet doit être égale à 1 (une seule couleur doit être activée pour un sommet) :
           pour tout v appartenant à E, Somme (v_c) = 1, pour c allant de 1 à k_max.

      - 3) Si a, b appartiennent à E tels que {a, b} appartient à A, alors Somme (a_c + b_c - Couleur_c) <= 0, pour c allant de 1 à k_max.


c) La fonction objectif à minimiser est : Somme (Couleur_c), pour c allant de 1 à k_max. On minimise le nombre de couleurs utilisées.

*/

#include <lpsolve/lp_lib.h>
#include<iostream>
#include<fstream>
#include<sstream>
#include<stdio.h>
//#include<math.h>
//#include<stdlib.h>

using namespace std;


// Fonction permettant de mettre toutes les cases du vecteur row à 0.

void vider_row ( REAL row[], const int& taille){
  for(int i = 0 ; i < taille; i++){
    row[i]=0.0;
  }

}

// Fonction principale du programme.
int main (int argc, char* argv[]){
  
  // On vérifie que le nom du fichier a été passé en paramètre.	
  if (argv[1] == nullptr){
	cout << "Erreur. Donnez un fichier en paramètre !" << endl;
  	exit(EXIT_FAILURE);
  }

  ifstream fichier (argv[1]); // ouverture du fichier contenant le graphe.

  // On vérifie que le fichier existe.
  if (!fichier.is_open()){

    cout << "Impossible d'ouvrir le fichier du graphe." << endl;
    exit(EXIT_FAILURE);
  }

  string nbSommets; // Initialisation de la variable qui va contenir le nombre de sommets du graphe.

  string nbArretes; // Initialisation de la variable qui va contenir le nombre d'arrêtes du graphe.

  string ligne; // Initialisation de la variable qui va contenir la ligne lue.

  while (getline (fichier, ligne)) {

    string mot; // Initialisation de la variable qui va contenir momentanément les mots de la ligne.

    istringstream iss (ligne); // Initialisation de l'objet qui va parcourir la ligne.

    iss >> mot; // On obtient le premier mot.

    // Si le premier mot = "p", alors nous sommes sur la ligne décrivant le graphe.
    if(mot == "p"){
      iss >> mot; // le mot edge.
      iss >> mot; // le mot décrivant le nombre de sommets.
      nbSommets = mot; // récupération du nombre de sommets.
      //iss >> mot; // le mot décrivant le nombre d'arrêtes.
      //nbArretes = mot; // récupération du nombre d'arrêtes.
      break; // on quitte le while pour le moment.
    }
  }

  int sommets = stoi(nbSommets); // Cast en Int du nombre de sommets.
  
  int k = sommets; // Initialisation du k_max.
  
  /*
  Il y a plusieurs possibilités pour l'initialisation du k_max. Rappelons que l'on cherche le k tel que 1 <= k <= k_max.
  Or, on sait que sur un graphe, le nombre maximum de couleurs pour colorer tous les sommets sera le nombre de sommets
  lui-même dans le pire des cas. On peut donc choisir k_max = sommets. Le programme tâchera de résoudre le problème de k_max-coloration.
  */

  //int arretes = stoi(nbArretes); // Cast en Int du nombre d'arrêtes.

  lprec* lp; // Pointeur sur notre lp.

  int nbVariables = k*k + k; // Initialisation du nombre de variables.

  int nbConstraints = 0; // Initialisation du nombre de contraintes, on débute à 0. Les contraintes seront ajoutées au fur et à mesure.

  lp = make_lp(nbConstraints, nbVariables); // Initialisation du problème lp avec le nombre de contraintes et le nombre de variables.

  REAL row[nbVariables +1]; // Initialisation du vecteur row qui va contenir les coefficients de nos variables.
  // On ajoute 1 au nombre de variables afin de ne pas prendre en compte la case row[0].

  vider_row(row, nbVariables+1); // A l'initialisation, le vecteur row prend des valeurs aléatoires. On commence donc par le vider.

  // Ajout de la contrainte 1), chaque variable doit être un Int appartenant à {0, 1}.
  for (int i = 1; i < nbVariables+1; i++){

    vider_row(row, nbVariables+1); // On vide le row.
    set_int(lp, i, true); // On informe le lp que chaque variable est un Int.
    row[i] = 1; // La contrainte va porter sur la case row[i].
    add_constraint(lp, row, LE, 1); // row[i] <= 1.
    add_constraint(lp, row, GE, 0); // row[i] >= 0.

  }

  vider_row(row, nbVariables+1); // On vide le row.

  // Ajout de la contrainte 2) : la somme des variables d'un sommet doit être égale à 1 (1 seule couleur activée).
  for (int i = 1; i < k*sommets+1; i++){
      row[i] = 1; // la case row[i] fait partie de la contrainte.
      if (i%k==0){ // On active des cases jusqu'à tomber sur un numéro multiple de k, on sera arrivée à la fin des variables pour le
        // sommet en question.
        add_constraint(lp, row, EQ, 1); // La somme doit être égale à 1.
        vider_row(row, nbVariables+1); // On vide le row.
      }
  }

  // On continue le parcours de notre fichier pour vérifier les arrêtes.
  while (getline (fichier, ligne)) {

      string mot; // Initialisation de la variable qui va contenir momentanément les mots de la ligne.

      istringstream iss (ligne); // Initialisation de l'objet qui va parcourir la ligne.

      iss >> mot ; // On obtient le premier mot.

      // Si le premier mot est un e, on va obtenir les composants d'une arrête.
      if (mot == "e"){
        iss >> mot; // Obtention du mot suivant.
        int s1 = stoi(mot); // Obtention du premier sommet formant l'arrête.
        iss >> mot ; // Obtention du mot suivant.
        int s2 = stoi(mot); // Obtention du deuxième sommet formant l'arrête.
        vider_row(row,nbVariables+1); // On vide le row.

        // Ajout de la contrainte 3)
        // On parcourt les couleurs.
        for (int j = 1; j<=k; j++){
          vider_row(row, nbVariables+1); // On vide le row.
          row[s1*k-k+j] = 1; // On active la case du premier sommet qui correspond à la couleur j.
          row[s2*k-k+j] = 1; // On active la case du deuxième sommet qui correspond à la couleur j.
          row[sommets*k+j] = -1; // On active la case de la couleur j.
          add_constraint(lp, row, LE, 0); // On ajoute la contrainte.
        }
      }
  }

  // On active toutes les cases qui correspondent aux couleurs.
  for (int i = 1; i <=k; i++){
    row[sommets*k + i] = 1; // Activation de la case.
  }

  // Application de la fonction objectif : la somme des couleurs.
  set_obj_fn(lp,row);


  //write_lp(lp, "temp.lp"); // Commande permettant d'obtenir un fichier afin de vérifier l'affectation des variables et des contraintes.

  int ret = solve(lp); // Calcul de la solution.

  // Optimisation
  if (ret == 0){
    ret = solve(lp);
    if (ret == OPTIMAL){
      ret = 0;
    }
    else{
      ret = k;
    }
  }

  // Affichage de la solution et des couleurs attribuées aux sommets.
  if (ret == 0){
    get_variables(lp, row);
    int cpt = 0;
    for (int i = 1; i<= sommets; i++){
      for (int j = 1; j<=k; j++){
        if(row[cpt] == 1){
          printf("sommets %d = couleur %d\n", i,j);
        }
        cpt = cpt + 1;
      }
    }
    printf("Nombre de couleurs minimales : %f\n", get_objective(lp));
  }

}
