cmake_minimum_required(VERSION 3.50...4.4.2)

# Enter name of project...
set(PROJECT_NAME "TempDB")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(${PROJECT_NAME} VERSION 1.0.0
                        DESCRIPTION "1B - Memory Record Database"
                        LANGUAGES C RC)

# =================================================================================================

add_executable(${PROJECT_NAME})

file(GLOB SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
)

target_sources(${PROJECT_NAME}
    PRIVATE
        ${SRC_FILES}
)

target_compile_features(${PROJECT_NAME} PUBLIC c_std_11)

set_target_properties(${PROJECT_NAME}
    PROPERTIES VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        "$<$<COMPILE_LANGUAGE:C>:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

if(WIN32)
    target_sources(${PROJECT_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/../resources/windows/TempDB.rc
)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE
    "$<$<AND:$<CONFIG:Debug>,$<C_COMPILER_ID:GNU,Clang>>:-Wall;-Wextra;-Wpedantic>"
)

# =================================================================================================

include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

set(CPACK_PACKAGE_NAME "TempDB")
set(CPACK_PACKAGE_VENDOR "McNylo")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Milestone 1B Project - In-Memory Record Database")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_CONTACT "mcnyloyt@gmail.com")
set(CPACK_RESOURCE_FILE_LICENSE
    "${CMAKE_SOURCE_DIR}/../LICENSE"
)

install(
    FILES "${CMAKE_SOURCE_DIR}/../LICENSE"
    DESTINATION .
)

if (WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_PACKAGE_EXECUTABLES "TempDB" "TempDB")
    set(CPACK_CREATE_DESKTOP_LINKS "TempDB")
    set(CPACK_NSIS_MUI_ICON "${CMAKE_SOURCE_DIR}/../resources/windows/TempDB.ico")
    set(CPACK_NSIS_MUI_UNIICON "${CMAKE_SOURCE_DIR}/../resources/windows/TempDB.ico")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin/TempDB.exe")

elseif (APPLE)
    set(CPACK_GENERATOR "productbuild;TGZ")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr/local")
    set(CPACK_PRODUCTBUILD_IDENTIFIER "com.mcnylo.tempdb")

elseif (UNIX)
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_PACKAGING_INSTALL_PREFIX "/usr")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER ${CPACK_PACKAGE_CONTACT})
    set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")

endif()

include(CPack)