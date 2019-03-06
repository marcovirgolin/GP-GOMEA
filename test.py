from pyGPGOMEA import GPGOMEARegressor as GPG
from sklearn.datasets import load_boston
import numpy as np
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split
from sklearn.model_selection import GridSearchCV

# Vanilla use
X, y = load_boston(return_X_y=True)
X_train, X_test, y_train, y_test = train_test_split( X, y, test_size=0.25, random_state=42 )

print 'Running GP-GOMEA...'
ea = GPG( gomea=True, ims='5_1', generations=10, seed=42, silent=False )
ea.fit(X_train, y_train)
print 'Model found:', ea.get_model()
print 'Evaluations taken:', ea.get_evaluations() # care: this is not correct if multiple threads were used when fitting
print 'Test RMSE:', np.sqrt( mean_squared_error(y_test, ea.predict(X_test)) )


# Grid search
print 'Running Grid Search'
hyperparams = [{ 'popsize': [50, 100], 
	'initmaxtreeheight': [3,5], 
	'gomea': [True, False] }]
ea = GPG( generations=10, parallel=2, ims=False )
gs = GridSearchCV( ea, hyperparams )
gs.fit(X, y)

print 'Best hyper-parameter settings:', gs.best_params_
