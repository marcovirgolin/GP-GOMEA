from pyGPGOMEA import GPGOMEARegressor as GPG
import numpy as np
from sklearn.metrics import mean_squared_error
from sklearn.model_selection import train_test_split
from sklearn.model_selection import GridSearchCV

SEED=42
np.random.seed(SEED)

# Example using Newton's gravitational law (except for the constant)
X = np.random.random(size=(1024,3)) * [1,4,8] # first column is mass1, second column is mass2, third column is distance
def grav(X):
	y = 6.67 * np.multiply(X[:,0],X[:,1])/np.square(X[:,2])
	return y
y = grav(X)

# Split train & test set
X_train, X_test, y_train, y_test = train_test_split( X, y, test_size=0.25, random_state=SEED )

print('Running GP-GOMEA...')
ea = GPG( 
	gomea=True,  											# uses GOMEA as search algorithm
	functions="+_-_*_p/_sqrt_plog", 	# functions to use
	coeffmut='0.5_0.5_0.5_10',		# mutate constants with prob. of 0.5, with std of 0.5*abs(curr. val), which decays by 0.5 every 10 gen without improvement
	time=10, generations=-1, evaluations=-1, # time limit of 10s, no genereations or evaluations limit
	initmaxtreeheight=3, 			# how tall trees are at initialization (they cannot grow taller when using GOMEA)
	ims='4_1', 	 			# interleaved multistart scheme: run a larger population doubled every 4 gen of the smaller one, patience of 1 iteration(s) before terminating a suboptimal population
	popsize=64,  				# initial pop size for the first run
	batchsize=256,				# batch size
	parallel=4,				# in parallel on 4 cores
	linearscaling=False, 			# whether to use a linear layer on top of the solutions (highly-recommended for real-world data!)
	silent=False)				# log progress
ea.fit(X_train, y_train)

print("\n")

# get the model and change some operators such as protected division and protected log to be sympy-digestible
model = ea.get_model().replace("p/","/").replace("plog","log")
# let's also call vars with their names
import sympy
model = str(sympy.simplify(model))
model = model.replace("x0","mass1").replace("x1","mass2").replace("x2","dist")
# sometimes due to numerical precision, sympy might be unable to do this
if model == "0":
	print("Warning: sympy couldn't make it due to numerical precision")
	# re-set to non-simplified model
	model = ea.get_model().replace("p/","/").replace("plog","log").replace("x0","mass1").replace("x1","mass2").replace("x2","dist")
print('Model found:', model)
print('Evaluations taken:', ea.get_evaluations()) # care: this is not correct if multiple threads were used when fitting
print('Test RMSE:', np.sqrt( mean_squared_error(y_test, ea.predict(X_test)) ))

quit()

# Further info, tests down here

print('A population member:', ea.get_final_population(X_train)[0])
# Grid search
print('Running Grid Search')
hyperparams = [{ 'popsize': [50, 100], 
	'initmaxtreeheight': [3,5], 
	'gomea': [True, False] }]
ea = GPG( generations=10, parallel=4, ims=False )
gs = GridSearchCV( ea, hyperparams )
gs.fit(X, y)

print('Best hyper-parameter settings:', gs.best_params_)
