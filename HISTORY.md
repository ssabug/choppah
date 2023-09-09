# Choppah

## Change records

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
