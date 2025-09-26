import numpy as np
from sklearn.datasets import load_boston

X_train, y_train = load_boston(return_X_y=True)

def exampleFitnessFunction(x):
	return np.mean(np.square(x-y_train[0:len(x)]))