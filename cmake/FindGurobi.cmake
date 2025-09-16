# Locate the Gurobi Optimizer installation.
#
# The module defines the following variables:
#   Gurobi_FOUND          - system has a working Gurobi installation
#   GUROBI_INCLUDE_DIR    - path to gurobi_c.h
#   GUROBI_LIBRARY        - path to the Gurobi C library
#
# It also defines an imported target:
#   Gurobi::Gurobi

include(FindPackageHandleStandardArgs)

set(_GUROBI_SEARCH_ROOTS)

if (DEFINED Gurobi_ROOT)
    list(APPEND _GUROBI_SEARCH_ROOTS "${Gurobi_ROOT}")
endif()

if (DEFINED ENV{GUROBI_HOME})
    list(APPEND _GUROBI_SEARCH_ROOTS "$ENV{GUROBI_HOME}")
endif()

file(GLOB _gurobi_default_roots
    "/opt/gurobi*"
    "/Library/gurobi*"
    "/usr/local/gurobi*"
)
list(APPEND _GUROBI_SEARCH_ROOTS ${_gurobi_default_roots})
list(REMOVE_DUPLICATES _GUROBI_SEARCH_ROOTS)

set(_GUROBI_INCLUDE_HINTS)
set(_GUROBI_LIBRARY_HINTS)
foreach(_root IN LISTS _GUROBI_SEARCH_ROOTS)
    list(APPEND _GUROBI_INCLUDE_HINTS
        "${_root}"
        "${_root}/include"
        "${_root}/mac64/include"
        "${_root}/macos_universal2/include"
        "${_root}/linux64/include"
    )
    list(APPEND _GUROBI_LIBRARY_HINTS
        "${_root}"
        "${_root}/lib"
        "${_root}/lib64"
        "${_root}/mac64"
        "${_root}/macos_universal2/lib"
        "${_root}/linux64/lib"
    )
endforeach()

find_path(GUROBI_INCLUDE_DIR
    NAMES gurobi_c.h
    HINTS ${_GUROBI_INCLUDE_HINTS}
    PATH_SUFFIXES include
)

set(_GUROBI_LIBRARY_NAMES
    gurobi
    gurobi140
    gurobi130
    gurobi125
    gurobi124
    gurobi123
    gurobi122
    gurobi121
    gurobi120
    gurobi119
    gurobi118
    gurobi117
    gurobi116
    gurobi115
    gurobi114
    gurobi113
    gurobi112
    gurobi111
    gurobi110
    gurobi100
    gurobi95
    gurobi91
    gurobi90
    gurobi81
    gurobi80
)

find_library(GUROBI_LIBRARY
    NAMES ${_GUROBI_LIBRARY_NAMES}
    HINTS ${_GUROBI_LIBRARY_HINTS}
    PATH_SUFFIXES lib lib64 mac64 macos_universal2/lib linux64/lib
)

find_package_handle_standard_args(Gurobi
    REQUIRED_VARS GUROBI_INCLUDE_DIR GUROBI_LIBRARY
)

if (Gurobi_FOUND)
    set(GUROBI_LIBRARIES "${GUROBI_LIBRARY}")
    set(GUROBI_INCLUDE_DIRS "${GUROBI_INCLUDE_DIR}")
    if (NOT TARGET Gurobi::Gurobi)
        add_library(Gurobi::Gurobi UNKNOWN IMPORTED)
        set_target_properties(Gurobi::Gurobi PROPERTIES
            IMPORTED_LOCATION "${GUROBI_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${GUROBI_INCLUDE_DIR}"
        )
    endif()
endif()

mark_as_advanced(GUROBI_INCLUDE_DIR GUROBI_LIBRARY)
