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

#include "fileInput/fileInput.h"
#include "utilities/zipUtilities.h"
#include <filesystem>

namespace griddyn
{

void loadGDZ (coreObject *parentObject, const std::string &fileName, readerInfo &ri)
{
    std::filesystem::path fpath (fileName);
    if (!std::filesystem::exists (fpath))
    {
        return;
    }

    auto extractPath = std::filesystem::temp_directory_path () / fpath.stem ();
    auto keyFile = extractPath / "simulation.xml";
    if (!std::filesystem::exists (keyFile))
    {
        auto ret = utilities::unzip (fileName, extractPath.string ());
        if (ret != 0)
        {
            return;
        }
    }

    if (!std::filesystem::exists (keyFile))
    {
        return;
    }
    ri.addDirectory (extractPath.string ());
    auto resourcePath = extractPath / "resources";
    if (std::filesystem::exists (resourcePath))
    {
        ri.addDirectory (resourcePath.string ());
    }
    loadFile (parentObject, keyFile.string (), &ri, "xml");
}

}//namespace griddyn
