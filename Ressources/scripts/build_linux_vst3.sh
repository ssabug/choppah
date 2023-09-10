workingpath="${HOME}/dev/chopper/Builds/LinuxMakefile"
buildpath="${workingpath}/build/"
pluginname=chopper.vst3
currentpath=$(pwd)
clear
echo "moving to dir ${workingath}"
cd "${workingath}"
make clean
make
echo "Compilation ended."
echo "Check if errors above, else you can copy ${buildpath}${pluginname}" folder to your VST directory
