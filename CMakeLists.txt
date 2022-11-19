cmake_minimum_required(VERSION 3.5)

project(soqt VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS SerialPort)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Network)

include_directories(srcs/)

set(PROJECT_SOURCES
        srcs/main.cpp
        srcs/mainwindow.cpp
        srcs/mainwindow.h
        srcs/mainwindow.ui

        srcs/portview.h
        srcs/portview.cpp

        srcs/myscene.h
        srcs/myscene.cpp

        ports/genericport.h
        ports/genericport.cpp

        srcs/textdisplay.cpp
        srcs/textdisplay.h
        srcs/grepper.cpp
        srcs/grepper.h

        srcs/portsettingdialog.cpp
        srcs/portsettingdialog.h
        srcs/portsettingdialog.ui

#        arrow.h arrow.cpp
#        diagramitem.h diagramitem.cpp

        #Serial Stuff
        ports/serial/widgeserial.h
        ports/serial/widgeserial.cpp
        ports/serial/widgeserial.ui
        ports/serial/widgelinuxserial.h
        ports/serial/widgelinuxserial.cpp
        ports/serial/widgelinuxserial.ui
        ports/serial/serial_reader.h
        ports/serial/serial_reader.cpp
        ports/serial/widgevirtualserial.h
        ports/serial/widgevirtualserial.cpp
        ports/serial/widgevirtualserial.ui

        #UDP Stuff
        ports/udp/widgeudp.h
        ports/udp/widgeudp.cpp
        ports/udp/widgeudp.ui

        #TCP Stuff
        ports/tcp/widgetcp.h
        ports/tcp/widgetcp.cpp
        ports/tcp/widgetcp.ui

        #Gate stuff
        ports/gate/widgegate.h
        ports/gate/widgegate.cpp
        ports/gate/widgegate.ui

        #File Stuff
        ports/file/widgefilehandler.h
        ports/file/widgefilehandler.cpp
        ports/file/widgefilehandler.ui

        #Trigger Stuff
        ports/trigger/widgetrigger.h
        ports/trigger/widgetrigger.cpp
        ports/trigger/widgetrigger.ui
        ports/trigger/widgetriggerwait.h
        ports/trigger/widgetriggerwait.cpp
        ports/trigger/widgetriggerwait.ui
        ports/trigger/generictrigger.h
        ports/trigger/generictrigger.cpp
        ports/trigger/widgetriggersend.h
        ports/trigger/widgetriggersend.cpp
        ports/trigger/widgetriggersend.ui
        ports/trigger/widgetriggerlisten.h
        ports/trigger/widgetriggerlisten.cpp
        ports/trigger/widgetriggerlisten.ui
        ports/trigger/widgetriggergoto.h
        ports/trigger/widgetriggergoto.cpp
        ports/trigger/widgetriggergoto.ui
        ports/trigger/widgetriggertag.h
        ports/trigger/widgetriggertag.cpp
        ports/trigger/widgetriggertag.ui
        ports/trigger/widgetriggerpause.h
        ports/trigger/widgetriggerpause.cpp
        ports/trigger/widgetriggerpause.ui

        srcs/general_functions.h
        srcs/general_functions.cpp

        resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(soqt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET soqt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(soqt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(soqt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(soqt
    PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Svg
    PRIVATE Qt${QT_VERSION_MAJOR}::SerialPort
    PRIVATE Qt${QT_VERSION_MAJOR}::Network)

set_target_properties(soqt PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(soqt)
endif()
