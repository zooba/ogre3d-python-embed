#pragma once

PYBIND11_EMBEDDED_MODULE(ogre, m) {
    py::class_<ColourValue>(m, "ColourValue")
        .def(py::init<float, float, float, float>());

    // Use nodelete since lights are owned by the Scene
    py::class_<Light, std::unique_ptr<Light, py::nodelete>>(m, "Light")
        .def_property("light_type", &Light::getType, &Light::setType)
        .def_property("diffuse_colour", &Light::getDiffuseColour, (void (Light::*)(const ColourValue&))&Light::setDiffuseColour)
        .def_property("specular_colour", &Light::getSpecularColour, (void (Light::*)(const ColourValue&))&Light::setSpecularColour)
        .def("set_attenuation", &Light::setAttenuation);

    py::class_<Viewport>(m, "Viewport")
        .def("set_background_colour", &Viewport::setBackgroundColour);

    py::class_<SceneManager>(m, "SceneManager")
        .def("create_light", (Light* (SceneManager::*)(const String&))&SceneManager::createLight)
        .def("get_light", &SceneManager::getLight)
        .def("set_ambient_light", &SceneManager::setAmbientLight)
        .def("set_fog", &SceneManager::setFog)
        .def("set_shadow_technique", &SceneManager::setShadowTechnique)
        .def("set_shadow_colour", &SceneManager::setShadowColour)
        .def("set_shadow_texture_count", &SceneManager::setShadowTextureCount)
        .def("set_shadow_texture_size", &SceneManager::setShadowTextureSize);

    py::enum_<FogMode>(m, "FogMode")
        .value("EXP", FogMode::FOG_EXP)
        .value("EXP2", FogMode::FOG_EXP2)
        .value("LINEAR", FogMode::FOG_LINEAR)
        .value("NONE", FogMode::FOG_NONE);

    py::enum_<Light::LightTypes>(m, "LightTypes")
        .value("DIRECTIONAL", Light::LT_DIRECTIONAL)
        .value("POINT", Light::LT_POINT)
        .value("SPOTLIGHT", Light::LT_SPOTLIGHT);

    py::enum_<ShadowTechnique>(m, "ShadowTechnique")
        .value("NONE", ShadowTechnique::SHADOWTYPE_NONE)
        .value("TEXTURE_ADDITIVE", ShadowTechnique::SHADOWTYPE_TEXTURE_ADDITIVE)
        .value("TEXTURE_MODULATIVE", ShadowTechnique::SHADOWTYPE_TEXTURE_MODULATIVE);

    py::class_<SinbadCharacterController, std::unique_ptr<SinbadCharacterController, py::nodelete>>(m, "CharacterController")
        .def("run", &SinbadCharacterController::run)
        .def("stand", &SinbadCharacterController::stand)
        .def("jump", &SinbadCharacterController::jump)
        .def("dance", &SinbadCharacterController::dance)
        .def("draw_swords", &SinbadCharacterController::drawSwords)
        .def("sheath_swords", &SinbadCharacterController::sheathSwords)
        .def("slice", &SinbadCharacterController::slice);
}

