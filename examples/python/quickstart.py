import numpy as np

# Try the wrapper package if installed
try:
    from pyGPGOMEA import GPGOMEARegressor
    use_wrapper = True
except Exception:
    # Fallback to the Boost.Python module
    import gpgomea as gpg
    use_wrapper = False

# Synthetic data: y = 3*x0 - 2*x1 + noise
rng = np.random.default_rng(42)
X = rng.normal(size=(200, 2))
y = 3.0 * X[:, 0] - 2.0 * X[:, 1] + rng.normal(scale=0.1, size=200)

# Hyperparameters (CLI-style string)
hyper = "--seed 42 --generations 50 --population_size 200 --max_tree_size 50 --use_ERC 1 --linear_scaling 1 --silent"

if use_wrapper:
    # Scikit-like interface (if available)
    model = GPGOMEARegressor(hyperparams=hyper)
    model.fit(X, y)
    y_pred = model.predict(X)
    print("Score (negative error):", model.score(X, y))
    print("Expression:", model.get_model())
else:
    # Direct API via the gpgomea module
    g = gpg.GPGOMEA(hyper)
    g.run(X, y.reshape(-1, 1))
    y_pred = g.predict(X)
    print("Score (negative error):", g.score(X, y.reshape(-1, 1)))
    print("Expression:", g.get_model())
