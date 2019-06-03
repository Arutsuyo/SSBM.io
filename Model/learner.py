import os
import sys
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Dropout, LSTM
from tensorflow.keras.utils import plot_model
from numpy import array
"""
Author: Chase M. Craig
Purpose: For being able to accept input from an application to train a neural network.



"""
def debugPrint(o, **kwargs):
	print(o, **kwargs)
choice = input()
if choice == '0':
	# SHUT UP!
	debugPrint = lambda o, **kwargs: None # :D

fallbackLSTM = LSTM
try:
	from tensorflow.keras.layers import CuDNNLSTM
	fallbackLSTM = lambda *args, **kw: CuDNNLSTM(*args, **{k:kw[k] for k in kw if k != "activation"})
	debugPrint("Using CuDNNLSTM for faster evaluation of LSTMs.")
except:
	debugPrint("Unable to import CuDNNLSTM, using fallback of LSTM with tanh activator.")
	
debugPrint("Hello! New (0) model or Load (1) model (directory: ./models/ssbm_offline) or Run (2) model (just predict mode): ")
export_dir = os.path.join("models","ssbm_offline.h5")
choice = input()
if choice != '0' and choice != '1' and choice != '2':
	debugPrint("That was neither! Exiting.")
	sys.exit(1)


# BUILD MODEL SECTION!
model = None
if choice == '1' or choice == '2':
	debugPrint("Loading model from file...")
	model = tf.keras.models.load_model(export_dir)
else:
	debugPrint("Building model:")
	model = Sequential()
	model.add(fallbackLSTM(30, input_shape=(None,8), activation='tanh', return_sequences=True))
	model.add(Dropout(0.2))
	model.add(fallbackLSTM(30, activation='tanh'))
	model.add(Dropout(0.5))
	model.add(Dense(10, activation='relu'))
	model.add(Dropout(0.5))
	model.add(Dense(6, activation='linear'))

	opt = tf.keras.optimizers.Adam(lr=1e-2, decay=1e-5)
	model.compile(loss='mean_squared_error', optimizer=opt, metrics=['accuracy'])
plot_model(model, to_file='model_plot.png', show_shapes=True, show_layer_names=True)
debugPrint("Finished building/loading! Please input data in the form of P1-HP P1-FD P1-X P1-Y P2-HP P2-FD P2-X P2-Y ", end="")

if choice != '2':
	debugPrint("Stick% A B Y Z L")
# Train...until we hit an end of file...
while True:
	try:
		input_k = input()
		if input_k == "-1 -1":
			break
	except:
		break
	vv = [float(x) for x in input_k.split(" ")]
	if choice == '2':
		k = model.predict(vv)[0] # should only be 8 characters)
		for i in range(5):
			k[i+1] = 1 if k[i] >= .5 else 0
		k[0] = -.8 if k[i] <= -.8 else (-.2 if k[i] <= -.2 else (0 if k[i] <= .2 else (.2 if k[i] <= .8 else .8)))
		print(*k)
	else:
		print(array(vv[0:8]).reshape((1,1,8)), array(vv[8:]).reshape((6,1)))
		model.fit(array(vv[0:8]).reshape((1,1,8)),array(vv[8:]).reshape((1,6)), epochs=1, verbose=0)
	

# Save!
debugPrint("End of file reached. Saving model.")
model.save(export_dir)
debugPrint("Finished saving...exiting...")
# train