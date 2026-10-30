cmake_minimum_required(VERSION 3.28)

# Project Definition
project(NacreEngine LANGUAGES CXX) # change the name of .exe file here

# --- Global Configuration ---
# Output binaries to a clean bin/ directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Interprocedural Optimization (Link Time Optimization)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Fix: Dynamically set MSVC Runtime Library to /MT for Release and /MTd for Debug
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Build Options
option(PRODUCTION_BUILD "Make this a production build!" OFF)

# --- SFML Dependency (FetchContent) ---
# Ensure SFML is built as static libraries
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build SFML as static libraries" FORCE)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.1.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM
)
FetchContent_MakeAvailable(SFML)

# --- Sources & Executable ---
# DON'T ADD THE SOURCES BY HAND, they are already added with this macro
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")

# --- Application Icon (Windows) ---
set(APP_ICON_RESOURCE "")
if(WIN32)
    # Include the .rc file in sources so CMake compiles it via rc.exe / windres
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
        set(APP_ICON_RESOURCE "${CMAKE_CURRENT_SOURCE_DIR}/app.rc")
    endif()
endif()

add_executable(${CMAKE_PROJECT_NAME} ${MY_SOURCES} ${APP_ICON_RESOURCE})

# --- Target Properties & Includes ---
# Require C++17
target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)

# Add Include Directory
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")

# --- Build Configuration Logic ---
if(PRODUCTION_BUILD)
    # Setup the RESOURCES_PATH macro to be in the root folder of your exe
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE RESOURCES_PATH="./resources/" PRODUCTION_BUILD=1)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE SPRITES_PATH="./resources/sprites/" PRODUCTION_BUILD=1) 
    
    if(MSVC)
        # Remove the console only in production builds
        set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup") 
    endif()
else()
    # This is useful to get a RESOURCES_PATH in your IDE during development
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/" PRODUCTION_BUILD=0) 
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE SPRITES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/sprites/" PRODUCTION_BUILD=0) 
endif()

# --- MSVC Specifics ---
if(MSVC) 
    # SIMD optimizations
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /arch:AVX2) 
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

# --- Linking ---
# Link SFML libraries statically (Note: SFML 3 requires the SFML:: namespace)
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
    SFML::Graphics 
    SFML::Window 
    SFML::System 
    SFML::Audio 
    # SFML::Network
)

# --- IDE Integration ---
# Set Visual Studio startup project automatically
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${CMAKE_PROJECT_NAME})