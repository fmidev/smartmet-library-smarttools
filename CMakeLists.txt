if(WIN32)
  cmake_minimum_required(VERSION 3.14.4)
  project(SmartTools)

  set(CMAKE_MFC_FLAG 2)

  add_definitions(
      -DFMI_SUPPORT_STATION_DATA_SMARTTOOL
      )

  file( GLOB SOURCE_FILES smarttools/* )

  add_library( 
      ${PROJECT_NAME} STATIC
      ${SOURCE_FILES} 
      )

# Remove comments for CatLog library, if you need do deep trace 
# debugging with smarttool language.
	
  target_link_libraries(
      ${PROJECT_NAME}
      Newbase
#    CatLog
      )

  target_include_directories( 
      ${PROJECT_NAME}
      PUBLIC ${PROJECT_SOURCE_DIR}/smarttools
      PUBLIC ${Newbase_INCLUDE_DIRS}
      PUBLIC ${BOOST_INCLUDE_DIR}
#    PUBLIC ${CatLog_INCLUDE_DIRS}
      )

  set(
      ${PROJECT_NAME}_INCLUDE_DIRS
      ${PROJECT_SOURCE_DIR}/smarttools
      ${PROJECT_SOURCE_DIR}
      ${Newbase_INCLUDE_DIRS}
      ${BOOST_INCLUDE_DIR}
#    ${CatLog_INCLUDE_DIRS}
      CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
      )
else()
  cmake_minimum_required(VERSION 3.7.1)
  project(SmartTools)

  add_compile_options(-std=c++11 -DUNIX)

  add_definitions(
      -DFMI_SUPPORT_STATION_DATA_SMARTTOOL
      )

  file( GLOB SOURCE_FILES smarttools/* )

  add_library( 
      ${PROJECT_NAME} STATIC
      ${SOURCE_FILES} 
      )

  if (NOT Boost_INCLUDE_DIRS)
      set(Boost_USE_STATIC_LIBS ON)
      set(Boost_USE_MULTITHREADED ON)
      set(Boost_USE_STATIC_RUNTIME OFF)
      find_package(Boost 1.60.0 QUIET COMPONENTS filesystem thread)
      if(Boost_FOUND)
          include_directories(${Boost_INCLUDE_DIRS})
          target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
      endif ()
  endif ()

  if (NOT Newbase_INCLUDE_DIRS)
      find_package(Newbase QUIET)
      if (Newbase_FOUND)
          include_directories(${Newbase_INCLUDE_DIRS})
          target_link_libraries(${PROJECT_NAME} ${Newbase_LIBRARIES})
      endif ()
  endif ()

# Uninstall old files
  find_package(SmartTools QUIET)
  if (SmartTools_FOUND)
      include(${SmartTools_CONFIG})
      file(GLOB INSTALLED_FILES
      ${SmartTools_DIR}
      ${SmartTools_INSTALLED_INCLUDE_DIRS}
      ${SmartTools_INSTALLED_LIBRARIES})
      add_custom_target(uninstall rm -rf ${INSTALLED_FILES})
  endif()

  target_link_libraries(
      ${PROJECT_NAME}
      Newbase
      )

  target_include_directories( 
      ${PROJECT_NAME}
      PUBLIC ${PROJECT_SOURCE_DIR}/smarttools
      PUBLIC ${Newbase_INCLUDE_DIRS}
      PUBLIC ${BOOST_INCLUDE_DIR}
      )

  set(
      ${PROJECT_NAME}_INCLUDE_DIRS
      ${PROJECT_SOURCE_DIR}/smarttools
      ${PROJECT_SOURCE_DIR}
      ${Newbase_INCLUDE_DIRS}
      ${BOOST_INCLUDE_DIR}
      CACHE INTERNAL "${PROJECT_NAME}: Include Directories" FORCE
      )

  install(DIRECTORY smarttools/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include/smarttools
    FILES_MATCHING PATTERN "*.h")

  install(TARGETS ${PROJECT_NAME} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)

  install(DIRECTORY cmake/ DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/smarttools
    FILES_MATCHING PATTERN "*onfig.cmake")
endif()
