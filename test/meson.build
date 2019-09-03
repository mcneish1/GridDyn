library_tests = [
    'testString_viewConversion',
    'testOperatingBounds',
    'testXML',
    'testElementReaders',
    'testGridDynRunner',
    'testCore',
    'testMatrixData',
    'simpleQueueTests',
    'testReaderInfo',
    'testZipUtilities',
    'testJsonReader',
    'testStringConversion',
    'testString_viewOps',
    'testGridParameter',
    'testStringOps',
    'libraryTests',
    'testValuePredictors',
    'testWorkQueue',
]

system_tests = [
    'testRoots',
    'testSolverModes',
    'testSystem',
    'testGridDynRunner',
    'testInputs',
    'testContingency',
    'testDyn1',
    'validationTests',
    'testDyn2',
    'testCloning',
    'faultTests',
    'testConstraints',
    'testOutputs',
    'testpFlow',
    # 'testMainExe',
]

component_tests = [
    'simulationTests',
    'testGenModels',
    'testBlocks',
    'ConditionTests',
    'testArea',
    'testdcLinks',
    'testExtraModels',
    'testRecorders',
    'testGridLab',
    'testGovernors',
    # 'testFMI',
    'testComponents',
    'testGenerators',
    'testLoads',
    'testRelays',
    'testLinks',
    'testSource',
    'testEvents',
    'testExciters',
    'testAdjustableTX',
]

extra_tests = [
    'performanceTests',
    # 'extraFMUtests',
    'objectSizeReport',
    'largeInputTests',
    'largeContingencyTests',
    'testExtra',
]

shared_library_tests = [
    'shared_library_tests',
    'basic_tests',
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
        test_exe = executable(test_name + '_' + folder_name, [
                folder_name + '/' + test_name + '.cpp',
                'testHelperFunctions.cpp'
            ],
            cpp_args : ['-DBOOST_TEST_DYN_LINK', '-DBOOST_TEST_MODULE=griddyn_tests'],
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
        test('test_' + test_name + '_' + folder_name, test_exe)
    endforeach
endforeach
