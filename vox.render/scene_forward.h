//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef DIGITALVOX_VOX_RENDER_SCENE_FORWARD_H_
#define DIGITALVOX_VOX_RENDER_SCENE_FORWARD_H_

#include <memory>

namespace vox {
class Scene;

using ScenePtr = std::shared_ptr<Scene>;

class Entity;

class Component;

class ComponentsManager;

class Transform;

class SubMesh;

class Mesh;

using MeshPtr = std::shared_ptr<Mesh>;

class ModelMesh;

using ModelMeshPtr = std::shared_ptr<ModelMesh>;

class BufferMesh;

using BufferMeshPtr = std::shared_ptr<BufferMesh>;

class Material;

using MaterialPtr = std::shared_ptr<Material>;

class UnlitMaterial;

using UnlitMaterialPtr = std::shared_ptr<UnlitMaterial>;

class BlinnPhongMaterial;

using BlinnPhongMaterialPtr = std::shared_ptr<BlinnPhongMaterial>;

class PbrMaterial;

using PBRMaterialPtr = std::shared_ptr<PbrMaterial>;

class Camera;

class Renderer;

class MeshRenderer;

class GpuSkinnedMeshRenderer;

class Script;

class Behaviour;

class Animator;

class SceneAnimator;

class Light;

class PointLight;

class SpotLight;

class DirectLight;

}  // namespace vox

#endif /* DIGITALVOX_VOX_RENDER_SCENE_FORWARD_H_ */
