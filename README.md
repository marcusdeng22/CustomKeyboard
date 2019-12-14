# CustomKeyboard
This is an adapation of [MicMuteToggle](https://github.com/fredemmott/MicMuteToggle/tree/master/MicMuteToggle)
and uses some code from [here](https://stackoverflow.com/questions/57778069/how-can-i-programmatically-set-the-default-input-and-output-audio-device-for-an).

## Overview
Listens on key press of `F23` and `F24`. `F23` is used to cycle through audio outputs; it will set the default output device to the next in queue. `F24` is used to toggle microphone mute.
This is achieved through usage of the Windows [mmdeviceapi.h](https://docs.microsoft.com/en-us/windows/win32/api/mmdeviceapi/).

## Customization
To change the mapped keys, edit the constants at the top of `CustomKeyboard.cpp`.