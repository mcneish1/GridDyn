#1 tests are passing
#2 tests fail with status 201
#3 tests fail with status 1
#4 tests time out

library_tests = [
    #1 'testString_viewConversion',
    #1 'testOperatingBounds',
    #1 'testXML',
    #1 'testElementReaders',
    #1 'testGridDynRunner',
    #1 'testCore',
    #1 'testMatrixData',
    #1 'simpleQueueTests',
    #1 'testReaderInfo',
    #1 'testZipUtilities',
    #1 'testJsonReader',
    #1 'testStringConversion',
    #1 'testString_viewOps',
    #1 'testGridParameter',
    #1 'testStringOps',
    #1 'testValuePredictors',
    #1 'testWorkQueue',
]

system_tests = [
    #1 'testRoots',
    #3 'testGridDynRunner',
    #4 'testInputs',
    #1 'testContingency',
    #2 'testDyn1',
    #4 'validationTests',
    #4 'testDyn2',
    #1 'testCloning',
    #4 'faultTests',
    #1 'testConstraints',
    #1 'testOutputs',
    #1 'testpFlow',
]

component_tests = [
    #1 'testGenModels',
    #3 'testBlocks',
    #1 'ConditionTests',
    #1 'testArea',
    #3 'testdcLinks',
    #3 'testExtraModels',
    #1 'testRecorders',
    #1 'testGridLab',
    #1 'testGovernors',
    #3 'testGenerators',
    #1 'testLoads',
    #3 'testRelays',
    #1 'testLinks',
    #1 'testSource',
    #1 'testEvents',
    #1 'testExciters',
    #1 'testAdjustableTX',
]

extra_tests = [
    #4 'performanceTests',
    #1 'objectSizeReport',
    #1 'largeInputTests',
    #4 'largeContingencyTests',
]

shared_library_tests = [
    #1 'basic_tests',
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
                '-DGRIDDYN_TEST_DIRECTORY="' + meson.current_source_dir() + '/test_files"',
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
                minizip_dep,
            ])
        test('test_' + folder_name + '_' + test_name, test_exe)
    endforeach
endforeach
