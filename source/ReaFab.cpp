#include <cmath>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <reaper_imgui_functions.h>
#include <reaper_plugin_functions.h>
#include <reascript_vararg.hpp>

namespace reafab {

MediaTrack* prevTrack {0};
MediaTrack* track {0}; // current track
bool hidden {false};
char bufOut[BUFSIZ] {0};
double step {0};
int inputValue {0};
size_t currentFXindex {0};
static ImGui_Context* ctx;
static int imguiActionId;
std::map<GUID*, int> previousBand;
static std::map<int, int> actions;
std::mutex m;
std::vector<int> fxIndices;

struct ControlTarget {
    std::string paramId {0};
    int ctrl {0};
    int bands {0};
    double rate {1.};
    double accel {3.};
    double minval {0};
    double maxval {0};
};

std::map<std::string, std::map<int, ControlTarget>> controlMap;

void RegisterPlugins()
{
    std::map<int, ControlTarget> tempMap;
    std::string id {"Pro-Q 3"};
    int bands = 24;
    tempMap[1] = ControlTarget {"Band 00 Frequency", 1, bands};
    tempMap[2] = ControlTarget {"Band 00 Gain", 1, bands};
    tempMap[3] = ControlTarget {"Band 00 Q", 1, bands};
    tempMap[4] = ControlTarget {"Band 00 Dynamic Range", 1, bands};
    tempMap[5] = ControlTarget {"Band 00 Threshold", 1, bands};
    tempMap[9] = ControlTarget {"Band 00 Used", 1, bands};
    tempMap[10] = ControlTarget {"Band 00 Gain", 4, bands};
    tempMap[17] = ControlTarget {"-band", 2, bands};
    tempMap[18] = ControlTarget {"band", 2, bands};
    tempMap[19] = ControlTarget {"-Band 00 Shape", 1, bands};
    tempMap[20] = ControlTarget {"Band 00 Shape", 1, bands};
    tempMap[21] = ControlTarget {"-Band 00 Slope", 1, bands};
    tempMap[22] = ControlTarget {"Band 00 Slope", 1, bands};
    controlMap[id] = tempMap;

    tempMap.clear();
    id = "Pro-G";
    tempMap[1] = ControlTarget {"Threshold", 1};
    tempMap[2] = ControlTarget {"Ratio", 1};
    tempMap[3] = ControlTarget {"Attack", 1};
    tempMap[4] = ControlTarget {"Release", 1};
    tempMap[5] = ControlTarget {"Hold", 1};
    tempMap[6] = ControlTarget {"Knee", 1};
    tempMap[7] = ControlTarget {"Range", 1};
    tempMap[17] = ControlTarget {"-Style", 1};
    tempMap[18] = ControlTarget {"Style", 1};
    controlMap[id] = tempMap;

    tempMap.clear();
    id = "Pro-C 2";
    tempMap[1] = ControlTarget {"Threshold", 1};
    tempMap[2] = ControlTarget {"Ratio", 1};
    tempMap[3] = ControlTarget {"Attack", 1};
    tempMap[4] = ControlTarget {"Release", 1};
    tempMap[5] = ControlTarget {"Hold", 1};
    tempMap[6] = ControlTarget {"Knee", 1};
    tempMap[7] = ControlTarget {"Dry Gain", 1};
    tempMap[8] = ControlTarget {"Wet Gain", 1};
    tempMap[17] = ControlTarget {"-Style", 1};
    tempMap[18] = ControlTarget {"Style", 1};
    tempMap[19] = ControlTarget {"Auto Gain", 1};
    tempMap[20] = ControlTarget {"Auto Release", 1};
    controlMap[id] = tempMap;

    tempMap.clear();
    id = "Pro-L 2";
    tempMap[1] = ControlTarget {"Gain", 1};
    tempMap[2] = ControlTarget {"Output Level", 1};
    tempMap[3] = ControlTarget {"Attack", 1};
    tempMap[4] = ControlTarget {"Release", 1};
    tempMap[5] = ControlTarget {"Channel Link Transients", 1};
    tempMap[6] = ControlTarget {"Channel Link Release", 1};
    tempMap[7] = ControlTarget {"Lookahead", 1};
    tempMap[17] = ControlTarget {"-Style", 1};
    tempMap[18] = ControlTarget {"Style", 1};
    tempMap[19] = ControlTarget {"Unity Gain", 1};
    controlMap[id] = tempMap;

    tempMap.clear();
    id = "Pro-DS";
    tempMap[1] = ControlTarget {"Threshold", 1};
    tempMap[2] = ControlTarget {"Range", 1};
    tempMap[3] = ControlTarget {"High-Pass Frequency", 1};
    tempMap[4] = ControlTarget {"Low-Pass Frequency", 1};
    tempMap[5] = ControlTarget {"Stereo Link", 1};
    tempMap[7] = ControlTarget {"Lookahead", 1};
    tempMap[17] = ControlTarget {"Mode", 1};
    tempMap[18] = ControlTarget {"Band Processing", 1};
    tempMap[19] = ControlTarget {"Stereo Link Mode", 1};
    tempMap[20] = ControlTarget {"Lookahead Enabled", 1};
    controlMap[id] = tempMap;

    tempMap.clear();
    bands = 6;
    id = "Pro-MB";
    tempMap[1] = ControlTarget {"Band 00 Threshold", 1, bands};
    tempMap[2] = ControlTarget {"Band 00 Ratio", 1, bands};
    tempMap[3] = ControlTarget {"Band 00 Attack", 1, bands};
    tempMap[4] = ControlTarget {"Band 00 Release", 1, bands};
    tempMap[5] = ControlTarget {"Band 00 Knee", 1, bands};
    tempMap[6] = ControlTarget {"Band 00 Range", 1, bands};
    tempMap[7] = ControlTarget {"Band 00 High Crossover", 1, bands};
    tempMap[8] = ControlTarget {"Band 00 Level", 1, bands};
    tempMap[9] = ControlTarget {"Band 00 State", 7, bands, 1.0, 3.0, 0.5, 1.0};
    tempMap[17] = ControlTarget {"-band", 2, bands};
    tempMap[18] = ControlTarget {"band", 2, bands};
    tempMap[19] = ControlTarget {"Band 00 Dynamics Mode", 1, bands};

    controlMap[id] = tempMap;
    return;
}

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

    // failsafe test
    if (currentControlTarget.ctrl == 0) {
        return;
    }

    // convinience variables
    double newValue {0};
    std::string paramName = currentControlTarget.paramId;
    int ctrl = currentControlTarget.ctrl;
    int bands = currentControlTarget.bands;
    int param {-1};

    // set and get optional previous band
    GUID* fx_guid = TrackFX_GetFXGUID(track, fx);
    int band = 1;
    if (previousBand[fx_guid]) {
        band = previousBand[fx_guid];
    }

    // change band
    if (ctrl == 2) {
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
    if (ctrl == 1) {
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
    if (ctrl == 6 || ctrl == 7) {
        minvalOut = currentControlTarget.minval;
        maxvalOut = currentControlTarget.maxval;
        if (ctrl == 6) {
            ctrl = 1;
        }
    }

    // force toggle
    if (ctrl == 5 || ctrl == 7) {
        istoggleOut = true;
        ctrl = 1;
    }

    // direct
    if (ctrl == 1) {
        if (inputValue > 64) {
            inputValue = inputValue - 128;
        }
        if (stepOut != 0.) {
            step = stepOut;
        }
        newValue = step * inputValue;
        newValue = newValue * currentControlTarget.rate;
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
    if (ctrl == 3) {
        if (stepOut != 0.) {
            step = stepOut;
        }
        newValue = step * inputValue;
        newValue = currentValue + newValue;
    }

    // invert
    if (ctrl == 4) {
        newValue = (currentValue - midvalOut) * -1 + midvalOut;
    }

    // keep values between min and max
    if (newValue > maxvalOut) {
        newValue = maxvalOut;
        if (ctrl == 3) { // cycle overflow
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
    step = 0.0013123359531164;
    fxIndices.clear();

    // find fx indices for supported plugins based on names
    for (int i = 0; i < TrackFX_GetCount(track); i++) {
        TrackFX_GetFXName(track, i, bufOut, BUFSIZ);
        std::string fx_name(bufOut);
        for (const auto& [key, value] : controlMap) {
            (void)value;
            if (fx_name.find(key) != std::string::npos) {
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

void ProcessCommand(int command, int val)
{
    std::scoped_lock<std::mutex> lock {m};
    if (command < 33) {
        inputValue = val;
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

static bool OnAction(
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
    if (actions[command] < 9 && relmode != 1) {
        ShowConsoleMsg("Not MIDI CC Relative1");
        ShowConsoleMsg("");
        return true;
    }
    ProcessCommand(actions[command], val);
    return true;
}

static bool Set(int command, int val)
{
    if (command < 1 || command > 32 || val < 1 || val > 127) {
        return false;
    }
    ProcessCommand(command, val);
    return true;
}

const char* defstring_Set =
    "bool\0int,int\0command,val\0"
    "Runs ReaFab actions/commands. First "
    "parameter (command) is ReaFab command number, e.g. 3 for 3rd encoder "
    "rotation. Second parameter (val) is MIDI CC Relative1 value. Value 1 is "
    "increment of 1, 127 is decrement of 1. 2 is inc 2, 126 is dec 2 and so "
    "on. For button press (commands 9-32) a value of 127 is recommended.";

static bool Get(int command, int* fxOut, int* paramOut)
{
    std::scoped_lock<std::mutex> lock {m};
    Initialize();
    if (!isValid() || command < 1 || command > 24) {
        return false;
    }
    int& fx = fxIndices[currentFXindex];
    int param {-1};
    TrackFX_GetFXName(track, fx, bufOut, BUFSIZ);
    std::string fx_name(bufOut);
    ControlTarget currentControlTarget;
    for (auto& [key, value] : controlMap) {
        if (fx_name.find(key) != std::string::npos) {
            currentControlTarget = value[command];
            break;
        }
    }
    std::string& paramName = currentControlTarget.paramId;
    if (currentControlTarget.bands > 0) {
        GUID* fx_guid = TrackFX_GetFXGUID(track, fx);
        int band = 1;
        if (previousBand[fx_guid]) {
            band = previousBand[fx_guid];
        }
        paramName.replace(paramName.find("00"), 2, std::to_string(band));
    }
    for (int i = 0; i < TrackFX_GetNumParams(track, fx); i++) {
        TrackFX_GetParamName(track, fx, i, bufOut, BUFSIZ);
        if (paramName.compare(std::string(bufOut)) == 0) {
            param = i;
            break;
        }
    }
    if (param != -1 || currentControlTarget.ctrl == 2) {
        *fxOut = fx;
        *paramOut = param;
        return true;
    }
    return false;
}

const char* defstring_Get =
    "bool\0int,int*,int*\0command,fxOut,paramOut\0"
    "Returns target FX and parameter index for given ReaFab command on "
    "currently selected track and ReaFab FX index. Valid command range 1 ... "
    "24. Returns false if no such command mapping is found. Returns param "
    "index -1 for ReaFab internal band change command.";

static bool Clear(const char* idStringInOptional)
{
    std::string s {idStringInOptional};
    if (s.empty()) {
        controlMap.clear();
        return true;
    }

    for (auto& [key, value] : controlMap) {
        if (s.compare(key) == 0) {
            controlMap.erase(key);
            return true;
        }
    }
    return false;
}

const char* defstring_Clear =
    "bool\0const char*\0idStringInOptional\0"
    "Clears ReaFab control map, optionally based on matching idString. Returns "
    "true on success.";

static bool Map(
    const char* fxId,
    int command,
    const char* paramId,
    int ctrl,
    const int* bandsInOptional,
    const double* rateInOptional,
    const double* accelInOptional,
    const double* minvalInOptional,
    const double* maxvalInOptional)
{
    if (command < 1 || command > 24 || !fxId || !paramId || ctrl < 1 ||
        ctrl > 7) {
        return false;
    }
    std::string fxIdString {fxId};
    ControlTarget currentControlTarget;
    currentControlTarget.paramId = std::string(paramId);
    currentControlTarget.ctrl = ctrl;
    if (bandsInOptional) {
        if (!(currentControlTarget.paramId.find("00") != std::string::npos)) {
            return false;
        }
        currentControlTarget.bands = *bandsInOptional;
    }
    if (rateInOptional) {
        currentControlTarget.rate = *rateInOptional;
    }
    if (accelInOptional) {
        currentControlTarget.accel = *accelInOptional;
    }
    if (minvalInOptional) {
        currentControlTarget.minval = *minvalInOptional;
    }
    if (maxvalInOptional) {
        currentControlTarget.maxval = *minvalInOptional;
    }
    controlMap[fxIdString][command] = currentControlTarget;
    return true;
}

const char* defstring_Map =
    "bool\0"
    "const char*,int,const "
    "char*,int,int*,double*,double*,double*,double*\0"
    "fxId,command,paramId,ctrl,bandsInOptional,rateInOptional,accelInOptional,"
    "minvalInOptional,maxvalInOptional\0"
    "Creates control mapping for ReaFab command. "
    "fxId e.g. \"ReaComp\". "
    "command 1-8 for encoders, 9-24 for buttons. "
    "paramId e.g. \"Ratio\". "
    "ctrl 1 = direct, 2 = band selector, 3 = cycle, 4 = invert, 5 = force "
    "toggle, 6 = force range, 7 = 5 & 6. "
    "bands define, if target fx has multiple identical target bands. In this "
    "case, paramId must include 00 placeholder, e.g. \"Band 00 Gain\". "
    "rate overrides built-in default control rate. "
    "accel overrides built-int default control acceleration rate. "
    "minval & maxval override target value range. "
    "Prefixing paramId with \"-\" reverses direction; useful for creating "
    "separate next/previous mappings for bands or list type value navigation. ";

static void ImGuiFrame()
{
    std::scoped_lock<std::mutex> lock {m};
    Initialize();
    if (!isValid()) {
        return;
    }
    int& fx = fxIndices[currentFXindex];
    TrackFX_GetFXName(track, fx, bufOut, BUFSIZ);
    std::string fx_name(bufOut);
    std::map<int, ControlTarget> currentControlMap;
    for (auto& [key, value] : controlMap) {
        if (fx_name.find(key) != std::string::npos) {
            currentControlMap = value;
            break;
        }
    }
    for (auto& [key, value] : currentControlMap) {
        (void)key;
        if (value.bands > 0) {
            GUID* fx_guid = TrackFX_GetFXGUID(track, fx);
            int band = 1;
            if (previousBand[fx_guid]) {
                band = previousBand[fx_guid];
            }
            if (value.paramId.find("00") != std::string::npos) {
                value.paramId.replace(
                    value.paramId.find("00"),
                    2,
                    std::to_string(band));
            }
        }
    }
    if (ImGui_BeginTable(ctx, "table1", 9)) {
        for (int row = 0; row < 4; row++) {
            ImGui_TableNextRow(ctx);
            for (int column = 0; column < 9; column++) {
                ImGui_TableSetColumnIndex(ctx, column);
                std::string text = currentControlMap[row * 8 + column].paramId;
                if (currentControlMap[row * 8 + column].paramId.empty()) {
                    text = std::string("");
                }
                if (column == 0) {
                    if (row == 0) {
                        text = std::string("enc1-8: ");
                    }
                    if (row == 1) {
                        text = std::string("encp1-8: ");
                    }
                    if (row == 2) {
                        text = std::string("b1-8: ");
                    }
                }
                ImGui_Text(ctx, text.c_str());
            }
        }
        ImGui_TableSetColumnIndex(ctx, 0);
        std::string text = "b9-16: ";
        ImGui_Text(ctx, text.c_str());
        for (int column = 1; column < 9; column++) {
            ImGui_TableSetColumnIndex(ctx, column);
            text = std::string("fx") + std::to_string(column);
            ImGui_Text(ctx, text.c_str());
        }
        ImGui_EndTable(ctx);
    }
    return;
}

static void ImGuiLoop()
{
    if (!ctx)
        ctx = ImGui_CreateContext("ReaFab");

    int cond {ImGui_Cond_FirstUseEver};
    ImGui_SetNextWindowSize(ctx, 800, 30, &cond);

    bool open {true};
    if (ImGui_Begin(ctx, "ReaFab Monitor", &open)) {
        ImGuiFrame();
        ImGui_End(ctx);
    }

    if (!open || !ImGui_ValidatePtr(ctx, "ImGui_Context*")) {
        plugin_register("-timer", reinterpret_cast<void*>(&ImGuiLoop));
        ImGui_DestroyContext(ctx);
        ctx = nullptr;
    }
}

static bool ImGuiCommandHook(
    KbdSectionInfo* sec,
    const int command,
    const int val,
    const int valhw,
    const int relmode,
    HWND hwnd)
{
    (void)sec;
    (void)val;
    (void)valhw;
    (void)relmode;
    (void)hwnd; // unused

    if (command == imguiActionId) {
        if (!ctx)
            plugin_register("timer", reinterpret_cast<void*>(&ImGuiLoop));
        return true;
    }

    return false;
}

void Register(bool load)
{
    auto count = 8;
    std::string regAction {"custom_action"};
    std::string regHook {"hookcommand2"};

    if (!load) {
        regAction = std::string {"-"} + regAction;
        regHook = std::string {"-"} + regHook;
    }

    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab Encoder "};
        commandStr = commandStr + std::to_string(i + 1);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register(regAction.c_str(), &action);
        actions[commandId] = i + 1;
    }

    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab Encoder Push "};
        commandStr = commandStr + std::to_string(i + count + 1);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register(regAction.c_str(), &action);
        actions[commandId] = i + count + 1;
    }

    count = 8;
    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab Button "};
        commandStr = commandStr + std::to_string(i + 2 * count + 1);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register(regAction.c_str(), &action);
        actions[commandId] = i + 2 * count + 1;
    }

    count = 8;
    for (int i = 0; i < count; i++) {
        std::string commandStr {"AK5K_REAFAB"};
        std::string actionStr {"ReaFab FX "};
        commandStr = commandStr + std::to_string(i + 3 * count + 1);
        actionStr = actionStr + std::to_string(i + 1);
        custom_action_register_t action =
            {0, commandStr.c_str(), actionStr.c_str(), NULL};
        auto commandId = plugin_register(regAction.c_str(), &action);
        actions[commandId] = i + 3 * count + 1;
    }

    plugin_register(regHook.c_str(), (void*)OnAction);

    if (load) {
        plugin_register("API_Fab_Get", (void*)Get);
        plugin_register("APIdef_Fab_Get", (void*)defstring_Get);
        plugin_register(
            "APIvararg_Fab_Get",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Get>));

        plugin_register("API_Fab_Set", (void*)Set);
        plugin_register("APIdef_Fab_Set", (void*)defstring_Set);
        plugin_register(
            "APIvararg_Fab_Set",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Set>));

        plugin_register("API_Fab_Map", (void*)Map);
        plugin_register("APIdef_Fab_Map", (void*)defstring_Map);
        plugin_register(
            "APIvararg_Fab_Map",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Map>));

        plugin_register("API_Fab_Clear", (void*)Clear);
        plugin_register("APIdef_Fab_Clear", (void*)defstring_Clear);
        plugin_register(
            "APIvararg_Fab_Clear",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Clear>));
    }
    else {
        plugin_register("-API_Fab_Get", (void*)Get);
        plugin_register("-APIdef_Fab_Get", (void*)defstring_Get);
        plugin_register(
            "-APIvararg_Fab_Get",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Get>));

        plugin_register("-API_Fab_Set", (void*)Set);
        plugin_register("-APIdef_Fab_Set", (void*)defstring_Set);
        plugin_register(
            "-APIvararg_Fab_Set",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Set>));

        plugin_register("-API_Fab_Map", (void*)Map);
        plugin_register("-APIdef_Fab_Map", (void*)defstring_Map);
        plugin_register(
            "-APIvararg_Fab_Map",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Map>));

        plugin_register("-API_Fab_Clear", (void*)Clear);
        plugin_register("-APIdef_Fab_Clear", (void*)defstring_Clear);
        plugin_register(
            "-APIvararg_Fab_Clear",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Clear>));
    }

    RegisterPlugins();

    return;
}

void RegisterImGui(bool load)
{
    custom_action_register_t imGuiAction {
        0,
        "AK5K_REAFAB_MONITOR",
        "ReaFab Monitor",
        NULL};

    if (!load) {
        plugin_register(
            "-hookcommand2",
            reinterpret_cast<void*>(&ImGuiCommandHook));
        plugin_register("-custom_action", &imGuiAction);
    }
    else {
        imguiActionId = plugin_register("custom_action", &imGuiAction);
        plugin_register(
            "hookcommand2",
            reinterpret_cast<void*>(&ImGuiCommandHook));
    }
}

} // namespace reafab