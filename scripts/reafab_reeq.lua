-- target plugin is identified based on this string 
-- can be the exact or partial plugin name
fxId = "ReEQ"

-- let's clear the built-in mapping for ReEQ first
reaper.Fab_Clear(fxId)

-- map first command
command = 1 -- first encoder, 1..8 for encoders, 9-24 for push and buttons
paramId = "Filter00 Frequency" -- 00 is a placeholder for selected band number
control = 8 -- control type, 8 for forced continuous, 1 for direct/autodetect
bands = 5 -- support for 5 bands, apparently 5 bands can be controlled via API in ReEQ
step = 0.4 -- change step to better suite hardware controlling ReEQ parameters

--[[
step value is dependent on target parameter, as in this case. when used with
control type 8, can be used to override detected or built-in step size.

when mapping a single or wideband processor, such as a regular compressor, bands
can be omitted/left out/nilled

bands = 1 will be a special case for some plugins, which while being single
band, still label parameters indexed with a number. although, this can be handled by
writing this by hand in paramId as well. or with native REAPER
mappings
]] --

reaper.Fab_Map(fxId, command, paramId, control, bands, step)

-- second encoder for Gain
reaper.Fab_Map(fxId, 2, "Filter00 Gain", 8, bands, step)

-- third for Q
reaper.Fab_Map(fxId, 3, "Filter00 Q", 8, bands, step)

--[[
4th for selecting band type
control type 1
encoder will function as stepped selector based on detected parameter
]] --
reaper.Fab_Map(fxId, 4, "Filter00 Type", 1, bands)

-- 5th for selecting band slope
reaper.Fab_Map(fxId, 5, "Filter00 Slope", 1, bands)

--[[
let's assign first encoder push (ReaFab cmd 9) to be band enabler.
control type 5 forces toggle behaviour
control type 6 overrides value range (here between 1.0 and 2.0)
control type 7, used here, is both control types 5 and 6
so, first encoder push will switch between ReEQ Filter Mode between values 1.0
and 2.0
note: like stepped parameters, ReaFab can directly identify native
toggles/switches but here we want to control stepped Filter Mode parameter as if
it were a on/off toggle.  
]] --
reaper.Fab_Map(fxId, 9, "Filter00 Mode", 7, bands, nil, nil, 1.0, 2.0)

-- second encoder push for inverting gain
-- control type 4 inverts parameter value
reaper.Fab_Map(fxId, 10, "Filter00 Gain", 4, bands)

--[[
assign first and second buttons (cmd 17 & 18) to be band selectors
control type 2 is used
prefixing paramId with "-" is reverse direction
with control type 2 paramId itself is only a label
]] --
reaper.Fab_Map(fxId, 17, "-band", 2, bands)
reaper.Fab_Map(fxId, 18, "band", 2, bands)

-- let buttons 3,4,5, and 6 be filter type and slope selectors
-- ReaFab commands 19,20,21 and 22
reaper.Fab_Map(fxId, 19, "-Filter00 Type", 1, bands)
reaper.Fab_Map(fxId, 20, "Filter00 Type", 1, bands)
reaper.Fab_Map(fxId, 21, "-Filter00 Slope", 1, bands)
reaper.Fab_Map(fxId, 22, "Filter00 Slope", 1, bands)

-- Phew! Yikes!!! *sigh*
-- now we've succesfully re-created the built-in mapping for ReEQ

-- but here it is with less explaining

fxId = "ReEQ"
reaper.Fab_Clear(fxId)
bands = 5
step = 0.4
reaper.Fab_Map(fxId, 1, "Filter00 Frequency", 8, bands, step)
reaper.Fab_Map(fxId, 2, "Filter00 Gain", 8, bands, step)
reaper.Fab_Map(fxId, 3, "Filter00 Q", 8, bands, step)
reaper.Fab_Map(fxId, 4, "Filter00 Type", 1, bands)
reaper.Fab_Map(fxId, 5, "Filter00 Slope", 1, bands)
reaper.Fab_Map(fxId, 9, "Filter00 Mode", 7, bands, nil, nil, 1.0, 2.0)
reaper.Fab_Map(fxId, 10, "Filter00 Gain", 4, bands)
reaper.Fab_Map(fxId, 17, "-band", 2, bands)
reaper.Fab_Map(fxId, 18, "band", 2, bands)
reaper.Fab_Map(fxId, 19, "-Filter00 Type", 1, bands)
reaper.Fab_Map(fxId, 20, "Filter00 Type", 1, bands)
reaper.Fab_Map(fxId, 21, "-Filter00 Slope", 1, bands)
reaper.Fab_Map(fxId, 22, "Filter00 Slope", 1, bands)

--[[ 
some plugins are easier to map, like ReaLimit

this type of mapping can also be handled with REAPER native mapping, unless you
wish experiment with ReaFab control types and steps etc.
 ]]

fxId = "ReaLimit"
reaper.Fab_Clear(fxId)
reaper.Fab_Map(fxId, 1, "Threshold", 1)
reaper.Fab_Map(fxId, 2, "Ceiling", 1)
reaper.Fab_Map(fxId, 3, "Lookahead", 1)

--[[ 
in fact, the most tedious part is just figuring out parameter names and types,
and how they respond.

some plugins will report some continous appearing parameters as stepped, like
ReEQ for frequency, gain and Q. or vice versa, toggles might appear as
continuous sliders etc. to some extend, this can be handled with control type
setting.

using FX parameter list, Track FX non-GUI view and TrackFX_Getters can be helpful.

see reascript API documentation for more information e.g. Fab_Get can be used to
build info display etc.
 ]]
