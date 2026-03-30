from perlin import PerlinNoise
import os
from ursina import (
    Button,
    color,
    scene,
    Ursina,
    Vec3,
)
from ursina.prefabs.first_person_controller import FirstPersonController


"""
Disclaimer: This solution is not scalable for creating a big world.
Creating a game like Minecraft requires specialized knowledge and is not as easy
to make as it looks.

You'll have to do some sort of chunking of the world and generate a combined mesh
instead of separate blocks if you want it to run fast. You can use the Mesh class for this.

You can then use blocks with colliders like in this example in a small area
around the player so you can interact with the world.
"""

# https://www.ursinaengine.org/minecraft_clone.html

os.environ["PANDA3D_GL_VERSION"] = "2 1"


app = Ursina()

# Define a Voxel class.
# By setting the parent to scene and the model to 'cube' it becomes a 3d button.


class my_color:
    def __init__(self, r, g, b):
        # ursina sucks, it wants colors to be between 0 and 1 instead of 0 and 255, so we have to divide by 255
        # made a constructor to make it easier to create colors with 0-255 values, but you can still use color.rgb if you want
        self.r = r
        self.g = g
        self.b = b

    def to_ursina(self):
        return color.rgb(self.r / 255, self.g / 255, self.b / 255)  # type: ignore


class Voxel(Button):
    def __init__(self, position=(0, 0, 0), block_type="ground"):
        # colors for block types
        # used google palette to pick colors so sorry if its not very minecrafty ahah
        block_colors = {
            # "water": color.rgb(50, 110, 230),
            # "sand": color.rgb(218, 200, 120),
            # "ground": color.rgb(70, 155, 70),
            "water": my_color(50, 110, 230).to_ursina(),
            "sand": my_color(218, 200, 120).to_ursina(),
            "ground": my_color(70, 155, 70).to_ursina(),
        }

        voxel_color = block_colors.get(block_type, block_colors["ground"])

        super().__init__(
            parent=scene,
            position=position,
            model="cube",  # type: ignore
            origin_y=0.5,
            texture="white_cube",  # type: ignore
            color=voxel_color,
            highlight_color=color.lime,
        )


h = 140
w = 140

echelle = 0.03
hauteur_max = 12

generateur = PerlinNoise()


# found on https://thebookofshaders.com/13/
# the idea is to combine multiple octaves of Perlin noise to create a more natural and less harsh terrain. (your version had VERY big mountains and no flat areas, this should fix that)
def fbm_noise(x, z, octaves=4, persistence=0.5, lacunarity=2.0):
    """Bruit de Perlin fractal pour un relief plus naturel et moins brutal."""
    total = 0.0
    amplitude = 1.0
    frequence = 1.0
    normalisation = 0.0

    for _ in range(octaves):
        total += generateur.noise(x * frequence, z * frequence) * amplitude
        normalisation += amplitude
        amplitude *= persistence
        frequence *= lacunarity

    return total / normalisation


plus_haute_colonne = float("-inf")

for z in range(h):
    for x in range(w):
        # Relief centré autour de 0 et borné en hauteur pour éviter les grosses montagnes.
        value = fbm_noise(x * echelle, z * echelle)
        y = int(round(((value - 0.5) * 2) * hauteur_max))
        plus_haute_colonne = max(plus_haute_colonne, y)

        # A voir comment adapter les couleurs
        # de ce que j'ai vu, les plages de valeurs pour les différents types de terrain sont généralement déterminées en fonction du résultat visuel souhaité.
        # A voir de comment on peut faire ça de manière plus automatique, mais pour l'instant j'ai juste choisi des valeurs qui me semblaient donner un résultat correct.
        if value < 0.40:
            block_type = "water"
        elif value < 0.52:
            block_type = "sand"
        else:
            block_type = "ground"

        Voxel(position=(x, y, z), block_type=block_type)


player = FirstPersonController(position=Vec3(w / 2, plus_haute_colonne + 4, h / 2))
app.run()
