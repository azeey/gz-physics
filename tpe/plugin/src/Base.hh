/*
 * Copyright (C) 2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef IGNITION_PHYSICS_TPE_PLUGIN_SRC_BASE_HH_
#define IGNITION_PHYSICS_TPE_PLUGIN_SRC_BASE_HH_

#include <ignition/physics/Implements.hh>

#include <map>
#include <memory>
#include <string>

#include "../../lib/src/World.hh"
#include "../../lib/src/Engine.hh"
#include "../../lib/src/Model.hh"
#include "../../lib/src/Link.hh"
#include "../../lib/src/Collision.hh"
#include "../../lib/src/Shape.hh"

namespace ignition {
namespace physics {
namespace tpeplugin {

/// \brief The structs tpelib::ModelInfo,
/// tpelib::LinkInfo, JointInfo, and ShapeInfo are used
/// for two reasons:
/// 1) Holding extra information such as the name or offset
///    that will be different from the underlying engine
/// 2) Wrap shared pointers to DART entities. Since these shared pointers (eg.
///    dart::dynamics::BodyNodePtr) are different from std::shared_ptr, we
///    cannot use them directly as parameters to GenerateIdentity. Instead we
///    create a std::shared_ptr of the struct that wraps the corresponding DART
///    shared pointer.

struct WorldInfo
{
  std::shared_ptr<tpelib::World> world;
};

struct ModelInfo
{
  tpelib::Model *model;
};

struct LinkInfo
{
  tpelib::Link *link;
};

struct CollisionInfo
{
  tpelib::Collision *collision;
};

class Base : public Implements3d<FeatureList<Feature>>
{
  public: inline Identity InitiateEngine(std::size_t /*_engineID*/) override
  {
    // tpesim does not have multiple "engines"
    return this->GenerateIdentity(0);
  }

  public: inline Identity AddWorld(std::shared_ptr<tpelib::World> _world)
  {
    auto worldPtr = std::make_shared<WorldInfo>();
    worldPtr->world = _world;
    this->worlds.insert({_world->GetId(), worldPtr});

    return this->GenerateIdentity(_world->GetId(), worldPtr);
  }

  public: inline Identity AddModel(std::size_t _worldId, tpelib::Model &_model)
  {
    auto modelPtr = std::make_shared<ModelInfo>();
    modelPtr->model = &_model;
    this->models.insert({_model.GetId(), modelPtr});
    this->childIdToParentId.insert({_model.GetId(), _worldId});

    return this->GenerateIdentity(_model.GetId(), modelPtr);
  }

  public: inline Identity AddLink(std::size_t _modelId, tpelib::Link &_link)
  {
    auto linkPtr = std::make_shared<LinkInfo>();
    linkPtr->link = &_link;
    this->links.insert({_link.GetId(), linkPtr});
    this->childIdToParentId.insert({_link.GetId(), _modelId});

    return this->GenerateIdentity(_link.GetId(), linkPtr);
  }

  public: inline Identity AddCollision(std::size_t _linkId,
    tpelib::Collision &_collision)
  {
    auto collisionPtr = std::make_shared<CollisionInfo>();
    collisionPtr->collision = &_collision;
    this->collisions.insert({_collision.GetId(), collisionPtr});
    this->childIdToParentId.insert({_collision.GetId(), _linkId});

    return this->GenerateIdentity(_collision.GetId(), collisionPtr);
  }

  public: std::map<std::size_t, std::shared_ptr<WorldInfo>> worlds;
  public: std::map<std::size_t, std::shared_ptr<ModelInfo>> models;
  public: std::map<std::size_t, std::shared_ptr<LinkInfo>> links;
  public: std::map<std::size_t, std::shared_ptr<CollisionInfo>> collisions;
  public: std::map<std::size_t, std::size_t> childIdToParentId;
};

}
}
}

#endif
