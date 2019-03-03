##---------------------------------------------------------------------------
## Author:      Pavel Kalian
## Copyright:   
## License:     wxWidgets License
##---------------------------------------------------------------------------

# define minimum cmake version
CMAKE_MINIMUM_REQUIRED(VERSION 3.1.0)

PROJECT(ocpnnmea2000_pi)

SET(PACKAGE_NAME ocpnnmea2000_pi)
SET(VERBOSE_NAME OpenCPNnmea2000)
SET(TITLE_NAME OpenCPNnmea2000)
SET(CPACK_PACKAGE_CONTACT "Manuel Bouyer")

SET(VERSION_MAJOR "0")
SET(VERSION_MINOR "5")

#SET(CMAKE_BUILD_TYPE Debug)

# OPTION(OCPNNMEA2000_USE_SVG "Use SVG graphics" ON)

INCLUDE("cmake/PluginConfigure.cmake")

IF(OCPNNMEA2000_USE_SVG)
  ADD_DEFINITIONS(-DOCPNNMEA2000_USE_SVG)
ENDIF(OCPNNMEA2000_USE_SVG)

SET(SRC_OCPNNMEA2000
            src/ocpnnmea2000_pi.h
            src/ocpnnmea2000_pi.cpp
            src/NMEA2000Properties.h
            src/NMEA2000Properties.cpp
            src/NMEA2000PropertiesDialog.h
            src/NMEA2000PropertiesDialog.cpp
	    src/NMEA2000.h
	    src/NMEA2000.cpp
	    src/nmea2000_defs.h
	    src/nmea2000_defs_rx.h
	    src/nmea2000_defs_tx.h
	    src/nmea2000_frame.h
	    src/nmea2000_rxtx.cpp
	    src/nmea2000_datetime_tx.cpp
	    src/nmea2000_sogcog_tx.cpp
	    src/nmea2000_navdata_tx.cpp
	    src/nmea2000_xte_tx.cpp
	    src/nmea2000_attitude_rx.cpp
        )

SET(SRC_NMA0183
	src/nmea0183/nmea0183.h
	src/nmea0183/nmea0183.hpp
	src/nmea0183/LatLong.hpp
	src/nmea0183/Response.hpp
	src/nmea0183/Sentence.hpp
	src/nmea0183/apb.hpp
	src/nmea0183/rmb.hpp
	src/nmea0183/hdm.hpp
	src/nmea0183/expid.cpp
	src/nmea0183/hexvalue.cpp
	src/nmea0183/latlong.cpp
	src/nmea0183/lat.cpp
	src/nmea0183/long.cpp
	src/nmea0183/nmea0183.cpp
	src/nmea0183/response.cpp
	src/nmea0183/sentence.cpp
	src/nmea0183/talkerid.cpp
	src/nmea0183/apb.cpp
	src/nmea0183/rmb.cpp
	src/nmea0183/hdm.cpp
    )

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/nmea0183)

IF(UNIX)
 INCLUDE("cmake/FindTinyXML.cmake")
 FIND_PACKAGE(TinyXML QUIET)
ENDIF(UNIX)

IF(TINYXML_FOUND)
 message (STATUS "Building with system tinyxml")
 INCLUDE_DIRECTORIES(${TINYXML_INCLUDE_DIR})
 ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_OCPNNMEA2000} ${SRC_NMA0183})
 TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${TINYXML_LIBRARIES})
ELSE(TINYXML_FOUND)
 message (STATUS "Building with embedded tinyxml")
 INCLUDE_DIRECTORIES(src/tinyxml/)
 SET(SRC_LTINYXML
            src/tinyxml/tinyxml.cpp
            src/tinyxml/tinyxmlparser.cpp
            src/tinyxml/tinyxmlerror.cpp
 )
 ADD_LIBRARY(${PACKAGE_NAME} SHARED ${SRC_OCPNNMEA2000} ${SRC_NMA0183} ${SRC_LTINYXML})
ENDIF(TINYXML_FOUND)

set_property(TARGET ${PACKAGE_NAME} PROPERTY CXX_STANDARD 11)

INCLUDE("cmake/PluginInstall.cmake")
INCLUDE("cmake/PluginLocalization.cmake")
INCLUDE("cmake/PluginPackage.cmake")
