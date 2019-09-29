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

#include "fileInput/elementReaderTemplates.hpp"
#include "formatInterpreters/tinyxml2ReaderElement.h"
#include "formatInterpreters/tinyxmlReaderElement.h"
#include "griddyn/gridDynSimulation.h"
#include "fileInput/readElement.h"
#include "fileInput/readElementFile.h"
#include "fileInput/readerHelper.h"

#include <cstdio>
#include <sstream>
#include <utility>
#include <filesystem>

namespace griddyn
{
using namespace readerConfig;

std::unique_ptr<gridDynSimulation> readSimXMLFile (const std::string &fileName, readerInfo *ri, xmlreader rtype)
{
	if (!std::filesystem::exists(fileName))
	{
		return nullptr;
	}
    if (rtype == xmlreader::default_reader)
    {
        rtype = readerConfig::default_xml_reader;
    }
    switch (rtype)
    {
    case xmlreader::tinyxml:
    default:
        return std::unique_ptr<gridDynSimulation> (
          static_cast<gridDynSimulation *> (loadElementFile<tinyxmlReaderElement> (nullptr, fileName, ri)));
    case xmlreader::tinyxml2:
        return std::unique_ptr<gridDynSimulation> (
          static_cast<gridDynSimulation *> (loadElementFile<tinyxml2ReaderElement> (nullptr, fileName, ri)));
    }
}

std::unique_ptr<gridDynSimulation> readSimXMLFile (file_input_throw_if_null, const std::string &fileName, readerInfo *ri, xmlreader rtype)
{
    auto ptr = readSimXMLFile(fileName, ri, rtype);
    if (!ptr)
        throw fileOperationError("Load failed");
    return ptr;
}

} //namespace griddyn
