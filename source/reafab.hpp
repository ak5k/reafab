#ifndef REAFAB_HPP
#define REAFAB_HPP

#include <cmath>
#include <map>
#include <mutex>
#include <string>
#include <vector>

#include <reaper_imgui_functions.h>
#include <reaper_plugin_functions.h>
#include <reascript_vararg.hpp>

namespace reafab {

#define NUMCONTROLTYPES 8

extern int inputValue;
extern ImGui_Context* ctx;
extern MediaTrack* prevTrack;
extern MediaTrack* track; // current track
extern bool hidden;
extern char bufOut[BUFSIZ];
extern int imguiActionId;
extern size_t currentFXindex;
extern std::map<GUID*, int> previousBand;
extern std::map<int, int> actions;
extern std::mutex m;
extern std::vector<int> fxIndices;

struct ControlTarget {
    std::string paramId {0};
    int control {0};
    int bands {0};
    // double step {1.};
    double step {0.0013123359531164};
    double accel {1.};
    double minval {0};
    double maxval {0};
};

extern std::map<std::string, std::map<int, ControlTarget>> controlMap;

void Initialize();
void ProcessCommand(int command, int val, int relmode = 1);
bool isValid();
bool OnAction(
    KbdSectionInfo* sec,
    int command,
    int val,
    int valhw,
    int relmode,
    HWND hwnd);

void Register(bool load);
void RegisterImGui(bool load);
void MapPlugins();
} // namespace reafab

#endif
