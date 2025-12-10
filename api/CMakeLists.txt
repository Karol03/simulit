qt_add_library(${CMAKE_PROJECT_NAME}-api STATIC
    simulation.hpp
    variable.hpp
    tools.hpp
    utils.hpp
)

target_link_libraries(${CMAKE_PROJECT_NAME}-api
    PUBLIC
        Qt6::Core
        Qt6::Gui
)

target_include_directories(${CMAKE_PROJECT_NAME}-api
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
