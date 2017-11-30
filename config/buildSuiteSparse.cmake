# This function is used to force a build on a dependant project at cmake configuration phase.
# 

function (build_suitesparse)

    set(trigger_build_dir ${CMAKE_BINARY_DIR}/autobuild/force_suitesparse)

    #mktemp dir in build tree
    file(MAKE_DIRECTORY ${trigger_build_dir} ${trigger_build_dir}/build)

    #set prefix for _INSTALL_PREFIX based on OS
    if(WIN32)
        set(prefix_install "SUITESPARSE")
    else()
        set(prefix_install "CMAKE")
    endif()

    #generate false dependency project
    set(CMAKE_LIST_CONTENT "
    cmake_minimum_required(VERSION 3.4)
    include(ExternalProject)
ExternalProject_Add(suitesparse
    SOURCE_DIR ${PROJECT_BINARY_DIR}/Download/suitesparse
    GIT_REPOSITORY  https://github.com/jlblancoc/suitesparse-metis-for-windows.git
    UPDATE_COMMAND " " 
    BINARY_DIR ${PROJECT_BINARY_DIR}/ThirdParty/suitesparse
     
    CMAKE_ARGS 
        -D${prefix_install}_INSTALL_PREFIX=${PROJECT_BINARY_DIR}/libs
        -DCMAKE_BUILD_TYPE=\$\{CMAKE_BUILD_TYPE\}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_LINKER=${CMAKE_LINKER}
		-DConfigPackageLocation=${PROJECT_BINARY_DIR}/cmake
        
      
    INSTALL_DIR ${PROJECT_BINARY_DIR}/libs
    )")


    file(WRITE ${trigger_build_dir}/CMakeLists.txt "${CMAKE_LIST_CONTENT}")

	message(STATUS "Configuring SuiteSparse Autobuild for release logging to ${PROJECT_BINARY_DIR}/logs/suitesparse_autobuild_config_release.log")
    execute_process(COMMAND ${CMAKE_COMMAND}  -Wno-dev -D CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -D CMAKE_C_COMPILER=${CMAKE_C_COMPILER} -D CMAKE_LINKER=${CMAKE_LINKER}
        -D CMAKE_BUILD_TYPE=Release -G ${CMAKE_GENERATOR} .. 
        WORKING_DIRECTORY ${trigger_build_dir}/build
		OUTPUT_FILE ${PROJECT_BINARY_DIR}/logs/suitesparse_autobuild_config_release.log
        )
		
	message(STATUS "Building SuiteSparse release build logging to ${PROJECT_BINARY_DIR}/logs/suitesparse_autobuild_build_release.log")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release
        WORKING_DIRECTORY ${trigger_build_dir}/build
		OUTPUT_FILE ${PROJECT_BINARY_DIR}/logs/suitesparse_autobuild_build_release.log
        )
		
	message(STATUS "Configuring SuiteSparse Autobuild for debug logging to ${PROJECT_BINARY_DIR}/logs/suitesparse_autobuild_config_debug.log")
	execute_process(COMMAND ${CMAKE_COMMAND}  -Wno-dev -D CMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER} -D CMAKE_C_COMPILER=${CMAKE_C_COMPILER} -D CMAKE_LINKER=${CMAKE_LINKER}
        -D CMAKE_BUILD_TYPE=Debug -G ${CMAKE_GENERATOR} .. 
        WORKING_DIRECTORY ${trigger_build_dir}/build
		OUTPUT_FILE ${PROJECT_BINARY_DIR}/logs/suitesparse_autobuild_config_debug.log
        )
		
	message(STATUS "Building SuiteSparse debug build logging to ${PROJECT_BINARY_DIR}/logs/suitesparse_autobuild_build_debug.log")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Debug 
        WORKING_DIRECTORY ${trigger_build_dir}/build
		OUTPUT_FILE ${PROJECT_BINARY_DIR}/logs/suitesparse_autobuild_build_debug.log
        )

endfunction()
