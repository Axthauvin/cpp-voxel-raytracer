import random
import numpy as np
from PIL import Image

class PerlinNoise:
    def __init__(self, seed=None):
        if seed is not None:
            random.seed(seed)
        
        # 1. Création de la table de permutation (pour le côté pseudo-aléatoire)
        # On crée une liste de 0 à 255 qu'on mélange, puis on la double pour éviter les débordements
        self.p = list(range(256))
        random.shuffle(self.p)
        self.p += self.p

    def _fade(self, t):
        # Fonction de lissage (smoothstep) de Ken Perlin : 6t^5 - 15t^4 + 10t^3
        return t * t * t * (t * (t * 6 - 15) + 10)

    def _lerp(self, t, a, b):
        # Interpolation linéaire simple
        return a + t * (b - a)

    def _grad(self, hash_val, x, y):
        # Sélectionne un vecteur de gradient basé sur le hash
        # Les 4 vecteurs gradient possible sont (1, 1), (1, -1), (-1, 1), (-1, -1)

        h = hash_val & 3  # On prend les 2 derniers bits (4 directions possibles)
        u = x if h < 2 else y
        v = y if h < 2 else x

        # (+/-)x + (+/-)y

        return (u if (h & 1) == 0 else -u) + (v if (h & 2) == 0 else -v)

    def noise(self, x, y):
        # Trouver les coordonnées de la case de la grille contenant (x, y)
        X = int(x) & 255
        Y = int(y) & 255

        # Trouver les coordonnées relatives de (x, y) DANS la case (entre 0.0 et 1.0)
        x -= int(x)
        y -= int(y)

        # Calculer les courbes de lissage pour x et y
        u = self._fade(x)
        v = self._fade(y)

        # Calculer et interpoler les résultats des 4 coins
        res = self._lerp(v,
                         self._lerp(u,
                                    self._grad(self.p[self.p[X] + Y], x, y),
                                    self._grad(self.p[self.p[X + 1] + Y], x - 1, y)
                                ),
                         self._lerp(u,
                                    self._grad(self.p[self.p[X] + Y + 1], x, y - 1),
                                    self._grad(self.p[self.p[X + 1] + Y + 1], x - 1, y - 1))
                     )
        
        # Le résultat brut est entre -1 et 1. On le ramène entre 0 et 1 pour plus de simplicité.
        return (res + 1) / 2

if __name__ == "__main__":

    # Exemple d'utilisation
    largeur_map = 200
    longueur_map = 100
    echelle = 0.1 # Plus l'échelle est petite, plus le terrain est "zoomé" et doux

    generateur = PerlinNoise()
    map_hauteurs = []

    for y in range(longueur_map):
        ligne = []
        for x in range(largeur_map):
            # On multiplie par l'échelle pour avancer par petits pas dans le bruit
            valeur_bruit = generateur.noise(x * echelle, y * echelle)

            # On convertit cette valeur (0 à 1) en hauteur de blocs (ex: max 20 blocs)
            hauteur_blocs = int(valeur_bruit * 255)
            ligne.append(hauteur_blocs)
        map_hauteurs.append(ligne)

    # Affichage du résultat dans la console

    im = Image.fromarray(np.array(map_hauteurs, dtype=np.uint8))
    im.show()

    # for ligne in map_hauteurs:
    #     print([f"{h:2d}" for h in ligne])
