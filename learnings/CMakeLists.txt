option(build_learnings_tobias "If ON, build learnings in the `tobias` directory." OFF)

if(build_learnings_tobias)
  add_subdirectory(tobias)
endif()