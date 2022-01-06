#include "reafab.hpp"

namespace reafab {

void MapPlugins()
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
    for (auto&& i : tempMap) {
        i.second.accel = 3;
    }
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
    for (auto&& i : tempMap) {
        i.second.accel = 3;
    }
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
    for (auto&& i : tempMap) {
        i.second.accel = 3;
    }
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
    for (auto&& i : tempMap) {
        i.second.accel = 3;
    }
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
    for (auto&& i : tempMap) {
        i.second.accel = 3;
    }
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
    for (auto&& i : tempMap) {
        i.second.accel = 3;
    }
    controlMap[id] = tempMap;

    tempMap.clear();
    id = "ReEQ";
    bands = 5;
    double step = 0.4;
    tempMap[1] = ControlTarget {"Filter00 Frequency", 8, bands, step};
    tempMap[2] = ControlTarget {"Filter00 Gain", 8, bands, step};
    tempMap[3] = ControlTarget {"Filter00 Q", 8, bands, step};
    tempMap[4] = ControlTarget {"Filter00 Type", 1, bands};
    tempMap[5] = ControlTarget {"Filter00 Slope", 1, bands};
    tempMap[9] = ControlTarget {"Filter00 Mode", 7, bands, 1.0, 1.0, 1.0, 2.0};
    tempMap[10] = ControlTarget {"Filter00 Gain", 4, bands};
    tempMap[17] = ControlTarget {"-band", 2, bands};
    tempMap[18] = ControlTarget {"band", 2, bands};
    tempMap[19] = ControlTarget {"-Filter00 Type", 1, bands};
    tempMap[20] = ControlTarget {"Filter00 Type", 1, bands};
    tempMap[21] = ControlTarget {"-Filter00 Slope", 1, bands};
    tempMap[22] = ControlTarget {"Filter00 Slope", 1, bands};
    controlMap[id] = tempMap;

    tempMap.clear();
    id = "ReaComp";
    tempMap[1] = ControlTarget {"Thresh", 1};
    tempMap[2] = ControlTarget {"Ratio", 1};
    tempMap[3] = ControlTarget {"Attack", 1};
    tempMap[4] = ControlTarget {"Release", 1};
    tempMap[5] = ControlTarget {"Knee", 1};
    tempMap[6] = ControlTarget {"RMS size", 1};
    tempMap[7] = ControlTarget {"Dry", 1};
    tempMap[8] = ControlTarget {"Wet", 1};
    tempMap[17] = ControlTarget {"AutoMkUp", 1};
    tempMap[18] = ControlTarget {"AutoRel", 1};
    for (auto&& i : tempMap) {
        i.second.step = i.second.step * 2;
    }
    controlMap[id] = tempMap;

    tempMap.clear();
    id = "ReaLimit";
    tempMap[1] = ControlTarget {"Threshold", 1};
    tempMap[2] = ControlTarget {"Ceiling", 1};
    tempMap[3] = ControlTarget {"Release", 1};
    controlMap[id] = tempMap;

    tempMap.clear();
    id = "ReaGate";
    tempMap[1] = ControlTarget {"Thresh", 1};
    tempMap[2] = ControlTarget {"Hold", 1};
    tempMap[3] = ControlTarget {"Attack", 1};
    tempMap[4] = ControlTarget {"Release", 1};
    tempMap[5] = ControlTarget {"Hystrsis", 1};
    tempMap[6] = ControlTarget {"RMS size", 1};
    tempMap[7] = ControlTarget {"Dry", 1};
    tempMap[8] = ControlTarget {"Wet", 1};
    tempMap[17] = ControlTarget {"InvrtWet", 5};
    for (auto&& i : tempMap) {
        i.second.step = i.second.step * 2;
    }
    controlMap[id] = tempMap;

    tempMap.clear();
    bands = 24;
    id = "ReaXcomp";
    tempMap[1] = ControlTarget {"00-Thresh", 1, bands};
    tempMap[2] = ControlTarget {"00-Ratio", 1, bands};
    tempMap[3] = ControlTarget {"00-Attack", 1, bands};
    tempMap[4] = ControlTarget {"00-Release", 1, bands};
    tempMap[5] = ControlTarget {"00-Knee", 1, bands};
    tempMap[6] = ControlTarget {"00-RMS", 1, bands};
    tempMap[7] = ControlTarget {"00-Freq", 1, bands};
    tempMap[9] = ControlTarget {"00-Active", 5, bands};
    tempMap[17] = ControlTarget {"-band", 2, bands};
    tempMap[18] = ControlTarget {"band", 2, bands};
    tempMap[19] = ControlTarget {"00-MakeUp", 5, bands};
    tempMap[20] = ControlTarget {"00-AutoRel", 5, bands};
    tempMap[21] = ControlTarget {"00-FeedBk", 5, bands};
    for (auto&& i : tempMap) {
        i.second.step = i.second.step * 2;
    }
    controlMap[id] = tempMap;

    tempMap.clear();
    bands = 8;
    id = "ReaEQ (ReaPlugs Edition)";
    tempMap[1] = ControlTarget {"00-Freq", 1, bands};
    tempMap[2] = ControlTarget {"00-Gain", 1, bands};
    tempMap[3] = ControlTarget {"00-Q", 1, bands};
    tempMap[17] = ControlTarget {"-band", 2, bands};
    tempMap[18] = ControlTarget {"band", 2, bands};
    for (auto&& i : tempMap) {
        i.second.step = i.second.step * 2;
    }
    controlMap[id] = tempMap;

    return;
}

} // namespace reafab
