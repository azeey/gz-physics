gz_get_sources(tests)

if (NOT DART_FOUND)
  list(REMOVE_ITEM tests DoublePendulum.cc)
endif()

configure_file (test_config.h.in ${PROJECT_BINARY_DIR}/test_config.h)

gz_build_tests(
  TYPE INTEGRATION
  SOURCES ${tests}
  LIB_DEPS
    Eigen3::Eigen
    gz-physics-test
  TEST_LIST list
  ENVIRONMENT
    GZ_PHYSICS_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
  TEST_LIST list)

if (BUILD_TESTING)
  foreach(test ${list})

    list(APPEND mock_plugins
        MockEntities
        MockFrames
        MockJoints)
    if (DART_FOUND)
      list(APPEND mock_plugins MockDoublePendulum)
    endif()

    foreach(mock_plugin IN LISTS mock_plugins)

      target_compile_definitions(${test} PRIVATE
        "${mock_plugin}_LIB=\"$<TARGET_FILE:${mock_plugin}>\"")

      add_dependencies(${test} ${mock_plugin})

    endforeach()

  endforeach()
endif()

if(TARGET INTEGRATION_ExamplesBuild_TEST)
  add_dependencies(INTEGRATION_ExamplesBuild_TEST FAKE_INSTALL)

  # Pass the parent's CMAKE_PREFIX_PATH and CMAKE_INSTALL_PREFIX to the test
  # environment so it can find dependencies when testing is enabled after a
  # colcon build without sourcing the setup.bash script again.
  set(TEST_CMAKE_PREFIX_PATH "${FAKE_INSTALL_PREFIX};${CMAKE_PREFIX_PATH};${CMAKE_INSTALL_PREFIX}")

  # Use ENVIRONMENT_MODIFICATION to correctly handle appending paths cross-platform
  # using the host's path list separator (':' on Unix, ';' on Windows)
  set_tests_properties(INTEGRATION_ExamplesBuild_TEST PROPERTIES
    ENVIRONMENT_MODIFICATION "CMAKE_PREFIX_PATH=path_list_prepend:${TEST_CMAKE_PREFIX_PATH}")
endif()
