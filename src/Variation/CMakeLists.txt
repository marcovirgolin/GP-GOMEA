add_library_unity(gpgomea_variation OBJECT SubtreeVariator.cpp TreeInitializer.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:gpgomea_variation>
    PARENT_SCOPE)

