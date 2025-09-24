add_library_unity(gpgomea_selection OBJECT TournamentSelection.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:gpgomea_selection>
    PARENT_SCOPE)

