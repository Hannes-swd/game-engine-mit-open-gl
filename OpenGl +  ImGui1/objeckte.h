#pragma once
#include "imgui.h"
#include <vector>
#include <string>

enum formen {
    Rechteck,
    Kreis,
    Dreieck,
};

// Simple RGBA color struct
struct ColorRGBA {
    unsigned char r, g, b, a;
};

struct Objeckte {
    std::string name;
    formen form;
    float PositionX;
    float PositionY;
    ColorRGBA farbe;
    bool visible = true;
    bool locked = false;
    int layer = 0;
    std::string tag = "";
};

extern std::vector<Objeckte> objeckteListe;
extern int selectedObjectIndex;
extern const char* items[];
extern int selected;

void objecktaddbutton();
void addObjeckt(formen form);
void zeigeObjeckte();
void zeigeErweiterteObjektListe();
void zeigeObjektBrowser();
void zeichneObjeckte();
void zeigeConsole();  // Neue Funktion