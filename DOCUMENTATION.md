# Choppah
![alt text](Ressources/images/GUI.png)
The chopper has 2 modes : pattern & sequence.

In **pattern** mode, the step seq (first line) current selected pattern is looping
You can change the pattern to play/edit with the upper left combo box.
Patterns have a fixed length of 16.

In **sequence** mode, the chopper runs the combobox sequence of patterns (second line).
The current sequence can be changed with the corresponding combobox to the left.
**Seq length** bottom left slider changes the sequence length.

To stop auto pattern display in **sequence** mode, disable **Auto** button.

**Clock division** applies for pattern mode only.

**Clear** erases current selected pattern.

**Reset** erases all patterns.

**Copy** copies the current selected pattern.

**Paste** pastes the clipboard content into current pattern.If no copy was done, it is an empty pattern.

**Load cfg** reloads plugin default state (defined in **config.xml**).

**Save** saves preset to the name filled in **Presets**, if the preset already exists, it will be overwritten.

Click on the logo to switch between skins.

**NOTE :** Theres a bug on some combo boxes : sometimes the button doesnt change the value.The workaround is to click on the combobox (not on the arrow) then reclick here again,then you can change values with **Up/Down keys**.  
