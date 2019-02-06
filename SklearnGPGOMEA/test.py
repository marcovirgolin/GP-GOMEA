from GPGOMEARegressor import GPGOMEARegressor as GPG
from sklearn.datasets import load_boston
import numpy as np
from sklearn.model_selection import GridSearchCV

X, y = load_boston(return_X_y=True)

hyperparams = [{ 'functions': ['+_-_*_aq', '+_-_*_aq_exp_plog'], 
	'popsize': [100, 200, 50], 'initmaxtreeheight': [3,5,7] }]

ea = GPG( gomea=False, ims=False, parallel=2, generations=20, silent=True )
gs = GridSearchCV( ea, hyperparams )

gs.fit(X, y)

print gs.best_params_