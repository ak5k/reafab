#include <cmath>
#include <map>
#include <string>
#include <vector>

#ifdef NDEBUG
#define REAPERAPI_MINIMAL
#define REAPERAPI_WANT_GetSelectedTrack2
#define REAPERAPI_WANT_TrackFX_GetCount
#define REAPERAPI_WANT_TrackFX_GetFXGUID
#define REAPERAPI_WANT_TrackFX_GetFXName
#define REAPERAPI_WANT_TrackFX_GetFormattedParamValue
#define REAPERAPI_WANT_TrackFX_GetNumParams
#define REAPERAPI_WANT_TrackFX_GetOpen
#define REAPERAPI_WANT_TrackFX_GetParam
#define REAPERAPI_WANT_TrackFX_GetParamName
#define REAPERAPI_WANT_TrackFX_GetParameterStepSizes
#define REAPERAPI_WANT_TrackFX_Show
#define REAPERAPI_WANT_TrackFX_SetParam
#define REAPERAPI_WANT_plugin_register
#endif
#define REAPERAPI_IMPLEMENT
#include <reaper_plugin_functions.h>

std::map<int, int> actions;
std::map<GUID*, int> previousBand;
std::map<int, std::string> controlMap;
std::vector<int> FXIndices;

char bufOut[BUFSIZ] {0};
double step {0};
size_t currentFXindex {0};
MediaTrack* prevTrack {0};
bool hidden {false};

std::vector<std::string> FXNames {
    "Pro-Q 3",
    "Pro-C 2",
    "Pro-G",
    "Pro-L 2",
    "Pro-MB",
    "Pro-DS",
    "Saturn 2",
    "ReaComp",
    "ReaGate",
    "ReaXcomp",
    "ReaLimit",
    "ReEQ"};

void SetTargetFXParam(MediaTrack* track, int index, int command, int val)
{
    // char bufOut[BUFSIZ] {0};
    double paramValue {0};
    for (int i = 0; i < TrackFX_GetNumParams(track, index); i++) {
        TrackFX_GetParamName(track, index, i, bufOut, BUFSIZ);
        std::string paramName(bufOut);
        if (paramName.compare(controlMap[command]) == 0) {
            double minvalOut, maxvalOut;
            double oldValue =
                TrackFX_GetParam(track, index, i, &minvalOut, &maxvalOut);
            double stepOut, smallStepOut, largeStepOut;
            bool isToggleOut {false};
            bool isStepped = TrackFX_GetParameterStepSizes(
                track,
                index,
                i,
                &stepOut,
                &smallStepOut,
                &largeStepOut,
                &isToggleOut);
            if (val < 64) {
                paramValue = val * step;
            }
            if (val > 64) {
                val = val - 128;
                paramValue = val * step;
            }
            if (val < -1 || val > 1) {
                paramValue = paramValue * 3;
            }

            TrackFX_GetFXName(track, index, bufOut, BUFSIZ);
            std::string FXName(bufOut);

            // ReaXcomp buttons
            if (command > 18 && command < 23 &&
                FXName.find("ReaXcomp") != std::string::npos) {
                isToggleOut = true;
            }

            // ReEQ scaling
            if (FXName.find("ReEQ") != std::string::npos && command > 0 &&
                command < 4) {
                stepOut = stepOut * 50;
            }

            if (isStepped) {
                paramValue = val * stepOut;
            }

            if (isToggleOut) {
                paramValue = 0.0;
                oldValue = abs(oldValue - 1.0);
            }

            //  Pro-Q 3 / ReEQ invert gain
            if (command == 10 && (FXName.find("Pro-Q 3") != std::string::npos ||
                                  FXName.find("ReEQ") != std::string::npos)) {
                oldValue = TrackFX_GetParam(track, index, i, 0, 0);
                paramValue = oldValue;
                if (FXName.find("ReEQ") != std::string::npos) {
                    paramValue = paramValue * -1;
                }
                else {
                    paramValue -= 0.5;
                    paramValue = paramValue * -1;
                    paramValue += 0.5;
                }
                oldValue = 0.0;
            }

            // ReEQ bypass
            if (FXName.find("ReEQ") != std::string::npos && command == 9) {
                if (oldValue == 2.0) {
                    paramValue = 1.0;
                }
                else {
                    paramValue = 2.0;
                }
                oldValue = 0.0;
            }

            double newValue = oldValue + paramValue;
            if (newValue < minvalOut) {
                newValue = minvalOut;
            }
            if (newValue > maxvalOut) {
                newValue = maxvalOut;
            }

            TrackFX_SetParam(track, index, i, newValue);
        }
    }
    return;
}

void SetReaLimit(int command, int val, int index, MediaTrack* track)
{
    controlMap[1] = std::string("Threshold");
    controlMap[2] = std::string("Ceiling");
    controlMap[3] = std::string("Release");
    SetTargetFXParam(track, index, command, val);
    return;
}

void SetReaXcomp(int command, int val, int index, MediaTrack* track)
{
    GUID* fx_guid = TrackFX_GetFXGUID(track, index);
    int band = 1;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }
    // select previous band
    if (command == 17) {
        band = band - 1;
        if (band < 1) {
            band = 1;
        }
    }
    // select next band
    if (command == 18) {
        band = band + 1;
        // if (band > 6) {
        //     band = 6;
        // }
    }

    previousBand[fx_guid] = band;
    std::string targetParam = std::to_string(band) + std::string("-");

    controlMap[1] = targetParam + std::string("Thresh");
    controlMap[2] = targetParam + std::string("Ratio");
    controlMap[3] = targetParam + std::string("Attack");
    controlMap[4] = targetParam + std::string("Release");
    controlMap[5] = targetParam + std::string("Knee");
    controlMap[6] = targetParam + std::string("RMS");
    controlMap[7] = targetParam + std::string("Freq");
    controlMap[8] = targetParam + std::string("Gain");
    controlMap[19] = targetParam + std::string("MakeUp");
    controlMap[20] = targetParam + std::string("AutoRel");
    controlMap[21] = targetParam + std::string("FeedBk");
    controlMap[22] = targetParam + std::string("Active");

    SetTargetFXParam(track, index, command, val);
    return;
}

void SetProMB(int command, int val, int index, MediaTrack* track)
{
    GUID* fx_guid = TrackFX_GetFXGUID(track, index);
    int band = 1;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }
    // select previous band
    if (command == 17) {
        band = band - 1;
        if (band < 1) {
            band = 1;
        }
    }
    // select next band
    if (command == 18) {
        band = band + 1;
        if (band > 6) {
            band = 6;
        }
    }

    previousBand[fx_guid] = band;
    std::string targetParam("Band ");
    targetParam = targetParam + std::to_string(band) + std::string(" ");

    controlMap[1] = targetParam + std::string("Threshold");
    controlMap[2] = targetParam + std::string("Ratio");
    controlMap[3] = targetParam + std::string("Attack");
    controlMap[4] = targetParam + std::string("Release");
    controlMap[5] = targetParam + std::string("Knee");
    controlMap[6] = targetParam + std::string("Range");
    controlMap[7] = targetParam + std::string("High Crossover");
    controlMap[8] = targetParam + std::string("Level");
    controlMap[19] = targetParam + std::string("Dynamics Mode");

    SetTargetFXParam(track, index, command, val);
    return;
}

void SetSaturn2(int command, int val, int index, MediaTrack* track)
{
    GUID* fx_guid = TrackFX_GetFXGUID(track, index);
    int band = 1;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }
    // select previous band
    if (command == 17) {
        band = band - 1;
        if (band < 1) {
            band = 1;
        }
    }
    // select next band
    if (command == 18) {
        band = band + 1;
        if (band > 6) {
            band = 6;
        }
    }

    previousBand[fx_guid] = band;
    std::string targetParam("Band ");
    targetParam = targetParam + std::to_string(band) + std::string(" ");

    controlMap[1] = targetParam + std::string("Drive");
    controlMap[2] = targetParam + std::string("Dynamics");
    controlMap[3] = targetParam + std::string("Crossover Frequency");
    controlMap[7] = targetParam + std::string("Level");
    controlMap[8] = targetParam + std::string("Mix");
    controlMap[18] = targetParam + std::string("Band Processing");
    controlMap[19] = targetParam + std::string("Style");
    controlMap[20] = targetParam + std::string("Style");
    controlMap[23] = std::string("Num Active Bands");
    controlMap[24] = std::string("Num Active Bands");

    if (command == 19 || command == 20) {
        val = 1;
        if (command == 19) {
            val = -1;
        }
    }
    if (command == 23 || command == 24) {
        val = 1;
        if (command == 23) {
            val = -1;
        }
    }

    SetTargetFXParam(track, index, command, val);
    return;
}

void SetProDS(int command, int val, int index, MediaTrack* track)
{
    controlMap[1] = std::string("Threshold");
    controlMap[2] = std::string("Range");
    controlMap[3] = std::string("High-Pass Frequency");
    controlMap[4] = std::string("Low-Pass Frequency");
    controlMap[5] = std::string("Stereo Link");
    controlMap[6] = std::string("Lookahead");
    controlMap[7] = std::string("Lookahead");
    controlMap[17] = std::string("Mode");
    controlMap[18] = std::string("Band Processing");
    controlMap[19] = std::string("Stereo Link Mode");
    controlMap[20] = std::string("Lookahead Enabled");

    SetTargetFXParam(track, index, command, val);
    return;
}

void SetProL2(int command, int val, int index, MediaTrack* track)
{
    controlMap[1] = std::string("Gain");
    controlMap[2] = std::string("Output Level");
    controlMap[3] = std::string("Attack");
    controlMap[4] = std::string("Release");
    controlMap[5] = std::string("Channel Link Transients");
    controlMap[6] = std::string("Channel Link Release");
    controlMap[7] = std::string("Lookahead");
    controlMap[17] = std::string("Style");
    controlMap[18] = std::string("Style");
    controlMap[19] = std::string("Unity Gain");

    if (command == 17 || command == 18) {
        val = 1;
        if (command == 17) {
            val = -1;
        }
    }
    if (command == 17 || command == 18) {
        val = 1;
        if (command == 17) {
            val = -1;
        }
    }
    SetTargetFXParam(track, index, command, val);
    return;
}

void SetProC2(int command, int val, int index, MediaTrack* track)
{
    controlMap[1] = std::string("Threshold");
    controlMap[2] = std::string("Ratio");
    controlMap[3] = std::string("Attack");
    controlMap[4] = std::string("Release");
    controlMap[5] = std::string("Hold");
    controlMap[6] = std::string("Knee");
    controlMap[7] = std::string("Dry Gain");
    controlMap[8] = std::string("Wet Gain");
    controlMap[17] = std::string("Style");
    controlMap[18] = std::string("Style");
    controlMap[19] = std::string("Auto Gain");

    if (command == 17 || command == 18) {
        val = 1;
        if (command == 17) {
            val = -1;
        }
    }
    SetTargetFXParam(track, index, command, val);
    return;
}

void SetReaComp(int command, int val, int index, MediaTrack* track)
{
    controlMap[1] = std::string("Thresh");
    controlMap[2] = std::string("Ratio");
    controlMap[3] = std::string("Attack");
    controlMap[4] = std::string("Release");
    controlMap[5] = std::string("RMS size");
    controlMap[6] = std::string("Knee");
    controlMap[7] = std::string("Dry");
    controlMap[8] = std::string("Wet");
    controlMap[17] = std::string("AutoMkUp");
    controlMap[18] = std::string("AutoRel");

    SetTargetFXParam(track, index, command, val);
    return;
}

void SetReaGate(int command, int val, int index, MediaTrack* track)
{
    controlMap[1] = std::string("Thresh");
    controlMap[2] = std::string("Hold");
    controlMap[3] = std::string("Attack");
    controlMap[4] = std::string("Release");
    controlMap[5] = std::string("RMS size");
    controlMap[6] = std::string("Hystrsis");
    controlMap[7] = std::string("Dry");
    controlMap[8] = std::string("Wet");

    SetTargetFXParam(track, index, command, val);
    return;
}

void SetProG(int command, int val, int index, MediaTrack* track)
{
    controlMap[1] = std::string("Threshold");
    controlMap[2] = std::string("Ratio");
    controlMap[3] = std::string("Attack");
    controlMap[4] = std::string("Release");
    controlMap[5] = std::string("Hold");
    controlMap[6] = std::string("Knee");
    controlMap[7] = std::string("Range");
    controlMap[17] = std::string("Style");
    controlMap[18] = std::string("Style");

    char bufOut[BUFSIZ] {0};
    TrackFX_GetFormattedParamValue(track, index, 5, bufOut, BUFSIZ);
    std::string paramName(bufOut);
    if (paramName.compare("Upward") == 0 && (command == 1 || command == 2)) {
        controlMap[1] = std::string("Threshold (Upward)");
        controlMap[2] = std::string("Ratio (Upward)");
    }

    SetTargetFXParam(track, index, command, val);
    return;
}

void SetProQ3(int command, int val, int index, MediaTrack* track)
{
    // char bufOut[64] {0};
    GUID* fx_guid = TrackFX_GetFXGUID(track, index);
    int band = 1;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }
    // select previous band
    if (command == 17) {
        band = band - 1;
        if (band < 1) {
            band = 1;
        }
    }
    // select next band
    if (command == 18) {
        band = band + 1;
        if (band > 24) {
            band = 24;
        }
    }

    previousBand[fx_guid] = band;
    std::string targetParam("Band ");
    targetParam = targetParam + std::to_string(band) + std::string(" ");

    controlMap[1] = targetParam + std::string("Frequency");
    controlMap[2] = targetParam + std::string("Gain");
    controlMap[3] = targetParam + std::string("Q");
    controlMap[4] = targetParam + std::string("Dynamic Range");
    controlMap[5] = targetParam + std::string("Threshold");
    // controlMap[17] = targetParam + std::string("Used");
    // controlMap[18] = targetParam + std::string("Used");
    controlMap[19] = targetParam + std::string("Shape");
    controlMap[20] = targetParam + std::string("Shape");
    controlMap[21] = targetParam + std::string("Slope");
    controlMap[22] = targetParam + std::string("Slope");

    controlMap[9] = targetParam + std::string("Used");
    controlMap[10] = targetParam + std::string("Gain");

    if (command == 19 || command == 20) {
        val = 1;
        if (command == 19) {
            val = -1;
        }
    }
    if (command == 21 || command == 22) {
        val = 1;
        if (command == 21) {
            val = -1;
        }
    }

    SetTargetFXParam(track, index, command, val);
    return;
}

void SetReEQ(int command, int val, int index, MediaTrack* track)
{
    GUID* fx_guid = TrackFX_GetFXGUID(track, index);
    int band = 1;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }
    // select previous band
    if (command == 17) {
        band = band - 1;
        if (band < 1) {
            band = 1;
        }
    }
    // select next band
    if (command == 18) {
        band = band + 1;
        if (band > 5) {
            band = 5;
        }
    }

    previousBand[fx_guid] = band;
    std::string targetParam("Filter");
    targetParam = targetParam + std::to_string(band) + std::string(" ");

    controlMap[1] = targetParam + std::string("Frequency");
    controlMap[2] = targetParam + std::string("Gain");
    controlMap[3] = targetParam + std::string("Q");
    controlMap[19] = targetParam + std::string("Type");
    controlMap[20] = targetParam + std::string("Type");
    controlMap[21] = targetParam + std::string("Slope");
    controlMap[22] = targetParam + std::string("Slope");

    controlMap[9] = targetParam + std::string("Mode");
    controlMap[10] = targetParam + std::string("Gain");

    if (command == 19 || command == 20) {
        val = 1;
        if (command == 19) {
            val = -1;
        }
    }
    if (command == 21 || command == 22) {
        val = 1;
        if (command == 21) {
            val = -1;
        }
    }

    SetTargetFXParam(track, index, command, val);
    return;
}

void SetFX(int command, int val, int index)
{
    auto track = GetSelectedTrack2(0, 0, true);

    TrackFX_GetFXName(track, index, bufOut, BUFSIZ);
    std::string fx_name(bufOut);
    if (fx_name.find(std::string("Pro-Q 3")) != std::string::npos) {
        SetProQ3(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-G")) != std::string::npos) {
        SetProG(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-C 2")) != std::string::npos) {
        SetProC2(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-L 2")) != std::string::npos) {
        SetProL2(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-DS")) != std::string::npos) {
        SetProDS(command, val, index, track);
    }
    if (fx_name.find(std::string("Saturn 2")) != std::string::npos) {
        SetSaturn2(command, val, index, track);
    }
    if (fx_name.find(std::string("Pro-MB")) != std::string::npos) {
        SetProMB(command, val, index, track);
    }
    if (fx_name.find(std::string("ReaComp")) != std::string::npos) {
        SetReaComp(command, val, index, track);
    }
    if (fx_name.find(std::string("ReaGate")) != std::string::npos) {
        SetReaGate(command, val, index, track);
    }
    if (fx_name.find(std::string("ReaXcomp")) != std::string::npos) {
        SetReaXcomp(command, val, index, track);
    }
    if (fx_name.find(std::string("ReaLimit")) != std::string::npos) {
        SetReaLimit(command, val, index, track);
    }
    if (fx_name.find(std::string("ReEQ")) != std::string::npos) {
        SetReEQ(command, val, index, track);
    }
    return;
}

void ProcessInput()
{
    step = 0.0013123359531164;
    controlMap.clear();
    FXIndices.clear();

    auto track = GetSelectedTrack2(0, 0, true);
    auto fx_count = TrackFX_GetCount(track);

    // char bufOut[BUFSIZ] {0};

    // find fx indices for supported plugins based on names
    for (int i = 0; i < fx_count; i++) {
        TrackFX_GetFXName(track, i, bufOut, BUFSIZ);
        std::string fx_name(bufOut);
        for (auto item : FXNames) {
            if (fx_name.find(item) != std::string::npos) {
                FXIndices.push_back(i);
            }
        }
    }

    return;
}

void ShowTrackFXChain()
{
    if (TrackFX_GetOpen(
            GetSelectedTrack2(0, 0, true),
            FXIndices[currentFXindex])) {
        TrackFX_Show(
            GetSelectedTrack2(0, 0, true),
            FXIndices[currentFXindex],
            0);
        TrackFX_Show(
            GetSelectedTrack2(0, 0, true),
            FXIndices[currentFXindex],
            2);
        hidden = true;
    }
    else {
        TrackFX_Show(
            GetSelectedTrack2(0, 0, true),
            FXIndices[currentFXindex],
            1);
        hidden = false;
    }

    return;
}

void ProcessCommand(int command, int val)
{
    ProcessInput();
    if (command > 24) {
        (void)val;
        currentFXindex = command - 24 - 1;
        if (FXIndices.size() > currentFXindex) {
            // SetFX(command, val, FXIndices[currentFXindex]);
            ShowTrackFXChain();
        }
        return;
    }
    else {
        if (FXIndices.size() > currentFXindex) {
            if (GetSelectedTrack2(0, 0, true) != prevTrack) {
                prevTrack = GetSelectedTrack2(0, 0, true);
                if (!hidden) {
                    TrackFX_Show(
                        GetSelectedTrack2(0, 0, true),
                        FXIndices[currentFXindex],
                        1);
                }
            }
            SetFX(command, val, FXIndices[currentFXindex]);
        }
    }
    return;
}

bool OnAction(
    KbdSectionInfo* sec,
    int command,
    int val,
    int valhw,
    int relmode,
    HWND hwnd)
{
    // treat unused variables 'pedantically'
    (void)sec;
    (void)hwnd;

    if (!actions[command]) {
        return false;
    }
    ProcessCommand(actions[command], val);
    return true;
}

void RegisterReaFab()
{
    auto count = 8;
    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab Encoder "};
        commandStr = commandStr + std::to_string(i);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register("custom_action", &action);
        actions[commandId] = i + 1;
    }

    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab Encoder Push "};
        commandStr = commandStr + std::to_string(i + count);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register("custom_action", &action);
        actions[commandId] = i + count + 1;
    }

    count = 16;
    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab Button "};
        commandStr = commandStr + std::to_string(i + count);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register("custom_action", &action);
        actions[commandId] = i + count + 1;
    }

    plugin_register("hookcommand2", (void*)OnAction);

    return;
}

extern "C" {
REAPER_PLUGIN_DLL_EXPORT int ReaperPluginEntry(
    REAPER_PLUGIN_HINSTANCE hInstance,
    reaper_plugin_info_t* rec)
{
    (void)hInstance;
    if (!rec) {
        return 0;
    }
    else if (
        rec->caller_version != REAPER_PLUGIN_VERSION ||
        REAPERAPI_LoadAPI(rec->GetFunc)) {
        return 0;
    }
    RegisterReaFab();
    return 1;
}
}
