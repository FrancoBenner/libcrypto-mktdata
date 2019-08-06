# This module tries to find IXWebSocket library and include files
#
# DATE_INCLUDE_DIR, path where to find IXWebSocket.h
# DATE_LIBRARY_DIR, path where to find libixwebsocket.so
# DATE_LIBRARIES, the library to link against
# DATE_FOUND, If false, do not try to use IXWebSocket
#
# This currently works probably only for Linux

include(FindPackageHandleStandardArgs)
SET ( DATE_FOUND FALSE )

FIND_PATH ( DATE_INCLUDE_DIR NAMES date/date.h
        HINTS /usr/local/include /usr/include
        )

FIND_LIBRARY ( DATE_LIBRARIES NAMES tz libtz
        HINTS /usr/local/lib /usr/lib
        )

GET_FILENAME_COMPONENT( DATE_LIBRARY_DIR ${DATE_LIBRARIES} PATH )

IF ( DATE_INCLUDE_DIR )
    IF ( DATE_LIBRARIES )
        SET ( DATE_FOUND TRUE )
    ENDIF ( DATE_LIBRARIES )
ENDIF ( DATE_INCLUDE_DIR )


IF ( DATE_FOUND )
    MARK_AS_ADVANCED(
            DATE_LIBRARY_DIR
            DATE_INCLUDE_DIR
            DATE_LIBRARIES
    )
ENDIF ( )

find_package_handle_standard_args(DATE
        DEFAULT_MSG
        DATE_INCLUDE_DIR
        DATE_LIBRARIES)