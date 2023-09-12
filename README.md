# Choppah

As I haven't found yet any trance gate VST for linux, DIY was the only way.

**IMPORTANT** For the moment the processing part has just started, so it may sound **CRAP**.

It is my very first JUCE project and i suck at C++ so...apologies for the crappy code ^^ .

## Description
![alt text](Ressources/images/GUI.png)
The chopper has 2 modes : pattern & sequence.

In **pattern** mode, the step seq current selected pattern is looping.It's the first line. 
You can change the pattern to play/edit with the upper left combo box.
Note that patterns have a fixed length of 16.

In **sequence** mode, the chopper runs the combobox sequence of patterns.It's the second line.
The current sequence can be changed with the corresponding combobox to the left.
You can change the sequence length with the **seq length** left slider.

To stop auto pattern display in **sequence** mode, disable **Auto** button.

**clock division** apply for pattern mode only.

**Clear** erases current selected pattern.

**Reset** erases all patterns.

**Copy** copies the current selected pattern.

**Paste** pastes the clipboard content into current pattern.If no copy was done, it is an empty pattern.

**NOTE :** Theres a bug on some combo boxes : sometimes the button doesnt change the value.The workaround is to click on the combobox (not on the arrow) then reclick here again,then you can change values with Up/Down keys.  

## Requirements
JUCE Version 7.0.7

gcc

## Installation
### Linux
 - get the pre-built **chopper.vst3** folder in **Releases** or build it (see below).
 - put it in your VST3 folder
 - run it. images should be missing but folders have been created in **~/.ssabug/choppa/**,
 - so, copy the following:
 - **{GIT_PACKAGE}/Ressources/config_files/config.xml** in **~/.ssabug/choppa/config/**
 - **{GIT_PACKAGE}/Ressources/config_files/skin.xml**   in **~/.ssabug/choppa/skins/default/**
 - all files of **{GIT_PACKAGE}/Ressources/images/**    in **~/.ssabug/choppa/skins/default/images/**
 - reload the plugin

## Building 
### Linux
 - git clone the repo or get a release zip. Extract if necessary.
 - in file **Builds/LinuxMakefile/Makefile** modify variable **JUCE_MODULES_INSTALLATION_DIR** to point to your juce module directory
 - build with `cd Builds/LinuxMakefile/ && clear && make clean && make`
 - the VST3 is the  **Builds/LinuxMakefile/build/chopper.vst3** directory

Tested OSs  : fedora

Tested DAWs : bitwig
