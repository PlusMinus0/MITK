OPTION(BUILD_PhotoacousticBeamformingTool "Build MiniApp for beamforming of a PA image" ON)

IF(BUILD_PhotoacousticBeamformingTool)
  PROJECT( MitkPABeamformingTool )
    mitk_create_executable(PABeamformingTool
      DEPENDS MitkCommandLine MitkCore MitkPhotoacousticsAlgorithms
      PACKAGE_DEPENDS
      CPP_FILES PABeamformingTool.cpp)

  install(TARGETS ${EXECUTABLE_TARGET} RUNTIME DESTINATION bin)
 ENDIF()
