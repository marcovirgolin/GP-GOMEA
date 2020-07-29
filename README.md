# Genetic Programming - GOMEA, Standard & Semantic
Implementations of the **Gene-pool Optimal Mixing Evolutionary Algorithm for Genetic Programming** (GP-GOMEA), **standard tree-based GP**, and **Semantic Backpropagation-based GP**.
This code uses **C++** under the hood for speed, and has a **Python 3** interface to **scikit-learn**.

## Algorithms
By estimating interdependencies between model components, and appropriately mixing them, GP-GOMEA is especially proficient in finding models in the form of small symbolic expressions, that can often be interpreted. Semantic Backpropagation-based GP works well when high accuracy is demanded, at the cost of producing very complex models, which are hard or impossible to interpret. Standard GP is a good baseline to compare the other methods with.

This code also implements the Interlaved Multistart Scheme (IMS), that makes GP more robust w.r.t. setting a specific population size. Setting the population size is tricky: if too small, GP will find models with poor accuracy; If too big, GP will waste computation time. The IMS starts and executes multiple evolutionary runs in an interleaved fashion, with subsequent runs using bigger population sizes. It terminates ongoing runs that perform poorly, according to some heuristics. The IMS is largely inspired from the Parameter-free Genetic Algorithm of Harik and Lobo: https://dl.acm.org/citation.cfm?id=2933949

## Related research work
If you use our code for academic purposes, please support our research by citing the paper that most applies, from the following:
* M. Virgolin, T. Alderliesten, C. Witteveen, and P.A.N. Bosman. *Scalable Genetic Programming by Gene-pool Optimal Mixing and Input-space Entropy-based Building Block Learning*. In Proceedings of the Genetic and Evolutionary Computation Conference - GECCO-2017, pages 1041-1048, ACM, 2017. (The source code for this paper is available on Peter's website: https://homepages.cwi.nl/~bosman/source_code.php)
* M. Virgolin, T. Alderliesten, A. Bel, C. Witteveen, and P.A.N. Bosman. *Symbolic Regression and Feature Construction with GP-GOMEA applied to Radiotherapy Dose Reconstruction of Childhood Cancer Survivors*. In Proceedings of the Genetic and Evolutionary Computation Conference - GECCO-2018, pages 1395-1402, ACM, 2018.
* M. Virgolin, T. Alderliesten, P.A.N. Bosman. *Linear Scaling with and within Semantic Backpropagation-based Genetic Programming for Symbolic Regression*.  In Proceedings of the Genetic and Evolutionary Computation Conference - GECCO-2019, pages 1084-1092, ACM, 2019.
* M. Virgolin, T. Alderliesten, C. Witteveen, P.A.N. Bosman. *Improving Model-based Genetic Programming for Symbolic Regression of Small Expressions*. Preprint on arXiv https://arxiv.org/abs/1904.02050 2019, accepted in Evolutionary Computation.
* M. Virgolin, T. Alderliesten, P.A.N. Bosman. *On Explaining Machine Learning Models by Evolving Crucial and Compact Features*. Swarm and Evolutionary Computation, v. 53, p. 100640, 2020.


## Installation
### Compilation
This code can be compiled on Linux (tested on Ubuntu and Fedora). 
There are a few steps to follow:
* Inspect and potentially edit `Makefile-variables.mk` to set the right Python version for the `boost` library (bottom lines of the file)
* Inspect and potentially edit the `m` file to align it to your system. It is currently set to run on Fedora using `dnf`. If you run from Ubuntu, replace `dnf` calls to `apt` calls, and set the correct library names (e.g., `boost-devel` becomes `libboost-dev`).

To install dependencies, compile the C++ code, and install the Python package, run `sudo ./m`. To test that everything works fine, run `python3 test.py`.

### Do you want to build a C++ executable instead?
Modify the file `Makefile-Python_Release.mk` by removing `-fPIC -shared` from `CXXFLAGS`, then run `make`. You will find a C++ executable called `gpgomea` in `dist/Python_Release/GNU-Linux/` that you can run using a parameter setting file, for example, `gpgomea --file params_sgp.txt`.


## Using the Python interface
See `test.py`. 
For example:
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


