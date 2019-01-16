#include "objectFactory.hpp"

#include "staticObjectFactory.hpp"

namespace griddyn
{
// non-static forwarding to coreObjectFactory
void staticObjectFactory::registerFactory (std::string const &name, std::shared_ptr<componentFactory> factory)
{
    return coreObjectFactory::instance ()->registerFactory (name, factory);
}

void staticObjectFactory::registerFactory (std::shared_ptr<componentFactory> factory)
{
    return coreObjectFactory::instance ()->registerFactory (factory);
}

std::string<std::vector> staticObjectFactory::getFactoryNames ()
{
    return coreObjectFactory::instance ()->getFactoryNames ();
}

std::string<std::vector> staticObjectFactory::getTypeNames (std::string const &component)
{
    return coreObjectFactory::instance ()->getTypeNames (component);
}

coreObject *staticObjectFactory::createObject (std::string const &component)
{
    return coreObjectFactory::instance ()->createObject (component);
}

coreObject *staticObjectFactory::createObject (std::string const &component, std::string const &typeName)
{
    return coreObjectFactory::instance ()->createObject (component, typeName);
}

coreObject *staticObjectFactory::createObject (std::string const &component,
                                               std::string const &typeName,
                                               std::string const &objName)
{
    return coreObjectFactory::instance ()->createObject (component, typeName, objName);
}

std::shared_ptr<componentFactory> staticObjectFactory::getFactory (std::string const &component)
{
    return coreObjectFactory::instance ()->getFactory (component);
}

bool staticObjectFactory::isValidObject (std::string const &component)
{
    return coreObjectFactory::instance ()->isValidObject (component);
}

bool staticObjectFactory::isValidType (std::string const &component, std::string const &typeName)
{
    return coreObjectFactory::instance ()->isValidType (component, typeName);
}

void staticObjectFactory::prepObjects (std::string const &component,
                                       std::string const &typeName,
                                       count_t numObjects,
                                       coreObject *obj)
{
    return coreObjectFactory::instance ()->prepObjects (component, typeName, numObjects, obj);
}

void staticObjectFactory::prepObjects (std::string const &component, count_t numObjects, coreObject *obj){}
{
    return coreObjectFactory::instance ()->prepObjects (component, numObjects, obj);
}

// statically create object from type list
coreObject *staticObjectFactory::createObject (griddyn_component_t component_type)
{
    if (component_type == griddyn_object_simulation)
        return nullptr;
    const std::unordered_map<griddyn_component_t, griddyn_component_subtype_t> defaults = {
      {griddyn_object_agc, griddyn_agc_basic},
      {griddyn_object_area, griddyn_area_basic},
      {griddyn_object_block, griddyn_block_basic},
      {griddyn_object_bus, griddyn_bus_ac},
      {griddyn_object_exciter, griddyn_exciter_type1},
      {griddyn_object_generator, griddyn_generator_dynamic},
      {griddyn_object_genmodel, griddyn_genmodel_fourth},
      {griddyn_object_governor, griddyn_governor_basic},
      {griddyn_object_link, griddyn_link_ac},
      {griddyn_object_load, griddyn_load_zip},
      {griddyn_object_pss, griddyn_pss_basic},
      {griddyn_object_relay, griddyn_relay_basic},
      {griddyn_object_source, griddyn_source_constant}};
    auto f = defaults.find (component_type);
    if (f == defaults.end ())
        return nullptr;
    return createObject (component_type, f->second);
}

coreObject *
staticObjectFactory::createObject (griddyn_component_t component_type, griddyn_component_subtype_t type)
{
    if (!is_valid_subtype (component_type, type))
        return nullptr;

    // TODO construct these directly
    // component -> string mapping
    const std::unordered_map<griddyn_component_t, const char *> component_type_map =
    { {griddyn_object_agc, "agc"},
      {griddyn_object_area, "area"},
      {griddyn_object_block, "block"},
      {griddyn_object_bus, "bus"},
      {griddyn_object_exciter, "exciter"},
      {griddyn_object_generator, "generator"},
      {griddyn_object_genmodel, "genmodel"},
      {griddyn_object_governor, "governor"},
      {griddyn_object_link, "link"},
      {griddyn_object_load, "load"},
      {griddyn_object_pss, "pss"},
      {griddyn_object_relay, "relay"},
      {griddyn_object_source, "source"} };
    // subcomponent -> string mapping
    const std::unordered_map<griddyn_component_subtype_t, const char *>
      component_subtype_map = {
          // agc options
          {griddyn_agc_basic, "agc"},
          {griddyn_agc_default, "default"},

          // area options
          {griddyn_area_basic, "basic"},
          {griddyn_area_simple, "simple"},

          // block options
          {griddyn_block_basic, "basic"},
          {griddyn_block_gain, "gain"},
          {griddyn_block_control, "control"},
          {griddyn_block_deadband, "deadband"},
          {griddyn_block_delay, "delay"},
          {griddyn_block_filter, "filter"},
          {griddyn_block_pid, "pid"},
          {griddyn_block_integral, "integral"},
          {griddyn_block_function, "function"},
          {griddyn_block_lut, "lut"},
          {griddyn_block_lookuptable, "lookuptable"},
          {griddyn_block_derivative, "derivative"},
          {griddyn_block_filtered_derivative, "filtered_derivative"},

          // bus options
          {griddyn_bus_basic, "basic"},
          {griddyn_bus_ac, "ac"},
          {griddyn_bus_pq, "pq"},
          {griddyn_bus_pv, "pv"},
          {griddyn_bus_slk, "slk"},
          {griddyn_bus_slack, "slack"},
          {griddyn_bus_afix, "afix"},
          {griddyn_bus_ref, "ref"},
          {griddyn_bus_dc, "dc"},
          {griddyn_bus_hvdc, "hvdc"},
          {griddyn_bus_inf, "inf"},
          {griddyn_bus_infinite, "infinite"},
          {griddyn_bus_psystem, "psystem"},

          // exciter options
          {griddyn_exciter_dc1a, "dc1a"},
          {griddyn_exciter_dc2a, "dc2a"},
          {griddyn_exciter_type1, "type1"},
          {griddyn_exciter_type2, "type2"},
          {griddyn_exciter_basic, "basic"},
          {griddyn_exciter_fast, "fast"},

          // generator options
          {griddyn_generator_basic, "basic"},
          {griddyn_generator_simple, "simple"},
          {griddyn_generator_pflow, "pflow"},
          {griddyn_generator_dynamic, "dynamic"},
          {griddyn_generator_spinning, "spinning"},
          {griddyn_generator_variable, "variable"},
          {griddyn_generator_renewable, "renewable"},
          {griddyn_generator_local_dynamic, "local_dynamic"},

          // generation model options
          {griddyn_genmodel_trivial, "trivial"},
          {griddyn_genmodel_inverter, "inverter"},
          {griddyn_genmodel_basic, "basic"},
          {griddyn_genmodel_second, "second"},
          {griddyn_genmodel_classical, "classical"},
          {griddyn_genmodel_third, "third"},
          {griddyn_genmodel_fourth, "fourth"},
          {griddyn_genmodel_grdc, "grdc"},
          {griddyn_genmodel_fifth, "fifth"},
          {griddyn_genmodel_fifthtype2, "fifthtype2"},
          {griddyn_genmodel_fifthtype3, "fifthtype3"},
          {griddyn_genmodel_six, "six"},
          {griddyn_genmodel_sixtype2, "sixtype2"},
          {griddyn_genmodel_sixtype3, "sixtype3"},
          {griddyn_genmodel_genrou, "genrou"},
          {griddyn_genmodel_eight, "eight"},

          // governor options
          {griddyn_governor_simple, "simple"},
          {griddyn_governor_fast, "fast"},
          {griddyn_governor_basic, "basic"},
          {griddyn_governor_ieeesimple, "ieeesimple"},
          {griddyn_governor_reheat, "reheat"},
          {griddyn_governor_ieeehydro, "ieeehydro"},
          {griddyn_governor_hydro, "hydro"},
          {griddyn_governor_ieeesteamnr, "ieeesteamnr"},
          {griddyn_governor_steamnr, "steamnr"},
          {griddyn_governor_ieeesteamtcsr, "ieeesteamtcsr"},
          {griddyn_governor_steamtcsr, "steamtcsr"},
          {griddyn_governor_tgov1, "tgov1"},

          // link options
          {griddyn_link_trivial, "trivial"},
          {griddyn_link_basic, "basic"},
          {griddyn_link_transport, "transport"},
          {griddyn_link_ac, "ac"},
          {griddyn_link_line, "line"},
          {griddyn_link_phase_shifter, "phase_shifter"},
          {griddyn_link_transformer, "transformer"},
          {griddyn_link_adjustable, "adjustable"},
          {griddyn_link_dc, "dc"},
          {griddyn_link_dclink, "dclink"},
          {griddyn_link_dcline, "dcline"},
          {griddyn_link_rectifier, "rectifier"},
          {griddyn_link_inverter, "inverter"},
          {griddyn_link_acdc, "acdc"},
          {griddyn_link_acdcconverter, "acdcconverter"},
          {griddyn_link_dcconverter, "dcconverter"},
          {griddyn_link_hvdc, "hvdc"},
          {griddyn_link_tie, "tie"},
          {griddyn_link_subsystem, "subsystem"},
          {griddyn_link_simple, "simple"},
          {griddyn_link_zbreaker, "zbreaker"},
          {griddyn_link_zline, "zline"},
          {griddyn_link_busbreaker, "busbreaker"},

          // load options
          {griddyn_load_simple, "simple"},
          {griddyn_load_constant, "constant"},
          {griddyn_load_basic, "basic"},
          {griddyn_load_zip, "zip"},
          {griddyn_load_pulse, "pulse"},
          {griddyn_load_sinusoidal, "sinusoidal"},
          {griddyn_load_ramp, "ramp"},
          {griddyn_load_random, "random"},
          {griddyn_load_file, "file"},
          {griddyn_load_source, "source"},
          {griddyn_load_exponential, "exponential"},
          {griddyn_load_fdep, "fdep"},
          {griddyn_load_threephase, "threephase"},
          {griddyn_load_approximating, "approximating"},
          {griddyn_load_motor, "motor"},
          {griddyn_load_motor3, "motor3"},
          {griddyn_load_motor5, "motor5"},
          {griddyn_load_composite, "composite"},
          {griddyn_load_cluster, "cluster"},
          {griddyn_load_group, "group"},
          {griddyn_load_svd, "svd"},
          {griddyn_load_switched_shunt, "switched_shunt"},
          {griddyn_load_ssd, "ssd"},

          // pss options
          {griddyn_pss_basic, "basic"},

          // relay options
          {griddyn_relay_basic, "basic"},
          {griddyn_relay_sensor, "sensor"},
          {griddyn_relay_zonal, "zonal"},
          {griddyn_relay_impedance, "impedance"},
          {griddyn_relay_distance, "distance"},
          {griddyn_relay_differential, "differential"},
          {griddyn_relay_bus, "bus"},
          {griddyn_relay_load, "load"},
          {griddyn_relay_fuse, "fuse"},
          {griddyn_relay_breaker, "breaker"},
          {griddyn_relay_pmu, "pmu"},
          {griddyn_relay_phasor, "phasor"},
          {griddyn_relay_synchrophasor, "synchrophasor"},
          {griddyn_relay_control, "control"},
          {griddyn_relay_thermaltx, "thermaltx"},
          {griddyn_relay_txage, "txage"},

          // no simulation options

          // source options
          {griddyn_source_basic, "basic"},
          {griddyn_source_constant, "constant"},
          {griddyn_source_simple, "simple"},
          {griddyn_source_pulse, "pulse"},
          {griddyn_source_sine, "sine"},
          {griddyn_source_ramp, "ramp"},
          {griddyn_source_random, "random"},
          {griddyn_source_file, "file"},
          {griddyn_source_grabber, "grabber"},
          {griddyn_source_data, "data"}
      };

    auto component_type_str = component_type_map.find(component_type);
    auto subtype_type_str = component_subtype_map.find(type);
    if (component_type_str == component_type_map.end() || subtype_type_str == component_subtype_map.end())
    {
        return nullptr;
    }
    return createObject(component_type_str->second, subtype_type_str->second);
}

bool staticObjectFactory::is_valid_subtype (griddyn_component_t component_type, griddyn_component_subtype_t type)
{
    const std::unordered_map<griddyn_component_t, std::unordered_set<griddyn_component_subtype_t>> valid = {
      {griddyn_object_agc, {griddyn_agc_basic, griddyn_agc_default}},
      {griddyn_object_area, {griddyn_area_basic, griddyn_area_simple}},
      {griddyn_object_block,
       {griddyn_block_basic, griddyn_block_gain, griddyn_block_control, griddyn_block_deadband,
        griddyn_block_delay, griddyn_block_filter, griddyn_block_pid, griddyn_block_integral,
        griddyn_block_function, griddyn_block_lut, griddyn_block_lookuptable, griddyn_block_derivative,
        griddyn_block_filtered_derivative}},
      {griddyn_object_bus,
       {griddyn_bus_basic, griddyn_bus_ac, griddyn_bus_pq, griddyn_bus_pv, griddyn_bus_slk, griddyn_bus_slack,
        griddyn_bus_afix, griddyn_bus_ref, griddyn_bus_dc, griddyn_bus_hvdc, griddyn_bus_inf, griddyn_bus_infinite,
        griddyn_bus_psystem}},
      {griddyn_object_exciter,
       {griddyn_exciter_dc1a, griddyn_exciter_dc2a, griddyn_exciter_type1, griddyn_exciter_type2,
        griddyn_exciter_basic, griddyn_exciter_fast}},
      {griddyn_object_generator,
       {griddyn_generator_basic, griddyn_generator_simple, griddyn_generator_pflow, griddyn_generator_dynamic,
        griddyn_generator_spinning, griddyn_generator_variable, griddyn_generator_renewable,
        griddyn_generator_local_dynamic}},
      {griddyn_object_genmodel,
       {griddyn_genmodel_trivial, griddyn_genmodel_inverter, griddyn_genmodel_basic, griddyn_genmodel_second,
        griddyn_genmodel_classical, griddyn_genmodel_third, griddyn_genmodel_fourth, griddyn_genmodel_grdc,
        griddyn_genmodel_fifth, griddyn_genmodel_fifthtype2, griddyn_genmodel_fifthtype3, griddyn_genmodel_six,
        griddyn_genmodel_sixtype2, griddyn_genmodel_sixtype3, griddyn_genmodel_genrou, griddyn_genmodel_eight}},
      {griddyn_object_governor,
       {griddyn_governor_simple, griddyn_governor_fast, griddyn_governor_basic, griddyn_governor_ieeesimple,
        griddyn_governor_reheat, griddyn_governor_ieeehydro, griddyn_governor_hydro, griddyn_governor_ieeesteamnr,
        griddyn_governor_steamnr, griddyn_governor_ieeesteamtcsr, griddyn_governor_steamtcsr,
        griddyn_governor_tgov1}},
      {griddyn_object_link,
       {griddyn_link_trivial,  griddyn_link_basic,         griddyn_link_transport,     griddyn_link_ac,
        griddyn_link_line,     griddyn_link_phase_shifter, griddyn_link_transformer,   griddyn_link_adjustable,
        griddyn_link_dc,       griddyn_link_dclink,        griddyn_link_dcline,        griddyn_link_rectifier,
        griddyn_link_inverter, griddyn_link_acdc,          griddyn_link_acdcconverter, griddyn_link_dcconverter,
        griddyn_link_hvdc,     griddyn_link_tie,           griddyn_link_subsystem,     griddyn_link_simple,
        griddyn_link_zbreaker, griddyn_link_zline,         griddyn_link_busbreaker}},
      {griddyn_object_load,
       {griddyn_load_simple,     griddyn_load_constant,       griddyn_load_basic,       griddyn_load_zip,
        griddyn_load_pulse,      griddyn_load_sinusoidal,     griddyn_load_ramp,        griddyn_load_random,
        griddyn_load_file,       griddyn_load_source,         griddyn_load_exponential, griddyn_load_fdep,
        griddyn_load_threephase, griddyn_load_approximating,  griddyn_load_motor,       griddyn_load_motor3,
        griddyn_load_motor5,     griddyn_load_composite,      griddyn_load_cluster,     griddyn_load_group,
        griddyn_load_svd,        griddyn_load_switched_shunt, griddyn_load_ssd}},
      {griddyn_object_pss, {griddyn_pss_basic}},
      {griddyn_object_relay,
       {griddyn_relay_basic, griddyn_relay_sensor, griddyn_relay_zonal, griddyn_relay_impedance,
        griddyn_relay_distance, griddyn_relay_differential, griddyn_relay_bus, griddyn_relay_load,
        griddyn_relay_fuse, griddyn_relay_breaker, griddyn_relay_pmu, griddyn_relay_phasor,
        griddyn_relay_synchrophasor, griddyn_relay_control, griddyn_relay_thermaltx, griddyn_relay_txage}},
      {griddyn_object_source,
       {griddyn_source_basic, griddyn_source_constant, griddyn_source_simple, griddyn_source_pulse,
        griddyn_source_sine, griddyn_source_ramp, griddyn_source_random, griddyn_source_file,
        griddyn_source_grabber, griddyn_source_data}}};

    if (component_type == griddyn_object_simulation)
        return false;
    auto f = valid.find (component_type);
    if (f == valid.end ())
        return false;

    return f->find (type) != f->end ();
}

}  // namespace griddyn
