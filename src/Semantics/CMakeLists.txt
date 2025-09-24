add_library_unity(gpgomea_semantics OBJECT SemanticBackpropagator.cpp SemanticLibrary.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:gpgomea_semantics>
    PARENT_SCOPE)

