import glob
import os
import re

startPath = os.path.dirname(__file__) + '\\scripts'
base_pawn_snippets = open(os.path.dirname(__file__) + '\\base_snippets.txt')

local_snippets_file = open(
    "C:\\Users\\Ciro\\AppData\\Roaming\\Code\\User\\snippets\\pawn.json", "w")
print(os.listdir(startPath))
local_snippets_file.write('{\n')

for line in base_pawn_snippets:
    local_snippets_file.write(line)

for root, dirs, files in os.walk(startPath):
    for file in files:
        if file.endswith('.pwn') or file.endswith('.p'):
            file_name = root.replace('scripts\\', '')
            f = open(root + '/' + file)
            local_snippets_file.write(
                '\t// ==========[' + file_name + '/' + file + ']==========\n')
            lastComments = []
            for line in f:
                if(line.startswith('//')):
                    line = line.replace('//', '')
                    lastComments.append(line)
                elif(line.startswith('forward') or line.startswith('static stock') or line.startswith('stock') or line.startswith('native')):
                    func_name = ''
                    params = ''
                    description = ''

                    line = line.replace('stock', '')
                    line = line.replace('static', '')
                    line = line.replace('forward', '')
                    line = line.replace('native', '')
                    line = line.replace('{', '')
                    line = line.replace(';', '')
                    line = line.replace('\"', '\'')
                    T = line.split('//', 1)
                    line = T[0]
                    line = line.split('return', 1)[0]

                    if(len(T) > 1):
                        lastComments.append(T[1].replace('//', ''))
                    # Porcamadonna? That should be faster than the previous
                    params = line[line.find('(')+len('('):line.rfind(')')]

                    # Is strip enough? Should I strip the whole line before?
                    func_name = line.replace(
                        '(' + params + ')', '').replace('\n', '').strip()
                    S = func_name.split(':')
                    if(len(S) > 1):
                        func_name = S[1]
                    split = params.split(',')
                    params = ''
                    # print(func_name)
                    # Params
                    index = 1
                    for s in split:
                        params = params + \
                            '${' + str(index) + ':' + s.strip() + '}'
                        if(index != len(split)):
                            params = params + ', '
                        index = index+1

                    # Description
                    for v in lastComments:
                        description = description + v.strip() + '\\n'

                    # print('Name: ' + func_name + ' Params: ' + params + ' Desc: ' + description)
                    local_snippets_file.write('\t"lsarp_' + func_name + '": {\n\t\t"scope": "pawn",\n\t\t"prefix": "'+func_name +
                                              '",\n\t\t"body": "'+func_name+'(' + params + ')$0",\n\t\t"description": "' + description + '"\n\t},\n')
                else:
                    lastComments = []
            local_snippets_file.write(
                '\t// ==================================================\n')
local_snippets_file.write('}')

#input("Press Enter to exit")
