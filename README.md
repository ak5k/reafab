# ReaFab
## REAPER plug-in extension to control FabFilter plugins
Registers dedicated custom actions in REAPER for MIDI CC Relative1 input to control FabFilter plugins. Similar to REAPER 'adjust last touched FX parameter', but for 8 endless rotary encoders, 16 buttons and 8 buttons for selecting different FabFilter (or other supported) plugins on currently selected track FX chain. Automatically maps to parameters on currently selected FabFilter plugin. Could also be described as contextual channelstrip hardware MIDI controller. Basically tries to emulate large format digital console workflow for setting up channelstrip parameters (EQ, Gate, Comp etc...), but within REAPER Track FX Chain. Also includes support for some stock REAPER dynamic plugins and the ReEQ. Features an optional small monitor window showing current active mapping for selected track and supported plugin; action name ReaFab Monitor.

Currently supported plugins: 
* [REAPER](https://www.reaper.fm/) Rea{Comp, Gate, Xcomp, Limit}.
* [FabFilter](https://www.fabfilter.com) Pro-{Q3,G,C2,DS,L2,MB} and Saturn.
* The [ReEQ](https://forum.cockos.com/showthread.php?t=213501).

[ReaImGui](https://github.com/cfillion/reaimgui) required. Download ReaImGui from [here](https://github.com/cfillion/reaimgui/releases/latest) or install from [ReaPack](https://reapack.com/)

### Quick start guide
1. Install ReaFab and ReaImgui, like any other REAPER plug-in extension.
2. Set REAPER to show only one FX Chain window at a time. Otherwise screen will get cluttered. Setting REAPER to not open floating windows for plugins on insertion might also be a good choice. Otherwise ReaFab will first try closing floating plugin windows on plugin selection. 
3. After succesful installation, REAPER action list should show ReaFab actions.
4. Set your MIDI hardware controller endless rotary encoders to MIDI CC Relative1 mode. Depending on your device brand, this might be called as Inc/Dec1 or something similar.
5. Map your MIDI hardware controller to ReaFab actions. Encoders in Relative1 mode, buttons preferrably as MIDI notes. E.g. Once encoders are set to Relative1, the Behringer X-Touch Mini/Compact default MIDI layer A works out-of-the-box.
6. Insert supported plugin (FX) on a track, and select the track.
7. Assuming the just inserted plugin is the only plugin (and first) on the track, press Button 9 to select it.
8. REAPER should open Track FX Chain window, with the selected plugin on focus.
9. Turning encoders (pots/knobs) and pressing buttons should now control plugin parameters.

### Notes
* Support for Pro-MB and Saturn 2 is currently somewhat limited/buggy.
* With EQs and multiband plugins, Button 1 and 2 function as next/previous band selectors. Encoder push 1 enables/disables band. Encoder push 2 inverts gain.
* Support for native REAPER ReaEQ will probably not happen with current REAPER API / my programming efforts. ReaEQ seems to index it's bands in relation to bandtypes instead of continues band numbering.
* Assigning dedicated controls to select next and previous track (native REAPER actions) will 'increase productivity'. Or using some other dedicated method for track selection.
* Plugins may be reordered in FX Chain. ReaFab plugin selector always selects 'first supported plugin, second supported plugin' and so on.

![image](https://i.imgur.com/vR1ljy8.gif)
