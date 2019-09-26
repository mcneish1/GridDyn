gsl = find_program('gsl')

gsl_script = files('property.gsl')

gsl_generator = generator(
    gsl,
    arguments : ['-q', '-a', '-script:@CURRENT_SOURCE_DIR@/property.gsl', '@INPUT@', '@OUTPUT@'],
    output : '@BASENAME@.cpp')

griddyn_gen = gsl_generator.process('griddyn.xml')
griddyn_core_gen = gsl_generator.process('griddyn_core.xml')
griddyn_optimization_gen = gsl_generator.process('griddyn_optimization.xml')
