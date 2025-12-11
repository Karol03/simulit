qt_add_library(montecarlo SHARED)
qt_add_qml_module(montecarlo
    URI montecarlo
    VERSION 1.0
    SOURCES
        montecarlo.cpp
        montecarlo.h
)

set_target_properties(montecarlo PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(montecarlo
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(montecarlo
    PRIVATE
        Qt6::Quick
        Qt6::Gui
        simulit-api
)

target_include_directories(montecarlo
    PUBLIC
        ${PROJECT_SOURCE_DIR}/api
)

if (WIN32)
    add_custom_command(TARGET montecarlo POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:montecarlo>"
            "${CMAKE_CURRENT_BINARY_DIR}/../montecarlo.dll"
    )
endif()
