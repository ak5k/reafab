# ReaFab

## Fabulous REAPER plug-in controller extension

A REAPER extension to emulate digital console type of workflow for setting channelstrip plugin parameters in the context of currently selected track/channel. Registers 32 custom actions which function as kind of API based virtual control surface, or 'dynamic' macro controls. Similar to Quick Controls in Cubase, except allows for e.g. switching EQ bands as 'pages'. 8 endless rotary encoders in MIDI Relative mode and 24 buttons are supported, and in practice required. 8 buttons are reserved for navigating the 'virtual channelstrip'. Built-in support for some [FabFilter](https://www.fabfilter.com)s, the [ReEQ](https://forum.cockos.com/showthread.php?t=213501) and some stock REAPER plugins.

- 8 encoders, 24 buttons. MIDI Relative mode for instant pickup without need for parameter feedback or soft pickup, and for wide hardware support. Last 8 buttons function as plugin selectors; "1st, 2nd, 3rd etc supported plugin".
- Hopefully sensible defaults for some common plugins. ReaFab should be able to deliver OOTB console type EQing experience with ReEQ and FabFilter Pro-Q 3 (with band dynamics too).
- Operates in the context of currently selected track, automatically assigning controls (ReaFab actions/commands) to detected plugins based on plugin control map support. Detection is based on plugin and parameter names.
- Can be controlled and configured via ReaScript API.
- Plug-in support can be extended practically endlessly with ReaScripting. See `scripts` folder for example.
- Scripts can function purely as custom config files, programmatically control ReaFab itself, or both. Can be expanded to 'infinite page' controller by ReaScripting, e.g. switching between different ReaFab customized configurations.
- ReaScript API functions `reaper.Fab_{Do, Dump, Get, Map, Clear}` for giving direct control input, dumping control map into file, getting absolute target plugin index and parameter and creating new and clearing control maps.
- If [ReaImGui](https://github.com/cfillion/reaimgui) is installed, registers ReaFab Monitor action, which opens up a small monitor window showing current control map for selected plugin on selected track.

### Installation and recommendations

- Install platform specific library file into REAPER UserPlugins directory.
- Set hardware MIDI controller encoders to MIDI Relative. E.g. in X-Touch Mini, set Encoder minval to Relative1. With Akai MPK series (those with endless encoders) use INC/DEC2 mode.
- Map corresponding MIDI controls to ReaFab actions. Encoders in Relative mode.
- Using native REAPER actions to navigate tracks will 'increase productivity'.
- Configure REAPER to 'Only allow one FX chain window open at a time, Open track FX window on track change, Only if any FX window is open'. Otherwise screen will get cluttered.

[More information](https://forum.cockos.com/showthread.php?t=261330)

![image](https://i.imgur.com/bAVdj05.gif)
