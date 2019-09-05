#- commented out tests are passing
library_tests = [
    #- 'testString_viewConversion',
    #- 'testOperatingBounds',
    'testXML',
    #- 'testElementReaders',
    #- 'testGridDynRunner',
    #- 'testCore',
    #- 'testMatrixData',
    #- 'simpleQueueTests',
    'testReaderInfo',
    #- 'testZipUtilities',
    'testJsonReader',
    #- 'testStringConversion',
    #- 'testString_viewOps',
    #- 'testGridParameter',
    #- 'testStringOps',
    #- 'testValuePredictors',
    #- 'testWorkQueue',
]

system_tests = [
    'testRoots',
    'testSolverModes',
    'testSystem',
    'testGridDynRunner',
    'testInputs',
    'testContingency',
    'testDyn1',
    #- 'validationTests',
    'testDyn2',
    'testCloning',
    'faultTests',
    'testConstraints',
    'testOutputs',
    'testpFlow',
]

component_tests = [
    'testGenModels',
    'testBlocks',
    #- 'ConditionTests',
    'testArea',
    'testdcLinks',
    'testExtraModels',
    'testRecorders',
    'testGridLab',
    'testGovernors',
    'testGenerators',
    'testLoads',
    'testRelays',
    'testLinks',
    #- 'testSource',
    #- 'testEvents',
    'testExciters',
    'testAdjustableTX',
]

extra_tests = [
    'performanceTests',
    #- 'objectSizeReport',
    #- 'largeInputTests',
    'largeContingencyTests',
    'testExtra',
]

shared_library_tests = [
    'shared_library_tests',
    'basic_tests', # completely broken ://
]

folder_map = [
    [library_tests, 'libraryTests'],
    [system_tests, 'systemTests'],
    [component_tests, 'componentTests'],
    [extra_tests, 'extraTests'],
    [shared_library_tests, 'testSharedLibrary'],
]

foreach test_name_folder_name : folder_map
    test_names = test_name_folder_name[0]
    folder_name = test_name_folder_name[1]

    foreach test_name : test_names
        test_exe = executable(folder_name + '_' + test_name, [
                folder_name + '/' + test_name + '.cpp',
                'testHelperFunctions.cpp'
            ],
            cpp_args : [
                '-DBOOST_TEST_DYN_LINK',
                '-DBOOST_TEST_MODULE=griddyn_tests',
                '-DGRIDDYN_TEST_DIRECTORY="' + meson.current_source_dir() + '/test_files/"',
            ],
            include_directories : griddyn_config_inc,
            dependencies : [
                griddyn_utility_dep,
                griddyn_core_dep,
                griddyn_runner_dep,
                griddyn_dep,
                griddyn_file_input_dep,
                griddyn_format_interpreter_dep,
                boost_dep,
                griddyn_loader_dep,
                griddyn_coupling_dep,
                griddyn_shared_dep,
                cereal_dep,
            ])
        test('test_' + folder_name + '_' + test_name, test_exe)
    endforeach
endforeach
