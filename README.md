# ReaFab
## REAPER plugin to control FabFilter plugins
Registers dedicated custom actions in REAPER for MIDI CC Relative1 input to control FabFilter plugins. Similar to REAPER 'adjust last touched FX parameter', but for 8 endless rotary encoders, 16 buttons and 8 buttons for selecting different FabFilter (or other supported) plugins on currently selected track FX chain. Automatically maps to parameters on currently selected FabFilter plugin. Could also be described as contextual channelstrip hardware MIDI controller. Basically tries to emulate large format digital console workflow for setting up channelstrip parameters (EQ, Gate, Comp etc...), but within REAPER Track FX Chain. Also includes support for some stock REAPER dynamic plugins and the ReEQ. Features an optional small monitor window showing current active mapping for selected track and supported plugin; action name ReaFab Monitor.

Currently supported plugins: 
* [REAPER](https://www.reaper.fm/) Rea{Comp, Gate, Xcomp, Limit}.
* [FabFilter](https://www.fabfilter.com) Pro-(Q3,G,C2,L2,MB) and Saturn.
* The [ReEQ](https://forum.cockos.com/showthread.php?t=213501).

[ReaImGui](https://github.com/cfillion/reaimgui) required. Download ReaImGui from [here](https://github.com/cfillion/reaimgui/releases/latest) or install from [ReaPack](https://reapack.com/)

[More information](https://forum.cockos.com/showthread.php?t=261330)

![image](https://i.imgur.com/vR1ljy8.gif)
