add_library_unity(gpgomea_runhandling OBJECT IMSHandler.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:gpgomea_runhandling>
    PARENT_SCOPE)

