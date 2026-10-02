if(CONFIG_LINEA40_POINTING_CONTROLS)
  target_include_directories(app PRIVATE include)
  target_sources(app PRIVATE
    src/behavior_pointing_control.c
    src/input_processor_pointing_control.c
    src/pointing_control_state.c
  )
endif()

if(CONFIG_PMW3610)
  zephyr_library_named(linea40_pmw3610)
  zephyr_library_sources(drivers/pmw3610/src/pmw3610.c)
  zephyr_library_include_directories(
    drivers/pmw3610/src
    ${APPLICATION_SOURCE_DIR}/include
  )
endif()
