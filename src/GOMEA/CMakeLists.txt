add_library_unity(gpgomea_gomea OBJECT GOMEAFOS.cpp GOMEAGenerationHandler.cpp GOMEATreeInitializer.cpp GOMVariator.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:gpgomea_gomea>
    PARENT_SCOPE)

