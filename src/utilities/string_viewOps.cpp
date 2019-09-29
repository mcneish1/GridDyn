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

#include "utilities/string_viewOps.h"
#include "generic_string_ops.hpp"
#include <algorithm>
#include <stdexcept>

namespace utilities
{
namespace string_viewOps
{
void trimString (std::string_view &input, std::string_view trimCharacters)
{
    input.remove_suffix (input.length () - std::min (input.find_last_not_of (trimCharacters) + 1, input.size ()));
    input.remove_prefix (std::min (input.find_first_not_of (trimCharacters), input.size ()));
}

std::string_view trim (std::string_view input, std::string_view trimCharacters)
{
    const auto strStart = input.find_first_not_of (trimCharacters);
    if (strStart == std::string::npos)
    {
        return {};
    }

    const auto strEnd = input.find_last_not_of (trimCharacters);

    return input.substr (strStart, strEnd - strStart + 1);
}

void trim (std::vector<std::string_view> &input, std::string_view trimCharacters)
{
    for (auto &istr : input)
    {
        istr = trim (istr, trimCharacters);
    }
}

std::string_view getTailString (std::string_view input, char separationCharacter)
{
    auto tc = input.find_last_of (separationCharacter);
    std::string_view ret = (tc == std::string_view::npos) ? input : input.substr (tc + 1);
    return ret;
}

std::string_view getTailString (std::string_view input, std::string_view separationCharacters)
{
    auto tc = input.find_last_of (separationCharacters);
    std::string_view ret = (tc == std::string_view::npos) ? input : input.substr (tc + 1);
    return ret;
}

std::string_view removeQuotes (std::string_view str)
{
    std::string_view ret = trim (str);
    if (!ret.empty ())
    {
        if ((ret.front () == '\"') || (ret.front () == '\'') || (ret.front () == '`'))
        {
            if (ret.back () == ret.front ())
            {
                return ret.substr (1, ret.size () - 2);
            }
        }
    }
    return ret;
}

static const auto pmap = pairMapper ();

std::string_view removeBrackets (std::string_view str)
{
    std::string_view ret = trim (str);
    if (!ret.empty ())
    {
        if ((ret.front () == '[') || (ret.front () == '(') || (ret.front () == '{') || (ret.front () == '<'))
        {
            if (ret.back () == pmap[ret.front ()])
            {
                return ret.substr (1, ret.size () - 2);
            }
        }
    }
    return ret;
}

std::string_view merge (std::string_view string1, std::string_view string2)
{
    int diff = (string2.data () - string1.data ()) + static_cast<int> (string1.length ());
    if ((diff >= 0) && (diff < 24))  // maximum of 24 bytes between the strings
    {
        return std::string_view (string1.data (), diff + string1.length () + string2.length ());
    }
    if (string1.empty ())
    {
        return string2;
    }
    if (string2.empty ())
    {
        return string1;
    }
    throw (std::out_of_range ("unable to merge string_views"));
}

std::vector<std::string_view> split (std::string_view str, std::string_view delimiters, delimiter_compression compression)
{
    return generalized_string_split (str, delimiters, (compression == delimiter_compression::on));
}

std::vector<std::string_view> splitlineQuotes (std::string_view line,
                                   std::string_view delimiters,
                                   std::string_view quoteChars,
                                   delimiter_compression compression)
{
    bool compress = (compression == delimiter_compression::on);
    return generalized_section_splitting (line, delimiters, quoteChars, pmap, compress);
}

std::vector<std::string_view> splitlineBracket (std::string_view line,
                                    std::string_view delimiters,
                                    std::string_view bracketChars,
                                    delimiter_compression compression)
{
    bool compress = (compression == delimiter_compression::on);
    return generalized_section_splitting (line, delimiters, bracketChars, pmap, compress);
}

int toIntSimple (std::string_view input)
{
    int ret = 0;
    for (auto c : input)
    {
        if (isdigit (c) != 0)
        {
            ret = 10 * ret + (c - '0');
        }
    }
    return ret;
}

static const std::string_view digits ("0123456789");
int trailingStringInt (std::string_view input, std::string_view &output, int defNum)
{
    if (isdigit (input.back ()) == 0)
    {
        output = input;
        return defNum;
    }
    int num = defNum;
    auto pos1 = input.find_last_not_of (digits);
    if (pos1 == std::string_view::npos)  // in case the whole thing is a number
    {
        output = std::string_view{};
        num = toIntSimple (input);
    }
    else
    {
        if (pos1 == input.length () - 2)
        {
            num = input.back () - '0';
        }
        else
        {
            num = toIntSimple (input.substr (pos1 + 1));
        }

        if ((input[pos1] == '_') || (input[pos1] == '#'))
        {
            output = input.substr (0, pos1);
        }
        else
        {
            output = input.substr (0, pos1 + 1);
        }
    }

    return num;
}

int trailingStringInt (std::string_view input, int defNum)
{
    if (isdigit (input.back ()) == 0)
    {
        return defNum;
    }

    auto pos1 = input.find_last_not_of (digits);
    if (pos1 == std::string_view::npos)  // in case the whole thing is a number
    {
        return toIntSimple (input);
    }
    if (pos1 == input.length () - 2)
    {
        return input.back () - '0';
    }
    return toIntSimple (input.substr (pos1 + 1));
}

}  // namespace string_viewOps
}  // namespace utilities
