#include "units_new.hpp"

namespace gridUnits {

double unitConversion (double val,
                       const units_t in,
                       const units_t out,
                       double basePower,
                       double localBaseVoltage)
{
  (void) basePower;
  (void) localBaseVoltage;

  if (in == units_t::error("(nil)") || out == units_t::error("(nil)"))
  {
    std::string error;
    if (in == units_t::error("(nil)"))
    {
      error += std::string("in's originating type: ") + in.name() + " -- ";
    }
    if (out == units_t::error("(nil)"))
    {
      error += std::string("out's originating type: ") + out.name();
    }
    throw std::runtime_error(std::string("in or out is dimensionless: ") + error);
  }

  if (in != out)
  {
    throw std::runtime_error(std::string("in is a different type from out: in: ") + in.name() + " -- out: " + out.name());
  }

  return out.unapply_transform(in.apply_transform(val));
}

} // namespace gridUnits
