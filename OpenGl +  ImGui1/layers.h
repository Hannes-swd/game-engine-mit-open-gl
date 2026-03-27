#pragma once
#include "imgui.h"
#include "objeckte.h"
#include <vector>
#include <string>
#include <algorithm>

// Layer structure
struct Layer {
    std::string name;
    bool visible;
    bool locked;
    int index;
    ColorRGBA color;
};

extern std::vector<Layer> layers;
extern int selectedLayerIndex;

void initLayers();
void zeigeLayerEditor();
void zeigeErweiterteEigenschaften();
void addLayer();
void removeLayer(int index);
void moveObjectToLayer(int objectIndex, int layerIndex);