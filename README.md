# CustomKeyboard
This is my custom lighting and macro application for the Logitech G910 keyboard. The G keys are bound to F16 to F24 to manage the macros.

Possible effects are:
- color cycle: cycles through all RGB
- echo: on a key press, change the color and fade out
- fill: fill a set of keys with a color, on a particular condition

Only fill effect layers are boundable to keys; color cycle and echo are global. To specify the key in a fill layer, refer to `toKey` in `CustomKeyboard.h`.

Possible macros are:
- key sequence: play a sequence of keys, with configurable delay
- delay: delay for a duration
- launch application: starts an application with arguments
- toggle mic: mute or unmute the microphone
- cycle audio: set the default audio output to the next in queue
- sleep: put the computer in standby

Macros are boundable to any key; to specify the key refer to `toVKCode` and `toScanCode` in `CustomKeyboard.h`.
As the names of the maps imply, both Virtual Keys and Scan Codes are supported.
To specify which, use the `keyType` parameter in the settings json, and use `virtual` or `scan`.
Some applications may only be able to interpret scan codes, but virtual keys offer more functionality.

This includes an adapation of [MicMuteToggle](https://github.com/fredemmott/MicMuteToggle/tree/master/MicMuteToggle)
and uses some code from [here](https://stackoverflow.com/questions/57778069/how-can-i-programmatically-set-the-default-input-and-output-audio-device-for-an)
to manage audio devices.

## Customization
To change the mapped keys (F16-F24), edit the constants at the top of `CustomKeyboard.cpp`.
To configure the macros and layers, edit the settings file; this should be placed in the same folder as the executable.
The application will automatically reload the settings file on change.

## TODO
- add macro trigger types: on key press, key up, repeat, etc; currently macros are fired on key up
- add key selection to color cycle and echo layers