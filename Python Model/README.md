Python trainer:
	Required packages:
		tensorflow and all required packages (installed using anaconda)
		h5py
		
To use this software:
	It will first require a single number (0 or 1). If it is zero, any and all debugging output (prompts, notifications, etc) will not be printed to the stdout stream, baring kera's training prints. If it is non-zero, then it will print to stdout as normal.
	
	On the following line, it will expect another number (0 or 1), to specify whether it should be creating a new neural net to train or if it should attempt to load the previously saved net.
	
	To train, it expects 8 numbers (floats, but may be ints)
		Player 1's HP (0 - 999)
		Player 1's Facing Direction (-1 and +1)
		Player 1's X position (? bounds)
		Player 1's Y position (? bounds)
		Player 2's HP (0 - 999)
		Player 2's Facing Direction (-1 and +1)
		Player 2's X position (? bounds)
		Player 2's Y position (? bounds)
		
	All of these should be on the same line, seperated by a space and the decimal seperator should be a '.' and not a ','.
	
	To finish training, either close the input (Control + D unix, Control+Z+Newline windows, or send EOF through the stdin), or send the line "-1 -1". The model will then be saved and the program should terminate on its own.
	
	
	Note this saves the keras models using h5 formatting.
	
Todo:
	Need to figure out how to silence kera's training notifications/debugging.