#ifndef GRIDDYN_OBJECTLIST_C_H
#define GRIDDYN_OBJECTLIST_C_H

// TODO we want to prevent this from being used directly, but also allow it to be used internally

typedef enum
{
    griddyn_object_agc,
    griddyn_object_area,
    griddyn_object_block,
    griddyn_object_bus,
    griddyn_object_exciter,
    griddyn_object_generator,
    griddyn_object_genmodel,
    griddyn_object_governor,
    griddyn_object_link,
    griddyn_object_load,
    griddyn_object_pss,
    griddyn_object_relay,
    griddyn_object_simulation,  // cannot create simulations, attempts will fail
    griddyn_object_source
} griddyn_component_t;

typedef enum
{
    // agc options
    griddyn_agc_basic,
    griddyn_agc_default,

    // area options
    griddyn_area_basic,
    griddyn_area_simple,

    // block options
    griddyn_block_basic,
    griddyn_block_gain,
    griddyn_block_control,
    griddyn_block_deadband,
    griddyn_block_delay,
    griddyn_block_filter,
    griddyn_block_pid,
    griddyn_block_integral,
    griddyn_block_function,
    griddyn_block_lut,
    griddyn_block_lookuptable,
    griddyn_block_derivative,
    griddyn_block_filtered_derivative,

    // bus options
    griddyn_bus_basic,
    griddyn_bus_ac,
    griddyn_bus_pq,
    griddyn_bus_pv,
    griddyn_bus_slk,
    griddyn_bus_slack,
    griddyn_bus_afix,
    griddyn_bus_ref,
    griddyn_bus_dc,
    griddyn_bus_hvdc,
    griddyn_bus_inf,
    griddyn_bus_infinite,
    griddyn_bus_psystem,

    // exciter options
    griddyn_exciter_dc1a,
    griddyn_exciter_dc2a,
    griddyn_exciter_type1,
    griddyn_exciter_type2,
    griddyn_exciter_basic,
    griddyn_exciter_fast,

    // generator options
    griddyn_generator_basic,
    griddyn_generator_simple,
    griddyn_generator_pflow,
    griddyn_generator_dynamic,
    griddyn_generator_spinning,
    griddyn_generator_variable,
    griddyn_generator_renewable,
    griddyn_generator_local_dynamic,

    // generation model options
    griddyn_genmodel_trivial,
    griddyn_genmodel_inverter,
    griddyn_genmodel_basic,
    griddyn_genmodel_second,
    griddyn_genmodel_classical,
    griddyn_genmodel_third,
    griddyn_genmodel_fourth,
    griddyn_genmodel_grdc,
    griddyn_genmodel_fifth,
    griddyn_genmodel_fifthtype2,
    griddyn_genmodel_fifthtype3,
    griddyn_genmodel_six,
    griddyn_genmodel_sixtype2,
    griddyn_genmodel_sixtype3,
    griddyn_genmodel_genrou,
    griddyn_genmodel_eight,

    // governor options
    griddyn_governor_simple,
    griddyn_governor_fast,
    griddyn_governor_basic,
    griddyn_governor_ieeesimple,
    griddyn_governor_reheat,
    griddyn_governor_ieeehydro,
    griddyn_governor_hydro,
    griddyn_governor_ieeesteamnr,
    griddyn_governor_steamnr,
    griddyn_governor_ieeesteamtcsr,
    griddyn_governor_steamtcsr,
    griddyn_governor_tgov1,

    // link options
    griddyn_link_trivial,
    griddyn_link_basic,
    griddyn_link_transport,
    griddyn_link_ac,
    griddyn_link_line,
    griddyn_link_phase_shifter,
    griddyn_link_transformer,
    griddyn_link_adjustable,
    griddyn_link_dc,
    griddyn_link_dclink,
    griddyn_link_dcline,
    griddyn_link_rectifier,
    griddyn_link_inverter,
    griddyn_link_acdc,
    griddyn_link_acdcconverter,
    griddyn_link_dcconverter,
    griddyn_link_hvdc,
    griddyn_link_tie,
    griddyn_link_subsystem,
    griddyn_link_simple,
    griddyn_link_zbreaker,
    griddyn_link_zline,
    griddyn_link_busbreaker,

    // load options
    griddyn_load_simple,
    griddyn_load_constant,
    griddyn_load_basic,
    griddyn_load_zip,
    griddyn_load_pulse,
    griddyn_load_sinusoidal,
    griddyn_load_ramp,
    griddyn_load_random,
    griddyn_load_file,
    griddyn_load_source,
    griddyn_load_exponential,
    griddyn_load_fdep,
    griddyn_load_threephase,
    griddyn_load_approximating,
    griddyn_load_motor,
    griddyn_load_motor3,
    griddyn_load_motor5,
    griddyn_load_composite,
    griddyn_load_cluster,
    griddyn_load_group,
    griddyn_load_svd,
    griddyn_load_switched_shunt,
    griddyn_load_ssd,

    // pss options
    griddyn_pss_basic,

    // relay options
    griddyn_relay_basic,
    griddyn_relay_sensor,
    griddyn_relay_zonal,
    griddyn_relay_impedance,
    griddyn_relay_distance,
    griddyn_relay_differential,
    griddyn_relay_bus,
    griddyn_relay_load,
    griddyn_relay_fuse,
    griddyn_relay_breaker,
    griddyn_relay_pmu,
    griddyn_relay_phasor,
    griddyn_relay_synchrophasor,
    griddyn_relay_control,
    griddyn_relay_thermaltx,
    griddyn_relay_txage,

    // no simulation options

    // source options
    griddyn_source_basic,
    griddyn_source_constant,
    griddyn_source_simple,
    griddyn_source_pulse,
    griddyn_source_sine,
    griddyn_source_ramp,
    griddyn_source_random,
    griddyn_source_file,
    griddyn_source_grabber,
    griddyn_source_data
} griddyn_component_subtype_t;

#endif
