from sklearn.base import BaseEstimator, RegressorMixin
from gpgomea import GPGOMEA
from sklearn.metrics import mean_squared_error
import inspect
import copyreg


class GPGOMEARegressor(BaseEstimator, RegressorMixin):
	
	''' GP-GOMEA as scikit-learn regressor '''
	def __init__(self, 
		time=60, generations=-1, evaluations=-1, 
		prob='symbreg', linearscaling=True, functions='+_*_-_aq', erc=True,
		gomea=True, gomfos='LT',
		subcross=0.5, submut=0.5, reproduction=0.0,
		sblibtype=False, sbrdo=0.0, sbagx=0.0,
		unifdepthvar=True, tournament=4, elitism=1,
		ims='5_1', syntuniqinit=1000, popsize=500,
		initmaxtreeheight=4, maxtreeheight=17, maxsize=1000,
		seed = -1, parallel=1, caching=False, silent=True):

		args, _, _, values = inspect.getargvalues(inspect.currentframe())
		values.pop("self")
		for arg, val in values.items():
			setattr(self, arg, val)

		hp_string = self._build_hyperparameters_string()
		self._ea = GPGOMEA(hp_string)

	def __del__(self):
		del self._ea


	def get_params(self, deep=True):
		attributes = inspect.getmembers(self, lambda a:not(inspect.isroutine(a)))
		attributes = [a for a in attributes if '_' not in a[0]]

		dic = {}
		for a in attributes:
			dic[a[0]] = a[1]

		return dic

	def set_params(self, **parameters):
		for parameter, value in parameters.items():
			setattr(self, parameter, value)

		hp_string = self._build_hyperparameters_string()
		self._ea = GPGOMEA(hp_string)

		return self

	def _build_hyperparameters_string(self):
		result = ''
		attributes = inspect.getmembers(self, lambda a:not(inspect.isroutine(a)))
		attributes = [a for a in attributes if '_' not in a[0]]
		
		for a in attributes:
			# special cases
			if a[0] == 'seed' and a[1] < 0:
				continue
			if type(a[1]) == bool:
				if a[1] == True:
					result = result + '--'+a[0]+' '
			else:
				result = result + '--'+a[0]+' '+str(a[1])+' '

		return result

	def fit(self, X, y):
		self._ea.run(X, y.reshape((-1,1)))
		return self

	def predict(self, X, y=None):
		prediction = self._ea.predict(X)
		return prediction

	def score(self, X, y=None):
		if y is None:
			raise ValueError('The ground truth y was not set.')
		prediction = self._ea.predict(X)
		neg_mse = -1.0 * mean_squared_error(prediction , y)
		return neg_mse

	def get_model(self):
		return self._ea.get_model()

	def get_evaluations(self):
		return self._ea.get_evaluations()
