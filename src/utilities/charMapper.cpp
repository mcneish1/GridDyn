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

#include "utilities/charMapper.h"

namespace utilities
{
charMapper<bool> numericStartMapper ()
{
    charMapper<bool> nm (false);
    nm.addKey ('0', true);
    nm.addKey ('1', true);
    nm.addKey ('2', true);
    nm.addKey ('3', true);
    nm.addKey ('4', true);
    nm.addKey ('5', true);
    nm.addKey ('6', true);
    nm.addKey ('7', true);
    nm.addKey ('8', true);
    nm.addKey ('9', true);
    nm.addKey ('+', true);
    nm.addKey ('-', true);
    nm.addKey (' ', true);
    nm.addKey ('\t', true);
    nm.addKey ('.', true);
    nm.addKey ('\n', true);
    nm.addKey ('\r', true);
    nm.addKey ('\0', true);
    return nm;
}

charMapper<bool> numericEndMapper ()
{
    charMapper<bool> nm (false);
    nm.addKey ('0', true);
    nm.addKey ('1', true);
    nm.addKey ('2', true);
    nm.addKey ('3', true);
    nm.addKey ('4', true);
    nm.addKey ('5', true);
    nm.addKey ('6', true);
    nm.addKey ('7', true);
    nm.addKey ('8', true);
    nm.addKey ('9', true);
    nm.addKey (' ', true);
    nm.addKey ('\t', true);
    nm.addKey ('\n', true);
    nm.addKey ('\r', true);
    nm.addKey ('\0', true);
    return nm;
}

charMapper<unsigned char> pairMapper ()
{
    charMapper<unsigned char> dMap (0);
    for (unsigned char ii = 0; ii < 255; ++ii)
    {
        dMap.addKey (ii, ii);
    }
    dMap.addKey ('(', ')');
    dMap.addKey ('<', '>');
    dMap.addKey ('[', ']');
    dMap.addKey ('{', '}');
    dMap.addKey (')', '(');
    dMap.addKey ('>', '<');
    dMap.addKey (']', '[');
    dMap.addKey ('}', '{');
    return dMap;
}

}  // namespace utilities
