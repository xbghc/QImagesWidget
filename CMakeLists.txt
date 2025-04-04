cmake_minimum_required(VERSION 3.16)

project(QImagesWidget VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FFTW3_DIR "D:/tools/vcpkg/installed/x64-windows/share/fftw3")
find_package(FFTW3 CONFIG REQUIRED)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        qimageswidget.cpp
        qimageswidget.h

)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(QImagesWidget
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mrdparser.h mrdparser.cpp
        utils.cpp
        utils.h
        qimageswidget.ui
        icons.qrc
        contentwidget.h contentwidget.cpp
        components/qcheckcombobox.h components/qcheckcombobox.cpp
        components/qcheckcombobox.qrc

    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET QImagesWidget APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

else()
    if(ANDROID)
        add_library(QImagesWidget SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(QImagesWidget
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_include_directories(QImagesWidget PUBLIC
    ${FFTW3_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(QImagesWidget PRIVATE
    FFTW3::fftw3
    Qt${QT_VERSION_MAJOR}::Widgets
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.QImagesWidget)
endif()
set_target_properties(QImagesWidget PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS QImagesWidget
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(QImagesWidget)
endif()
