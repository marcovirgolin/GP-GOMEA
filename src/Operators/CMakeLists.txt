add_library_unity(gpgomea_operators OBJECT Operator.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:gpgomea_operators>
    PARENT_SCOPE)

