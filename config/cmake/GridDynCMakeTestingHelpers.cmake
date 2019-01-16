# LLNS Copyright Start
# Copyright (c) 2014-2018, Lawrence Livermore National Security
# This work was performed under the auspices of the U.S. Department
# of Energy by Lawrence Livermore National Laboratory in part under
# Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
# Produced at the Lawrence Livermore National Laboratory.
# All rights reserved.
# For details, see the LICENSE file.
# LLNS Copyright End

function(add_griddyn_test test_category test_name)
  # TODO: test_category should make test_name run whenever test_category is run
  #       e.g. `ctest -L Continuous` should run test_name if test_category is testLibrary
  # TODO: add a named parameter for test name or category then
  #       (probably optional TEST_CATEGORY?)
  #       (actually, is that what boost::unit_test::label("quick") does???)

  # Create test executable
  set(options "")
  set(oneValueArgs NAME)
  set(multiValueArgs SOURCES REQUIRED_LIBRARIES)

  cmake_parse_arguments(_ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  if (NOT _ARG_SOURCES)
    message(FATAL_ERROR "Test ${test_name} created without any sources")
  endif()

  add_executable(${test_name} ${_ARG_SOURCES})

  # Set BOOST_TEST_MODULE name
  # TODO can this be put somewhere else? I don't think cmakedefine works because
  #      this wants to be different for every test
  target_compile_options(${test_name} PUBLIC "-DGRIDDYN_TEST_CATEGORY=${test_category}")

  # all tests require griddyn_test_base
  target_link_libraries(${test_name} PRIVATE griddyn_test_base)

  # todo: delete the shit out of this
  target_link_libraries(${test_name} PRIVATE fileInput)

  if(_ARG_REQUIRED_LIBRARIES)
    target_link_libraries(${test_name} PRIVATE "${_ARG_REQUIRED_LIBRARIES}")
  endif()

  # Create test
  set(full_test_name "${test_category}/${test_name}")
  add_test(NAME ${full_test_name} COMMAND $<TARGET_FILE:${test_name}>)

  message(STATUS "Adding test ${full_test_name}")
  set_property(
      TEST ${full_test_name}
      PROPERTY
          LABELS
          Quick
          Continuous
          Valgrind
  )

# TODO: ??. by default ctest looks for `return 0;`, not sure what boost test does
#  set_tests_properties(${test_name}
#    PROPERTIES
#    PASS_REGULAR_EXPRESSION
#    "All tests passed")

endfunction()
