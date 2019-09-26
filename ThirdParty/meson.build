# TODO: use internal zlib
zlib_dep = dependency('zlib')

minizip_lib = library('minizip', [
        'Minizip/minizip.c',
        'Minizip/miniunz.c',
        'Minizip/zip.c',
        'Minizip/ioapi.c',
        'Minizip/unzip.c',
        'Minizip/argv.c',
    ],
    dependencies : zlib_dep)

minizip_inc = include_directories('Minizip')
minizip_dep = declare_dependency(link_with : minizip_lib, include_directories : minizip_inc)

jsoncpp_inc = include_directories('json')
jsoncpp_lib = library('jsoncpp',
    'json/jsoncpp.cpp')
jsoncpp_dep = declare_dependency(link_with : jsoncpp_lib, include_directories : jsoncpp_inc)

toml_inc = include_directories('toml')
toml_dep = declare_dependency(include_directories : toml_inc)

ticpp_inc = include_directories('ticpp')
ticpp_lib = library('ticpp', [
        'ticpp/ticpp.cpp',
        'ticpp/tinystr.cpp',
        'ticpp/tinyxml.cpp',
        'ticpp/tinyxmlerror.cpp',
        'ticpp/tinyxmlparser.cpp',
    ],
    cpp_args : '-DTIXML_USE_TICPP',
    include_directories : include_directories(['ticpp/ticpp', 'ticpp']))
ticpp_dep = declare_dependency(link_with : ticpp_lib, include_directories : ticpp_inc)

inih_inc = include_directories('inih')
inih_dep = declare_dependency(include_directories : inih_inc)

tinyxml2_inc = include_directories('tinyxml2')
tinyxml2_lib = library('tinyxml2',
    'tinyxml2/tinyxml2.cpp',
    include_directories : include_directories('tinyxml2/tinyxml2'))
tinyxml2_dep = declare_dependency(link_with : tinyxml2_lib, include_directories : tinyxml2_inc)

cereal_inc = include_directories('cereal')
cereal_dep = declare_dependency(include_directories : cereal_inc)

yaml_cpp_inc = include_directories('yaml-cpp/include')
yaml_cpp_lib = library('yaml_cpp', [
        'yaml-cpp/src/binary.cpp',
        'yaml-cpp/src/convert.cpp',
        'yaml-cpp/src/directives.cpp',
        'yaml-cpp/src/emit.cpp',
        'yaml-cpp/src/emitfromevents.cpp',
        'yaml-cpp/src/emitter.cpp',
        'yaml-cpp/src/emitterstate.cpp',
        'yaml-cpp/src/emitterutils.cpp',
        'yaml-cpp/src/exceptions.cpp',
        'yaml-cpp/src/exp.cpp',
        'yaml-cpp/src/memory.cpp',
        'yaml-cpp/src/nodebuilder.cpp',
        'yaml-cpp/src/node.cpp',
        'yaml-cpp/src/node_data.cpp',
        'yaml-cpp/src/nodeevents.cpp',
        'yaml-cpp/src/null.cpp',
        'yaml-cpp/src/ostream_wrapper.cpp',
        'yaml-cpp/src/parse.cpp',
        'yaml-cpp/src/parser.cpp',
        'yaml-cpp/src/regex_yaml.cpp',
        'yaml-cpp/src/scanner.cpp',
        'yaml-cpp/src/scanscalar.cpp',
        'yaml-cpp/src/scantag.cpp',
        'yaml-cpp/src/scantoken.cpp',
        'yaml-cpp/src/simplekey.cpp',
        'yaml-cpp/src/singledocparser.cpp',
        'yaml-cpp/src/stream.cpp',
        'yaml-cpp/src/tag.cpp',
    ],
    include_directories : yaml_cpp_inc)
yaml_cpp_dep = declare_dependency(link_with : yaml_cpp_lib, include_directories : yaml_cpp_inc)

zmq_dep = dependency('libzmq')
cppzmq_inc = include_directories('cppzmq')
cppzmq_dep = declare_dependency(dependencies : zmq_dep, include_directories : cppzmq_inc)

subdir('sundials/sundials') # put sundials_config in the right place

cc = meson.get_compiler('c')
m_lib = cc.find_library('m')

sundials_inc = include_directories('sundials/sundials-3.1.1/include')
sundials_config_file_inc = include_directories('sundials')
sundials_lib = library('sundials', [
        sundials_config_file,
        'sundials/sundials-3.1.1/src/kinsol/kinsol_io.c',
        'sundials/sundials-3.1.1/src/kinsol/kinsol.c',
        'sundials/sundials-3.1.1/src/nvec_ser/nvector_serial.c',
        'sundials/sundials-3.1.1/src/sundials/sundials_math.c',
        'sundials/sundials-3.1.1/src/sundials/sundials_nvector.c',
        'sundials/sundials-3.1.1/src/kinsol/kinsol_direct.c',
        'sundials/sundials-3.1.1/src/sundials/sundials_linearsolver.c',
        'sundials/sundials-3.1.1/src/sundials/sundials_matrix.c',
        'sundials/sundials-3.1.1/src/sunmat_dense/sunmatrix_dense.c',
        'sundials/sundials-3.1.1/src/sunmat_band/sunmatrix_band.c',
        'sundials/sundials-3.1.1/src/arkode/arkode.c',
        'sundials/sundials-3.1.1/src/arkode/arkode_io.c',
        'sundials/sundials-3.1.1/src/arkode/arkode_butcher.c',
        'sundials/sundials-3.1.1/src/arkode/arkode_direct.c',
        'sundials/sundials-3.1.1/src/ida/ida.c',
        'sundials/sundials-3.1.1/src/ida/ida_io.c',
        'sundials/sundials-3.1.1/src/ida/ida_direct.c',
        'sundials/sundials-3.1.1/src/ida/ida_ic.c',
        'sundials/sundials-3.1.1/src/sunlinsol_dense/sunlinsol_dense.c',
        'sundials/sundials-3.1.1/src/sundials/sundials_dense.c',
        'sundials/sundials-3.1.1/src/cvode/cvode.c',
        'sundials/sundials-3.1.1/src/cvode/cvode_direct.c',
        'sundials/sundials-3.1.1/src/cvode/cvode_io.c',
    ],
    dependencies : m_lib,
    include_directories : [ sundials_inc, sundials_config_file_inc ])

sundials_dep = declare_dependency(link_with : sundials_lib, include_directories : [ sundials_inc, sundials_config_file_inc ])
