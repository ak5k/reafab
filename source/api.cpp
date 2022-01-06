#include "reafab.hpp"

namespace reafab {

static bool Do(int command, int val)
{
    if (command < 1 || command > 32 || val < 1 || val > 127) {
        return false;
    }
    ProcessCommand(command, val);
    return true;
}

const char* defstring_Do =
    "bool\0int,int\0command,val\0"
    "Runs ReaFab actions/commands. First "
    "parameter (command) is ReaFab command number, e.g. 3 for 3rd encoder "
    "rotation. Second parameter (val) is MIDI CC Relative value. Value 1 is "
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
    if (param != -1 || currentControlTarget.control == 2) {
        *fxOut = fx;
        *paramOut = param;
        return true;
    }
    return false;
}

const char* defstring_Get =
    "bool\0int,int*,int*\0command,fxOut,paramOut\0"
    "Returns target FX and parameter index for given ReaFab command in context "
    "of selected track and ReaFab FX index. Valid command range 1 ... 24. "
    "Returns false if no such command mapping is found. Returns param index -1 "
    "for ReaFab internal band change command.";

static bool Clear(const char* idStringInOptional)
{
    std::string s {idStringInOptional};
    if (s.empty()) {
        controlMap.clear();
        return true;
    }

    for (auto& i : controlMap) {
        if (s.compare(i.first) == 0) {
            controlMap.erase(i.first);
            return true;
        }
    }
    return false;
}

const char* defstring_Clear =
    "bool\0const char*\0idStringInOptional\0"
    "Clears ReaFab control map, optionally based on matching idString. Returns "
    "true on success.";

bool Map(
    const char* fxId,
    int command,
    const char* paramId,
    int control,
    const int* bandsInOptional,
    // const double* rateInOptional,
    const double* stepInOptional,
    const double* accelInOptional,
    const double* minvalInOptional,
    const double* maxvalInOptional)
{
    if (command < 1 || command > 24 || !fxId || !paramId || control < 1 ||
        control > NUMCONTROLTYPES) {
        return false;
    }
    std::string fxIdString {fxId};
    ControlTarget currentControlTarget;
    currentControlTarget.paramId = std::string(paramId);
    currentControlTarget.control = control;
    if (bandsInOptional) {
        if (!(currentControlTarget.paramId.find("00") != std::string::npos)) {
            return false;
        }
        currentControlTarget.bands = *bandsInOptional;
    }
    // if (rateInOptional) {
    //     currentControlTarget.step = *rateInOptional;
    // }
    if (accelInOptional) {
        currentControlTarget.accel = *accelInOptional;
    }
    if (minvalInOptional) {
        currentControlTarget.minval = *minvalInOptional;
    }
    if (maxvalInOptional) {
        currentControlTarget.maxval = *minvalInOptional;
    }
    if (stepInOptional) {
        currentControlTarget.step = *stepInOptional;
    }

    controlMap[fxIdString][command] = currentControlTarget;
    return true;
}

const char* defstring_Map =
    "bool\0"
    "const char*,int,const "
    "char*,int,int*,double*,double*,double*,double*\0"
    "fxId,command,paramId,control,bandsInOptional,stepInOptional,"
    "accelInOptional,"
    "minvalInOptional,maxvalInOptional\0"
    "Creates control mapping for ReaFab command.\n"
    "fxId e.g. \"ReaComp\".\n"
    "command 1-8 for encoders, 9-24 for buttons.\n"
    "paramId e.g. \"Ratio\".\n"
    "control 1 = direct, 2 = band selector, 3 = cycle, 4 = invert, 5 = force "
    "toggle, 6 = force range, 7 = 5 and 6, 8 = force continuous.\n"
    "bands define, if target fx has multiple identical target bands. In this "
    "case, paramId must include 00 placeholder, e.g. \"Band 00 Gain\".\n"
    "step overrides built-in default step of ~0.001 for continuous "
    "parameters.\n"
    "accel overrides built-in default control acceleration step of 1.0.\n"
    "minval & maxval override default detected target param value range.\n"
    "Prefixing paramId with \"-\" reverses direction; useful for creating "
    "separate next/previous mappings for bands or list type value navigation.";

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

        plugin_register("API_Fab_Do", (void*)Do);
        plugin_register("APIdef_Fab_Do", (void*)defstring_Do);
        plugin_register(
            "APIvararg_Fab_Do",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Do>));

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

        plugin_register("-API_Fab_Do", (void*)Do);
        plugin_register("-APIdef_Fab_Do", (void*)defstring_Do);
        plugin_register(
            "-APIvararg_Fab_Do",
            reinterpret_cast<void*>(&InvokeReaScriptAPI<&Do>));

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

    MapPlugins();

    return;
}

} // namespace reafab
