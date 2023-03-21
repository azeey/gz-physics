/*
 * Copyright (C) 2022 Open Source Robotics Foundation
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
#include <gtest/gtest.h>

#include <gz/common/Console.hh>
#include <gz/plugin/Loader.hh>

#include "TestLibLoader.hh"

#include <gz/physics/ConstructEmpty.hh>
#include <gz/physics/FindFeatures.hh>
#include <gz/physics/GetEntities.hh>
#include <gz/physics/RequestEngine.hh>
#include <gz/physics/RemoveEntities.hh>

#include "gz/physics/BoxShape.hh"

template <class T>
class ConstructEmptyWorldTest:
  public testing::Test, public gz::physics::TestLibLoader
{
  // Documentation inherited
  public: void SetUp() override
  {
    gz::common::Console::SetVerbosity(4);

    loader.LoadLib(ConstructEmptyWorldTest::GetLibToTest());

    // TODO(ahcorde): We should also run the 3f, 2d, and 2f variants of
    // FindFeatures
    pluginNames = gz::physics::FindFeatures3d<T>::From(loader);
    if (pluginNames.empty())
    {
      std::cerr << "No plugins with required features found in "
                << GetLibToTest() << std::endl;
      GTEST_SKIP();
    }
  }

  using FeaturePolicy3d = gz::physics::FeaturePolicy3d;

  public: void MakeEmptyWorld(
              const std::string &_pluginName,
              gz::physics::EnginePtr<FeaturePolicy3d, T> &_engine,
              gz::physics::WorldPtr<FeaturePolicy3d, T> &_world)
  {
    std::cout << "Testing plugin: " << _pluginName << std::endl;
    gz::plugin::PluginPtr plugin = this->loader.Instantiate(_pluginName);

    _engine = gz::physics::RequestEngine3d<T>::From(plugin);
    ASSERT_NE(nullptr, _engine);

    _world = _engine->ConstructEmptyWorld("empty world");
    ASSERT_NE(nullptr, _world);
  }

  public: std::set<std::string> pluginNames;
  public: gz::plugin::Loader loader;
};

using gz::physics::FeaturePolicy3d;

using FeaturesUpToEmptyWorld = gz::physics::FeatureList<
  gz::physics::GetEngineInfo,
  gz::physics::ConstructEmptyWorldFeature
>;

using ConstructEmptyWorldTestFeaturesUpToEmptyWorld =
    ConstructEmptyWorldTest<FeaturesUpToEmptyWorld>;

/////////////////////////////////////////////////
TEST_F(ConstructEmptyWorldTestFeaturesUpToEmptyWorld, ConstructUpToEmptyWorld)
{
  for (const std::string &name : this->pluginNames)
  {
    gz::physics::Engine3dPtr<FeaturesUpToEmptyWorld> engine;
    gz::physics::World3dPtr<FeaturesUpToEmptyWorld> world;

    this->MakeEmptyWorld(name, engine, world);
  }
}

using FeaturesUpToGetWorldFromEngine = gz::physics::FeatureList<
  FeaturesUpToEmptyWorld,
  gz::physics::GetWorldFromEngine
>;

using ConstructEmptyWorldTestUpToGetWorldFromEngine =
    ConstructEmptyWorldTest<FeaturesUpToGetWorldFromEngine>;

/////////////////////////////////////////////////
TEST_F(ConstructEmptyWorldTestUpToGetWorldFromEngine,
       ConstructUpToEmptyWorld)
{
  for (const std::string &name : this->pluginNames)
  {
    gz::physics::Engine3dPtr<FeaturesUpToGetWorldFromEngine> engine;
    gz::physics::World3dPtr<FeaturesUpToGetWorldFromEngine> world;
    this->MakeEmptyWorld(name, engine, world);

    EXPECT_EQ("empty world", world->GetName());
    EXPECT_EQ(engine, world->GetEngine());
  }
}

using FeaturesUpToEmptyModelFeature = gz::physics::FeatureList<
  FeaturesUpToGetWorldFromEngine,
  gz::physics::ConstructEmptyModelFeature
>;

using ConstructEmptyWorldTestUpToEmptyModelFeature =
    ConstructEmptyWorldTest<FeaturesUpToEmptyModelFeature>;

/////////////////////////////////////////////////
TEST_F(ConstructEmptyWorldTestUpToEmptyModelFeature,
       ConstructUpToEmptyWorld)
{
  for (const std::string &name : this->pluginNames)
  {
    gz::physics::Engine3dPtr<FeaturesUpToEmptyModelFeature> engine;
    gz::physics::World3dPtr<FeaturesUpToEmptyModelFeature> world;
    this->MakeEmptyWorld(name, engine, world);

    EXPECT_EQ("empty world", world->GetName());
    EXPECT_EQ(engine, world->GetEngine());

    auto model = world->ConstructEmptyModel("empty model");
    ASSERT_NE(nullptr, model);
    EXPECT_NE(model, world->ConstructEmptyModel("dummy"));
  }
}

using FeaturesUpToGetModelFromWorld = gz::physics::FeatureList<
  FeaturesUpToEmptyModelFeature,
  gz::physics::GetModelFromWorld
>;

using ConstructEmptyWorldTestUpToGetModelFromWorld =
    ConstructEmptyWorldTest<FeaturesUpToGetModelFromWorld>;

/////////////////////////////////////////////////
TEST_F(ConstructEmptyWorldTestUpToGetModelFromWorld,
       ConstructUpToEmptyWorld)
{
  for (const std::string &name : this->pluginNames)
  {
    gz::physics::Engine3dPtr<FeaturesUpToGetModelFromWorld> engine;
    gz::physics::World3dPtr<FeaturesUpToGetModelFromWorld> world;
    this->MakeEmptyWorld(name, engine, world);

    EXPECT_EQ("empty world", world->GetName());
    EXPECT_EQ(engine, world->GetEngine());

    auto model = world->ConstructEmptyModel("empty model");
    ASSERT_NE(nullptr, model);
    EXPECT_EQ("empty model", model->GetName());
    EXPECT_EQ(world, model->GetWorld());
    EXPECT_NE(model, world->ConstructEmptyModel("dummy"));
  }
}

using FeaturesUpToEmptyNestedModelFeature = gz::physics::FeatureList<
  FeaturesUpToGetModelFromWorld,
  gz::physics::ConstructEmptyNestedModelFeature,
  gz::physics::GetNestedModelFromModel
>;

using ConstructEmptyWorldTestUpToEmptyNestedModelFeature =
    ConstructEmptyWorldTest<FeaturesUpToEmptyNestedModelFeature>;

/////////////////////////////////////////////////
TEST_F(ConstructEmptyWorldTestUpToEmptyNestedModelFeature,
       ConstructUpToEmptyWorld)
{
  for (const std::string &name : this->pluginNames)
  {
    gz::physics::Engine3dPtr<FeaturesUpToEmptyNestedModelFeature> engine;
    gz::physics::World3dPtr<FeaturesUpToEmptyNestedModelFeature> world;
    this->MakeEmptyWorld(name, engine, world);

    auto model = world->ConstructEmptyModel("empty model");

    auto nestedModel = model->ConstructEmptyNestedModel("empty nested model");
    ASSERT_NE(nullptr, nestedModel);
    EXPECT_EQ("empty nested model", nestedModel->GetName());
    EXPECT_EQ(1u, model->GetNestedModelCount());
    EXPECT_EQ(0u, model->GetIndex());
    EXPECT_EQ(nestedModel, model->GetNestedModel(0));
    EXPECT_EQ(nestedModel, model->GetNestedModel("empty nested model"));
    EXPECT_NE(nestedModel, nestedModel->ConstructEmptyNestedModel("dummy"));
    // This should remain 1 since we're adding a nested model in `nestedModel` not
    // in `model`.
    EXPECT_EQ(1u, model->GetNestedModelCount());
    EXPECT_EQ(1u, nestedModel->GetNestedModelCount());
  }
}

using FeaturesUpToEmptyLink = gz::physics::FeatureList<
  FeaturesUpToEmptyModelFeature,
  gz::physics::GetLinkFromModel,
  gz::physics::ConstructEmptyLinkFeature
>;

using ConstructEmptyWorldTestUpToEmptyLink =
    ConstructEmptyWorldTest<FeaturesUpToEmptyLink>;

/////////////////////////////////////////////////
TEST_F(ConstructEmptyWorldTestUpToEmptyLink, ConstructUpToEmptyWorld)
{
  for (const std::string &name : this->pluginNames)
  {
    gz::physics::Engine3dPtr<FeaturesUpToEmptyLink> engine;
    gz::physics::World3dPtr<FeaturesUpToEmptyLink> world;
    this->MakeEmptyWorld(name, engine, world);

    EXPECT_EQ("empty world", world->GetName());
    EXPECT_EQ(engine, world->GetEngine());

    auto model = world->ConstructEmptyModel("empty model");

    auto link = model->ConstructEmptyLink("empty link");
    ASSERT_NE(nullptr, link);
    EXPECT_EQ("empty link", link->GetName());
    EXPECT_EQ(model, link->GetModel());
    EXPECT_NE(link, model->ConstructEmptyLink("dummy"));
    EXPECT_EQ(model, link->GetModel());

    auto child = model->ConstructEmptyLink("child link");
    EXPECT_EQ(model, child->GetModel());
  }
}

using FeaturesUpToRemove = gz::physics::FeatureList<
  gz::physics::GetEngineInfo,
  gz::physics::ConstructEmptyWorldFeature,
  gz::physics::GetWorldFromEngine,
  gz::physics::ConstructEmptyModelFeature,
  gz::physics::GetModelFromWorld,
  gz::physics::GetLinkFromModel,
  gz::physics::ConstructEmptyLinkFeature,
  gz::physics::ConstructEmptyNestedModelFeature,
  gz::physics::GetNestedModelFromModel,
  gz::physics::RemoveEntities
>;

using ConstructEmptyWorldTestUpToRemove =
    ConstructEmptyWorldTest<FeaturesUpToRemove>;

/////////////////////////////////////////////////
TEST_F(ConstructEmptyWorldTestUpToRemove, ConstructUpToEmptyWorld)
{
  for (const std::string &name : this->pluginNames)
  {
    gz::physics::Engine3dPtr<FeaturesUpToRemove> engine;
    gz::physics::World3dPtr<FeaturesUpToRemove> world;
    this->MakeEmptyWorld(name, engine, world);

    auto model = world->ConstructEmptyModel("empty model");
    ASSERT_NE(nullptr, model);
    auto modelAlias = world->GetModel(0);

    model->Remove();
    EXPECT_TRUE(model->Removed());
    EXPECT_TRUE(modelAlias->Removed());
    EXPECT_EQ(nullptr, world->GetModel(0));
    EXPECT_EQ(nullptr, world->GetModel("empty model"));
    EXPECT_EQ(0ul, world->GetModelCount());

    auto model2 = world->ConstructEmptyModel("model2");
    ASSERT_NE(nullptr, model2);
    EXPECT_EQ(0ul, model2->GetIndex());
    world->RemoveModel(0);
    EXPECT_EQ(0ul, world->GetModelCount());

    auto model3 = world->ConstructEmptyModel("model 3");
    ASSERT_NE(nullptr, model3);
    EXPECT_EQ(1u, world->GetModelCount());
    world->RemoveModel("model 3");
    EXPECT_EQ(0ul, world->GetModelCount());
    EXPECT_EQ(nullptr, world->GetModel("model 3"));

    auto parentModel = world->ConstructEmptyModel("parent model");
    ASSERT_NE(nullptr, parentModel);
    EXPECT_EQ(0u, parentModel->GetNestedModelCount());
    auto nestedModel1 =
        parentModel->ConstructEmptyNestedModel("empty nested model1");
    ASSERT_NE(nullptr, nestedModel1);
    EXPECT_EQ(1u, parentModel->GetNestedModelCount());

    EXPECT_TRUE(parentModel->RemoveNestedModel(0));
    EXPECT_EQ(0u, parentModel->GetNestedModelCount());
    EXPECT_TRUE(nestedModel1->Removed());

    auto nestedModel2 =
        parentModel->ConstructEmptyNestedModel("empty nested model2");
    ASSERT_NE(nullptr, nestedModel2);
    EXPECT_EQ(nestedModel2, parentModel->GetNestedModel(0));
    EXPECT_TRUE(parentModel->RemoveNestedModel("empty nested model2"));
    EXPECT_EQ(0u, parentModel->GetNestedModelCount());
    EXPECT_TRUE(nestedModel2->Removed());

    auto nestedModel3 =
        parentModel->ConstructEmptyNestedModel("empty nested model3");
    ASSERT_NE(nullptr, nestedModel3);
    EXPECT_EQ(nestedModel3, parentModel->GetNestedModel(0));
    EXPECT_TRUE(nestedModel3->Remove());
    EXPECT_EQ(0u, parentModel->GetNestedModelCount());
    EXPECT_TRUE(nestedModel3->Removed());

    auto nestedModel4 =
        parentModel->ConstructEmptyNestedModel("empty nested model4");
    ASSERT_NE(nullptr, nestedModel4);
    EXPECT_EQ(nestedModel4, parentModel->GetNestedModel(0));
    // Remove the parent model and check that the nested model is removed as well
    EXPECT_TRUE(parentModel->Remove());
    EXPECT_TRUE(nestedModel4->Removed());
  }
}

using FeaturesUpToEmptyNestedModelFeature2 = gz::physics::FeatureList<
  gz::physics::GetEngineInfo,
  gz::physics::ConstructEmptyWorldFeature,
  gz::physics::GetWorldFromEngine,
  gz::physics::ConstructEmptyModelFeature,
  gz::physics::GetModelFromWorld,
  gz::physics::ConstructEmptyNestedModelFeature,
  gz::physics::GetNestedModelFromModel,
  gz::physics::RemoveEntities
>;

using ConstructEmptyWorldTestUpToEmptyNestedModelFeature2 =
    ConstructEmptyWorldTest<FeaturesUpToEmptyNestedModelFeature2>;

/////////////////////////////////////////////////
TEST_F(ConstructEmptyWorldTestUpToEmptyNestedModelFeature2,
       ModelByIndexWithNestedModels)
{
  for (const std::string &name : this->pluginNames)
  {
    gz::physics::Engine3dPtr<FeaturesUpToEmptyNestedModelFeature2> engine;
    gz::physics::World3dPtr<FeaturesUpToEmptyNestedModelFeature2> world;
    this->MakeEmptyWorld(name, engine, world);

    auto model1 = world->ConstructEmptyModel("model1");
    ASSERT_NE(nullptr, model1);
    EXPECT_EQ(0ul, model1->GetIndex());

    auto parentModel = world->ConstructEmptyModel("parent model");
    ASSERT_NE(nullptr, parentModel);
    EXPECT_EQ(1ul, parentModel->GetIndex());

    auto nestedModel1 =
        parentModel->ConstructEmptyNestedModel("empty nested model1");
    ASSERT_NE(nullptr, nestedModel1);
    EXPECT_EQ(0ul, nestedModel1->GetIndex());

    auto model2 = world->ConstructEmptyModel("model2");
    ASSERT_NE(nullptr, model2);
    EXPECT_EQ(2ul, model2->GetIndex());
    EXPECT_TRUE(model2->Remove());

    auto model2Again = world->ConstructEmptyModel("model2_again");
    ASSERT_NE(nullptr, model2Again);
    EXPECT_EQ(2ul, model2Again->GetIndex());
  }
}

int main(int argc, char *argv[])
{
  ::testing::InitGoogleTest(&argc, argv);
  if (!gz::physics::TestLibLoader::init(argc, argv))
  {
    return -1;
  }
  return RUN_ALL_TESTS();
}
