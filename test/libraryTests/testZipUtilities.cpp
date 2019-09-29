/*
* LLNS Copyright Start
* Copyright (c) 2014-2018, Lawrence Livermore National Security
* This work was performed under the auspices of the U.S. Department
* of Energy by Lawrence Livermore National Laboratory in part under
* Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
* Produced at the Lawrence Livermore National Laboratory.
* All rights reserved.
* For details, see the LICENSE file.
* LLNS Copyright End
*/

// test case for workQueue

#include "utilities/zipUtilities.h"
#include <filesystem>
#include <boost/test/unit_test.hpp>
#include <fstream>

#include "../testHelper.h"

static const std::string zip_test_directory (GRIDDYN_TEST_DIRECTORY "/zip_tests/");

BOOST_AUTO_TEST_SUITE (zipUtilities_tests, * boost::unit_test::label("quick"))

BOOST_AUTO_TEST_CASE (unzip_test)
{
    std::string file = zip_test_directory + "Rectifier.fmu";
    std::string directory = zip_test_directory + "Rectifier";
    int status = utilities::unzip (file, directory);
    BOOST_CHECK (status == 0);
    BOOST_REQUIRE (std::filesystem::exists (directory));
    std::filesystem::remove_all (directory);
}

BOOST_AUTO_TEST_CASE (zip_test2)
{
    // make two files with very simple text
    int fileSize1 = 1000000;
    std::vector<char> a0 (fileSize1, '0');
    std::string fileZeros = zip_test_directory + "zeros.txt";
    std::ofstream outZeros (fileZeros);
    outZeros.write (a0.data (), fileSize1);
    outZeros.close ();
    int fileSize2 = 981421;
    std::vector<char> a1 (fileSize2, '1');
    std::string fileOnes = zip_test_directory + "ones.txt";
    std::ofstream outOnes (fileOnes);
    outOnes.write (a1.data (), fileSize2);
    outOnes.close ();
    // zip them up into a zip file
    auto zipfile = zip_test_directory + "data.zip";
    auto status = utilities::zip (zipfile, std::vector<std::string>{fileZeros, fileOnes});
    BOOST_CHECK (status == 0);
    BOOST_REQUIRE (std::filesystem::exists (zipfile));

    // get the sizes of the original files
    auto filesize1 = std::filesystem::file_size (fileZeros);
    auto filesize2 = std::filesystem::file_size (fileOnes);

    auto zipsize = std::filesystem::file_size (zipfile);
    // make sure we compressed a lot
    BOOST_CHECK (zipsize < (filesize1 + filesize2) / 40);

    // remove the files
    std::filesystem::remove (fileZeros);
    std::filesystem::remove (fileOnes);
    // extract them and recheck sizes
    status = utilities::unzip (zipfile, zip_test_directory);
    BOOST_CHECK (status == 0);
    BOOST_REQUIRE (std::filesystem::exists (fileZeros));
    BOOST_REQUIRE (std::filesystem::exists (fileOnes));

    auto filesize1b = std::filesystem::file_size (fileZeros);
    auto filesize2b = std::filesystem::file_size (fileOnes);

    BOOST_CHECK (filesize1 == filesize1b);
    BOOST_CHECK (filesize2 == filesize2b);
    // remove all the files
    std::filesystem::remove (fileZeros);
    std::filesystem::remove (fileOnes);
    std::filesystem::remove (zipfile);
}


BOOST_AUTO_TEST_SUITE_END ()
