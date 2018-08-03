
get_filename_component(SOURCE_ROOT ${CMAKE_CURRENT_LIST_FILE}/../.. ABSOLUTE)
if(${SOURCE_ROOT} STREQUAL ${CMAKE_SOURCE_DIR})
  set(ALL_IN_ONE_MODE 1)
endif()
file(RELATIVE_PATH PROJECT_PATH ${SOURCE_ROOT} ${CMAKE_SOURCE_DIR})

function(add_executable EXENAME)
  add_target("EXECUTABLE" ${EXENAME} ${ARGN})
endfunction()

function(add_library LIBNAME)
  add_target("LIBRARY" ${LIBNAME} ${ARGN})
endfunction()

function(add_static_library LIBNAME)
  add_target("STATIC_LIBRARY" ${LIBNAME} ${ARGN})
endfunction()

function(add_shared_library LIBNAME)
  add_target("SHARED_LIBRARY" ${LIBNAME} ${ARGN})
endfunction()

function(add_target TARGETTYPE TARGETNAME)
  parse_arguments(THIS_TARGET
    "SOURCES;SOURCE_DIRS;RESOURCE_DIRS;PUBLIC_HEADERS;PUBLIC_HEADER_DIRS;DEPENDS;LINK_LIBS;COMPONENT;INSTALL_DIR;SKIP_INSTALL;HUDSON_JOB;EXCLUDED_SOURCES;TEST_SOURCE_DIRS;TEST_EXCLUDED_SOURCES;TEST_LINK_LIBS;DEPEND_HEADERS;MORE_DEPENDS"
    ""
    ${ARGN}
    )
  list(APPEND THIS_TARGET_SOURCES ${THIS_TARGET_DEFAULT_ARGS})

  # source can be set by left arguments or srcdirs or default 'src' and '.' dir
    if(THIS_TARGET_SOURCE_DIRS)
      foreach(THIS_TARGET_SOURCE_DIR ${THIS_TARGET_SOURCE_DIRS})
        message(STATUS "${TARGETNAME}: Found sources at ${THIS_TARGET_SOURCE_DIR}")
        aux_source_directory(${THIS_TARGET_SOURCE_DIR} THIS_TARGET_SOURCES)
      endforeach()
      include_directories(${THIS_TARGET_SOURCE_DIRS})
  endif()

  if(THIS_TARGET_SOURCES)
    # do nothing
  else()
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/src/)
      message(STATUS "${TARGETNAME}: Found sources at 'src'")
      aux_source_directory(src THIS_TARGET_SOURCES)
      include_directories(src)
    else()
      message(STATUS "${TARGETNAME}: Found sources at '.'")
      aux_source_directory(. THIS_TARGET_SOURCES)
      include_directories(.)
    endif()
  endif()
  
  # exclude unusable sources
  if(THIS_TARGET_EXCLUDED_SOURCES)
    list(REMOVE_ITEM THIS_TARGET_SOURCES ${THIS_TARGET_EXCLUDED_SOURCES})
  endif()  

  # set THIS_TARGET_PUBLIC_HEADER_PATHS to the set of all its
  # public headers path. Default find public headers from 'include'
  if(THIS_TARGET_PUBLIC_HEADERS)
    foreach(PUBLIC_HEADER ${THIS_TARGET_PUBLIC_HEADERS})
      get_filename_component(PUBLIC_HEADER_PATH ${PUBLIC_HEADER}/.. ABSOLUTE)
      list(FIND THIS_TARGET_PUBLIC_HEADER_PATHS ${PUBLIC_HEADER_PATH} HEADER_PATH_INDEX)
      if (HEADER_PATH_INDEX EQUAL -1)
        list(APPEND THIS_TARGET_PUBLIC_HEADER_PATHS ${PUBLIC_HEADER_PATH})
      endif()
    endforeach()
  endif()
  if(NOT THIS_TARGET_PUBLIC_HEADER_DIRS)
    # try to find public headers from 'include'
    if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/include/)
      set(THIS_TARGET_PUBLIC_HEADER_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include/)
    endif()
  endif()
  # set public headers from THIS_TARGET_PUBLIC_HEADER_DIRS
	file(GLOB_RECURSE THIS_TARGET_PUBLIC_HEADERS_INCLUDE
  	${THIS_TARGET_PUBLIC_HEADER_DIRS} *.h *.hh )
	list(APPEND THIS_TARGET_PUBLIC_HEADERS ${THIS_TARGET_PUBLIC_HEADERS_INCLUDE})
  # set public header paths
  foreach(PUBLIC_HEADER_DIR ${THIS_TARGET_PUBLIC_HEADER_DIRS})
    get_filename_component(PUBLIC_HEADER_PATH ${PUBLIC_HEADER_DIR} ABSOLUTE)
    list(APPEND THIS_TARGET_PUBLIC_HEADER_PATHS ${PUBLIC_HEADER_PATH})
  endforeach()
  message(STATUS "${TARGETNAME}: Found public headers at ${THIS_TARGET_PUBLIC_HEADER_PATHS}")
    
  # find all dependencies transitively.
  if(THIS_TARGET_MORE_DEPENDS)
    list(APPEND THIS_TARGET_DEPENDS ${THIS_TARGET_MORE_DEPENDS})
  endif()
  list(APPEND THIS_TARGET_INCLUDE_DIRS ${THIS_TARGET_PUBLIC_HEADER_PATHS})
  foreach(DEP ${THIS_TARGET_DEPENDS})
 	# load dependence project setting
  	FIND_PACKAGE(${DEP})
  		
	# add the include dirs of dependence projects
    get_target_property(DEP_INCLUDE_DIRS ${DEP} INCLUDE_DIRS)
    if(DEP_INCLUDE_DIRS)
      foreach(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIRS})
        list(FIND THIS_TARGET_INCLUDE_DIRS ${DEP_INCLUDE_DIR} FOUND_INDEX)
        if (FOUND_INDEX EQUAL -1)
          list(APPEND THIS_TARGET_INCLUDE_DIRS ${DEP_INCLUDE_DIR})
        endif()
      endforeach()
    endif()
    
	# Set THIS_TARGET_DEPENDS_ALL to the set of all of its
	# dependencies, its dependencies' dependencies, etc., transitively.
    get_target_property(DEP_DEPENDS_ALL ${DEP} DEPENDS_ALL)
    if(DEP_DEPENDS_ALL)
      list(APPEND DEP_DEPENDS_ALL ${DEP})
    else()
      set(DEP_DEPENDS_ALL ${DEP})
    endif()
    foreach(DEP_DEPEND ${DEP_DEPENDS_ALL})
    	list(FIND THIS_TARGET_DEPENDS_ALL ${DEP_DEPEND} DEPDEP_INDEX)
    	if (DEPDEP_INDEX EQUAL -1)
     		# load transitive dependence project setting
    		FIND_PACKAGE(${DEP_DEPEND})
      	    list(APPEND THIS_TARGET_DEPENDS_ALL ${DEP_DEPEND})
    	endif()
    endforeach()	      		
  endforeach() 

  foreach(DEP ${THIS_TARGET_DEPEND_HEADERS})
    # load dependence project setting
    FIND_PACKAGE(${DEP})
        
    # add the include dirs of dependence projects
    get_target_property(DEP_INCLUDE_DIRS ${DEP} INCLUDE_DIRS)
    if(DEP_INCLUDE_DIRS)
      foreach(DEP_INCLUDE_DIR ${DEP_INCLUDE_DIRS})
        list(FIND THIS_TARGET_INCLUDE_DIRS ${DEP_INCLUDE_DIR} FOUND_INDEX)
        if (FOUND_INDEX EQUAL -1)
          list(APPEND THIS_TARGET_INCLUDE_DIRS ${DEP_INCLUDE_DIR})
        endif()
      endforeach()
    endif()
  endforeach() 

  message(STATUS "${TARGETNAME}: DEPENDS=${THIS_TARGET_DEPENDS}, DEPENDS_ALL=${THIS_TARGET_DEPENDS_ALL}, LINK_LIBS=${THIS_TARGET_LINK_LIBS}, INCLUDE_DIRS=${THIS_TARGET_INCLUDE_DIRS}")

  # add target by type
  if (TARGETTYPE STREQUAL "EXECUTABLE")
    add_executable(${TARGETNAME} ${THIS_TARGET_SOURCES})
  elseif (TARGETTYPE STREQUAL "STATIC_LIBRARY")
    add_library(${TARGETNAME} STATIC ${THIS_TARGET_SOURCES})
  elseif (TARGETTYPE STREQUAL "SHARED_LIBRARY")
    add_library(${TARGETNAME} SHARED ${THIS_TARGET_SOURCES})
  elseif (TARGETTYPE STREQUAL "MODULE_LIBRARY")
    add_library(${TARGETNAME} MODULE ${THIS_TARGET_SOURCES})
  else()
    add_library(${TARGETNAME} ${THIS_TARGET_SOURCES})
  endif()
    
  # set dependence include directories and link libs
  include_directories(${THIS_TARGET_INCLUDE_DIRS})
  target_link_libraries (${TARGETNAME} ${THIS_TARGET_DEPENDS} ${THIS_TARGET_LINK_LIBS})
endfunction()
