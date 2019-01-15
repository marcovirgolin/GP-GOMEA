#include <cstdlib>

#include <iostream>
#include <armadillo>
#include <boost/python.hpp>

#include "Evolution/EvolutionState.h"
#include "Evolution/EvolutionRun.h"
#include "Tests/TestsSemanticBackpropagation.h"
#include "RunHandling/IMSHandler.h"


using namespace std;
using namespace boost::python;

struct GPGOMEA {

    void run(std::string options) {

        // parsing options
        std::vector<std::string> opts = Utils::SplitStringByChar(options, ' ');
        vector<string> argv_v = {"dummy"};
        argv_v.reserve(100);
        for (size_t i = 0; i < opts.size(); i++) {
            argv_v.push_back(opts[i]);
        }
        size_t argc = argv_v.size();
        char * argv[argc];
        for (size_t i = 0; i < argc; i++)
            argv[i] = (char*) argv_v[i].c_str();
        
        // setting options
        EvolutionState & st = *EvolutionState::GetInstance();
        st.SetOptions(argc, argv);

        // running
        IMSHandler * imsh = new IMSHandler(&st);
        imsh->Start();
        delete imsh;

	delete &st;
    };

};

BOOST_PYTHON_MODULE(gpgomea) {
    class_<GPGOMEA>("GPGOMEA")
            .def("run", &GPGOMEA::run)
            ;
}
