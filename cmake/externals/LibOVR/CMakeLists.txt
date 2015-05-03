include(ExternalProject)
include(SelectLibraryConfigurations)

set(EXTERNAL_NAME LibOVR)

string(TOUPPER ${EXTERNAL_NAME} EXTERNAL_NAME_UPPER)

if (WIN32)

  ExternalProject_Add(
    ${EXTERNAL_NAME}
    URL http://static.oculus.com/sdk-downloads/0.6.0.1/Public/1435190862/ovr_sdk_win_0.6.0.1.zip
    URL_MD5 4b3ef825f9a1d6d3035c9f6820687da9
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ""
    LOG_DOWNLOAD 1
  )

  ExternalProject_Get_Property(${EXTERNAL_NAME} SOURCE_DIR)

  # FIXME need to account for different architectures 
  set(${EXTERNAL_NAME_UPPER}_INCLUDE_DIRS ${SOURCE_DIR}/LibOVR/Include CACHE TYPE INTERNAL)
  if ("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
    set(${EXTERNAL_NAME_UPPER}_LIBRARIES ${SOURCE_DIR}/LibOVR/Lib/Windows/x64/Release/VS2013/LibOVR.lib CACHE TYPE INTERNAL)
  else()
    set(${EXTERNAL_NAME_UPPER}_LIBRARIES ${SOURCE_DIR}/LibOVR/Lib/Windows/Win32/Release/VS2013/LibOVR.lib CACHE TYPE INTERNAL)
  endif()

else()

  message(FATAL "Oculus SDK 0.6 and above supported only on windows")

endif()

# Hide this external target (for ide users)
set_target_properties(${EXTERNAL_NAME} PROPERTIES FOLDER "hidden/externals")


