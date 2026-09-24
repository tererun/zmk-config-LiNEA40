if(CONFIG_LINEA40_POINTING_CONTROLS)
  target_include_directories(app PRIVATE include)
  target_sources(app PRIVATE
    src/behavior_pointing_control.c
    src/input_processor_pointing_control.c
    src/pointing_control_state.c
  )
endif()
