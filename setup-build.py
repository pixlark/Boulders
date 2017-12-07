import os

directories = ['bin64', 'debug64', 'dll64', 'bin32', 'debug32', 'dll32']
sub_dirs = ['obj']

for d in directories:
	os.system('mkdir ' + d)
	for s in sub_dirs:
		os.system('mkdir ' + d + '\\' + s)

print('Build directories initialized')
