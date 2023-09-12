# Choppah

## Change records

### v0.11 11092023 XML & control template class on going
#### Features
- [x] XML config (READ ONLY) : skin, patterns & seq implemented,**plugin controls values to be implemented**
- [x] XML skin (READ ONLY): colours implemented, only in reloadGui(), **control location/style to be implemented** 
- [ ] XML skin & config write 
- [x] GUI control template class created **(to be better defined)** : only in reloadGui(), **implementation on editor construction in progress**
- [x] reloadGui() function to update controls skins/color/images on the fly 
- [ ] find a safe way to trigger reloadGui() once at startup (manual trigger for now)
- [ ] configure/build scripts exists but not portable
- [ ] save sequencer state variables
- [ ] gain slider custom range implementation
- [ ] code cleaning + opti
#### Bugs
- [ ] XML skin implementation : trigerred manually,differences on default skin between init and load
- [ ] reloadGui() crashes when triggered sometimes
- [ ] reloadGui() causes dry/wet value change
- [ ] sequences dont update after loadXMLConfig
- [ ] optimize/compensate latency
- [ ] sequence mode : sequence 0 not init as expected
- [ ] sequence mode/pattern/sequence change hard to switch
- [ ] plugin parameters not in line with GUI values on start

### v0.10 10092023 DryWet implementation & project cleaning
#### Features
- [x] dry/wet processing implementation
- [x] out gain applies to mixed out
- [x] plugin data directories defined & automatically created (linux only)
- [ ] XML file defined but unable to copy XML elements to variable
- [x] proper JUCE definition project (manufacturer,vendor ids, ...)
- [x] removed standalone building
- [x] added VisualStudio2017 and MacOSX built folders (but not used yet)
- [ ] configure/build scripts exists but not portable (dont forget to copy config.xml and default skin files)
- [ ] save sequencer state variables
- [ ] gain slider custom range implementation
- [ ] code cleaning + opti
#### Bugs
- [ ] optimize/compensate latency
- [ ] sequence mode : sequence 0 not init as expected
- [ ] sequence mode/pattern&sequence change hard to switch
- [ ] plugin parameters not in line with GUI values on start

### v0.05 09092023 Sequence mode polish
#### Features
- [x] better pattern default filling
- [ ] do clean configure/build scripts
- [ ] dry/wet processing implementation
- [ ] gain slider custom range implementation
- [ ] proper JUCE definition project (manufacturer,vendor ids, ...)
- [ ] code cleaning + opti
#### Bugs
- [ ] sequence mode : sequence 0 not init as expected
- [ ] sequence mode hard to switch
- [ ] plugin parameters not in line with GUI values on start

### v0.04 09092023 Sequence mode fixing end?
#### Features
- [x] sequence mode works as expected
- [x] added auto pattern change button
- [ ] do clean configure/build scripts
- [ ] dry/wet processing implementation
- [ ] gain slider custom range implementation
- [ ] code cleaning + opti
#### Bugs
- [ ] sequence mode : sequence 0 not init as expected
- [x] sequence mode : need to change patterns in pattern mode before switching to sequence mode
- [ ] sequence mode hard to switch
- [ ] plugin parameters not in line with GUI values on start

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
