# GP-GOMEA
C++ implementation of the Gene-pool Optimal Mixing Evolutionary Algorithm for Genetic Programming (GP-GOMEA). It also comes with standard tree-based GP, and Semantic Backpropagation-based GP.

GP-GOMEA is especially proficient in finding small and accurate symbolic expressions, to have reliable and interpretable models. Semantic Backpropagation-based GP works well when high accuracy is demanded, at the cost of producing very complex models, which are hard or impossible to interpret. Standard GP is a good baseline to compare the other methods with.

This code also implements the Interlaved Multistart Scheme (IMS), that makes GP more robust w.r.t. setting a specific population size. Setting the population size is tricky: if too small, GP will find models with poor accuracy; If too big, GP will waste computation time. The IMS starts and executes multiple evolutionary runs in an interleaved fashion, with subsequent runs using bigger population sizes. It terminates ongoing runs that perform poorly, according to some heuristics. The IMS is largely inspired from the Parameter-free Genetic Algorithm of Harik and Lobo: https://dl.acm.org/citation.cfm?id=2933949

## Related research work
Coming soon!

## Setup
### Compilation
This code can be compiled on Linux (tested on Ubuntu and Fedora). Use g++ with C++ standard '14 or higher. This code comes as a Netbeans project, which makes compilation and linking easier through an interface. Alternatively, pre-made compilation files are available in the folder `nbproject`. You can change compilation configuration in `nbproject/Makefile-impl.mk`, line 30 (to Debug or Release), then edit `nbproject/Makefile-Release.mk` (or `Makefile-Debug.mk`) to your liking. Finally, call `make`.

### Dependencies
You need the following libraries to use GP-GOMEA:
* Armadillo http://arma.sourceforge.net (see ARMADILLO_NOTICE.txt)
* OpenMP https://www.openmp.org/ (compile using -fopenmp)
* Boost https://www.boost.org/

### Python packaging
You can compile the code into a python-callable library. To do so, compile using the `-shared` flag to produce a library (`gpgomea.so`), and link Boost.Python (`-libboost_python` or `-lboost_python` depending on your system). Add the library to your library path.

## Usage
Call `./gp-gomea --help` to get a comprehensive list of parameters. Some parameters are mandatory, some others have defaults. You can set the parameters when calling the executable with `gp-gomea --param1 value1 --param2 value2`; or write them in a parameter file (see the examples for GP-GOMEA, standard GP, and Semantic Backpropagation-based GP), and finally call `./gp-gomea --file param_file.txt`.

### Usage from Python
If you built the code into a library, then you can call it from python. For example:
```python
from gpgomea import GPGOMEA

ea = GPGOMEA()
# display usage
ea.run('--help') 
# run using parameter settings from file
ea.run('--file params_gpgomea.txt')
```
I'll make a scikit-learn compatible version in the future.

### Datasets
Datasets must be organized as follows. Each row is an example, and each column is a feature. The last column must contain the target variable. Values should be separated by spaces. Do not include any textual header.
You can find examples at: https://goo.gl/9D2z3b 

### Output
The file `stats_generations.txt` contains information that is logged every generation (iteration) of the algorithm. When the run terminates, an output `result.txt` is produced, that also contains the symbolic expression found by GP.

