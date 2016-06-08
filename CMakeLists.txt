#############################
# Global settings
#############################
CMAKE_MINIMUM_REQUIRED (VERSION 3.0)
PROJECT (marpaEBNF C CXX)

IF (POLICY CMP0063)
  CMAKE_POLICY (SET CMP0063 NEW)
ENDIF ()
IF (POLICY CMP0018)
  CMAKE_POLICY (SET CMP0018 NEW)
ENDIF ()
SET (CMAKE_C_VISIBILITY_PRESET   hidden)
SET (CMAKE_CXX_VISIBILITY_PRESET hidden)
SET (CMAKE_POSITION_INDEPENDENT_CODE ON)

SET (MARPAEBNF_VERSION_MAJOR 1)
SET (MARPAEBNF_VERSION_MINOR 0)
SET (MARPAEBNF_VERSION_PATCH 0)
SET (MARPAEBNF_VERSION "${MARPAEBNF_VERSION_MAJOR}.${MARPAEBNF_VERSION_MINOR}.${MARPAEBNF_VERSION_PATCH}")

SET (CMAKE_MODULE_PATH              ${PROJECT_SOURCE_DIR}/cmake)           # General module search path
SET (INCLUDE_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/include)  # General include output path
SET (LIBRARY_OUTPUT_PATH            ${PROJECT_SOURCE_DIR}/output/lib)      # General library output path
SET (BINARY_OUTPUT_PATH             ${PROJECT_SOURCE_DIR}/output/bin)      # General binary output path
SET (3RDPARTY_OUTPUT_PATH           ${PROJECT_SOURCE_DIR}/output/3rdparty) # General 3rdparty output path

# For all outputs directories
# C.f. http://stackoverflow.com/questions/7747857/in-cmake-how-do-i-work-around-the-debug-and-release-directories-visual-studio-2
SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${LIBRARY_OUTPUT_PATH}")
FOREACH (OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    STRING( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
    SET ( CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${LIBRARY_OUTPUT_PATH}")
    SET ( CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${LIBRARY_OUTPUT_PATH}")
    SET ( CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} "${LIBRARY_OUTPUT_PATH}")
ENDFOREACH (OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

# --------------------
# CFLAGS customization
# --------------------
if (("${CMAKE_C_COMPILER_ID} x" MATCHES "MSVC") OR MSVC)
  # Loosely based on http://www.hdfgroup.org/ftp/HDF/HDF_Current/src/unpacked/config/cmake/ConfigureChecks.cmake
  ADD_DEFINITIONS(-DWIN32_LEAN_AND_MEAN)
  ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)
  ADD_DEFINITIONS(-D_CRT_NONSTDC_NO_DEPRECATE)
ENDIF ()
IF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
  ADD_DEFINITIONS(-DMARPAEBNF_NTRACE)
ENDIF ((NOT CMAKE_BUILD_TYPE MATCHES Debug) AND (NOT CMAKE_BUILD_TYPE MATCHES RelWithDebInfo))
ADD_DEFINITIONS(-DMARPAEBNF_VERSION="${MARPAEBNF_VERSION}")

# -----------------------------------
# genericLogger: required
# -----------------------------------
FIND_PACKAGE (genericLogger)
IF (NOT GENERICLOGGER_FOUND)
    MESSAGE (FATAL_ERROR "genericLogger must be detected")
ENDIF ()

# -----------------------------------
# genericStack: required
# -----------------------------------
FIND_PACKAGE (genericStack)
IF (NOT GENERICSTACK_FOUND)
    MESSAGE (FATAL_ERROR "genericStack must be detected")
ENDIF ()

# -----------------------------------
# marpaWrapper: required
# -----------------------------------
FIND_PACKAGE (marpaWrapper)
IF (NOT MARPAWRAPPER_FOUND)
    MESSAGE (FATAL_ERROR "marpaWrapper must be detected")
ENDIF ()

# --------------------------------------
# config.h (we will share it with marpa)
# --------------------------------------
INCLUDE (FindInline)
IF ("${C_INLINE}" EQUAL "inline")
  SET (C_INLINE_IS_INLINE TRUE)
ELSE ("${C_INLINE}" EQUAL "inline")
  SET (C_INLINE_IS_INLINE FALSE)
ENDIF ("${C_INLINE}" EQUAL "inline")

# ----------------------------------------------------
# Configuration
# ----------------------------------------------------
SET (CONFIG_H ${INCLUDE_OUTPUT_PATH}/config.h)
SET (CONFIG_H_IN ${CMAKE_CURRENT_SOURCE_DIR}/include/config.h.in)
CONFIGURE_FILE(${CONFIG_H_IN} ${CONFIG_H})
ADD_CUSTOM_TARGET(marpaEBNFConfig SOURCES "${CONFIG_H}")

# ------------------
# marpaEBNF library
# -----------------
FILE (GLOB lib_src src/*.c)
ADD_LIBRARY (marpaEBNF SHARED ${lib_src})
INCLUDE_DIRECTORIES (
  "${INCLUDE_OUTPUT_PATH}"                             # Include path for generated files
  "${PROJECT_SOURCE_DIR}/include"                      # Include path for the other headers
  "${GENERICLOGGER_INCLUDE_DIRS}"                      # Include path for genericLogger (external - required)
  "${GENERICSTACK_INCLUDE_DIRS}"                       # Include path for genericStack (external - required)
  "${MARPAWRAPPER_INCLUDE_DIRS}"                       # Include path for marpaWrapper (external - required)
  )
TARGET_LINK_LIBRARIES (marpaEBNF "${GENERICLOGGER_LINK_FLAGS}" "${MARPAWRAPPER_LINK_FLAGS}")
SET_TARGET_PROPERTIES (marpaEBNF
  PROPERTIES
  COMPILE_FLAGS "-DmarpaEBNF_EXPORTS"
  VERSION "${MARPAEBNF_VERSION}"
  SOVERSION "${MARPAEBNF_VERSION_MAJOR}")

# ------------
# Export flags
# ------------
INCLUDE (GenerateExportHeader)
EXECUTE_PROCESS(COMMAND "${CMAKE_COMMAND}" -E make_directory "${INCLUDE_OUTPUT_PATH}/marpaEBNF")
GENERATE_EXPORT_HEADER(marpaEBNF
    BASE_NAME marpaEBNF
    EXPORT_MACRO_NAME marpaEBNF_EXPORT
    EXPORT_FILE_NAME "${INCLUDE_OUTPUT_PATH}/marpaEBNF/export.h"
    STATIC_DEFINE marpaEBNF_STATIC)
ADD_CUSTOM_TARGET(marpaEBNFExport SOURCES "${INCLUDE_OUTPUT_PATH}/marpaEBNF/export.h")

# ----------------------------------------
# Explicit dependencies on generated files
# ----------------------------------------
ADD_DEPENDENCIES(marpaEBNF marpaEBNFConfig marpaEBNFExport)
SET_TARGET_PROPERTIES(marpaEBNF PROPERTIES COMPILE_FLAGS "-DmarpaEBNF_EXPORTS"
                                               VERSION       "${MARPAEBNF_VERSION}"
                                               SOVERSION     "${MARPAEBNF_VERSION_MAJOR}"
                                               LINKER_LANGUAGE C)


# -----
# Check
# -----
ENABLE_TESTING()

ADD_EXECUTABLE        (marpaEBNFTester EXCLUDE_FROM_ALL test/marpaEBNFTester.c)
#
# Painful but putting the binaries produced with dynamic linking in the same directory as the
# dynamic libraries is the only way to make sure the OS will always see the dynamic library
# at runtime, REGARDLESS of the OS (we are thinking to Windows and Visual Studio for instance).
# and since we must do that for the dynamic test, we do it as well for any runtime thingy, just for coherency.
#
SET_TARGET_PROPERTIES (marpaEBNFTester       PROPERTIES LINKER_LANGUAGE C RUNTIME_OUTPUT_DIRECTORY ${LIBRARY_OUTPUT_PATH})
TARGET_LINK_LIBRARIES (marpaEBNFTester       marpaEBNF)

ADD_TEST              (NAME marpaEBNFTest
  COMMAND marpaEBNFTester
  WORKING_DIRECTORY ${LIBRARY_OUTPUT_PATH})
ADD_CUSTOM_TARGET     (check COMMAND ${CMAKE_CTEST_COMMAND} DEPENDS marpaEBNFTester)

# -------
# Install
# -------
INSTALL (TARGETS marpaEBNF ARCHIVE DESTINATION lib LIBRARY DESTINATION lib RUNTIME DESTINATION bin COMPONENT libraries)
INSTALL (FILES ${INCLUDE_OUTPUT_PATH}/marpaEBNF/export.h DESTINATION include/marpaEBNF)
FILE (GLOB include_marpa_src "include/marpaEBNF/*.h")
INSTALL (FILES ${include_marpa_src} DESTINATION include)
INSTALL (FILES include/marpaEBNF.h DESTINATION include)

IF (UNIX)
  INCLUDE (Pod2Man)
  ADD_MANPAGE_TARGET()
  POD2MAN(README.pod "MARPAEBNF" "3" "${MARPAEBNF_VERSION}")
ENDIF (UNIX)
