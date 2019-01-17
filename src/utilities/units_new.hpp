
#pragma once

#include <limits>
#include <string>

namespace {
template<typename T>
T checked_add(T a, T b, T min_val = std::numeric_limits<T>::min, T max_val = std::numeric_limits<T>::max)
{
  if ((a > 0 && b > max_val - a) || (a < 0 && b < min_val - a))
  {
    throw std::runtime_error(std::to_string(a) + " + " + std::to_string(b) + " overflowed");
  }
  return a + b;
}

template<typename T>
T checked_assign(T val, T min_val = std::numeric_limits<T>::min, T max_val = std::numeric_limits<T>::max)
{
  if (val > max_val || val < min_val)
  {
    throw std::runtime_error(std::to_string(val) + " out of range");
  }
  return val;
}
} // anon namespace

namespace griddyn {
namespace units {

class unit {
public:
  std::string name() const
  {
    // TODO: actually implement this
    return "(some unit type)";
  }

  static unit parse(std::string const& input)
  {
    return parse(input.c_str());
  }

  // TODO: create transform with parse

  static unit parse(const char* str)
  {
    // Note this function makes no attempt to provide the affine transform required

    if (str == nullptr) { throw std::runtime_error("Please don't attempt to parse null pointers"); }

    #define STR_TO_RETURNED_UNIT(s, unit) else if (!strcmp(str, s)) return (unit)
    if (false) {}
    STR_TO_RETURNED_UNIT("mw", watt());
    STR_TO_RETURNED_UNIT("mws", watt());
    STR_TO_RETURNED_UNIT("mvar", dimensionless());
    STR_TO_RETURNED_UNIT("mva", dimensionless());
    STR_TO_RETURNED_UNIT("mvars", dimensionless());
    STR_TO_RETURNED_UNIT("amp", ampere());
    STR_TO_RETURNED_UNIT("a", ampere());
    STR_TO_RETURNED_UNIT("amps", ampere());
    STR_TO_RETURNED_UNIT("v", volt());
    STR_TO_RETURNED_UNIT("volt", volt());
    STR_TO_RETURNED_UNIT("volts", volt());
    STR_TO_RETURNED_UNIT("kv", volt());
    STR_TO_RETURNED_UNIT("kw", watt());
    STR_TO_RETURNED_UNIT("w", watt());
    STR_TO_RETURNED_UNIT("watt", watt());
    STR_TO_RETURNED_UNIT("watts", watt());
    STR_TO_RETURNED_UNIT("ohm", ohm());
    STR_TO_RETURNED_UNIT("ohms", ohm());
    STR_TO_RETURNED_UNIT("hz", hertz());
    STR_TO_RETURNED_UNIT("1/s", hertz());
    STR_TO_RETURNED_UNIT("cps", hertz());
    STR_TO_RETURNED_UNIT("rad/s", hertz());
    STR_TO_RETURNED_UNIT("rps", hertz());
    STR_TO_RETURNED_UNIT("rpm", hertz());
    STR_TO_RETURNED_UNIT("rev/min", hertz());
    STR_TO_RETURNED_UNIT("pumw", dimensionless());
    STR_TO_RETURNED_UNIT("mw/s", watt() / second());
    STR_TO_RETURNED_UNIT("mw/min", watt() / second());
    STR_TO_RETURNED_UNIT("mw/hr", watt() / second());
    STR_TO_RETURNED_UNIT("pumw/s", dimensionless());
    STR_TO_RETURNED_UNIT("pumw/min", dimensionless());
    STR_TO_RETURNED_UNIT("pumw/hr", dimensionless());
    STR_TO_RETURNED_UNIT("puhz", dimensionless());
    STR_TO_RETURNED_UNIT("pu", dimensionless());
    STR_TO_RETURNED_UNIT("puohm", dimensionless());
    STR_TO_RETURNED_UNIT("pua", dimensionless());
    STR_TO_RETURNED_UNIT("m", meter());
    STR_TO_RETURNED_UNIT("meter", meter());
    STR_TO_RETURNED_UNIT("meters", meter());
    STR_TO_RETURNED_UNIT("mi", meter());
    STR_TO_RETURNED_UNIT("mile", meter());
    STR_TO_RETURNED_UNIT("miles", meter());
    STR_TO_RETURNED_UNIT("ft", meter());
    STR_TO_RETURNED_UNIT("feet", meter());
    STR_TO_RETURNED_UNIT("foot", meter());
    STR_TO_RETURNED_UNIT("km", meter());
    STR_TO_RETURNED_UNIT("kilometer", meter());
    STR_TO_RETURNED_UNIT("kilometers", meter());
    STR_TO_RETURNED_UNIT("deg", dimensionless());
    STR_TO_RETURNED_UNIT("degree", dimensionless());
    STR_TO_RETURNED_UNIT("degrees", dimensionless());
    STR_TO_RETURNED_UNIT("rad", dimensionless());
    STR_TO_RETURNED_UNIT("radian", dimensionless());
    STR_TO_RETURNED_UNIT("radians", dimensionless());
    STR_TO_RETURNED_UNIT("s", second());
    STR_TO_RETURNED_UNIT("sec", second());
    STR_TO_RETURNED_UNIT("second", second());
    STR_TO_RETURNED_UNIT("seconds", second());
    STR_TO_RETURNED_UNIT("min", second());
    STR_TO_RETURNED_UNIT("minute", second());
    STR_TO_RETURNED_UNIT("minutes", second());
    STR_TO_RETURNED_UNIT("hr", second());
    STR_TO_RETURNED_UNIT("hour", second());
    STR_TO_RETURNED_UNIT("hours", second());
    STR_TO_RETURNED_UNIT("day", second());
    STR_TO_RETURNED_UNIT("days", second());
    STR_TO_RETURNED_UNIT("week", second());
    STR_TO_RETURNED_UNIT("wk", second());
    STR_TO_RETURNED_UNIT("weeks", second());
    STR_TO_RETURNED_UNIT("$", dimensionless());
    STR_TO_RETURNED_UNIT("cost", dimensionless());
    STR_TO_RETURNED_UNIT("$/hr", dimensionless());
    STR_TO_RETURNED_UNIT("cph", dimensionless());
    STR_TO_RETURNED_UNIT("$/mwh", dimensionless());
    STR_TO_RETURNED_UNIT("cpmwh", dimensionless());
    STR_TO_RETURNED_UNIT("$/mvarh", dimensionless());
    STR_TO_RETURNED_UNIT("$/mvah", dimensionless());
    STR_TO_RETURNED_UNIT("cpmvarh", dimensionless());
    STR_TO_RETURNED_UNIT("$/mw^2h", dimensionless());
    STR_TO_RETURNED_UNIT("cpmw2h", dimensionless());
    STR_TO_RETURNED_UNIT("$/mw2h", dimensionless());
    STR_TO_RETURNED_UNIT("$/mvar^2h", dimensionless());
    STR_TO_RETURNED_UNIT("$/mvah^2", dimensionless());
    STR_TO_RETURNED_UNIT("cpmvar2h", dimensionless());
    STR_TO_RETURNED_UNIT("$/mva2h", dimensionless());
    STR_TO_RETURNED_UNIT("$/pumwh", dimensionless());
    STR_TO_RETURNED_UNIT("cppumwh", dimensionless());
    STR_TO_RETURNED_UNIT("$/pumvarh", dimensionless());
    STR_TO_RETURNED_UNIT("$/pumvah", dimensionless());
    STR_TO_RETURNED_UNIT("cppumvarh", dimensionless());
    STR_TO_RETURNED_UNIT("$/pumw^2h", dimensionless());
    STR_TO_RETURNED_UNIT("cppumw2h", dimensionless());
    STR_TO_RETURNED_UNIT("$/mwh2h", dimensionless());
    STR_TO_RETURNED_UNIT("$/pumvar^2h", dimensionless());
    STR_TO_RETURNED_UNIT("$/pumvar^2h", dimensionless());
    STR_TO_RETURNED_UNIT("cpmvar2h", dimensionless());
    STR_TO_RETURNED_UNIT("$/pumvar2h", dimensionless());
    STR_TO_RETURNED_UNIT("F", kelvin());
    STR_TO_RETURNED_UNIT("fahrenheit", kelvin());
    STR_TO_RETURNED_UNIT("C", kelvin());
    STR_TO_RETURNED_UNIT("Celsius", kelvin());
    STR_TO_RETURNED_UNIT("K", kelvin());
    STR_TO_RETURNED_UNIT("Kelvin", kelvin());
    #undef STR_TO_RETURNED_UNIT
    else { throw std::runtime_error(std::string("Unable to parse unit: ") + str); }
  }

  unit operator+(unit const& other) const
  {
    if (data == other.data) return other;
    throw std::runtime_error("Cannot add units");
  }

  // TODO: intermediate type with full-range data_t?
  // TODO: 1 * unit, 1 / unit ought to work eventually
  unit operator*(unit const& other) const
  {
    return unit(data + other.data);
  }

  unit operator/(unit const& other) const
  {
    return unit(data - other.data);
  }

  bool operator==(unit const& other) const
  {
    return data == other.data;
  }

  bool operator!=(unit const& other) const
  {
    return !(*this == other);
  }

  // derived units
  // TODO: we should produce these automatically from modelica type things

  // Base units:
  static unit dimensionless() { return unit(data_t(0, 0, 0, 0, 0, 0, 0)); }

  static unit meter()     { return unit(data_t(1, 0, 0, 0, 0, 0, 0)); }
  static unit kilogram()  { return unit(data_t(0, 1, 0, 0, 0, 0, 0)); }
  static unit second()    { return unit(data_t(0, 0, 1, 0, 0, 0, 0)); }
  static unit ampere()    { return unit(data_t(0, 0, 0, 1, 0, 0, 0)); }
  static unit kelvin()    { return unit(data_t(0, 0, 0, 0, 1, 0, 0)); }
  static unit mole()      { return unit(data_t(0, 0, 0, 0, 0, 1, 0)); }
  static unit candela()   { return unit(data_t(0, 0, 0, 0, 0, 0, 1)); }

  // Derived units:
  static unit hertz() { return unit(data_t(0, 0, -1, 0, 0, 0, 0)); }
  static unit volt() { return unit(data_t(2, 1, -3, -1, 0, 0, 0)); }
  static unit newton() { return unit(data_t(1, 1, -2, 0, 0, 0, 0)); }
  static unit pascal() { return unit(data_t(-1, 1, -2, 0, 0, 0, 0)); }
  static unit joule() { return unit(data_t(2, 1, -2, 0, 0, 0, 0)); }
  static unit watt() { return unit(data_t(2, 1, -3, 0, 0, 0, 0)); }
  static unit coulomb() { return unit(data_t(0, 0, 1, 1, 0, 0, 0)); }
  static unit farad() { return unit(data_t(-2, -1, 4, 2, 0, 0, 0)); }
  static unit ohm() { return unit(data_t(2, 1, -3, -2, 0, 0, 0)); }
  static unit siemens() { return unit(data_t(-2, -1, 3, 2, 0, 0, 0)); }
  static unit weber() { return unit(data_t(2, 1, -2, -1, 0, 0, 0)); }
  static unit tesla() { return unit(data_t(0, 1, -2, -1, 0, 0, 0)); }
  static unit henry() { return unit(data_t(2, 1, -2, -2, 0, 0, 0)); }
  static unit celsius() { return unit(data_t(0, 0, 0, 0, 1, 0, 0)); }
  static unit lumen() { return unit(data_t(0, 0, 0, 0, 0, 0, 1)); }
  static unit lux() { return unit(data_t(-2, 0, 0, 0, 0, 0, 1)); }
  static unit becquerel() { return unit(data_t(0, 0, -1, 0, 0, 0, 0)); }
  static unit gray() { return unit(data_t(2, 0, -2, 0, 0, 0, 0)); }
  static unit sievert() { return unit(data_t(2, 0, -2, 0, 0, 0, 0)); }
  static unit katal() { return unit(data_t(0, 0, -1, 0, 0, 1, 0)); }


  double apply_transform(double val) const { return transformer.apply(val); }
  double unapply_transform(double val) const { return transformer.unapply(val); }

private:
  struct data_t
  {
    data_t()
    {
      // default member initialization is only in c++20 for bitfields
      meter = 0;
      kilogram = 0;
      second = 0;
      ampere = 0;
      kelvin = 0;
      mole = 0;
      candela = 0;
    }

    data_t(int meters, int kilograms, int seconds, int amperes, int kelvins, int moles, int candelas)
    {
      #define ASSIGN(var, value) var = checked_assign(value, -8, 7)
      ASSIGN(meter, meters);
      ASSIGN(kilogram, kilograms);
      ASSIGN(second, seconds);
      ASSIGN(ampere, amperes);
      ASSIGN(kelvin, kelvins);
      ASSIGN(mole, moles);
      ASSIGN(candela, candelas);
      #undef ASSIGN
    }

    int meter: 4;
    int kilogram: 4;
    int second: 4;
    int ampere: 4;
    int kelvin: 4;
    int mole: 4;
    int candela: 4;

    data_t operator+(data_t const& other) const
    {
      data_t ret;
      #define ASSIGN(type) ret.type = checked_add(type, other.type, -8, 7)
      ASSIGN(meter);
      ASSIGN(kilogram);
      ASSIGN(second);
      ASSIGN(ampere);
      ASSIGN(kelvin);
      ASSIGN(mole);
      ASSIGN(candela);
      #undef ASSIGN
      return ret;
    }

    data_t operator-(data_t const& other) const
    {
      data_t ret;
      #define ASSIGN(type) ret.type = checked_add(type, -other.type, -8, 7)
      ASSIGN(meter);
      ASSIGN(kilogram);
      ASSIGN(second);
      ASSIGN(ampere);
      ASSIGN(kelvin);
      ASSIGN(mole);
      ASSIGN(candela);
      #undef ASSIGN
      return ret;
    }

    bool operator==(data_t const& other) const
    {
      #define COMPARE(type) (type == other.type)
      return COMPARE(meter) &&
             COMPARE(kilogram) &&
             COMPARE(second) &&
             COMPARE(ampere) &&
             COMPARE(kelvin) &&
             COMPARE(mole) &&
             COMPARE(candela);
      #undef COMPARE
    }

  };

  template<typename T>
  struct affine_transform {
    T scale = 1.0;
    T offset = 0.0;

    T apply(T value) const { return value * scale + offset; }
    T unapply(T result) const { return (result - offset) / scale; }
  };

  unit(data_t new_data) : data(new_data) {}

  data_t data;
  affine_transform<double> transformer;
};

inline std::ostream& operator<<(std::ostream& stream, unit const& u)
{
  stream << u.name();
  return stream;
}

// TODO: split the transformer off, add this back
// static_assert(sizeof(unit) == 4, "Unit is too big!");

} // namespace units
} // namespace griddyn

// current interface
namespace gridUnits {

using units_t = griddyn::units::unit;

static units_t defUnit = units_t::dimensionless();
static units_t deg = units_t::dimensionless();
static units_t puMW = units_t::dimensionless();
static units_t MVAR = units_t::dimensionless();
static units_t puHz = units_t::dimensionless();
static units_t rad = units_t::dimensionless();
static units_t puV = units_t::dimensionless();
static units_t puA = units_t::dimensionless();
static units_t rps = units_t::dimensionless();
static units_t MWps = units_t::dimensionless();
static units_t puOhm = units_t::dimensionless();
static units_t puMWps = units_t::dimensionless();

static units_t hour = units_t::second();
static units_t kW = units_t::watt();
static units_t Watt = units_t::watt();
static units_t Ohm = units_t::ohm();
static units_t Amp = units_t::ampere();
static units_t C = units_t::kelvin();
static units_t km = units_t::meter();
static units_t kV = units_t::volt();
static units_t MW = units_t::watt();
static units_t sec = units_t::second();
static units_t Hz = units_t::hertz();


inline std::string to_string (units_t unitType)
{
  return unitType.name();
}

inline units_t getUnits (const std::string &unitString, units_t defValue = units_t::dimensionless())
{
  try
  {
    return units_t::parse(unitString);
  }
  catch (...)
  {
    return defValue;
  }
}

inline double unitConversion (
                       double val,
                       const units_t in,
                       const units_t out,
                       double basePower = 100,
                       double localBaseVoltage = 100)
{
  (void) basePower;
  (void) localBaseVoltage;
  if (in == units_t::dimensionless() || out == units_t::dimensionless()) { throw std::runtime_error("in or out is dimensionless, so probably not something we support yet"); }
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionTime (double val, units_t in, units_t out)
{
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionPower (double val,
                            const units_t in,
                            const units_t out,
                            double basePower = 100,
                            double localBaseVoltage = 100)
{
  (void) basePower;
  (void) localBaseVoltage;
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionDistance (double val, units_t in, units_t out)
{
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionAngle (double val, units_t in, units_t out)
{
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionFreq (double val, units_t in, units_t out, double baseFreq = 60)
{
  (void) baseFreq;
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionCost (double val, units_t in, units_t out, double basePower = 100)
{
  (void) basePower;
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionTemperature (double val, units_t in, units_t out)
{
  return out.apply_transform(in.unapply_transform(val));
}

} // namespace gridUnits
