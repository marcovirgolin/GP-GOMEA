#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/61f3bb6/main.o \
	${OBJECTDIR}/Evolution/EvolutionRun.o \
	${OBJECTDIR}/Evolution/EvolutionState.o \
	${OBJECTDIR}/Evolution/GenerationHandler.o \
	${OBJECTDIR}/Evolution/PopulationInitializer.o \
	${OBJECTDIR}/Fitness/AccuracyFitness.o \
	${OBJECTDIR}/Fitness/Fitness.o \
	${OBJECTDIR}/Fitness/SymbolicRegressionFitness.o \
	${OBJECTDIR}/Fitness/SymbolicRegressionLinearScalingFitness.o \
	${OBJECTDIR}/GOMEA/GOMEAFOS.o \
	${OBJECTDIR}/GOMEA/GOMEAGenerationHandler.o \
	${OBJECTDIR}/GOMEA/GOMEATreeInitializer.o \
	${OBJECTDIR}/GOMEA/GOMVariator.o \
	${OBJECTDIR}/Genotype/Node.o \
	${OBJECTDIR}/Operators/Operator.o \
	${OBJECTDIR}/RunHandling/IMSHandler.o \
	${OBJECTDIR}/Selection/TournamentSelection.o \
	${OBJECTDIR}/Semantics/SemanticBackpropagator.o \
	${OBJECTDIR}/Semantics/SemanticLibrary.o \
	${OBJECTDIR}/Utils/KDTree.o \
	${OBJECTDIR}/Utils/Logger.o \
	${OBJECTDIR}/Utils/Utils.o \
	${OBJECTDIR}/Variation/SubtreeVariator.o \
	${OBJECTDIR}/Variation/TreeInitializer.o \
	${OBJECTDIR}/python_exposer.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-fopenmp
CXXFLAGS=-fopenmp

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=`pkg-config --libs python-2.7`  

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/gpgomea

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/gpgomea: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/gpgomea ${OBJECTFILES} ${LDLIBSOPTIONS} -larmadillo -lboost_program_options -lboost_python -lboost_system -lboost_numpy

${OBJECTDIR}/_ext/61f3bb6/main.o: main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/61f3bb6
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/61f3bb6/main.o main.cpp

${OBJECTDIR}/Evolution/EvolutionRun.o: Evolution/EvolutionRun.cpp 
	${MKDIR} -p ${OBJECTDIR}/Evolution
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Evolution/EvolutionRun.o Evolution/EvolutionRun.cpp

${OBJECTDIR}/Evolution/EvolutionState.o: Evolution/EvolutionState.cpp 
	${MKDIR} -p ${OBJECTDIR}/Evolution
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Evolution/EvolutionState.o Evolution/EvolutionState.cpp

${OBJECTDIR}/Evolution/GenerationHandler.o: Evolution/GenerationHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/Evolution
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Evolution/GenerationHandler.o Evolution/GenerationHandler.cpp

${OBJECTDIR}/Evolution/PopulationInitializer.o: Evolution/PopulationInitializer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Evolution
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Evolution/PopulationInitializer.o Evolution/PopulationInitializer.cpp

${OBJECTDIR}/Fitness/AccuracyFitness.o: Fitness/AccuracyFitness.cpp 
	${MKDIR} -p ${OBJECTDIR}/Fitness
	${RM} "$@.d"
	$(COMPILE.cc) -g -Wall -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Fitness/AccuracyFitness.o Fitness/AccuracyFitness.cpp

${OBJECTDIR}/Fitness/Fitness.o: Fitness/Fitness.cpp 
	${MKDIR} -p ${OBJECTDIR}/Fitness
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Fitness/Fitness.o Fitness/Fitness.cpp

${OBJECTDIR}/Fitness/SymbolicRegressionFitness.o: Fitness/SymbolicRegressionFitness.cpp 
	${MKDIR} -p ${OBJECTDIR}/Fitness
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Fitness/SymbolicRegressionFitness.o Fitness/SymbolicRegressionFitness.cpp

${OBJECTDIR}/Fitness/SymbolicRegressionLinearScalingFitness.o: Fitness/SymbolicRegressionLinearScalingFitness.cpp 
	${MKDIR} -p ${OBJECTDIR}/Fitness
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Fitness/SymbolicRegressionLinearScalingFitness.o Fitness/SymbolicRegressionLinearScalingFitness.cpp

${OBJECTDIR}/GOMEA/GOMEAFOS.o: GOMEA/GOMEAFOS.cpp 
	${MKDIR} -p ${OBJECTDIR}/GOMEA
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GOMEA/GOMEAFOS.o GOMEA/GOMEAFOS.cpp

${OBJECTDIR}/GOMEA/GOMEAGenerationHandler.o: GOMEA/GOMEAGenerationHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/GOMEA
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GOMEA/GOMEAGenerationHandler.o GOMEA/GOMEAGenerationHandler.cpp

${OBJECTDIR}/GOMEA/GOMEATreeInitializer.o: GOMEA/GOMEATreeInitializer.cpp 
	${MKDIR} -p ${OBJECTDIR}/GOMEA
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GOMEA/GOMEATreeInitializer.o GOMEA/GOMEATreeInitializer.cpp

${OBJECTDIR}/GOMEA/GOMVariator.o: GOMEA/GOMVariator.cpp 
	${MKDIR} -p ${OBJECTDIR}/GOMEA
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/GOMEA/GOMVariator.o GOMEA/GOMVariator.cpp

${OBJECTDIR}/Genotype/Node.o: Genotype/Node.cpp 
	${MKDIR} -p ${OBJECTDIR}/Genotype
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Genotype/Node.o Genotype/Node.cpp

${OBJECTDIR}/Operators/Operator.o: Operators/Operator.cpp 
	${MKDIR} -p ${OBJECTDIR}/Operators
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Operators/Operator.o Operators/Operator.cpp

${OBJECTDIR}/RunHandling/IMSHandler.o: RunHandling/IMSHandler.cpp 
	${MKDIR} -p ${OBJECTDIR}/RunHandling
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/RunHandling/IMSHandler.o RunHandling/IMSHandler.cpp

${OBJECTDIR}/Selection/TournamentSelection.o: Selection/TournamentSelection.cpp 
	${MKDIR} -p ${OBJECTDIR}/Selection
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Selection/TournamentSelection.o Selection/TournamentSelection.cpp

${OBJECTDIR}/Semantics/SemanticBackpropagator.o: Semantics/SemanticBackpropagator.cpp 
	${MKDIR} -p ${OBJECTDIR}/Semantics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Semantics/SemanticBackpropagator.o Semantics/SemanticBackpropagator.cpp

${OBJECTDIR}/Semantics/SemanticLibrary.o: Semantics/SemanticLibrary.cpp 
	${MKDIR} -p ${OBJECTDIR}/Semantics
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Semantics/SemanticLibrary.o Semantics/SemanticLibrary.cpp

${OBJECTDIR}/Utils/KDTree.o: Utils/KDTree.cpp 
	${MKDIR} -p ${OBJECTDIR}/Utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Utils/KDTree.o Utils/KDTree.cpp

${OBJECTDIR}/Utils/Logger.o: Utils/Logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/Utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Utils/Logger.o Utils/Logger.cpp

${OBJECTDIR}/Utils/Utils.o: Utils/Utils.cpp 
	${MKDIR} -p ${OBJECTDIR}/Utils
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Utils/Utils.o Utils/Utils.cpp

${OBJECTDIR}/Variation/SubtreeVariator.o: Variation/SubtreeVariator.cpp 
	${MKDIR} -p ${OBJECTDIR}/Variation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Variation/SubtreeVariator.o Variation/SubtreeVariator.cpp

${OBJECTDIR}/Variation/TreeInitializer.o: Variation/TreeInitializer.cpp 
	${MKDIR} -p ${OBJECTDIR}/Variation
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/Variation/TreeInitializer.o Variation/TreeInitializer.cpp

${OBJECTDIR}/python_exposer.o: python_exposer.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -O2 -I/usr/include/python2.7 `pkg-config --cflags python-2.7` -std=c++14  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/python_exposer.o python_exposer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/gpgomea

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
