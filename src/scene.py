# coding: utf-8

from ogre import (ColourValue, FogMode, LightTypes, ShadowTechnique,
                  Viewport, SceneManager)

def setup_shadows(scene):
    scene.set_shadow_technique(ShadowTechnique.TEXTURE_MODULATIVE)
    scene.set_shadow_colour(ColourValue(0.5, 0.5, 0.5, 1))
    scene.set_shadow_texture_size(2048)
    scene.set_shadow_texture_count(1)

def setup_lights(scene):
    # Use a small amount of ambient lighting
    scene.set_ambient_light(ColourValue(1.0, 0.9, 0.9, 1.0));

    # Create a bright light above the scene
    light = scene.create_light("sun")
    light.light_type = LightTypes.POINT
    light.diffuse_colour = ColourValue(1, 1, 1, 1)
    light.specular_colour = ColourValue(1, 1, 1, 1)
    light.set_attenuation(325, 0, 0.014, 0.0007)

def setup_scene(viewport : Viewport, scene : SceneManager):
    # Set background and some fog
    viewport.set_background_colour(ColourValue(1, 1, 0.8, 1))
    scene.set_fog(FogMode.LINEAR, ColourValue(1, 0, 0, 1), 0, 10, 20)

    # Set shadow properties
    setup_shadows(scene)

    # Set lights
    setup_lights(scene)


