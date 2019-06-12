import glob
import os
import re

commands = open('commands.txt')

string = "const static banned_commands[] = {"

for line in commands:
	string += ("\"" + line.strip() + "\",")

string += "};"

print(string)