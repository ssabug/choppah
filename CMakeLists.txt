cmake_minimum_required(VERSION 3.22)

project (choppah VERSION 0.1)

# Adding JUCE
add_subdirectory (External/JUCE)


# Set the C++ compiler to the cross-compiler for Windows
#set(CMAKE_CXX_COMPILER /path/to/cross-compiler)
# The subdirectory contains its own CMakeLists that creates BinaryData targets containing graphics and fonts
#add_subdirectory (BinaryResources)

# Adding JSON
#set (JSON_BuildTests OFF CACHE INTERNAL "")
#set (JSON_Install OFF CACHE INTERNAL "")
#add_subdirectory (Ext/JBPluginBase)
#add_subdirectory (Ext/Resvg4JUCE)

if (NOT DEFINED COPY_PLUGIN_AFTER_BUILD)
    set (COPY_PLUGIN_AFTER_BUILD TRUE)
endif()

function (add_choppah_version format)

# Add the plugin target itself
juce_add_plugin (choppah-${format}

        COMPANY_NAME Ssabug
        PRODUCT_NAME "choppah"

        COPY_PLUGIN_AFTER_BUILD ${COPY_PLUGIN_AFTER_BUILD}
        FORMATS ${format}

        ${ARGN})

# We want to compile the plugin with C++14
target_compile_features (choppah-${format} PRIVATE cxx_std_17)
set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall  -O3 -march=native -std=c++11")
# Gather some info regarding our git commit to inject them into a header file.
#jb_add_git_version_info (choppah-${format})

# Adding all source files to the plugin target
target_sources (choppah-${format}
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp)

target_compile_definitions (choppah-${format}
    PUBLIC
        DONT_SET_USING_JUCE_NAMESPACE=1
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_STRICT_REFCOUNTEDPTR=1
        JUCE_VST3_CAN_REPLACE_VST2=0
        JB_INCLUDE_JSON=1)

target_link_libraries (choppah-${format}
    PRIVATE
        # JUCE Modules
        juce::juce_dsp

        # Custom modules
        #jb_plugin_base
        #jb::Resvg4JUCE

        # JSON dependncy in plugin base
        #nlohmann_json::nlohmann_json

        # Binary Data
        #EmbeddedBinaryData

        # Recommended flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
        juce::juce_recommended_config_flags)

if (UNIX AND NOT APPLE)
    set_target_properties (choppah-${format}
        PROPERTIES
            POSITION_INDEPENDENT_CODE TRUE)

   # set_target_properties (jb_git_version-choppah-${format}
   #     PROPERTIES
   #         POSITION_INDEPENDENT_CODE TRUE)
endif()

endfunction()

if (UNIX AND NOT APPLE)
    #set_target_properties (EmbeddedBinaryData
    #    PROPERTIES
    #        POSITION_INDEPENDENT_CODE TRUE)
endif()

add_choppah_version (VST3
        # A four-character manufacturer id with at least one upper-case character
        PLUGIN_MANUFACTURER_CODE Sabg
        # A unique four-character plugin id with at least one upper-case character
        PLUGIN_CODE FXSQ

        # When the choppah was released, there was a bug in JUCE that wrongly reported the plugin code "proj". Applying this
        # to not break existing sessions
        USE_LEGACY_COMPATIBILITY_PLUGIN_CODE TRUE

        VST3_CATEGORIES Fx)

        target_compile_definitions (choppah-VST3 PRIVATE JUCE_DISPLAY_SPLASH_SCREEN=0)

#add_choppah_version (LV2
        # A four-character manufacturer id with at least one upper-case character
#        PLUGIN_MANUFACTURER_CODE Sabg
        # A unique four-character plugin id with at least one upper-case character
#        PLUGIN_CODE FXSQ

 #       LV2URI "http://github.com/ssabug/choppah"
#        LVZ_SHARED_LIBRARY_NAME choppah)
 #       target_compile_definitions (choppah-LV2 PRIVATE JUCE_DISPLAY_SPLASH_SCREEN=0)

if (APPLE)
add_choppah_version (AU
        # A four-character manufacturer id with at least one upper-case character
        PLUGIN_MANUFACTURER_CODE Sabg
        # A unique four-character plugin id with at least one upper-case character
        PLUGIN_CODE Fxsq

        AU_MAIN_TYPE kAudioUnitType_Effect)
        target_compile_definitions (choppah-AU PRIVATE JUCE_DISPLAY_SPLASH_SCREEN=0)
endif()

if (AAX_SDK_PATH)

    if (WIN32)
        # Extra escaping is needed to not mess up the macro inside the AAX wrapper with the path created by the
        # GitHub Actions runner
        string (REPLACE "\\" "\\\\" AAX_SDK_PATH ${AAX_SDK_PATH})
    endif()

    juce_set_aax_sdk_path ("${AAX_SDK_PATH}")
    add_choppah_version (AAX
            # A four-character manufacturer id with at least one upper-case character
            PLUGIN_MANUFACTURER_CODE Sabg
            # A unique four-character plugin id with at least one upper-case character
            PLUGIN_CODE Fxsq

            AAX_IDENTIFIER io.ssabug.choppah
            AAX_CATEGORY AAX_ePlugInCategory_Harmonic)

    set_target_properties(choppah-AAX_AAX PROPERTIES OSX_ARCHITECTURES x86_64)
    
    target_compile_definitions (choppah-AAX
        PRIVATE
            JUCE_DISPLAY_SPLASH_SCREEN=1
            _CRT_SECURE_NO_WARNINGS=1)
endif()

add_library (choppah-ALL_FORMATS INTERFACE)
target_link_libraries (choppah-ALL_FORMATS
    INTERFACE
    choppah-AU_AU
    choppah-VST3_VST3
    #choppah-LV2_LV2
    choppah-AAX_AAX)