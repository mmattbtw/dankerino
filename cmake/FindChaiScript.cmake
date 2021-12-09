include(FindPackageHandleStandardArgs)

find_path(ChaiScript_INCLUDE_DIR chaiscript/chaiscript.hpp HINTS ${CMAKE_SOURCE_DIR}/lib/ChaiScript/include)

find_package_handle_standard_args(ChaiScript DEFAULT_MSG ChaiScript_INCLUDE_DIR)

if (ChaiScript_FOUND)
    add_library(ChaiScript INTERFACE IMPORTED)
    set_target_properties(ChaiScript PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES "${ChaiScript_INCLUDE_DIR}"
            )
endif ()

mark_as_advanced(ChaiScript_INCLUDE_DIR)
