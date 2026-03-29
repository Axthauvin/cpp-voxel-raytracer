from perlin import PerlinNoise

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

import os
os.environ["PANDA3D_GL_VERSION"] = "2 1"

from ursina import *
from ursina.prefabs.first_person_controller import FirstPersonController


app = Ursina()

# Define a Voxel class.
# By setting the parent to scene and the model to 'cube' it becomes a 3d button.


class Voxel(Button):
    def __init__(self, position=(0, 0, 0)):
        super().__init__(
            parent=scene,
            position=position,
            model="cube",
            origin_y=0.5,
            texture="white_cube",
            color=color.hsv(0, 0, random.uniform(0.9, 1.0)),
            highlight_color=color.lime,
        )

h = 20
w = 20

echelle = 0.03 

generateur = PerlinNoise()
map_hauteurs = []

for z in range(h):
    for x in range(w):
        # On multiplie par l'échelle pour avancer par petits pas dans le bruit
        value = generateur.noise(x * echelle, z * echelle) - 1

        # On convertit cette valeur (0 à 1) en hauteur de blocs (ex: max 20 blocs)
        # voxel = Voxel(position=(x, value * 255, z))
        voxel = Voxel(position=(x, value * 40, z))

def input(key):
    if key == "left mouse down":
        hit_info = raycast(camera.world_position, camera.forward, distance=5)
        if hit_info.hit:
            Voxel(position=hit_info.entity.position + hit_info.normal)
    if key == "right mouse down" and mouse.hovered_entity:
        destroy(mouse.hovered_entity)


player = FirstPersonController()
app.run()
