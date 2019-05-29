import os
import sys
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense, Dropout, LSTM
from collections import deque
"""
Author: Chase M. Craig
Purpose: For being able to accept input from an application to train a neural network.

Using https://towardsdatascience.com/reinforcement-learning-w-keras-openai-dqns-1eed3a5338c 

as a template
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


class DQN:
	def __init__(self):
		self.memory  = deque(maxlen=2000)
		
		self.gamma = 0.95
		self.epsilon = 1.0
		self.epsilon_min = 0.01
		self.epsilon_decay = 0.995
		self.learning_rate = 0.01
		self.input_size = 10
		self.tau = .05
		self.actions = [[0,0,0,0,0,0]]*30
		c = 0
		for p in [-.8,-.2,0,.2,8]:
			for ab in [0, 1]:
				for lyz in [0,1,2]:
					self.actions[c] = [p,ab,1-ab,1 if lyz == 0 else 0, 1 if lyz == 1 else 0, 1 if lyz == 2 else 0]
					c = c + 1
		self.model = self.create_model()
		# "hack" implemented by DeepMind to improve convergence
		self.target_model = self.create_model()
		
	def get_Score(self, prev_state, new_state):
		reward = 0
		if prev_state[4] > new_state[4] and new_state[4] == 0:
			reward = reached + 100 # :D Not enough to overcome suicide.
		if prev_state[0] > new_state[0] and new_state[0] == 0:
			reward = reward - 500 # Died, get a reward of -500
		else:
			reward = reward - (new_state[0] - prev_state[0])*.1*new_state[0] # So reward penalty gets worse for getting hit
			reward = reward + (new_state[4] - prev_state[4])*.5*new_state[4] # Reward if hitting!
		reward = reward - ((abs(new_state[2] - new_state[6]) * .25) + (abs(new_state[3] - new_state[7])*.1)) # Slight penalty for going away from the user.
		reward = reward - (10 * abs(new_state[1] - new_state[5]))
		return reward
	def create_model(self):
		model = Sequential()
		model.add(fallbackLSTM(30,input_shape=(self.input_size,1), activation='tanh', return_sequences=True))
		model.add(Dropout(0.2))
		model.add(fallbackLSTM(30, activation='tanh'))
		model.add(Dropout(0.5))
		model.add(Dense(15, activation='relu'))
		# Reason for a small second to last dense layer is that 
		# A LOT of the values are highly correlated (sadly), so we don't expect a lot of difference here. 
		model.add(Dropout(0.5))
		model.add(Dense(30, activation='linear'))
		# 5 stick positions (-.8, -.2, 0, .2, .8)
		# A or B
		# L or Y or Z
		self.opt = tf.keras.optimizers.Adam(lr=self.learning_rate, decay=1e-5)
		model.compile(loss="mean_squared_error",
			optimizer=self.opt)
		return model
	def remember(self, state, action, reward, new_state, done):
		self.memory.append([state, action, reward, new_state, done])
	def replay(self):
		batch_size = 32
		if len(self.memory) < batch_size:
			return
		samples = random.sample(self.memory, batch_size)
		for sample in samples:
			state, action, reward, new_state, done = sample
			target = self.target_model.predict(state)
			if done:
				target[0][action] = reward
			else:
				Q_future = max(self.target_model.predict(new_state)[0])
				target[0][action] = reward + Q_future * self.gamma
			self.model.fit(state, target, epochs=1, verbose=0)
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



export_dir = os.path.join("models","ssbm.h5")
choice = input()
if choice != '0' and choice != '1':
	debugPrint("That was neither! Exiting.")
	sys.exit(1)

# BUILD MODEL SECTION!
agent = None
if choice == '1':
	debugPrint("Loading model from file...")
	agent = DQN()
	agent.load_model(export_dir)
else:
	debugPrint("Building model:")
	agent = DQN() # Prebuilds...

debugPrint("Finished building/loading! Please input data in the form of P1-HP P1-FD P1-X P1-Y P2-HP P2-FD P2-X P2-Y (as specified ")
# Train...until we hit an end of file...
pa = [0,0,0,0,0,0,0,0]
while True:
	try:
		input_k = input()
		if input_k == "-1 -1":
			break
		action = agent.act(pa) 
		# It is 6 values, brute force
		print(action[0] + " " + action[1] + " " + action[2] + " " + action[3] + " " + action[4] + " " + action[5])
		# Output action....
		
		vv = [float(x) for x in input_k.split(" ")] # Cur state!
		reward = agent.get_Score(pa, vv)
		agent.remember(pa, action, reward, vv, False)
		agent.replay()
		agent.target_train()
		pa = [x for x in vv]
	except:
		break
	

# Save!
debugPrint("End of file reached. Saving model.")
agent.save_model(export_dir)
debugPrint("Finished saving...exiting...")
# train
