# GP-GOMEA
C++ implementation of the Gene-pool Optimal Mixing Evolutionary Algorithm for Genetic Programming (GP-GOMEA). It also comes with standard tree-based GP, and Semantic Backpropagation-based GP.

By estimating interdependencies between model components, and appropriately mixing them, GP-GOMEA is especially proficient in finding models in the form of small symbolic expressions, that can often be interpreted. Semantic Backpropagation-based GP works well when high accuracy is demanded, at the cost of producing very complex models, which are hard or impossible to interpret. Standard GP is a good baseline to compare the other methods with.

This code also implements the Interlaved Multistart Scheme (IMS), that makes GP more robust w.r.t. setting a specific population size. Setting the population size is tricky: if too small, GP will find models with poor accuracy; If too big, GP will waste computation time. The IMS starts and executes multiple evolutionary runs in an interleaved fashion, with subsequent runs using bigger population sizes. It terminates ongoing runs that perform poorly, according to some heuristics. The IMS is largely inspired from the Parameter-free Genetic Algorithm of Harik and Lobo: https://dl.acm.org/citation.cfm?id=2933949

## Related research work
If you use our code for academic purposes, please support our research by citing the paper that most applies, from the following:
* M. Virgolin, T. Alderliesten, C. Witteveen, and P.A.N. Bosman. *Scalable Genetic Programming by Gene-pool Optimal Mixing and Input-space Entropy-based Building Block Learning*. In Proceedings of the Genetic and Evolutionary Computation Conference - GECCO-2017, pages 1041-1048, ACM Press, New York, 2017. (The source code for this paper is available on Peter's website: https://homepages.cwi.nl/~bosman/source_code.php)
* M. Virgolin, T. Alderliesten, A. Bel, C. Witteveen, and P.A.N. Bosman. *Symbolic Regression and Feature Construction with GP-GOMEA applied to Radiotherapy Dose Reconstruction of Childhood Cancer Survivors*. In Proceedings of the Genetic and Evolutionary Computation Conference - GECCO-2018, pages 1395-1402, ACM Press, New York, 2018.
* M. Virgolin, T. Alderliesten, P.A.N. Bosman. *Linear Scaling with and within Semantic Backpropagation-based Genetic Programming for Symbolic Regression*.  In Proceedings of the Genetic and Evolutionary Computation Conference - GECCO-2019, pages 1084-1092, ACM Press, New York, 2019.
* M. Virgolin, T. Alderliesten, C. Witteveen, P.A.N. Bosman. *Improving Model-based Genetic Programming for Symbolic Regression of Small Expressions*. Preprint on arXiv 2019, submitted to Evolutionary Computation.
* M. Virgolin, T. Alderliesten, P.A.N. Bosman. *On Explaining Machine Learning Models by Evolving Crucial and Compact Features*. Preprint on arXiv 2019, submitted to Swarm and Evolutionary Computation.
* *MORE COMING SOON*


## Setup
### Compilation
This code can be compiled on Linux (tested on Ubuntu and Fedora). Use g++ with C++ standard '14 or higher. This code comes as a Netbeans project, which makes compilation and linking easier through an interface. Alternatively, pre-made compilation files are available in the folder `nbproject`. You can change compilation configuration in `nbproject/Makefile-impl.mk`, line 30 (to Debug or Release), then edit `nbproject/Makefile-Release.mk` (or `Makefile-Debug.mk`) to your liking. Finally, call `make`.

### Dependencies
You need the following libraries to use GP-GOMEA:
* Armadillo http://arma.sourceforge.net (see ARMADILLO_NOTICE.txt)
* OpenMP https://www.openmp.org/ (compile using -fopenmp)
* Boost https://www.boost.org/ (program options, python, numpy, system)
* Python developer (`python-dev` or `python-devel` depending on your OS)

### Linking
You need to link to: `-larmadillo -lboost_program_options -lboost_python -lboost_system -lboost_numpy` (or `-lib[...]` depending on your OS)

### Python packaging
Run `python setup.py install [--user]` to install a scikit-learn-compatible Python interface.

## Usage
### Using the C++ executable
Call `./gpgomea --help` to get a comprehensive list of parameters. Some parameters are mandatory, some others have defaults. You can set the parameters when calling the executable with `gpgomea --param1 value1 --param2 value2`; or write them in a parameter file (see the examples for GP-GOMEA, standard GP, and Semantic Backpropagation-based GP), and finally call `./gpgomea --file param_file.txt`.

### Using the Python interface
If you built the code into a library, then you can call it from python. For example:
```python
from pyGPGOMEA import GPGOMEARegressor as GPGR
from sklearn.datasets import load_boston
import numpy as np

X, y = load_boston(return_X_y=True)
model = GPGR( gomea=True, ims='5_1', generations=10, seed=42 )
model.fit(X, y)
print 'Model found:', model.get_model()
print 'RMSE:', np.sqrt( np.mean( np.square( model.predict(X) - y ) ) ) 
```
Take a look at `test.py` for more details.

### Datasets
For the C++ executable, datasets must be organized as follows. Each row is an example, and each column is a feature, with exception for the last column, which is the target variable. Values should be separated by spaces. Do not include any textual header.
You can find examples at: https://goo.gl/9D2z3b 

### Output
The file `stats_generations.txt` contains information that is logged every generation (iteration) of the algorithm. When the run terminates, an output `result.txt` is produced, that also contains the symbolic expression found by GP. These files are not generated when the code is run from Python.

