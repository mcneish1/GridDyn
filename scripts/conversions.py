from __future__ import division

import math

def affine_transform(transform, scale, translate):
    m, b = transform
    m = scale*m
    b = scale*b + translate
    return m, b

def inv_affine_transform(transform, scale, translate):
    return affine_transform(transform, 1/scale, -translate/scale)

def apply_transform(x, transform):
    m, b = transform
    return x*m+b

class Unit(object):
    def convert(self, other):
        raise "Unimplemented"

f = 1
f2 = 5

c = apply_transform(f, affine_transform((1, 0), 5/9, -32*5/9))
c2 = apply_transform(f2, affine_transform((1, 0), 5/9, -32*5/9))

print(f, c)
print(f2, c2)

# defUnit = 0,
# MW = 1,
# kW = 2,
# Watt = 3,
# MVAR = 4,
# Volt = 5,
# kV = 6,
# Ohm = 7,
# Amp = 8,
# MWps = 9,  //!< MW/s
# MWpmin = 10,  //!< MW/min
# MWph = 11,  //!< MW/hr
# Hz = 51,
# rps = 52,  // radians per second
# rpm = 53,  // revolutions per minute
# pu = 100,
# puMW = 101,
# puV = 105,
# puOhm = 107,
# puA = 108,
# puMWps = 109,  //!< per unit MW/s
# puMWpmin = 110,  //!< per unit MW/min
# puMWph = 111,  //!< per unit MW/hr
# puHz = 151,
# meter = 201,
# km = 202,
# mile = 203,
# ft = 204,
# deg = 301,
# rad = 302,
# sec = 401,
# min = 402,
# hour = 403,
# day = 404,
# week = 405,
# cost = 800,
# Cph = 801,  //!< cost per hour
# CpMWh = 802,  //!< cost per MWh
# CpMW2h = 803,  //!< cost per MWh^2
# CppuMWh = 804,  //!< cost per puMWh
# CppuMW2h = 805,  //!< cost per puMWh^2
# CpMVARh = 806,  //!< cost per MVARh
# CpMVAR2h = 807
# CppuMVARh = 808
# CppuMVAR2h = 809
# F = 1001
# C = 1002
# K = 1003
#

def assert_same_contents(a, b):
    a = set(a)
    b = set(b)

    assert a == b, missing(a, b)

def missing(seta, setb):
    x = setb - seta
    y = seta - setb
    return "{} missing from set a, {} missing from set b".format(x, y)


class UnitHolder(object):
    units = [
        # "defUnit", # ??
        "MW", "kW", "Watt", "MVAR", "Volt", "kV", "Ohm", "Amp", "Hz",
        "rps", "rpm", "meter", "km", "mile", "ft", "deg", "rad", "sec",
        "min", "hour", "day", "week", "cost", "F", "C", "K"
    ]

    relative_units = [
        "pu", "puMW", "puV", "puOhm", "puA", "puMVAR", "puHz"
    ]

    composite_units = [
        # note frequency units are not composite, as revolutions is unitless
        "MWps", "MWpmin", "MWph", "Cph", "CpMWh", "CpMW2h", "CppuMWh",
        "CppuMW2h", "CpMVARh", "CpMVAR2h", "CppuMVARh", "CppuMVAR2h"
    ]

    unit_types = [
        "distance", "angle", "power", "voltage", "frequency", "time", "cost",
        "temperature", "resistance", "current", "reactive power"
    ]

    unit_type_map = {
        # "defUnit"   : None, # TODO ??
        "MW"    : "power",
        "kW"    : "power",
        "Watt"  : "power",
        "MVAR"  : "reactive power",
        "Volt"  : "voltage",
        "kV"    : "voltage",
        "Ohm"   : "resistance",
        "Amp"   : "current",
        "Hz"    : "frequency",
        "rps"   : "frequency",
        "rpm"   : "frequency",
        "meter" : "distance",
        "km"    : "distance",
        "mile"  : "distance",
        "ft"    : "distance",
        "deg"   : "angle",
        "rad"   : "angle",
        "sec"   : "time",
        "min"   : "time",
        "hour"  : "time",
        "day"   : "time",
        "week"  : "time",
        "cost"  : "cost",
        "F"     : "temperature",
        "C"     : "temperature",
        "K"     : "temperature",
    }

    cannonical_unit_map = {
        "distance"          : "m",
        "power"             : "MW",
        "voltage"           : "V",
        "frequency"         : "Hz",
        "time"              : "sec",
        "temperature"       : "C",
        "angle"             : "rad",
        "cost"              : "cost",
        "resistance"        : "Ohm",
        "current"           : "Amp",
        "reactive power"    : "MVAR"
    }

    composite_unit_components = {
        #                 [ [numerator], [denominator] ]
        "MWps"          : [ [ "MW" ], [ "sec" ] ],
        "MWpmin"        : [ [ "MW" ], [ "min" ] ],
        "MWph"          : [ [ "MW" ], [ "hour" ] ],
        "Cph"           : [ [ "cost" ], [ "hour" ] ],
        "CpMWh"         : [ [ "cost" ], [ "hour", "MW" ] ],
        "CpMW2h"        : [ [ "cost" ], [ "hour", "MW", "MW" ] ],
        "CppuMWh"       : [ [ "cost" ], [ "puMW", "hour" ] ],
        "CppuMW2h"      : [ [ "cost" ], [ "puMW", "puMW", "hour" ] ],
        "CpMVARh"       : [ [ "cost" ], [ "MVAR", "hour" ] ],
        "CpMVAR2h"      : [ [ "cost" ], [ "MVAR", "MVAR", "hour" ] ],
        "CppuMVARh"     : [ [ "cost" ], [ "puMVAR", "hour" ] ],
        "CppuMVAR2h"    : [ [ "cost" ], [ "puMVAR", "puMVAR", "hour" ] ]
    }

    TODO = float('nan')

    cannonical_unit_conversion = {
        # affine_transform(lhs value, *rhs) -> cannonical_unit[lhs type]
        "Watt" :    (1000000, 0),
        "kW" :      (1000, 0),
        "kV" :      (1/1000, 0),
        "rps" :     (1, 0),
        "rpm" :     (60, 0),
        "km" :      (1/1000, 0),
        "mile" :    (TODO, 0),
        "ft" :      (TODO, 0),
        "deg" :     (math.pi/180, 0),
        "week" :    (TODO, 0),
        "day" :     (TODO, 0),
        "hour" :    (TODO, 0),
        "min" :     (1/60, 0),
        "F" :       (5/9, -32*5/9),
        "K" :       (1, 273.15)
    }

    def convert(self, unit_target):
        # TODO composite units

        unit_type = self.unit_type_map[self.unit]
        dest_type = self.unit_type_map[unit_target]
        assert(unit_type == dest_type)

        transform = (1, 0)
        if self.unit != self.cannonical_unit_map[dest_type]:
            transform = affine_transform((1, 0), *self.cannonical_unit_conversion[self.unit])
        if unit_target != self.cannonical_unit_map[dest_type]:
            transform = inv_affine_transform(transform, *self.cannonical_unit_conversion[unit_target])

        return UnitHolder(apply_transform(self.value, transform), unit_target)

    def __init__(self, value, unit):
        assert_same_contents(self.unit_type_map.keys(), self.units)
        assert_same_contents(self.unit_type_map.values(), self.unit_types)
        assert_same_contents(self.cannonical_unit_map.keys(), self.unit_types)

        if unit not in self.units:
            raise RuntimeError("bad unit")
        self.value = value
        self.unit = unit

f1 = UnitHolder(1, "F")
f2 = UnitHolder(5, "F")
c1 = f1.convert("C")
c2 = f2.convert("C")
print(f1.value, c1.value)
print(f2.value, c2.value)
