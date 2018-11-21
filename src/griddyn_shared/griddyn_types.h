
typedef enum {
    agc,
    block,
    exciter,
    generator,
    genmodel,
    governor,
    link,
    load,
    bus,
    area,
    simulation,
    pss,
    relay
} griddyn_component;

typedef enum {
    basic,          // multiple sources
    fast,
    simple,         // multiple sources
    agc,            // agc
    gain,           // block
    pflow,          // generator
    local_dynamic,  // generator
    trivial,        // genmodel
    zbreaker,       // link
    zline,
    busbreaker,
    subsystem,
    hvdc,
    tie,
    rectifier,
    rect,
    inverter,
    inv,
    transport,      // link
    constant,       // load
    gridlabd,
    gridlab,
    motor,
    motor1,
    motor3,
    motor5,
    composite,
    cluster,
    group,
    svd,
    switched_shunt,
    pulse,
    sine,
    sin,
    sinusoidal,
    random,
    rand
    ssd,            // load
    dc,             // bus
    hvdc,           // bus
    optimization,   // simulation
    optim,
    GridDyn,
    gridlab,
    gridlabd,       // simulation
    sensor,         // relay
    zonal,
    z,
    impedance,
    distance,
    differential,
    bus,
    load,
    fuse,
    breaker,
    control         // relay
} griddyn_component_subtype;
