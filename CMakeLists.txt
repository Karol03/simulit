cmake_minimum_required(VERSION 3.16)

project(simulit VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Quick QuickControls2 Gui)

include_directories(src)

qt_standard_project_setup(REQUIRES 6.8)

add_subdirectory(api)
add_subdirectory(simulations)

qt_add_executable(appsimulit
    main.cpp

    src/simulationhandler.hpp
    src/simulationhandler.cpp

    src/providers/iprovider.hpp
    src/providers/properties.hpp
    src/providers/properties.cpp
    src/providers/simulations.hpp
    src/providers/simulations.cpp
    src/providers/statistics.hpp
    src/providers/statistics.cpp

    src/loaders/iloader.hpp
    src/loaders/dllloader.hpp
    src/loaders/dllloader.cpp

    src/adapters/iadapter.hpp
    src/adapters/statistic.hpp
    src/adapters/property.hpp
    src/adapters/simulationrecord.hpp

    src/controllers/icontroller.hpp
    src/controllers/controllerstate.hpp
    src/controllers/simplecontroller.hpp
    src/controllers/simplecontroller.cpp
    src/controllers/animatedcontroller.hpp
    src/controllers/animatedcontroller.cpp

    src/workers/iworker.hpp
    src/workers/iworkerhandler.hpp
    src/workers/pool.hpp
    src/workers/pool.cpp
    src/workers/workerhandler.hpp
    src/workers/workerhandler.cpp
    src/workers/workerhandlerfactory.hpp
    src/workers/workerhandlerfactory.cpp

    src/tools/randomnumbergenerator.cpp
    src/tools/randomnumbergenerator.hpp
    src/tools/determinenumbergenerator.cpp
    src/tools/determinenumbergenerator.hpp
    src/tools/numbergeneratorfactory.cpp
    src/tools/numbergeneratorfactory.hpp

    resources.qrc
)

qt_add_qml_module(appsimulit
    URI simulit
    QML_FILES
        gui/Main.qml
        gui/MenuButton.qml
        gui/PropertyItem.qml
        gui/PropertyNodeDelegate.qml
        gui/StatisticItem.qml

        gui/controllers/SimpleController.qml
        gui/controllers/AnimatedController.qml
        gui/controllers/FocalButton.qml
        gui/controllers/AssistButton.qml
    SOURCES
        src/controllers/controllerstate.hpp
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appsimulit PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appsimulit
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# required include for enum generation
target_include_directories(appsimulit
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src/controllers
)

target_link_libraries(appsimulit
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Gui
        ${CMAKE_PROJECT_NAME}-api
)

include(GNUInstallDirs)
install(TARGETS appsimulit
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
