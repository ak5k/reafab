#include "reafab.hpp"

namespace reafab {

MediaTrack* prevTrack {0};
MediaTrack* track {0}; // current track
bool hidden {false};
char bufOut[BUFSIZ] {0};
int inputValue {0};
size_t currentFXindex {0};
std::map<GUID*, int> previousBand;
std::map<int, int> actions;
std::map<std::string, std::map<int, ControlTarget>> controlMap;
std::mutex m;
std::vector<int> fxIndices;

bool isValid()
{
    if (fxIndices.size() > currentFXindex) {
        return true;
    }
    return false;
}

void ExecuteCommand(int command)
{
    // variables
    int& fx = fxIndices[currentFXindex];
    TrackFX_GetFXName(track, fx, bufOut, BUFSIZ);
    std::string fx_name(bufOut);
    ControlTarget currentControlTarget;
    for (auto& [key, value] : controlMap) {
        if (fx_name.find(key) != std::string::npos) {
            currentControlTarget = value[command];
            break;
        }
    }

    double step = currentControlTarget.step;

    // failsafe test
    if (currentControlTarget.control == 0) {
        return;
    }

    // convinience variables
    double newValue {0};
    std::string paramName = currentControlTarget.paramId;
    int control = currentControlTarget.control;
    int bands = currentControlTarget.bands;
    int param {-1};

    // set and get optional previous band
    GUID* fx_guid = TrackFX_GetFXGUID(track, fx);
    int band = 1;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }

    // change band
    if (control == 2) {
        if (paramName.rfind("-", 0) == 0) {
            band--;
            if (band < 1) {
                band = 1;
            }
        }
        else {
            band++;
            if (band > bands) {
                band = bands;
            }
        }
        previousBand[fx_guid] = band;
        return;
    }

    // set band to parameter string
    if (bands > 0) {
        paramName.replace(paramName.find("00"), 2, std::to_string(band));
    }

    // store current band as previous band
    previousBand[fx_guid] = band;

    // flip sign
    if (control != 2) {
        if (paramName.rfind("-", 0) == 0) {
            inputValue = -1 * inputValue;
            paramName.erase(0, 1);
        }
    }

    // FabFilter Pro-G fix
    if (fx_name.find("Pro-G") != std::string::npos) {
        TrackFX_GetFormattedParamValue(track, fx, 5, bufOut, BUFSIZ);
        if (std::string(bufOut).compare("Upward") == 0 &&
            (paramName.compare("Threshold") == 0 ||
             paramName.compare("Ratio") == 0)) {
            paramName = paramName + std::string(" (Upward)");
        }
    }

    // find parameter index for parameter string
    for (int i = 0; i < TrackFX_GetNumParams(track, fx); i++) {
        TrackFX_GetParamName(track, fx, i, bufOut, BUFSIZ);
        if (paramName.compare(std::string(bufOut)) == 0) {
            param = i;
            break;
        }
    }

    // failsafe
    if (param == -1) {
        return;
    }

    // get parameter parameters
    double minvalOut {0}, maxvalOut {0}, midvalOut {0};
    double stepOut {0}, smallstepOut {0}, largestepOut {0};
    bool istoggleOut {false};
    double currentValue = TrackFX_GetParamEx(
        track,
        fx,
        param,
        &minvalOut,
        &maxvalOut,
        &midvalOut);
    TrackFX_GetParameterStepSizes(
        track,
        fx,
        param,
        &stepOut,
        &smallstepOut,
        &largestepOut,
        &istoggleOut);

    // force range
    if (control == 6 || control == 7) {
        minvalOut = currentControlTarget.minval;
        maxvalOut = currentControlTarget.maxval;
        if (control == 6) {
            control = 1;
        }
    }

    // force toggle
    if (control == 5 || control == 7) {
        istoggleOut = true;
        control = 1;
    }

    // force continous
    if (control == 8) {
        stepOut = 0.;
        control = 1;
    }

    // direct
    if (control == 1) {
        if (stepOut != 0.) {
            step = stepOut;
        }
        else {
            // step = step * currentControlTarget.step;
        }
        newValue = step * inputValue;
        if (abs(inputValue) > 1) {
            newValue = newValue * currentControlTarget.accel;
        }
        newValue = currentValue + newValue;
        if (istoggleOut) {
            if (currentValue == minvalOut) {
                newValue = maxvalOut;
            }
            else {
                newValue = minvalOut;
            }
        }
    }

    // cycle
    if (control == 3) {
        if (stepOut != 0.) {
            step = stepOut;
        }
        newValue = step * inputValue;
        newValue = currentValue + newValue;
    }

    // invert
    if (control == 4) {
        newValue = (currentValue - midvalOut) * -1 + midvalOut;
    }

    // keep values between min and max
    if (newValue > maxvalOut) {
        newValue = maxvalOut;
        if (control == 3) { // cycle overflow
            newValue = minvalOut;
        }
    }
    if (newValue < minvalOut) {
        newValue = minvalOut;
    }

    TrackFX_SetParam(track, fx, param, newValue);
    return;
}

void Initialize()
{
    track = GetSelectedTrack2(0, 0, true);
    if (!track) {
        return;
    }
    fxIndices.clear();

    // find fx indices for supported plugins based on names
    for (int i = 0; i < TrackFX_GetCount(track); i++) {
        TrackFX_GetFXName(track, i, bufOut, BUFSIZ);
        std::string fx_name(bufOut);
        for (const auto& j : controlMap) {
            if (fx_name.find(j.first) != std::string::npos) {
                fxIndices.push_back(i);
            }
        }
    }

    return;
}

void ShowTrackFXChain()
{
    if (TrackFX_GetOpen(track, fxIndices[currentFXindex])) {
        TrackFX_Show(track, fxIndices[currentFXindex], 0);
        TrackFX_Show(track, fxIndices[currentFXindex], 2);
        hidden = true;
    }
    else {
        TrackFX_Show(track, fxIndices[currentFXindex], 1);
        hidden = false;
    }
    return;
}

void ProcessCommand(int command, int val, int relmode)
{
    std::scoped_lock<std::mutex> lock {m};
    if (command < 33) {
        inputValue = val;
        if (relmode == 1) {
            if (val > 64) {
                inputValue = val - 128;
            }
        }
        if (relmode == 2) {
            inputValue = val - 64;
        }
        if (relmode == 3) {
            if (val > 64) {
                inputValue = -1 * (val - 64);
            }
        }
        if (command > 8) {
            inputValue = 1;
        }

        Initialize();
        if (command > 24) {
            currentFXindex = command - 24 - 1;
            if (isValid()) {
                ShowTrackFXChain();
            }
            return;
        }
        else {
            if (isValid()) {
                if (track != prevTrack) {
                    prevTrack = track;
                    if (!hidden) {
                        TrackFX_Show(track, fxIndices[currentFXindex], 1);
                    }
                }
                ExecuteCommand(command);
            }
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
    (void)hwnd;
    (void)sec;
    (void)valhw;

    if (!actions[command]) {
        return false;
    }
    if (actions[command] < 9 && relmode == 0) {
        ShowConsoleMsg("Not MIDI Relative");
        ShowConsoleMsg("");
        return true;
    }
    ProcessCommand(actions[command], val, relmode);
    return true;
}
} // namespace reafab
