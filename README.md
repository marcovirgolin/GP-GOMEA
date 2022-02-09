# Genetic Programming - GOMEA, Standard & Semantic
Implementations of the **Gene-pool Optimal Mixing Evolutionary Algorithm for Genetic Programming** ([GP-GOMEA](https://direct.mit.edu/evco/article-abstract/29/2/211/97362/Improving-Model-Based-Genetic-Programming-for?redirectedFrom=fulltext)), **standard tree-based GP** ([Standard GP](http://www.gp-field-guide.org.uk/)), and **Semantic Backpropagation-based GP** ([SBP-GP](https://ieeexplore.ieee.org/document/6808504)). The last two can also be used in a multi-objective setting (in which case [NSGA-II](https://doi.org/10.1007/3-540-45356-3_83) is used) --- Multi-objective versions of GP-GOMEA and its population size-free scheme (called *interleaved multistart scheme*) have yet to be implemented.
This code uses **C++** under the hood for speed, and has a **Python 3** interface to **scikit-learn**.

## Algorithms
**GP-GOMEA**: GP-GOMEA estimates interdependencies between model components and uses this info to cross-over interdependent components *en block*, to preserve their concerted action. GP-GOMEA is especially proficient in finding models in the form of small symbolic expressions, that can often be interpreted. 

**SBP-GP**: SBP-GP uses recursive function inversion to estimate effective ways of modifying evolving models. SBP-GP works well when high accuracy is demanded, at the cost of producing very complex models, which are hard or impossible to interpret. 


GP-GOMEA and SBP-GP are some of the **top-performing** algorithms when it comes to symbolic regression:

![image](https://user-images.githubusercontent.com/5921746/121817988-cb178e00-cc84-11eb-8360-713c875c5b4a.png)

*Image from [SRBench](https://github.com/EpistasisLab/srbench/tree/master/postprocessing), a large benchmark for symbolic regression including over 250 datasets.*



## Related research work
If you use our code for academic purposes, please support our research by citing the papers that most apply:
* M. Virgolin, T. Alderliesten, C. Witteveen, and P.A.N. Bosman. *Scalable Genetic Programming by Gene-pool Optimal Mixing and Input-space Entropy-based Building Block Learning*. In Proceedings of the Genetic and Evolutionary Computation Conference - GECCO-2017, pages 1041-1048, ACM, 2017. (The source code for this paper is available on Peter's website: https://homepages.cwi.nl/~bosman/source_code.php)
* M. Virgolin, T. Alderliesten, A. Bel, C. Witteveen, and P.A.N. Bosman. *Symbolic Regression and Feature Construction with GP-GOMEA applied to Radiotherapy Dose Reconstruction of Childhood Cancer Survivors*. In Proceedings of the Genetic and Evolutionary Computation Conference - GECCO-2018, pages 1395-1402, ACM, 2018.
* M. Virgolin, T. Alderliesten, P.A.N. Bosman. *Linear Scaling with and within Semantic Backpropagation-based Genetic Programming for Symbolic Regression*.  In Proceedings of the Genetic and Evolutionary Computation Conference - GECCO-2019, pages 1084-1092, ACM, 2019.
* M. Virgolin, T. Alderliesten, C. Witteveen, P.A.N. Bosman. *Improving Model-based Genetic Programming for Symbolic Regression of Small Expressions*. Preprint on arXiv https://arxiv.org/abs/1904.02050 2019, accepted in Evolutionary Computation.
* M. Virgolin, T. Alderliesten, P.A.N. Bosman. *On Explaining Machine Learning Models by Evolving Crucial and Compact Features*. Swarm and Evolutionary Computation, v. 53, p. 100640, 2020.
* M. Virgolin, A. De Lorenzo, E. Medvet, F. Randone. "Learning a Formula of Interpretability to Learn Interpretable Formulas". Parallel Problem Solving from Nature - PPSN-XVI, pages 79-93, Springer, 2020.

## Installation

### Native
This code can be compiled on Linux, we tested on Ubuntu and Fedora (kudos to [@abouter](https://github.com/abouter) for helping out).
There are a few steps to follow:
* Inspect and potentially edit the `deps_ubuntu` (or `deps_fedora`) file to align it to your system (no change should be needed). 
* Run `chmod +x deps_ubuntu; sudo ./deps_ubuntu` (or `chmod +x deps_fedora; sudo ./deps_fedora)` to install the needed dependencies on your system.

The project is built using CMake (kudos to [@EviSijben](https://github.com/EviSijben)). Run `make` (or `make debug` for a debug build). To test that everything works fine, run `python3 test.py`. You can use Ninja to speed up builds, by prefixing the make command with `GEN=ninja` (e.g. `GEN=ninja make release`).

### Conda
To install using [conda](https://www.anaconda.com/), run:
```
conda create --name gpgomenv python=3.8
conda activate gpgomenv
conda install -c conda-forge armadillo
conda install boost 
conda install -c conda-forge openmp
make
```

### Docker
This code can also be compiled and run inside a [Docker](https://www.docker.com/why-docker) container (kudos to [@roy-tc](https://github.com/roy-tc) for providing this!):

```bash
docker build -t gp-gomea .
```

This will:
* Install dependencies (implementation derived from on `deps_ubuntu`).
* Build the project.
* Test that everything is fine using `python3 test.py`.

You can run the container in interactive mode using `docker run -it gp-gomea` and issue for instance `python3 test.py` or execute your own script.

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
print('Model found:', model.get_model())
print('RMSE:', np.sqrt( np.mean( np.square( model.predict(X) - y ) ) ))
```
Take a look at `test.py` for more details.

## Do you want to run a C++ executable instead?
After running `make`, you will find a C++ executable called `main` in `build/release/src/` that you can run using a parameter setting file, for example, `main --file params_sgp.txt`.

### Datasets
For the C++ executable, datasets must be organized as follows. Each row is an example, and each column is a feature, with exception for the last column, which is the target variable. Values should be separated by spaces. Do not include any textual header.
You can find examples at: https://goo.gl/9D2z3b 
