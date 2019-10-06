#ifndef GRIDDYN_TEST_DIRECTORY
#error Please define GRIDDYN_TEST_DIRECTORY
#endif

#include <gtest/gtest.h>

#include <filesystem>

#include "fileInput/fileInput.h"
#include "griddyn/gridDynSimulation.h"

std::filesystem::path local_test_directory()
{
    return std::filesystem::path(GRIDDYN_TEST_DIRECTORY) / "ACTIVS" / "SyntheticUSA";
}

TEST(SyntheticUSA, Loads)
{
    auto test_file = local_test_directory() / "SyntheticUSA.RAW";

    ASSERT_TRUE(std::filesystem::exists(test_file)) << "Could not find " << test_file;

    auto sim = std::make_unique<griddyn::gridDynSimulation>();

    griddyn::loadFile(sim.get(), test_file);
}
