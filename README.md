# ReaFab
## REAPER plugin to control FabFilter plugins
Registers dedicated custom actions in REAPER for MIDI CC Relative1 input to control FabFilter plugins. Similar to REAPER 'adjust last touched FX parameter', but for 8 endless rotary encoders, 16 buttons and 8 buttons for selecting different FabFilter plugins on currently selected track FX chain. Automatically maps to parameters on currently selected FabFilter plugin.

[More information](https://forum.cockos.com/showthread.php?t=261330)

![image](https://i.imgur.com/vR1ljy8.gif)

| Ctrl | Q3         | G       | C2      | L2    | MB      | Saturn2 | all |
|------|----        |---      |----     |----   |----     |-----    | --- |
| Enc1 | band freq  | thr     | thr     |gain   |b thr    |drive    |
| Enc2 | band gain  | rat     | rat     |output |b rat    |dyn      |
| Enc3 | band q     | att     | att     |att    |b att    |cross    |
| Enc4 | band dr    | rel     | rel     |rel    |b rel    |level    |
| Enc5 | band thr   | hold    | hold    |link tr|b knee   |mix      |
| Enc6 |            | knee    | knee    |lin rel|b range  |         |
| Enc7 |            | range   | dry     |looka  |b cross  |         |
| Enc8 |            |         | wet     |       |b level  |         |
| B1   | prev band  | p style | p style |p style|p band   |p band   |
| B2   | next band  | n style | n style |n style|n band   |n band   |
| B3   | prev shape |         | auto g  |unity g|dyn mode |p style  |
| B4   | next shape |         |         |       |         |n style  |
| B5   | prev slope |         |         |       |         |         |
| B6   | next slope |         |         |       |         |         |
| B7   |            |         |         |       |         |dec bands|
| B8   |            |         |         |       |         |inc bands|
| B9   |            |         |         |       |         |         |plugin1
| B10  |            |         |         |       |         |         |plugin2
| B11  |            |         |         |       |         |         |plugin3
| B12  |            |         |         |       |         |         |plugin4
| B13  |            |         |         |       |         |         |plugin5
| B14  |            |         |         |       |         |         |plugin6
| B15  |            |         |         |       |         |         |plugin7
| B16  |            |         |         |       |         |         |plugin8
| Enp1 |            |   |    |    |         |
| Enp2 |            |   |    |    |         |
| Enp3 |            |   |    |    |         |
| Enp4 |            |   |    |    |         |
| Enp5 |            |   |    |    |         |
| Enp6 |            |   |    |    |         |
| Enp7 |            |   |    |    |         |
| Enp8 |            |   |    |    |         |

