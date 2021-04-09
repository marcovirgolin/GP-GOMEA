add_library_unity(gpgomea_evolution OBJECT EvolutionRun.cpp EvolutionState.cpp GenerationHandler.cpp PopulationInitializer.cpp)
set(ALL_OBJECT_FILES
    ${ALL_OBJECT_FILES} $<TARGET_OBJECTS:gpgomea_evolution>
    PARENT_SCOPE)

