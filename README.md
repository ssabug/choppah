# Choppah

## Description
![alt text](Ressources/images/GUI.png)
As I haven't found yet any trance gate VST for linux, DIY was the only way.

**IMPORTANT** For the moment the processing part has just started, so it may sound **CRAP**.

It is my very first JUCE project and i suck at C++ so...apologies for the crappy code ^^ .

## Documentation
see ![DOCUMENTATION](DOCUMENTATION.md) for more details.

## Requirements
 - JUCE Version 7.0.7
 - CMake

## Installation
### Linux
<details>
  <summary> Instructions </summary>
  
 - get the pre-built **.vst3/.lv2** folder in **Releases** or build it (see below).
 - put it in your VST3/LV2 folder
 - run the script `./Ressources/scripts/copy_plugin_data_linux.sh`

 </details>

 The plugin data is stored in `${HOME}/.ssabug/choppah`

### Windows
<details>
  <summary> Instructions </summary>

 - get the pre-built **.vst3** folder in **Releases** or build it (see below).
 - put it in your VST3 folder
 - run the script `.\Ressources\scripts\copy_plugin_data_windows.cmd`

 </details>

  The plugin data is stored in `%HomePath%\ssabug\choppah`

## Building 

In the root folder of the repository, create a symbolic link or a copy to your JUCE folder ( named "JUCE" ).

### Linux
<details>
  <summary> Instructions </summary>

 - git clone the repo or get a release zip. Extract if necessary.
 - then in a terminal, type `mkdir build && cd build && cmake .. && make`

 Cross compilation can be done with `cmake -DCOMP_TARGET::STRING=windows ..`.You'll have to edit `CMakeLists.txt` file to select the compiler and the libraries.
 </details>

 ### Windows
 <details>
  <summary> Instructions </summary>

 - git clone the repo or get a release zip. Extract if necessary.
 - in the created folder `choppah`, open the file **choppah.jucer** with JUCE Projucer
 - generate the project for your IDE and build
 </details>

## Compatibility
### Linux  
|**Working**           |  **Not working**      |
|:--------------------:|:---------------------:|
| Bitwig               | Ardour                |
| Qtractor             | Tracktion Waveform 12 |
|                      | Carla                 |
### Windows
|**Working**           |  **Not working**      |
|:--------------------:|:---------------------:| 
|                      |  Bitwig (TBC)         |
|                      |  Ableton 11 (TBC)     |

Linux distributions tested : Fedora, Arch Linux

Windows versions tested : 11

## History
see ![HISTORY](HISTORY.md) for more details.