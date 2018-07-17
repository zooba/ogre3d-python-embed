/*
 -----------------------------------------------------------------------------
 This source file is part of OGRE
 (Object-oriented Graphics Rendering Engine)
 For the latest info, see http://www.ogre3d.org/
 
 Copyright (c) 2000-2014 Torus Knot Software Ltd
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 -----------------------------------------------------------------------------
 */

#include "pch.h"

namespace py = pybind11;
using namespace py::literals;

using namespace Ogre;
using namespace OgreBites;

#include "ogre_module.h"

class _OgreSampleClassExport Sample_Character : public SdkSample
{
public:

    Sample_Character()
    {
        mInfo["Title"] = "Python Character";
        mInfo["Description"] = "A demo showing 3rd-person character controlled by Python.";
        mInfo["Thumbnail"] = "thumb_char.png";
        mInfo["Category"] = "Animation";
        mInfo["Help"] = "Use the WASD keys to move Sinbad, and the space bar to jump. "
            "Use mouse to look around and mouse wheel to zoom. Press Q to take out or put back "
            "Sinbad's swords. With the swords equipped, you can left click to slice vertically or "
            "right click to slice horizontally. When the swords are not equipped, press E to "
            "start/stop a silly dance routine.";

        mChara = nullptr;
        mLight = nullptr;
    }

    bool frameRenderingQueued(const FrameEvent& evt)
    {
        try {
            if (mAIModule) {
                mAIModule.attr("on_frame").call(mChara, mContextDict, evt.timeSinceLastFrame);
            }
        }
        catch (const std::runtime_error &re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error initializing sample", MB_OK);
            throw re;
        }

        // let character update animations and camera
        mChara->addTime(evt.timeSinceLastFrame);
        return SdkSample::frameRenderingQueued(evt);
    }

    bool keyPressed(const KeyboardEvent& evt)
    {
        if (evt.keysym.sym == SDLK_F5) {
            try {
                mAIModule.reload();
            }
            catch (const std::runtime_error &re) {
                OutputDebugStringA(re.what());
                ::MessageBoxA(NULL, re.what(), "Error initializing sample", MB_OK);
            }
        }

        // relay input events to character controller
        if (!mTrayMgr->isDialogVisible()) mChara->injectKeyDown(evt);
        return SdkSample::keyPressed(evt);
    }

    bool keyReleased(const KeyboardEvent& evt)
    {
        // relay input events to character controller
        if (!mTrayMgr->isDialogVisible()) mChara->injectKeyUp(evt);
        return SdkSample::keyReleased(evt);
    }

    bool mouseMoved(const MouseMotionEvent& evt)
    {
        // Relay input events to character controller.
        if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);
        return SdkSample::mouseMoved(evt);
    }

    virtual bool mouseWheelRolled(const MouseWheelEvent& evt) {
        // Relay input events to character controller.
        if (!mTrayMgr->isDialogVisible()) mChara->injectMouseWheel(evt);
        return SdkSample::mouseWheelRolled(evt);
    }

    bool mousePressed(const MouseButtonEvent& evt)
    {
        // Relay input events to character controller.
        if (!mTrayMgr->isDialogVisible()) mChara->injectMouseDown(evt);
        return SdkSample::mousePressed(evt);
    }

protected:

    void setupContent()
    {
        try {
            auto scene = py::module::import("scene");

            scene.attr("setup_scene").call(mViewport, mSceneMgr);
        } catch (const std::runtime_error &re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error initializing sample", MB_OK);
            throw re;
        }

        // position the sun above the scene
        auto light = mSceneMgr->getLight("sun");
        mLight = mSceneMgr->getRootSceneNode()
            ->createChildSceneNode(Vector3(-10, 20, 20));
        mLight->attachObject(light);

        // disable default camera control so the character can do its own
        mCameraMan->setStyle(CS_MANUAL);

        // create a floor mesh resource
        MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
            Plane(Vector3::UNIT_Y, 0), 1000, 1000, 100, 100, true, 1, 10, 10, Vector3::UNIT_Z);

        // create a floor entity, give it a material, and place it at the origin
        Entity* floor = mSceneMgr->createEntity("Floor", "floor");
        floor->setMaterialName("Examples/Rockwall");
        //floor->setCastShadows(false);
        mSceneMgr->getRootSceneNode()->attachObject(floor);

        // create our character controller
        mChara = new SinbadCharacterController(mCamera);

        try {
            mContextDict = py::dict();
            mAIModule = py::module::import("ai");
            mAIModule.attr("on_initialize").call(mChara, mContextDict);
        }
        catch (const std::runtime_error &re) {
            OutputDebugStringA(re.what());
            ::MessageBoxA(NULL, re.what(), "Error initializing sample", MB_OK);
            throw re;
        }

        mTrayMgr->toggleAdvancedFrameStats();

        StringVector items{ "Help", "Reload" };
        ParamsPanel* help = mTrayMgr->createParamsPanel(TL_TOPLEFT, "HelpMessage", 100, items);
        help->setParamValue("Help", "H / F1");
        help->setParamValue("Reload", "F5");
    }

    void cleanupContent()
    {
        // clean up character controller and the floor mesh
        if (mChara)
        {
            delete mChara;
            mChara = 0;
        }
        mContextDict.clear();
        MeshManager::getSingleton().remove("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    }

    SinbadCharacterController* mChara;

    Ogre::SceneNode* mLight;
    py::dict mContextDict;
    py::module mAIModule;
};


#pragma region Install sample in browser

DefaultSamplesPlugin::DefaultSamplesPlugin() : SamplePlugin("Sample Python Character")
{
    addSample(new Sample_Character);
}

DefaultSamplesPlugin::~DefaultSamplesPlugin()
{
    for (SampleSet::iterator i = mSamples.begin(); i != mSamples.end(); ++i)
    {
        delete *i;
    }
}

#ifndef OGRE_STATIC_LIB
static SamplePlugin* sp = nullptr;

extern "C" void _OgreSampleExport dllStartPlugin(void);
extern "C" void _OgreSampleExport dllStopPlugin(void);

extern "C" _OgreSampleExport void dllStartPlugin()
{
    py::initialize_interpreter();
    try {
        py::module::import("ogre");
        py::module::import("scene");
    } catch (const std::runtime_error &re) {
        OutputDebugStringA(re.what());
        ::MessageBoxA(NULL, re.what(), "Error initializing sample", MB_OK);
        py::finalize_interpreter();
        return;
    }

    sp = new DefaultSamplesPlugin();
    Root::getSingleton().installPlugin(sp);
}

extern "C" _OgreSampleExport void dllStopPlugin()
{
    if (sp) {
        Root::getSingleton().uninstallPlugin(sp);
        delete sp;
        py::finalize_interpreter();
    }
}
#endif

#pragma endregion
