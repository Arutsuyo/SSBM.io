import os
import sys
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Dropout, LSTM

"""
Author: Chase M. Craig
Purpose: For being able to accept input from an application to train a neural network.



"""
def debugPrint(o):
	print(o)
choice = input()
if choice == '0':
	# SHUT UP!
	debugPrint = lambda o: None # :D

fallbackLSTM = LSTM
try:
	from tensorflow.keras.layers import CuDNNLSTM
	fallbackLSTM = lambda *args, **kw: CuDNNLSTM(*args, **{k:kw[k] for k in kw if k != "activation"})
	debugPrint("Using CuDNNLSTM for faster evaluation of LSTMs.")
except:
	debugPrint("Unable to import CuDNNLSTM, using fallback of LSTM with tanh activator.")
	
debugPrint("Hello! New (0) model or Load (1) model (directory: ./models/ssbm): ")
export_dir = os.path.join("models","ssbm.h5")
choice = input()
if choice != '0' and choice != '1':
	debugPrint("That was neither! Exiting.")
	sys.exit(1)

# BUILD MODEL SECTION!
model = None
if choice == '1':
	debugPrint("Loading model from file...")
	model = tf.keras.models.load_model(export_dir)
else:
	debugPrint("Building model:")
	model = Sequential()
	model.add(fallbackLSTM(30, input_dim=15, activation='tanh', return_sequences=True, stateful=True))
	model.add(Dropout(0.2))
	model.add(fallbackLSTM(30, activation='tanh',stateful=True))
	model.add(Dropout(0.5))
	model.add(Dense(10, activation='relu'))
	model.add(Dropout(0.5))
	model.add(Dense(7, activation='linear'))

	opt = tf.keras.optimizers.Adam(lr=1e-2, decay=1e-5)
	model.compile(loss='mean_squared_error', optimizer=opt, metrics=['accuracy'])

debugPrint("Finished building/loading! Please input data in the form of P1-HP P1-FD P1-X P1-Y P2-HP P2-FD P2-X P2-Y (as specified ")
# Train...until we hit an end of file...
while True:
	try:
		input_k = input()
		if input_k == "-1 -1":
			break
		vv = [float(x) for x in input_k.split(" ")]
		model.fit(vv[0:8], vv[8:], epochs=1)
	except:
		break
	

# Save!
debugPrint("End of file reached. Saving model.")
model.save(export_dir)
debugPrint("Finished saving...exiting...")
# train
