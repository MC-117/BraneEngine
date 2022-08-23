# BraneEngine
>by Luo Yizhou

## *This version is out of maintenance. New upadate is in BraneEngineDx repo.*
https://github.com/MC-117/BraneEngineDx

## Introduction
This is a tiny 3D game engine as my university graduation project.

This engine is based on OpenGL graphic library and Eigen math graphic library. **There only have x64 version**

## How to use
Set the ***/include***, ***/lib (have lib of debug and release version in x64/debug and x64/release folder. Make sure configure respectively)*** folder path in the VS project configuration window. And add all the libs in the /lib folder to the linker input configuration window.

After you bulid your own project, make sure you copy all the ***.dll*** files in ***/bin/x64*** folder and ***/Engine*** folder to the same folder as exe file.

In you own .cpp file include Engine.h at first. Then implement the two function.

    void SetupWorld(); // set the default window size or backgroud color
    void InitialWorld(); // initialize the scene (Config.ini set guiOnly = false)
    void Tool(); // initialize tool ui view (Config.ini set guiOnly = true)

A global variable ***world*** will construct at first for you to setup the camera, input and etc. You can also add some constructed ***Object*** to the ***world***. For example:

    Mesh& mesh = *new Mesh();
    Material& mat = world.defaultMaterial.instantiate(true);
    Actor& actor = *new Actor(mesh, mat, "TestActor");
    actor.setPosition(0, 0, 10);
    world += actor;

***Notice: All the Object must use new operator to construct at Function InitialWorld***

You can design your own game/graghic object by inheriting class ***Object***, ***Transform***, ***Actor***.

Every class inherited ***Object*** have 4 import virtual methods.

    virtual void begin(); // called at the first frame after add to world
    virtual void tick(float deltaTime); // called every frame
    // called every frame after finishing all the tick method
    virtual void afterTick();
    virtual void end(); // called when it is destroying
