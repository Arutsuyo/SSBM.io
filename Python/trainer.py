import time
import threading
import os
from os import fdopen
import sys
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Dropout, LSTM
from tensorflow.keras.utils import plot_model
from collections import deque
import zc.lockfile as lf
import numpy as np
from time import sleep
import random
"""
Author: Chase M. Craig
Purpose: For being able to accept input from an application to train a neural network.

Using https://towardsdatascience.com/reinforcement-learning-w-keras-openai-dqns-1eed3a5338c 

as a template
"""

# Make sure the pipes are open!!!
stdoin = open(0, "r")
stdout = open(1, "w")
stderr = open(2, "w")

# When writing to the pipe, we're using 
# '\0' + ("pred: " + <str>msg) + '\0'
# as the identifier for the pipe parser. by using "pred: " at the beginning we 
# can guarantee that we know what to look for

stderr.write('\0' + "GO GO TENSORFLOW!" + '\0')
stderr.flush()

def debugPrint(msg):
	stdout.write('\0' + ("pred: " + msg) + '\0')
	stdout.flush()
	stderr.flush()

def PipePrint(*vars):
	vLen = len(vars)
	if vLen > 0:
		msg = str(vars[0])
		for i in range(1, vLen):
			msg += " %s" % str(vars[i])
		stdout.write('\0' + ("pred: " + msg) + '\0')
		stdout.flush()
		stderr.flush()

def getInput(n):
	stderr.flush()
	return os.read(0, n).decode("utf-8")

choice = getInput(1)
if "0" in choice:
	# SHUT UP!
	debugPrint = lambda o: None # :D

fallbackLSTM = LSTM
try:
	from tensorflow.keras.layers import CuDNNLSTM
	fallbackLSTM = lambda *args, **kw: CuDNNLSTM(*args, **{k:kw[k] for k in kw if k != "activation"})
	debugPrint("Using CuDNNLSTM for faster evaluation of LSTMs.\n")
except:
	debugPrint("Unable to import CuDNNLSTM, using fallback of LSTM with tanh activator.\n")
	
debugPrint("Initialization: (0) New Model (1) Load Model (2) Load in Prediction-Only\n")
stderr.flush()

class DQN:
	def __init__(self):
		self.memory  = deque(maxlen=2000)
		self.game_score = 0
		self.gamma = 0.95
		self.epsilon = 1.0
		self.epsilon_min = 0.01
		self.epsilon_decay = 0.9975 # takes a LONG time to train...
		self.learning_rate = 0.01
		self.input_size = 8
		self.tau = .05
		self.actions = [[0,0,0,0,0,0,0]]*90
		c = 0
		for p in [-1,-.5,0,.5,1]:
			for u in [-1,0,1]:
				for ab in [0, 1, 2, 3, 4, 5]:
					self.actions[c] = [p, u,1 if ab ==1 else 0,1 if ab ==2 else 0,1 if ab ==3 else 0,1 if ab ==4 else 0,1 if ab ==5 else 0]
					c = c + 1
		self.model = self.create_model()
		# "hack" implemented by DeepMind to improve convergence
		self.target_model = self.create_model()
		
	def get_Score(self, prev_state, new_state):
		reward = 0
		prev_state = prev_state[-1]
		if prev_state[4] > new_state[4] and new_state[4] == 0:
			reward = reward + 100 # :D Not enough to overcome suicide.
		if prev_state[0] > new_state[0] and new_state[0] == 0:
			reward = reward - 500 # Died, get a reward of -500
		else:
			reward = reward - (new_state[0] - prev_state[0])*.1*new_state[0] # So reward penalty gets worse for getting hit
			reward = reward + (new_state[4] - prev_state[4])*.5*new_state[4] # Reward if hitting!
		reward = reward - ((abs(new_state[2] - new_state[6]) * .25) + (abs(new_state[3] - new_state[7])*.1)) # Slight penalty for going away from the user.
		self.add_OverallScore(prev_state[0] > new_state[0], prev_state[4] > new_state[4], new_state[0]-prev_state[0], new_state[4]-prev_state[4])
		return reward
	def add_OverallScore(self, hasDied, otherDied, myHP, theirHP):
		self.game_score = self.game_score + (-500 if hasDied == 1 else 0) + (500 if otherDied == 1 else 0)
		#(-myHP * .2) + (theirHP * .15)
		if not hasDied:
			self.game_score = self.game_score + (-myHP * .2)
		if not otherDied:
			self.game_score = self.game_score + (myHP * .15)
	def create_model(self):
		model = Sequential()
		model.add(fallbackLSTM(30,input_shape=(20,8), activation='tanh', return_sequences=True))
		model.add(Dropout(0.2))
		model.add(fallbackLSTM(30, activation='tanh'))
		model.add(Dropout(0.5))
		model.add(Dense(15, activation='relu'))
		# Reason for a small second to last dense layer is that 
		# A LOT of the values are highly correlated (sadly), so we don't expect a lot of difference here. 
		model.add(Dropout(0.5))
		model.add(Dense(90, activation='linear'))
		# 5 stick positions (-.8, -.2, 0, .2, .8)
		# A or B
		# L or Y or Z
		self.opt = tf.keras.optimizers.Adam(lr=self.learning_rate, decay=1e-5)
		model.compile(loss="mean_squared_error",
			optimizer=self.opt)
		return model
	def remember(self, state, action, reward, new_state, done):
		self.memory.append([state, action, reward, new_state, done])
	def get_real_action(self, action):
		for y in range(len(self.actions)):
			x = self.actions[y]
			if sum([(1 if abs(x[i] - action[i]) >= 0.01 else 0) for i in range(len(action))]) == 0:
				return y
	def replay(self):
		batch_size = 256
		if len(self.memory) < batch_size:
			return
		samples = random.sample(self.memory, batch_size)
		for sample in samples:
			state, action, reward, new_state, done = sample
			action = self.get_real_action(action)
			target = self.target_model.predict(state)
			if done:
				target[0][action] = reward
			else:
				Q_future = max(self.target_model.predict(new_state)[0])
				target[0][action] = reward + Q_future * self.gamma
			self.model.fit(state, target, epochs=1, verbose=0)
		self.memory.clear()
	def act(self, state):
		self.epsilon *= self.epsilon_decay
		self.epsilon = max(self.epsilon_min, self.epsilon)
		if np.random.random() < self.epsilon:
			return self.actions[int(np.random.random()*30//1)]
		return self.actions[np.argmax(self.model.predict(state)[0])]
	def target_train(self):
		weights = self.model.get_weights()
		target_weights = self.target_model.get_weights()
		for i in range(len(target_weights)):
			target_weights[i] = weights[i] * self.tau + target_weights[i] * (1 - self.tau)
		self.target_model.set_weights(target_weights)
	def save_model(self, fn):
		self.model.save(fn)
	def load_model(self, fn):
		self.model = tf.keras.models.load_model(fn)
		self.target_model = tf.keras.models.load_model(fn)
	def test(self, fn):
		plot_model(self.model, to_file='model_plot.png', show_shapes=True, show_layer_names=True)



export_dir = os.path.join(os.getcwd(), "AI","ssbm.h5")
best_file = os.path.join(os.getcwd(), "AI", "modelscore.txt")
choice = getInput(1)
if "0" not in choice and "1" not in choice and "2" not in choice:
	debugPrint("That was neither! Exiting.\n")
	sys.exit(1)

# BUILD MODEL SECTION!
agent = None
if "1" in choice or "2" in choice:
	debugPrint("Loading model from file...\n")
	agent = DQN()
	agent.load_model(export_dir)
	stderr.flush()
else:
	debugPrint("Building model:\n")
	agent = DQN() # Prebuilds...
	stderr.flush()
#agent.test("cool")
debugPrint("Finished building/loading!\nPlease input data in the form of:\nP1-HP P1-FD P1-X P1-Y P2-HP P2-FD P2-X P2-Y\n")

pa = deque(maxlen=20)
kill_me = deque(maxlen=20)
for i in range(20):
	pa.append([0,0,0,0,0,0,0,0])
	kill_me.append([0,0,0,0,0,0,0,0])
while True:
	try:
		input_k = getInput(256)
		if "-1 -1" in input_k:
			break
	except Exception, e:
		stderr.write(str(e)+'\0')
		stderr.flush()
		break
	action = agent.act(np.reshape(np.array(pa), (1,20,8)))
	# It is 6 values, brute force
	PipePrint((action[0]+1)/2,(action[1]+1)/2,action[2],action[3],action[4],action[5],action[6])
	stderr.flush()
	# Output action....
	
	vv = [float(x) for x in input_k.strip().split(" ")] # Cur state!
	
	if(len(vv) != 8):
		stderr.write('\0' + "GO GO TENSORFLOW!" + '\0')
		stderr.flush()
		continue
	
	if "2" not in choice:
		kill_me.append(vv)
		reward = agent.get_Score(pa, vv)
		agent.remember(np.reshape(np.array(pa), (1,20,8)), action, reward, np.reshape(np.array(kill_me), (1,20,8)), False)
		agent.replay()
		agent.target_train()
	pa.append(vv)
	
if "2" not in choice:
	# Save!
	stderr.flush()
	debugPrint("End of file reached. Saving model.\n")
	while True:
		lock = None
		try: 
			lock = lf.LockFile('lock.lck')
			# We got it fam
			e = os.path.isfile(best_file)
			if e:
				f = open(best_file, 'r')
				s = f.read()
				f.close()
				if float(s) > agent.game_score:
					debugPrint("A worse score was recorded! Not going to save this model.\n")
					lock.close()
					break
			f = open(best_file, 'w+')
			f.write(str(agent.game_score) + "")
			f.close()
			agent.save_model(export_dir)
			lock.close()
			break
		except lf.LockError:
			sleep(0.05)			
	debugPrint("Finished saving...exiting...\n")
PipePrint(-1,-1)