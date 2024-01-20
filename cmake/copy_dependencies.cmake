cmake_minimum_required(VERSION 3.16)

file(GET_RUNTIME_DEPENDENCIES
  LIBRARIES ${library_target_list}
  RESOLVED_DEPENDENCIES_VAR resolved
  UNRESOLVED_DEPENDENCIES_VAR unresolved
  PRE_EXCLUDE_REGEXES ".*"
  PRE_INCLUDE_REGEXES ${include_libraries_list}
  POST_EXCLUDE_REGEXES ".*"
  POST_INCLUDE_REGEXES ${include_libraries_list}
)

foreach (_file ${resolved})
  get_filename_component(_filename ${_file} NAME)
  string(TOLOWER "{_filename}" _filename)
  file(COPY "${_file}" DESTINATION ${copy_to_dir} FOLLOW_SYMLINK_CHAIN)
endforeach()

list(LENGTH unresolved _u_length)
if(_u_length GREATER 0)
  message(WARNING "Unable to resolve dependency: ${unresolved}")
endif()