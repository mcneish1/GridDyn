/*
* LLNS Copyright Start
 * Copyright (c) 2017, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department
 * of Energy by Lawrence Livermore National Laboratory in part under
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
*/

#include "Condition.h"
#include "grabberInterpreter.hpp"
#include "grabberSet.h"
#include "utilities/mapOps.hpp"

namespace griddyn
{
std::unique_ptr<Condition> make_condition (const std::string &condString, coreObject *rootObject)
{
    auto cString = stringOps::xmlCharacterCodeReplace (condString);
    // size_t posA = condString.find_first_of("&|");
    // TODO:PT: deal with parenthesis and &| conditions

    size_t pos = cString.find_first_of ("><=!");
    if (pos == std::string::npos)
    {
        return nullptr;
    }

    char A = cString[pos];
    char B = cString[pos + 1];
    std::string BlockA = stringOps::trim (cString.substr (0, pos - 1));
    std::string BlockB = (B == '=') ? cString.substr (pos + 2) : cString.substr (pos + 1);

    stringOps::trimString (BlockB);
    auto gc = std::make_unique<Condition> ();

    // get the state grabbers part

    gc->setConditionLHS (std::make_shared<grabberSet> (BlockA, rootObject));

    gc->setConditionRHS (std::make_shared<grabberSet> (BlockB, rootObject));

    std::string condstr;
    condstr.push_back (A);
    if (B == '=')
    {
        condstr.push_back (B);
    }

    gc->setComparison (comparisonFromString (condstr));

    return gc;
}

static const std::unordered_map<std::string, comparison_type> compStrMap{
  {">", comparison_type::gt},   {"gt", comparison_type::gt},   {">=", comparison_type::ge},
  {"ge", comparison_type::ge},  {"<", comparison_type::lt},    {"lt", comparison_type::lt},
  {"<=", comparison_type::le},  {"le", comparison_type::le},   {"=", comparison_type::eq},
  {"eq", comparison_type::eq},  {"==", comparison_type::eq},   {"!=", comparison_type::ne},
  {"ne", comparison_type::ne},  {"~=", comparison_type::ne},   {"<>", comparison_type::ne},
  {"===", comparison_type::eq}, {"??", comparison_type::null},
};

comparison_type comparisonFromString (const std::string &compStr)
{
    return mapFind (compStrMap, compStr, comparison_type::null);
}

std::string to_string (comparison_type comp)
{
    switch (comp)
    {
    case comparison_type::gt:
        return ">";
    case comparison_type::ge:
        return ">=";
    case comparison_type::lt:
        return "<";
    case comparison_type::le:
        return "<=";
    case comparison_type::eq:
        return "==";
    case comparison_type::ne:
        return "!=";
    case comparison_type::null:
        return "??";
    default:
        return "??";
    }
}

std::unique_ptr<Condition>
make_condition (const std::string &field, const std::string &compare, double level, coreObject *rootObject)
{
    return make_condition (field, comparisonFromString (compare), level, rootObject);
    // get the state grabbers part
}

std::unique_ptr<Condition>
make_condition (const std::string &field, comparison_type comp, double level, coreObject *rootObject)
{
    try
    {
        auto gset = std::make_shared<grabberSet> (field, rootObject);
        auto gc = std::make_unique<Condition> (gset);
        gc->setConditionRHS (level);

        gc->setComparison (comp);

        return gc;
    }
    catch (const std::invalid_argument &ia)
    {
        rootObject->log (rootObject, print_level::warning, ia.what ());
        return nullptr;
    }
}

Condition::Condition (std::shared_ptr<grabberSet> valGrabber) : conditionA (std::move (valGrabber)) {}
Condition::~Condition () = default;

std::shared_ptr<Condition> Condition::clone (std::shared_ptr<Condition> gc) const
{
    auto ngc = std::move (gc);
    if (!ngc)
    {
        ngc = std::make_shared<Condition> ();
    }
    ngc->m_constant = m_constant;
    ngc->m_margin = m_margin;
    ngc->m_constB = m_constB;
    ngc->m_curr_margin = m_curr_margin;
    ngc->use_margin = use_margin;

    if (conditionA)
    {
        ngc->conditionA = conditionA->clone (ngc->conditionA);
    }

    if (!m_constB)
    {
        if (conditionB)
        {
            ngc->conditionB = conditionB->clone (ngc->conditionB);
        }
    }
    return ngc;
}

void Condition::setConditionLHS (std::shared_ptr<grabberSet> valGrabber)
{
    if (valGrabber)
    {
        conditionA = std::move (valGrabber);
    }
}

void Condition::setConditionRHS (std::shared_ptr<grabberSet> valGrabber)
{
    if (valGrabber)
    {
        conditionB = std::move (valGrabber);
        m_constB = false;
    }
}

void Condition::updateObject (coreObject *obj, object_update_mode mode)
{
    // Update object may throw an error if it does everything is fine
    // if it doesn't then B update may throw an error in which case we need to rollback A for exception safety
    // this would be very unusual to occur.
    coreObject *keyObject = nullptr;
    if (conditionA)
    {
        keyObject = conditionA->getObject ();
        conditionA->updateObject (obj, mode);
    }

    if (conditionB)
    {
        try
        {
            conditionB->updateObject (obj, mode);
        }
        catch (objectUpdateFailException &oe)
        {
            if ((conditionA) && (keyObject != nullptr))
            {
                // now rollback A
                conditionA->updateObject (keyObject->getRoot (), object_update_mode::match);
            }
            throw (oe);
        }
    }
}

void Condition::setComparison (const std::string &compStr) { setComparison (comparisonFromString (compStr)); }
void Condition::setComparison (comparison_type ct)
{
    comp = ct;
    switch (comp)
    {
    case comparison_type::gt:
    case comparison_type::ge:
        evalf = [](double p1, double p2, double margin) { return p2 - p1 - margin; };
        break;
    case comparison_type::lt:
    case comparison_type::le:
        evalf = [](double p1, double p2, double margin) { return p1 - p2 + margin; };
        break;
    case comparison_type::eq:
        evalf = [](double p1, double p2, double margin) { return std::abs (p1 - p2) - margin; };
        break;
    case comparison_type::ne:
        evalf = [](double p1, double p2, double margin) { return -std::abs (p1 - p2) + margin; };
        break;
    default:
        evalf = [](double, double, double) { return kNullVal; };
        break;
    }
}

double Condition::evalCondition ()
{
    double v1 = conditionA->grabData ();
    double v2 = (m_constB) ? m_constant : conditionB->grabData ();

    return evalf (v1, v2, m_curr_margin);
}

double Condition::evalCondition (const stateData &sD, const solverMode &sMode)
{
    double v1 = conditionA->grabData (sD, sMode);
    double v2 = (m_constB) ? m_constant : conditionB->grabData (sD, sMode);
    return evalf (v1, v2, m_curr_margin);
}

double Condition::getVal (int side) const
{
    double v = (side == 2) ? ((m_constB) ? m_constant : conditionB->grabData ()) : conditionA->grabData ();
    return v;
}

double Condition::getVal (int side, const stateData &sD, const solverMode &sMode) const
{
    double v = (side == 2) ? ((m_constB) ? m_constant : conditionB->grabData (sD, sMode)) :
                             conditionA->grabData (sD, sMode);
    return v;
}

inline bool isEqualityComparison (comparison_type comp)
{
    switch (comp)
    {
    case comparison_type::ge:
    case comparison_type::le:
    case comparison_type::eq:
        return true;
    default:
        return false;
    }
}

bool Condition::checkCondition () const
{
    double v1 = conditionA->grabData ();
    double v2 = (m_constB) ? m_constant : conditionB->grabData ();
    double ret = evalf (v1, v2, m_curr_margin);
    return (isEqualityComparison (comp)) ? (ret <= 0.0) : (ret < 0.0);
}

bool Condition::checkCondition (const stateData &sD, const solverMode &sMode) const
{
    double v1 = conditionA->grabData (sD, sMode);
    double v2 = (m_constB) ? m_constant : conditionB->grabData (sD, sMode);

    double ret = evalf (v1, v2, m_curr_margin);
    return (isEqualityComparison (comp)) ? (ret <= 0.0) : (ret < 0.0);
}

void Condition::setMargin (double val)
{
    m_margin = val;
    if (use_margin)
    {
        m_curr_margin = m_margin;
    }
}

coreObject *Condition::getObject () const
{
    if (conditionA)
    {
        return conditionA->getObject ();
    }

    return nullptr;
}

void Condition::getObjects (std::vector<coreObject *> &objects) const
{
    if (conditionA)
    {
        conditionA->getObjects (objects);
    }

    if (!m_constB)
    {
        if (conditionB)
        {
            conditionB->getObjects (objects);
        }
    }
}

}  // namespace griddyn