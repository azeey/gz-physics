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

#include <string>

#include "Link.hh"
#include "Model.hh"

using namespace ignition;
using namespace physics;
using namespace tpesim;

//////////////////////////////////////////////////
Model::Model() : Entity()
{
}

//////////////////////////////////////////////////
Entity &Model::AddLink()
{
  Link link;
  uint64_t linkId = link.GetId();
  const auto [it, success]  = this->children.insert({linkId, link});
  return it->second;
}

//////////////////////////////////////////////////
Entity &Model::GetLinkByName(const std::string &_name)
{
  for (auto it = this->children.begin(); it != this->children.end(); ++it)
  {
    if (it->second.GetName() == _name)
    {
      return it->second;
    }
  }
  return Entity::kNullEntity;
}
