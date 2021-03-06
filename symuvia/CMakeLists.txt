cmake_minimum_required (VERSION 3.3)
project (SymuVia)

set(SYMUVIA_VERSION_MAJOR 9)
set(SYMUVIA_VERSION_MINOR 1)
set(SYMUVIA_VERSION_REVISION 0)
set(SYMUVIA_VERSION_BUILD 0)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/SymuviaVersion.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/SymuviaVersion.h
  @ONLY)
  
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/symuvia.rc.in
  ${CMAKE_CURRENT_SOURCE_DIR}/symuvia.rc
  @ONLY)

# Test 32/64 bits
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    message(STATUS "Target is 64 bits")
    set(IS_X64 "True")
else("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    message(STATUS "Target is 32 bits")
    set(IS_X64 "False")
endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'Release' as none was specified.")
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

get_filename_component(SDK_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../SDKs" REALPATH)

# rapidjson Headers
set(RAPIDJSON_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/rapidjson/rapidjson-1.1.0/include CACHE PATH "rapidjson headers directory")
include_directories(${RAPIDJSON_INCLUDE_DIRECTORY}) 

# Boost Headers
if(MSVC)
set(BOOST_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/boost/boost_1_70_0 CACHE PATH "Boost include folder")
include_directories(
                    $<$<CONFIG:Debug>:${BOOST_INCLUDE_DIRECTORY}>
                    $<$<CONFIG:Release>:${BOOST_INCLUDE_DIRECTORY}>) 
else(MSVC)
set(BOOST_INCLUDE_DIRECTORY "/usr/local/include" CACHE PATH "Boost include folder")
include_directories(${BOOST_INCLUDE_DIRECTORY}) 
endif(MSVC)


# GDAL Headers
if(MSVC)
if(IS_X64)
set(GDAL_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/gdal/1.11.5/x64/include CACHE PATH "GDAL x64 include folder")
else(IS_X64)
set(GDAL_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/gdal/1.11.5/win32/include CACHE PATH "GDAL x86 include folder")
endif()
else()
set(GDAL_INCLUDE_DIRECTORY "/usr/local/include" CACHE PATH "GDAL include folder")
endif()
include_directories(${GDAL_INCLUDE_DIRECTORY}) 


get_filename_component(SYMUTOOLS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../SymuTools" REALPATH)

# SymuCore Headers
get_filename_component(SYMUCORE_DIRECTORY "${SYMUTOOLS_DIRECTORY}/SymuCore" REALPATH)
set(SYMUCORE_INCLUDE_DIRECTORY ${SYMUCORE_DIRECTORY} CACHE PATH "SymuCore include folder")
include_directories(${SYMUCORE_INCLUDE_DIRECTORY})

# x64 Headers
if(IS_X64)

    # Python Headers
    if(MSVC)
        set(PYTHON_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/python/Python27/include CACHE PATH "Python x64 include folder")
    elseif(APPLE)
	set(PYTHON_INCLUDE_DIRECTORY "/System/Library/Frameworks/Python.framework/Versions/2.7/Headers" CACHE PATH "Python x64 include folder")
    else(APPLE)
        set(PYTHON_INCLUDE_DIRECTORY "/usr/include/python2.7" CACHE PATH "Python x64 include folder")
    endif(MSVC)
    include_directories(${PYTHON_INCLUDE_DIRECTORY})
    
    # Xerces Headers
    if(MSVC)
    set(XERCES_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/xerces/xerces-c-3.1.4-x86_64-windows-vc-12.0/include CACHE PATH "Xerces x64 include folder")
    include_directories(
                        $<$<CONFIG:Debug>:${XERCES_INCLUDE_DIRECTORY}>
                        $<$<CONFIG:Release>:${XERCES_INCLUDE_DIRECTORY}>)
    else(MSVC)
    set(XERCES_INCLUDE_DIRECTORY "/usr/local/include" CACHE PATH "Xerces c64 include folder")
    include_directories(${XERCES_INCLUDE_DIRECTORY})
    endif(MSVC)
                        
    # XQilla Headers
    if(MSVC)
    set(XQILLA_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/xqilla/xqilla-2.3.3-x86_64-windows-vc-12.0/include CACHE PATH "XQilla x64 include folder")
    include_directories(
                        $<$<CONFIG:Debug>:${XQILLA_INCLUDE_DIRECTORY}>
                        $<$<CONFIG:Release>:${XQILLA_INCLUDE_DIRECTORY}>)
    else(MSVC)
    set(XQILLA_INCLUDE_DIRECTORY "/usr/local/include" CACHE PATH "XQilla include folder")
    include_directories(${XQILLA_INCLUDE_DIRECTORY})
    endif(MSVC)
    
# 32 bits headers
else(IS_X64)

    # Python Headers 
    set(PYTHON_x86_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/python/Python27_x86/include CACHE PATH "Python x86 include folder")
    include_directories(${PYTHON_x86_INCLUDE_DIRECTORY}) 
    
    # Xerces Headers 
    set(XERCES_x86_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/xerces/xerces-c-3.1.4-x86-windows-vc-12.0/include CACHE PATH "Xerces x86 include folder")
    include_directories(
                        $<$<CONFIG:Debug>:${XERCES_x86_INCLUDE_DIRECTORY}>
                        $<$<CONFIG:Release>:${XERCES_x86_INCLUDE_DIRECTORY}>)
                        
    # XQilla Headers 
    set(XQILLA_x86_INCLUDE_DIRECTORY ${SDK_DIRECTORY}/xqilla/xqilla-2.3.3-x86-windows-vc-12.0/include CACHE PATH "XQilla x86 include folder")
    include_directories(
                        $<$<CONFIG:Debug>:${XQILLA_x86_INCLUDE_DIRECTORY}>
                        $<$<CONFIG:Release>:${XQILLA_x86_INCLUDE_DIRECTORY}>)
                        
endif(IS_X64)

if(MSVC)
   set(CMAKE_CONFIGURATION_TYPES Debug Release)
   set(CMAKE_CONFIGURATION_TYPES "${CMAKE_CONFIGURATION_TYPES}" CACHE STRING
     "Reset the configurations to what we need"
     FORCE)
   SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Zc:wchar_t")
   SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zc:wchar_t")
   SET(CMAKE_SHARED_LINKER_FLAGS_DEBUG ${CMAKE_SHARED_LINKER_FLAGS_DEBUG})
   SET(CMAKE_SHARED_LINKER_FLAGS_RELEASE ${CMAKE_SHARED_LINKER_FLAGS_RELEASE})
 endif(MSVC)
 
 # On Windows, define /bigobj needed
if(MSVC)
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj" )
    set ( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /bigobj" )
endif()


# Preprocessor definitions...
add_definitions(-DTRAFFICSTATE_EXPORTS)
add_definitions(-DSYMUVIA_EXPORTS)
add_definitions(-DUSE_SYMUCORE)

if(MSVC)
    add_definitions(-DMICROSOFT)
    add_definitions(-DUNICODE)
endif(MSVC)

add_definitions(-DBOOST_LOG_DYN_LINK)

# On Linux, we activate C++ 11 support (for using the std::tuple, may not be needed anymore since we don't use it anymore ?)
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(UNIX)


file(GLOB symuvia_src
    "*.h"
	"*.hpp"
    "*.cpp"
    "*.c"
    "*/*.h"
	"*/*.hpp"
    "*/*.cpp"
    "*/*/*.h"
    "*/*/*.cpp"
    "*/*/*/*.h"
    "*/*/*/*.cpp"
    "*.rc"
    "*.rgs"
 )
 
 file(GLOB symuvia_src_cpp
    "*.cpp"
    "*/*.cpp"
    "*/*/*.cpp"
    "*/*/*/*.cpp"
 )
 
# SymuVia lib definition
add_library(SymuVia SHARED ${symuvia_src})

# Adding the root path as include directory (if not, the #include "stdafx.h" of source files of child folders don't work on Linux and with Intellisense
include_directories(${CMAKE_SOURCE_DIR})

get_filename_component(SYMUVIABIN_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../Bin" REALPATH)

# Output folder definition
# To simplify ???
if(IS_X64)

if(MSVC)
	set_target_properties( SymuVia
	    PROPERTIES
	    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${SYMUVIABIN_DIRECTORY}/Bin"
	    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${SYMUVIABIN_DIRECTORY}/Bin"
	    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SYMUVIABIN_DIRECTORY}/Bin"
	    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${SYMUVIABIN_DIRECTORY}/Bin"
	    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${SYMUVIABIN_DIRECTORY}/Bin"
	    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SYMUVIABIN_DIRECTORY}/Bin"
	)
endif(MSVC)

if(APPLE)
	set_target_properties( SymuVia
	    PROPERTIES
	    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${SYMUVIABIN_DIRECTORY}/Bin"
	    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${SYMUVIABIN_DIRECTORY}/Bin"
	    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SYMUVIABIN_DIRECTORY}/Bin"
	    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${SYMUVIABIN_DIRECTORY}/Bin"
	    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${SYMUVIABIN_DIRECTORY}/Bin"
	    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SYMUVIABIN_DIRECTORY}/Bin"
	)
endif(APPLE)

else(IS_X64)

set_target_properties( SymuVia
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${SYMUVIABIN_DIRECTORY}/Bin"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG "${SYMUVIABIN_DIRECTORY}/Bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${SYMUVIABIN_DIRECTORY}/Bin"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${SYMUVIABIN_DIRECTORY}/Bin"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE "${SYMUVIABIN_DIRECTORY}/Bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${SYMUVIABIN_DIRECTORY}/Bin"
)

endif(IS_X64)

# Suffix by d in debug mode
set_target_properties( SymuVia
    PROPERTIES
    OUTPUT_NAME_DEBUG "SymuViad"
)


# Link definitions

# Boost
if(IS_X64)
    if(MSVC)
        set(BOOST_LIB_DIRECTORY ${SDK_DIRECTORY}/boost/boost_1_70_0/lib/x64/ CACHE PATH "Boost x64 libs folder")
    else(MSVC)
        set(BOOST_LIB_DIRECTORY "/usr/local/lib" CACHE PATH "Boost x64 libs folder")
    endif(MSVC)
else(IS_X64)
set(BOOST_LIB_DIRECTORY ${SDK_DIRECTORY}/boost/boost_1_70_0/lib/win32/ CACHE PATH "Boost x86 libs folder")
endif(IS_X64)

if(MSVC)
set(BOOST_SERIALIZATION_RELEASE_LIB_NAME "libboost_serialization-vc141-mt-x64-1_70.lib" CACHE STRING "Boost serialization release lib name")
set(BOOST_SERIALIZATION_DEBUG_LIB_NAME "libboost_serialization-vc141-mt-gd-x64-1_70.lib" CACHE STRING "Boost serialization debug lib name")
set(BOOST_WSERIALIZATION_RELEASE_LIB_NAME "libboost_wserialization-vc141-mt-x64-1_70.lib" CACHE STRING "Boost wserialization release lib name")
set(BOOST_WSERIALIZATION_DEBUG_LIB_NAME "libboost_wserialization-vc141-mt-gd-x64-1_70.lib" CACHE STRING "Boost wserialization release lib name")
set(BOOST_PYTHON_RELEASE_LIB_NAME "boost_python-vc141-mt-x64-1_70.lib" CACHE STRING "Boost python release lib name")
set(BOOST_PYTHON_DEBUG_LIB_NAME "boost_python-vc141-mt-gd-x64-1_70.lib" CACHE STRING "Boost python debug lib name")
set(BOOST_DATE_TIME_RELEASE_LIB_NAME "libboost_date_time-vc141-mt-x64-1_70.lib" CACHE STRING "Boost date_time release lib name")
set(BOOST_DATE_TIME_DEBUG_LIB_NAME "libboost_date_time-vc141-mt-gd-x64-1_70.lib" CACHE STRING "Boost date_time debug lib name")
set(BOOST_LOG_RELEASE_LIB_NAME "boost_log-vc141-mt-x64-1_70.lib" CACHE STRING "Boost log release lib name")
set(BOOST_LOG_DEBUG_LIB_NAME "boost_log-vc141-mt-gd-x64-1_70.lib" CACHE STRING "Boost log debug lib name")
set(BOOST_SYSTEM_RELEASE_LIB_NAME "libboost_system-vc141-mt-x64-1_70.lib" CACHE STRING "Boost system release lib name")
set(BOOST_SYSTEM_DEBUG_LIB_NAME "libboost_system-vc141-mt-gd-x64-1_70.lib" CACHE STRING "Boost system debug lib name")
set(BOOST_FILESYSTEM_RELEASE_LIB_NAME "libboost_filesystem-vc141-mt-x64-1_70.lib" CACHE STRING "Boost filesystem release lib name")
set(BOOST_FILESYSTEM_DEBUG_LIB_NAME "libboost_filesystem-vc141-mt-gd-x64-1_70.lib" CACHE STRING "Boost filesystem debug lib name")
set(BOOST_THREAD_RELEASE_LIB_NAME "libboost_thread-vc141-mt-x64-1_70.lib" CACHE STRING "Boost thread release lib name")
set(BOOST_THREAD_DEBUG_LIB_NAME "libboost_thread-vc141-mt-gd-x64-1_70.lib" CACHE STRING "Boost thread debug lib name")
set(BOOST_CHRONO_RELEASE_LIB_NAME "libboost_chrono-vc141-mt-x64-1_70.lib" CACHE STRING "Boost chrono release lib name")
set(BOOST_CHRONO_DEBUG_LIB_NAME "libboost_chrono-vc141-mt-gd-x64-1_70.lib" CACHE STRING "Boost chrono debug lib name")
elseif(APPLE)
set(BOOST_SERIALIZATION_RELEASE_LIB_NAME "libboost_serialization.dylib" CACHE STRING "Boost serialization release lib name")
set(BOOST_WSERIALIZATION_RELEASE_LIB_NAME "libboost_wserialization.dylib" CACHE STRING "Boost wserialization release lib name")
set(BOOST_PYTHON_RELEASE_LIB_NAME "libboost_python.dylib" CACHE STRING "Boost python release lib name")
set(BOOST_LOCALE_RELEASE_LIB_NAME "libboost_locale.dylib" CACHE STRING "Boost locale release lib name")
set(BOOST_DATE_TIME_RELEASE_LIB_NAME "libboost_date_time.dylib" CACHE STRING "Boost date_time release lib name")
set(BOOST_LOG_RELEASE_LIB_NAME "libboost_log.dylib" CACHE STRING "Boost log release lib name")
set(BOOST_SYSTEM_RELEASE_LIB_NAME "libboost_system.dylib" CACHE STRING "Boost system release lib name")
set(BOOST_FILESYSTEM_RELEASE_LIB_NAME "libboost_filesystem.dylib" CACHE STRING "Boost filesystem release lib name")
set(BOOST_THREAD_RELEASE_LIB_NAME "libboost_thread.dylib" CACHE STRING "Boost thread release lib name")
set(BOOST_CHRONO_RELEASE_LIB_NAME "libboost_chrono.dylib" CACHE STRING "Boost chrono release lib name")
else()
set(BOOST_SERIALIZATION_RELEASE_LIB_NAME "libboost_serialization.so" CACHE STRING "Boost serialization release lib name")
set(BOOST_WSERIALIZATION_RELEASE_LIB_NAME "libboost_wserialization.so" CACHE STRING "Boost wserialization release lib name")
set(BOOST_PYTHON_RELEASE_LIB_NAME "libboost_python.so" CACHE STRING "Boost python release lib name")
set(BOOST_LOCALE_RELEASE_LIB_NAME "libboost_locale.so" CACHE STRING "Boost locale release lib name")
set(BOOST_DATE_TIME_RELEASE_LIB_NAME "libboost_date_time.so" CACHE STRING "Boost date_time release lib name")
set(BOOST_LOG_RELEASE_LIB_NAME "libboost_log.so" CACHE STRING "Boost log release lib name")
set(BOOST_SYSTEM_RELEASE_LIB_NAME "libboost_system.so" CACHE STRING "Boost system release lib name")
set(BOOST_FILESYSTEM_RELEASE_LIB_NAME "libboost_filesystem.so" CACHE STRING "Boost filesystem release lib name")
set(BOOST_THREAD_RELEASE_LIB_NAME "libboost_thread.so" CACHE STRING "Boost thread release lib name")
set(BOOST_CHRONO_RELEASE_LIB_NAME "libboost_chrono.so" CACHE STRING "Boost chrono release lib name")
endif()

if(MSVC)
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${BOOST_LIB_DIRECTORY}/${BOOST_SERIALIZATION_DEBUG_LIB_NAME}>
                        $<$<CONFIG:Release>:${BOOST_LIB_DIRECTORY}/${BOOST_SERIALIZATION_RELEASE_LIB_NAME}> )
                        
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${BOOST_LIB_DIRECTORY}/${BOOST_WSERIALIZATION_DEBUG_LIB_NAME}>
                        $<$<CONFIG:Release>:${BOOST_LIB_DIRECTORY}/${BOOST_WSERIALIZATION_RELEASE_LIB_NAME}> )
                        
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${BOOST_LIB_DIRECTORY}/${BOOST_PYTHON_DEBUG_LIB_NAME}>
                        $<$<CONFIG:Release>:${BOOST_LIB_DIRECTORY}/${BOOST_PYTHON_RELEASE_LIB_NAME}> )
						
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${BOOST_LIB_DIRECTORY}/${BOOST_DATE_TIME_DEBUG_LIB_NAME}>
                        $<$<CONFIG:Release>:${BOOST_LIB_DIRECTORY}/${BOOST_DATE_TIME_RELEASE_LIB_NAME}> )
						
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${BOOST_LIB_DIRECTORY}/${BOOST_LOG_DEBUG_LIB_NAME}>
                        $<$<CONFIG:Release>:${BOOST_LIB_DIRECTORY}/${BOOST_LOG_RELEASE_LIB_NAME}> )
						
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${BOOST_LIB_DIRECTORY}/${BOOST_SYSTEM_DEBUG_LIB_NAME}>
                        $<$<CONFIG:Release>:${BOOST_LIB_DIRECTORY}/${BOOST_SYSTEM_RELEASE_LIB_NAME}> )
						
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${BOOST_LIB_DIRECTORY}/${BOOST_FILESYSTEM_DEBUG_LIB_NAME}>
                        $<$<CONFIG:Release>:${BOOST_LIB_DIRECTORY}/${BOOST_FILESYSTEM_RELEASE_LIB_NAME}> )
						
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${BOOST_LIB_DIRECTORY}/${BOOST_THREAD_DEBUG_LIB_NAME}>
                        $<$<CONFIG:Release>:${BOOST_LIB_DIRECTORY}/${BOOST_THREAD_RELEASE_LIB_NAME}> )
	
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${BOOST_LIB_DIRECTORY}/${BOOST_CHRONO_DEBUG_LIB_NAME}>
                        $<$<CONFIG:Release>:${BOOST_LIB_DIRECTORY}/${BOOST_CHRONO_RELEASE_LIB_NAME}> )
						
else(MSVC)
target_link_libraries( SymuVia ${BOOST_LIB_DIRECTORY}/${BOOST_SERIALIZATION_RELEASE_LIB_NAME}
                               ${BOOST_LIB_DIRECTORY}/${BOOST_WSERIALIZATION_RELEASE_LIB_NAME}
                               ${BOOST_LIB_DIRECTORY}/${BOOST_PYTHON_RELEASE_LIB_NAME}
                               ${BOOST_LIB_DIRECTORY}/${BOOST_LOCALE_RELEASE_LIB_NAME})

target_link_libraries( SymuVia ${BOOST_LIB_DIRECTORY}/${BOOST_DATE_TIME_RELEASE_LIB_NAME})
target_link_libraries( SymuVia ${BOOST_LIB_DIRECTORY}/${BOOST_LOG_RELEASE_LIB_NAME})
target_link_libraries( SymuVia ${BOOST_LIB_DIRECTORY}/${BOOST_SYSTEM_RELEASE_LIB_NAME})
target_link_libraries( SymuVia ${BOOST_LIB_DIRECTORY}/${BOOST_FILESYSTEM_RELEASE_LIB_NAME})
target_link_libraries( SymuVia ${BOOST_LIB_DIRECTORY}/${BOOST_THREAD_RELEASE_LIB_NAME})
target_link_libraries( SymuVia ${BOOST_LIB_DIRECTORY}/${BOOST_CHRONO_RELEASE_LIB_NAME})

endif(MSVC)
                        
# Python 
if(IS_X64)
    if(MSVC)
        set(PYTHON_LIB_PATH ${SDK_DIRECTORY}/python/Python27/libs/python27.lib CACHE FILE "Full path ot the python x64 lib")
    elseif(APPLE)
        set(PYTHON_LIB_PATH "/System/Library/Frameworks/Python.framework/Versions/2.7/lib/libpython2.7.dylib" CACHE FILE "Full path ot the python x64 lib")
    else()
        set(PYTHON_LIB_PATH "/usr/lib/x86_64-linux-gnu/libpython2.7.so" CACHE FILE "Full path ot the python x64 lib")
    endif()
else(IS_X64)
    set(PYTHON_LIB_PATH ${SDK_DIRECTORY}/python/Python27_x86/libs/python27.lib CACHE FILE "Full path ot the python x86 lib")
endif(IS_X64)

target_link_libraries( SymuVia ${PYTHON_LIB_PATH})

# Xerces
if(IS_X64)
if(MSVC)
set(XERCES_LIB_PATH ${SDK_DIRECTORY}/xerces/xerces-c-3.1.4-x86_64-windows-vc-12.0/lib/xerces-c_3.lib CACHE FILE "Full path of the xerces x64 lib")
elseif(APPLE)
set(XERCES_LIB_PATH "/usr/local/lib/libxerces-c.dylib" CACHE FILE "Full path of the xerces x64 lib")
else()
set(XERCES_LIB_PATH "/usr/local/lib/libxerces-c.so" CACHE FILE "Full path of the xerces x64 lib")
endif()
else(IS_X64)
set(XERCES_LIB_PATH ${SDK_DIRECTORY}/xerces/xerces-c-3.1.4-x86-windows-vc-12.0/lib/xerces-c_3.lib CACHE FILE "Full path of the xerces x86 lib")
endif(IS_X64)

if(MSVC)
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${XERCES_LIB_PATH}>
                        $<$<CONFIG:Release>:${XERCES_LIB_PATH}>)
else(MSVC)
target_link_libraries( SymuVia ${XERCES_LIB_PATH})
endif(MSVC)                        
                        
# XQilla
if(IS_X64)
if(MSVC)
set(XQILLA_LIB_PATH ${SDK_DIRECTORY}/xqilla/xqilla-2.3.3-x86_64-windows-vc-12.0/lib/xqilla23.lib CACHE FILE "Full path of the xqilla x64 lib")
elseif(APPLE)
set(XQILLA_LIB_PATH "/usr/local/lib/libxqilla.dylib" CACHE FILE "Full path of the xqilla x64 lib")
else()
set(XQILLA_LIB_PATH "/usr/local/lib/libxqilla.so" CACHE FILE "Full path of the xqilla x64 lib")
endif()
else(IS_X64)
set(XQILLA_LIB_PATH ${SDK_DIRECTORY}/xqilla/xqilla-2.3.3-x86-windows-vc-12.0/lib/xqilla23.lib CACHE FILE "Full path of the xqilla x86 lib")
endif(IS_X64)

if(MSVC)
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${XQILLA_LIB_PATH}>
                        $<$<CONFIG:Release>:${XQILLA_LIB_PATH}>)
else(MSVC)
target_link_libraries( SymuVia ${XQILLA_LIB_PATH})
endif(MSVC)
                        
# GDAL
if(IS_X64)
if(MSVC)
set(GDAL_LIB_PATH ${SDK_DIRECTORY}/gdal/1.11.5/x64/lib/gdal_i.lib CACHE FILE "Full path of the GDAL x64 lib")
elseif(APPLE)
set(GDAL_LIB_PATH "/usr/local/lib/libgdal.dylib" CACHE FILE "Full path of the GDAL x64 lib")
else()
set(GDAL_LIB_PATH "/usr/local/lib/libgdal.so" CACHE FILE "Full path of the GDAL x64 lib")
endif()
else(IS_X64)
set(GDAL_LIB_PATH ${SDK_DIRECTORY}/gdal/1.11.5/win32/lib/gdal_i.lib CACHE FILE "Full path of the GDAL x86 lib")
endif(IS_X64)

target_link_libraries( SymuVia ${GDAL_LIB_PATH})

# SymuCore
if(IS_X64)
	if(MSVC)
		set(SYMUCORE_RELEASE_LIB_PATH "${SYMUVIABIN_DIRECTORY}/Bin/SymuCore.lib" CACHE FILE "SymuCore release x64 full path")
		set(SYMUCORE_DEBUG_LIB_PATH "${SYMUVIABIN_DIRECTORY}/Bin/SymuCored.lib" CACHE FILE "SymuCore debug x64 full path")
	elseif(APPLE)
		set(SYMUCORE_LIB_PATH "${SYMUVIABIN_DIRECTORY}/Bin/libSymuCore.dylib" CACHE FILE "SymuCore release x64 full path")
	else()
		set(SYMUCORE_LIB_PATH "${SYMUVIABIN_DIRECTORY}/Bin/libSymuCore.so" CACHE FILE "SymuCore release x64 full path")
	endif(MSVC)
else(IS_X64)
	set(SYMUCORE_RELEASE_LIB_PATH "${SYMUVIABIN_DIRECTORY}/Bin/SymuCore.lib" CACHE FILE "SymuCore release x64 full path")
	set(SYMUCORE_DEBUG_LIB_PATH "${SYMUVIABIN_DIRECTORY}/Bin/SymuCored.lib" CACHE FILE "SymuCore debug x64 full path")
endif(IS_X64)

if(MSVC)
target_link_libraries( SymuVia 
                        $<$<CONFIG:Debug>:${SYMUCORE_DEBUG_LIB_PATH}>
                        $<$<CONFIG:Release>:${SYMUCORE_RELEASE_LIB_PATH}>)
else(MSVC)
target_link_libraries( SymuVia ${SYMUCORE_LIB_PATH} )
endif(MSVC)

#ODBC32
if(MSVC)
target_link_libraries( SymuVia odbc32.lib )
elseif(APPLE)
target_link_libraries( SymuVia /usr/local/lib/libodbc.dylib )
else(APPLE)
target_link_libraries( SymuVia odbc )
endif(MSVC)

#WINVER
if(MSVC)
target_link_libraries( SymuVia Version.lib )
endif(MSVC)

# On Windows, we set the use of the precompiled header file stdafx for cpp files
if (MSVC)
   set_source_files_properties(${SymuVia_src_cpp} PROPERTIES COMPILE_FLAGS "/Yustdafx.h")
   set_source_files_properties(stdafx.cpp PROPERTIES COMPILE_FLAGS "/Ycstdafx.h")
endif(MSVC)
