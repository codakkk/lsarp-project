import glob, os
import re

startPath = 'scripts'
snippets_file = open("snippets.txt", "w") 

print(os.listdir(startPath))

for root, dirs, files in os.walk(startPath):
    for file in files:
        if file.endswith('.pwn'):
            file_name = root.replace('scripts\\', '')
            f = open(root + '/' + file)
            snippets_file.write('\t// ==========[' + file_name + '/' + file + ']==========\n')
            lastComments = []
            for line in f:
                if(line.startswith('//')):
                    line = line.replace('//', '')
                    lastComments.append(line)
                elif(line.startswith('forward') or line.startswith('static stock') or line.startswith('stock')):
                    func_name = ''
                    params = ''
                    description = ''
                    
                    line = line.replace('stock', '')
                    line = line.replace('static', '')
                    line = line.replace('forward', '')
                    line = line.replace(';', '')
                    T = line.split('//', 1)
                    line = T[0]
                    line = line.split('return', 1)[0]
                    if(len(T) > 1):
                        lastComments.append(T[1].replace('//', ''))
                    params = line[line.find('(')+len('('):line.rfind(')')] # Porcamadonna? That should be faster than the previous 
                    
                    func_name = line.replace('(' + params + ')', '').replace('\n', '').strip() # Is strip enough? Should I strip the whole line before?
                    
                    split = params.split(',')
                    params = ''

                    # Params
                    index = 1
                    for s in split:
                        params = params + '${' + str(index) + ':' + s.strip() + '}'
                        if(index != len(split)):
                            params = params + ', '
                        index = index+1

                    # Description
                    for v in lastComments:
                        description = description + v.strip() + '\\n'
                    
                    # print('Name: ' + func_name + ' Params: ' + params + ' Desc: ' + description)
                    snippets_file.write('\t"lsarp_' + func_name + '": {\n\t\t"prefix": "'+func_name+'",\n\t\t"body": "'+func_name+'(' + params + ')",\n\t\t"description": "' + description + '"\n\t},\n')
                else:
                    lastComments = []
            snippets_file.write('\t// ==================================================\n')    


# input("Press Enter to exit")