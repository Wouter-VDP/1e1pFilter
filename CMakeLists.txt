# ======================================================================
#  pandoraElectronFilter main build file
#
#  cd .../path/to/build/directory
#  source .../path/to/mypandoraanalysis/ups/setup_for_development <-d|-p>
#  cmake [-DCMAKE_INSTALL_PREFIX=/install/path]
#        -DCMAKE_BUILD_TYPE=$CETPKG_TYPE
#        $CETPKG_SOURCE
#  make
#  make test
#  make install
#  make package (builds distribution tarfile)
# ======================================================================


# use cmake 2.8 or later
cmake_minimum_required (VERSION 2.8)

project(pandoraElectronFilter)

# cetbuildtools contains our cmake modules
SET ( CETBUILDTOOLS_VERSION $ENV{CETBUILDTOOLS_VERSION} )
IF (NOT CETBUILDTOOLS_VERSION)
    MESSAGE (FATAL_ERROR "ERROR: setup cetbuildtools to get the cmake modules")
ENDIF()

set(CMAKE_MODULE_PATH $ENV{ART_DIR}/Modules
		      $ENV{CETBUILDTOOLS_DIR}/Modules
		      ${CMAKE_MODULE_PATH})

include(CetCMakeEnv)
cet_cmake_env()

cet_set_compiler_flags(DIAGS CAUTIOUS
  WERROR
  NO_UNDEFINED
  ALLOW_DEPRECATIONS
  EXTRA_FLAGS -pedantic -Wno-unused-local-typedefs
)

cet_report_compiler_flags()

# these are minimum required versions, not the actual product versions
find_ups_product( larcore v1_00_00 )
find_ups_product( lardata v1_00_00 )
find_ups_product( larevt v1_00_00 )
find_ups_product( larsim v1_00_00 )
find_ups_product( larpandora v1_00_00 )
find_ups_product( nutools v1_00_00 )
find_ups_product( boost v1_00_00 )
find_ups_product( art v1_09_00 )
find_ups_product( uboonecode v1_00_00 )
find_ups_product( cetbuildtools v4_09_00 )
find_ups_product( gsl )

# macros for dictionary and simple_plugin
include(ArtDictionary)
include(ArtMake)
include(BuildPlugins)

# source
add_subdirectory(pandoraElectronFilter)
#add_subdirectory(flashChargeAnalyzer)

# ups - table and config files
add_subdirectory(ups)

# packaging utility
include(UseCPack)
