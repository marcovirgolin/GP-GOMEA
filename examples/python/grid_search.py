import numpy as np
import itertools

try:
    from pyGPGOMEA import GPGOMEARegressor
    use_wrapper = True
except Exception:
    import gpgomea as gpg
    use_wrapper = False

rng = np.random.default_rng(0)
X = rng.normal(size=(300, 3))
y = 2.0 * X[:, 0] + 0.5 * X[:, 1]**2 - 1.5 * np.sin(X[:, 2]) + rng.normal(0, 0.1, size=300)

def make_hyper(seed, gens, pop):
    return f"--seed {seed} --generations {gens} --population_size {pop} --max_tree_size 60 --use_ERC 1 --linear_scaling 1 --silent"

grid = {
    "seed": [0, 1],
    "generations": [50, 100],
    "population_size": [200, 400]
}

best = None
best_score = -1e18

for seed, gens, pop in itertools.product(grid["seed"], grid["generations"], grid["population_size"]):
    hyper = make_hyper(seed, gens, pop)
    if use_wrapper:
        model = GPGOMEARegressor(hyperparams=hyper)
        model.fit(X, y)
        score = model.score(X, y)
        desc = model.get_model()
    else:
        g = gpg.GPGOMEA(hyper)
        g.run(X, y.reshape(-1, 1))
        score = g.score(X, y.reshape(-1, 1))
        desc = g.get_model()

    if score > best_score:
        best_score = score
        best = (hyper, desc)

print("Best score:", best_score)
print("Best hyperparameters:", best[0])
print("Model:", best[1])