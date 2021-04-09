add_library_unity(gpgomea_utils OBJECT KDTree.cpp Logger.cpp Utils.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:gpgomea_utils>
    PARENT_SCOPE)

