#1 tests are passing
#2 tests fail with status 201
#3 tests fail with status 1
#4 tests time out

library_tests = [
    #1 'testString_viewConversion',
    #1 'testOperatingBounds',
    #1 'testXML',
    #1 'testElementReaders',
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
    #2 'testInputs',
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

folder_map = [
    [library_tests, 'libraryTests'],
    [system_tests, 'systemTests'],
    [component_tests, 'componentTests'],
    [extra_tests, 'extraTests'],
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
                '-DGRIDDYN_TEST_DIRECTORY="' + join_paths(meson.current_source_dir(), 'test_files') + '"',
            ],
            include_directories : griddyn_config_inc,
            dependencies : [
                griddyn_utility_dep,
                griddyn_core_dep,
                griddyn_dep,
                griddyn_file_input_dep,
                griddyn_format_interpreter_dep,
                boost_dep,
                cereal_dep,
                minizip_dep,
            ])
        test('test_' + folder_name + '_' + test_name, test_exe)
    endforeach
endforeach

gtest_dep = subproject('gtest').get_variable('gtest_main_dep')
new_tests = [
    #4 ['systemTests', 'synthetic_usa']
]

foreach t : new_tests
    dir = t[0]
    name = t[1]


    exe = executable(
        name,
        join_paths(dir, name + '.cpp'),
        cpp_args : [
            '-DGRIDDYN_TEST_DIRECTORY="' + join_paths(meson.current_source_dir(), 'test_files') + '"'
        ],
        include_directories : griddyn_config_inc,
        dependencies : [
            gtest_dep,
            fs_lib,
            griddyn_dep,
            griddyn_core_dep,
            griddyn_file_input_dep,
        ])
    test(name, exe)
endforeach
