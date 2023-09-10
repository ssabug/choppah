#remake && rm -rf ~/VST/chopper.vst3/ && cp -r /home/pwner/dev/chopper/Builds/LinuxMakefile/build/chopper.vst3 ~/VST/
workingpath="${HOME}/dev/choppah/Builds/LinuxMakefile"
buildpath="${workingpath}/build/"
vstpath="${HOME}/VST/"
pluginname=choppah.vst3
currentpath=$(pwd)
clear
echo "moving to dir ${workingpath}"
cd "${workingpath}"
make clean
make
echo "removing old vst ${vstpath}${pluginname}/"
rm -rf "${vstpath}${pluginname}/"
echo "copying new vst ${buildpath}/${pluginname} to ${vstpath}"
cp -r "${buildpath}/${pluginname}" "$vstpath"


echo "clean build folder for github"
make clean
echo "coming back to dir ${currentpath}"
cd "${currentpath}"

