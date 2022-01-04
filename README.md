# ReaFab
## REAPER Fabulous contextual plug-in controller extension
A REAPER extension to emulate digital console type of workflow for setting plugin parameters in the context of currently selected track/channel. Registers 32 custom actions which function as kind of API based virtual control surface. 8 endless rotary encoders in MIDI CC Relative1 mode and 24 buttons are supported. 8 buttons are reserved for navigating the 'virtual channelstrip'. Built-in support for some [FabFilter](https://www.fabfilter.com)s, the [ReEQ](https://forum.cockos.com/showthread.php?t=213501) and some stock REAPER plugins.

* 8 encoders, 24 buttons. MIDI CC Relative1 mode for instant pickup without need for parameter feedback or soft pickup, and for wide hardware support.
* Hopefully sensible defaults for some common plugins.
* Operates in the context of currently selected track, automatically assigning controls (ReaFab actions/commands) to detected plugins based on plugin control map support. 
* Can be controlled and configured via ReaScript API. Plug-in support can be extended practically endlessly with ReaScripts. See ```scripts``` folder for examples. Scripts can function purely as custom config files, programmatical control of ReaFab itself, or both.
* In fact, with ReaScripting can be expanded to 'infinite page' controller.
* API functions for creating new and clearing control maps, setting direct control input and getting absolute target plugin index and parameter.
* If [ReaImGui](https://github.com/cfillion/reaimgui) is installed, registers ReaFab Monitor action which opens up a small monitor window showing current control map for selected plugin on selected track.


![image](https://i.imgur.com/vR1ljy8.gif)
