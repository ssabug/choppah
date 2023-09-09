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

**Gate length**, **clock division** apply for pattern mode only.

**Clear** erases current selected pattern.

**Reset** erases all patterns.

**Copy** copies the current selected pattern.

**Paste** pastes the clipboard content into current pattern.If no copy was done, it is an empty pattern.

**NOTE :** Theres a bug on pattern/sequence/mode selection; sometimes the button doesnt change the value.The workaround is to click on the combobox (not on the arrow) then reclick here again,then you can change values with Up/Down keys.  

## Requirements
JUCE Version 7.0.7

gcc

## Installation
### Linux
 - get the pre-built **chopper.vst3** folder in **Releases** or build it (see below).
 - put it in your VST3 folder

## Building 
### Linux
 - git clone the repo or get a release zip. Extract if necessary.
 - in file **Source/PluginEditor.h** modify variable **const std::string imagePath** to the **Ressources/images** path of the git installed
 - in file **Builds/LinuxMakefile/Makefile** modify variable **JUCE_MODULES_INSTALLATION_DIR** to point to your juce module directory
 - build with `cd Builds/LinuxMakefile/ && clear && make clean && make`
 - the VST3 is the  **Builds/LinuxMakefile/build/chopper.vst3** directory

Tested OSs  : fedora

Tested DAWs : bitwig  

## Change records
### v0.03 09092023 Sequence mode fixing
#### Features
- [x] sequence mode almost work as expected
- [ ] do clean configure/build scripts
- [ ] dry/wet processing implementation
- [ ] gain slider custom range implementation
- [ ] code cleaning + opti
#### Bugs
- [ ] sequence mode : sequence 0 not init as expected
- [ ] sequence mode : need to change patterns in pattern mode before switching to sequence mode
- [ ] changes not consistent while using plugin params/plugin GUI
### v0.02 09092023 Smoothing
- [x] smooth chopped gain with juce::SmoothedValue
- [x] images path defined in pluginProcessor.h
- [ ] do clean configure/build scripts
- [ ] dry/wet processing implementation
- [ ] gain slider custom range implementation
- [ ] pattern sequence management
- [ ] problem with updating processor parameters via GUI // processor parameter change updating UI - ex : sometimes you try to change pattern on the GUI, it doesnt
- [ ] cannot attach custom lambda function to processor parameter, ex : the pattern# change to trigger gateMap update 
### v0.01 09092023 Initial release
- [x] draft GUI design
- [x] first implementation of basic chopper processing code
- [x] main processor parameters and controls definition
- [ ] dry/wet processing implementation
- [ ] gain slider custom range implementation
- [ ] pattern sequence management
- [ ] clicks as fuck : switch gateMap to pointer or LinearSmoothedValue
- [ ] remove all absolute path and find an easy way to update them
- [ ] problem with updating processor parameters via GUI // processor parameter change updating UI - ex : sometimes you try to change pattern on the GUI, it doesnt
- [ ] cannot attach custom lambda function to processor parameter, ex : the pattern# change to trigger gateMap update 



