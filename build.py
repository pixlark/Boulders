import os
import sys

names = ["main", "utility"]
include_dirs = [
	"G:\\C\\.libraries\\SDL2_ttf-2.0.14\\include",
	"G:\\C\\.libraries\\SDL2-2.0.7\\include",
	"G:\\C\\.libraries\\SDL2_mixer-2.0.2\\include",
	"G:\\C\\.libraries\\SDL2_image-2.0.2\\include"
]

# DEPENDENCIES FOR 64-BIT
dependencies64 = [
	"G:\\C\\.libraries\\SDL2-2.0.7\\lib\\x64\\SDL2.lib",
	"G:\\C\\.libraries\\SDL2-2.0.7\\lib\\x64\\SDL2main.lib",
	"G:\\C\\.libraries\\SDL2_ttf-2.0.14\\lib\\x64\\SDL2_ttf.lib",
	"G:\\C\\.libraries\\SDL2_mixer-2.0.2\\lib\\x64\\SDL2_mixer.lib",
	"G:\\C\\.libraries\\SDL2_image-2.0.2\\lib\\x64\\SDL2_image.lib"
]
release_dependencies64 = [
]
debug_dependencies64 = [
]

# DEPENDENCIES FOR 32-BIT
dependencies32 = [
	"G:\\C\\.libraries\\SDL2-2.0.7\\lib\\x86\\SDL2.lib",
	"G:\\C\\.libraries\\SDL2-2.0.7\\lib\\x86\\SDL2main.lib",
	"G:\\C\\.libraries\\SDL2_ttf-2.0.14\\lib\\x86\\SDL2_ttf.lib",
	"G:\\C\\.libraries\\SDL2_mixer-2.0.2\\lib\\x86\\SDL2_mixer.lib",
	"G:\\C\\.libraries\\SDL2_image-2.0.2\\lib\\x86\\SDL2_image.lib"
]
release_dependencies32 = [
]
debug_dependencies32 = [
]

source_files = ["..\\src\\" + n + ".cc" for n in names]
source_files = ' '.join(source_files)

obj_files = ["obj\\" + n + ".obj" for n in names]
obj_files = ' '.join(obj_files)

include_opts = ["/I \"" + i + "\"" for i in include_dirs]
include_opts = ' '.join(include_opts)

x64_dir = "C:\\\"Program Files (x86)\"\\\"Microsoft Visual Studio 14.0\"\\VC\\bin\\amd64\\"
x86_dir = "C:\\\"Program Files (x86)\"\\\"Microsoft Visual Studio 14.0\"\\VC\\bin\\"

build_types = []

exe_name = "boulders.exe"

for i in range(len(sys.argv)):
	if sys.argv[i].startswith('-64'):
		if ('r' in sys.argv[i]):
			build_types.append("RELEASE64")
		if ('d' in sys.argv[i]):
			build_types.append("DEBUG64")
	if sys.argv[i].startswith('-32'):
		if ('r' in sys.argv[i]):
			build_types.append("RELEASE32")
		if ('d' in sys.argv[i]):
			build_types.append("DEBUG32")

if ("RELEASE64" in build_types):
	print("""\
- - - - - - - - - - - - - - - - - - 
COMPILING RELEASE VERSION FOR 64-BIT
- - - - - - - - - - - - - - - - - - """)
	run_cmds = []
	run_cmds.append(x64_dir + "vcvars64.bat")
	run_cmds.append("cd bin64")
	run_cmds.append("cl /EHsc {0} {1} /MT /c /Fo.\\obj\\"\
		.format(source_files, include_opts))
	run_cmds.append("link /LTCG {0} {1} {2} /out:{3}"\
		.format(obj_files, ' '.join(dependencies64), ' '.join(release_dependencies64), exe_name))
	run_cmds.append("cd ..")
	run_cmds.append("robocopy dll64 bin64 /COPYALL /E > nul")
	os.system(' & '.join(run_cmds))

if ("DEBUG64" in build_types):
	print("""\
- - - - - - - - - - - - - - - - - 
COMPILING DEBUG VERSION FOR 64-BIT
- - - - - - - - - - - - - - - - - """)
	run_cmds = []
	run_cmds.append(x64_dir + "vcvars64.bat")
	run_cmds.append("cd debug64")
	run_cmds.append("cl /ZI /EHsc {0} {1} /MTd /c /Fo.\\obj\\"\
		.format(source_files, include_opts))
	run_cmds.append("link /DEBUG:FULL /LTCG {0} {1} {2} /out:{3}"\
		.format(obj_files, ' '.join(dependencies64), ' '.join(debug_dependencies64), exe_name))
	run_cmds.append("cd ..")
	run_cmds.append("robocopy dll64 debug64 /COPYALL /E > nul")
	os.system(' & '.join(run_cmds))

if ("RELEASE32" in build_types):
	print("""\
- - - - - - - - - - - - - - - - - - 
COMPILING RELEASE VERSION FOR 32-BIT
- - - - - - - - - - - - - - - - - - """)
	run_cmds = []
	run_cmds.append(x86_dir + "vcvars32.bat")
	run_cmds.append("cd bin32")
	run_cmds.append("cl /EHsc {0} {1} /MT /c /Fo.\\obj\\"\
		.format(source_files, include_opts))
	run_cmds.append("link /LTCG {0} {1} {2} /out:{3}"\
		.format(obj_files, ' '.join(dependencies32), ' '.join(release_dependencies32), exe_name))
	run_cmds.append("cd ..")
	run_cmds.append("robocopy dll32 bin32 /COPYALL /E > nul")
	os.system(' & '.join(run_cmds))

if ("DEBUG32" in build_types):
	print("""\
- - - - - - - - - - - - - - - - - 
COMPILING DEBUG VERSION FOR 32-BIT
- - - - - - - - - - - - - - - - - """)
	run_cmds = []
	run_cmds.append(x86_dir + "vcvars32.bat")
	run_cmds.append("cd debug32")
	run_cmds.append("cl /ZI /EHsc {0} {1} /MTd /c /Fo.\\obj\\"\
		.format(source_files, include_opts))
	run_cmds.append("link /DEBUG:FULL /LTCG {0} {1} {2} /out:{3}"\
		.format(obj_files, ' '.join(dependencies32), ' '.join(debug_dependencies32), exe_name))
	run_cmds.append("cd ..")
	run_cmds.append("robocopy dll32 debug32 /COPYALL /E > nul")
	os.system(' & '.join(run_cmds))
