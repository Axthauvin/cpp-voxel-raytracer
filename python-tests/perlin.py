import random
import numpy as np
import math
import colorsys
import matplotlib.pyplot as plt
from PIL import Image

def cart_distance(x1, y1, x2, y2):
    return math.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)

class BiomeType:
    def __init__(self, s, hue):
        self.s = s
        self.hue = hue

class Seed:
    types = [
        BiomeType("SEA", 237),
        BiomeType("DESERT", 52),
        BiomeType("PLAINE", 130),
        BiomeType("MONTAGNE", 207),
    ]

    def __init__(self, random_seed=None, biome_type=None):
        if random_seed is not None:
            random.seed(random_seed)
        
        self.x = random.random()
        self.y = random.random()

        if biome_type is not None:
            self.biome_type = biome_type
        else:
            self.biome_type = self.types[random.randint(1, len(self.types) - 1)]
        
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

# Octaves

nb_octaves = 3
persistance = 2
lacunarity = 0.5

# Exemple d'utilisation

largeur_map = 500
longueur_map = 500
echelle = 0.01 # Plus l'échelle est petite, plus le terrain est "zoomé" et doux
seeds = []

for i in range(6):
    seeds.append(Seed(i))

generateur = PerlinNoise()
map_hauteurs = []

sea = Seed.types[0]

for y in range(longueur_map):
    ligne = []
    for x in range(largeur_map):
        min_d = float("inf")
        biome_type = None
        valeur_bruit = 0

        frequence = echelle
        ampl = 1
        max_possible = 0
        
        for i in range(nb_octaves):
            valeur_bruit += generateur.noise(x * frequence, y * frequence) * ampl
            max_possible += ampl
            
            ampl *= persistance
            frequence *= lacunarity

        valeur_bruit /= max_possible

        if valeur_bruit < 0.3:
            biome_type = sea
        else:
            for seed in seeds:
                noise_x = generateur.noise(x * 0.02, y * 0.02) * 20
                noise_y = generateur.noise(x * 0.02, y * 0.02) * 20
                d = cart_distance(x + noise_x, y + noise_y, seed.x * longueur_map, seed.y * largeur_map)
                if d < min_d:
                    min_d = d
                    biome_type = seed.biome_type
            
        # On multiplie par l'échelle pour avancer par petits pas dans le bruit

        # On convertit cette valeur (0 à 1) en hauteur de blocs (ex: max 20 blocs)
        rgb_unit = colorsys.hsv_to_rgb(biome_type.hue / 360, 0.8, valeur_bruit)
        rgb = tuple(int(c * 255) for c in rgb_unit)
        ligne.append(rgb)
    map_hauteurs.append(ligne)

# Affichage du résultat dans la console

im_np = np.array(map_hauteurs, dtype=np.uint8)
print(im_np)
im = Image.fromarray(np.array(map_hauteurs, dtype=np.uint8))
im.show()

# for ligne in map_hauteurs:
#     print([f"{h:2d}" for h in ligne])
