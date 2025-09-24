GENERATOR=
FORCE_COLOR=
WARNINGS_AS_ERRORS=
DISABLE_SANITIZER_FLAG=
ifeq ($(GEN),ninja)
        GENERATOR=-G "Ninja"
        FORCE_COLOR=-DFORCE_COLORED_OUTPUT=1
endif
ifeq (${DISABLE_SANITIZER}, 1)
        DISABLE_SANITIZER_FLAG=-DENABLE_SANITIZER=FALSE -DENABLE_UBSAN=0
endif
ifeq (${FORCE_SANITIZER}, 1)
        DISABLE_SANITIZER_FLAG:=${DISABLE_SANITIZER_FLAG} -DFORCE_SANITIZER=1
endif

release:
	mkdir -p build/release && \
        cd build/release && \
        cmake $(GENERATOR) $(FORCE_COLOR) ${DISABLE_SANITIZER_FLAG} -DCMAKE_BUILD_TYPE=Release ../.. && \
        cmake --build . && \
	cp src/libgpgomea_python.so ../../pythonpkg/pyGPGOMEA/gpgomea.so && \
	cd ../../pythonpkg && \
	python3 setup.py install --user --force

debug:
	mkdir -p build/debug && \
        cd build/debug && \
        cmake $(GENERATOR) $(FORCE_COLOR) ${DISABLE_SANITIZER_FLAG} -DCMAKE_BUILD_TYPE=Debug ../.. && \
        cmake --build . && \
	cp src/libgpgomea_python.so ../../pythonpkg/pyGPGOMEA/gpgomea.so && \
	cd ../../pythonpkg && \
	python3 setup.py install --user --force

clean:
	rm -rf build pythonpkg/build pythonpkg/dist

