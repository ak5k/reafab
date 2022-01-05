# ReaFab
## REAPER Fabulous contextual plug-in controller and API extension
A REAPER extension to emulate digital console type of workflow for setting plugin parameters in the context of currently selected track/channel. Registers 32 custom actions which function as kind of API based virtual control surface, or 'dynamic automapping' macro controls. 8 endless rotary encoders in MIDI Relative mode and 24 buttons are supported. 8 buttons are reserved for navigating the 'virtual channelstrip'. Built-in support for some [FabFilter](https://www.fabfilter.com)s, the [ReEQ](https://forum.cockos.com/showthread.php?t=213501) and some stock REAPER plugins.

* 8 encoders, 24 buttons. MIDI Relative mode for instant pickup without need for parameter feedback or soft pickup, and for wide hardware support. Last 8 buttons function as plugin selectors; "1st, 2nd, 3rd etc supported plugin".
* Hopefully sensible defaults for some common plugins.
* Operates in the context of currently selected track, automatically assigning controls (ReaFab actions/commands) to detected plugins based on plugin control map support. Based on detected plugin and parameter names.
* Can be controlled and configured via ReaScript API. 
* Plug-in support can be extended practically endlessly with ReaScripting. See ```scripts``` folder for example. 
* Scripts can function purely as custom config files, programmatically control of ReaFab itself, or both. Can be expanded to 'infinite page' controller by ReaScripting, e.g. switching between different ReaFab configurations.
* ReaScript API functions ```reaper.Fab_{Do, Get, Map, Clear}``` for giving direct control input, getting absolute target plugin index and parameter and creating new and clearing control maps.
* If [ReaImGui](https://github.com/cfillion/reaimgui) is installed, registers ReaFab Monitor action. It opens up a small monitor window showing current control map for selected plugin on selected track.

### Installation and recommendations
* Place platform specific library file into REAPER UserPlugins directory.
* Map corresponding MIDI controls to ReaFab actions. Encoders in Relative mode.
* Using REAPER native actions to navigate tracks will 'increase productivity'.
* Configure REAPER to show only one Track FX Chain window at a time. Otherwise screen will get cluttered.

![image](https://i.imgur.com/bAVdj05.gif)
