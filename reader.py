import glob
import os
import re

commands = open('commands.txt')

string = "switch(YHash(cmdtext, false))\n{ \n\t"
index = 0

for line in commands:
	line = line.replace('/', '')
	line = line.replace('\n', '')
	line = line.replace(' ', '')
	# string += "case _I<" + line + ">: unallowedCommand = true, return SendClientMessage(playerid, COLOR_ERROR, message);\n"
	if('.' in line):
		continue #string += "_I(" + ','.join(list(line)) + "), "
	else:
		string += "_I<" + line + ">, "
	if index == 10:
		index = 0
		string += "  \n\t"
	index = index + 1
	#string += "_I(" + ','.join(l) + "), "

string += "}"

print(string)

