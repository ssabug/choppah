

read -p "Enter the folder of your JUCE installation" JUCE_DIR
read -p "Enter the folder of your Choppa git directory" GIT_DIR
JUCE_DIR="${JUCE_DIR}/modules"
echo "JUCE PATH = ${JUCE_DIR}" 
echo "GIT PATH = ${GIT_DIR}" 

##### MAKEFILE for juce module path modification 
MAKEFILE_PATH="${GIT_DIR}/Builds/LinuxMakefile/Makefile"
line_to_be_removed="JUCE_MODULES_INSTALLATION_DIR := "
echo "Modifying Makefile ${MAKEFILE_PATH}" 
#sed 's/.*JUCE_MODULES_INSTALLATION_DIR := .*/./' /tmp/foo

##### edit imagePath in PluginEditor.h
EDITOR_PATH="${GIT_DIR}/Source/PluginEditor.h"
IMAGE_PATH="${GIT_DIR}/Ressources/images"
line_to_be_removed="const std::string imagePath="
echo "Modifying image folder in ${EDITOR_PATH} to ${IMAGE_PATH}" 

##### edit build directory in build script 
SCRIPT_PATH="${GIT_DIR}/Ressources/scripts/build_vst3_linus.sh"
line_to_be_removed="workingpath="
echo "Modifying build directory in ${SCRIPT_PATH} "

echo "Finished"

echo "If no signifiant error showed above, you can now run ./build_vst3_linux.sh "

