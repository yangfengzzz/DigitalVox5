# DigitalVox5 - Vulkan Cross Platform Engine and Editor

DigitalVox is the name of a series. Although each version is based on a component system, the specific implementation is
very different. Therefore, different code repositories are used for management. Please obtain the code you need on
demand:

1. DigitalVox2: Engine written in Swift, 3rd party library bridged by Objective-C++.
2. DigitalVox3: Engine developed in C++, based on the math library provided by Ozz.
3. DigitalVox4 : This engine is based on C++, and the underlying mathematical library is templated to support the
   precision requirements of rendering and physical simulation at the same time. The rendering layer abstraction for
   Metal makes it possible to use the C++ compiler instead of Objective-C++ globally, and encapsulates GLFW events to
   achieve cross-platform. Further, clean up the component system and eliminate the global dependency. And make the
   rendering pipeline independent of the camera, so it is easier to customize the rendering pipeline.
4. Arche-cpp: A cross-platform engine based on WebGPu, as well as Arche.js, a web version forked with TypeScript. Please
   refer to [ArcheGraphics](https://github.com/ArcheGraphics/) and [Document](https://arche.graphics).
5. **DigitalVox5** : Since the WGSL shading language cannot meet the needs of advanced features, a cross-platform engine
   is re-implemented based on Vulkan. The engine is based on the capabilities provided by SPIRV as a shader for
   cross-platform compilation and reflection, and is built with Cmake, which truly enables cross-platform compilation
   and execution.

## Motivation

This project is inspired by [Oasis](https://github.com/oasis-engine) which is an ECS-liked based engine(not very strict)
. Based on entity and component, it is easy to combine other open-source ability:

1. [ImGui](https://github.com/ocornut/imgui): GUI system
2. [OZZ-Animation](https://github.com/guillaumeblanc/ozz-animation): CPU Animation System
3. [PhysX](https://github.com/NVIDIAGameWorks/PhysX): Physical System

## Cloning && Install

This repository contains submodules for external dependencies, so when doing a fresh clone you need to clone
recursively:

```
git clone --recursive https://github.com/yangfengzzz/DigitalVox5.git
```

Existing repositories can be updated manually:

```
git submodule init
git submodule update
```

After clone all third-party dependencies, use the script in third_party folder to build them all:

```
./build.sh
```

## Feature

You can read code in [apps](https://github.com/yangfengzzz/DigitalVox5/tree/main/apps) to find the feature in this code,
like physx and shadow:
![PhysX](https://github.com/yangfengzzz/DigitalVox3/raw/master/doc/img/physx.gif "PhysX")
You can also redirect to [effect](https://github.com/yangfengzzz/DigitalVoxEffect) to see more effects.