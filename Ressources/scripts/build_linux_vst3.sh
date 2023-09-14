workingpath="${HOME}/dev/chopper/Builds/LinuxMakefile"
workingpath ="$1"
buildpath="${workingpath}/build/"
pluginname=chopper.vst3
currentpath=$(pwd)

if [ -n "$1" ]
then
    echo "Did you modify the Makefile with your JUCE path? (y/n)"
    read answer
    if [ "$answer" = "y" ]; then

        clear
        echo "moving to dir ${workingath}"
        cd "${workingath}"
        make clean
        make
        echo "Compilation ended."
        echo "Check if errors above, else you can copy ${buildpath}${pluginname}" folder to your VST directory
    else
        echo "Have a look at README.md file for more details"

else
    echo "Error : no git hub files folder provided as argument"
fi
