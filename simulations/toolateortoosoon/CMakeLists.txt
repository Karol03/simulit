qt_add_library(toolateortoosoon SHARED)
qt_add_qml_module(toolateortoosoon
    URI toolateortoosoon
    VERSION 1.0
    SOURCES
        toolateortoosoon.cpp
        toolateortoosoon.h
)

set_target_properties(toolateortoosoon PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_compile_definitions(toolateortoosoon
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(toolateortoosoon
    PRIVATE
        Qt6::Quick
        Qt6::Gui
        simulit-api
)

target_include_directories(toolateortoosoon
    PUBLIC
        ${PROJECT_SOURCE_DIR}/api
)

if (WIN32)
    add_custom_command(TARGET toolateortoosoon POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "$<TARGET_FILE:toolateortoosoon>"
            "${CMAKE_CURRENT_BINARY_DIR}/../toolateortoosoon.dll"
    )
endif()
