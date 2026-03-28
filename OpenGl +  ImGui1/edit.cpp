#include "edit.h"
#include "objeckte.h"
#include <cmath>
#include <string>
#include <cstring>

void zeichneEditFenster() {
    ImGui::Begin("Edit Fenster");
    ImGui::Separator();

    if (selectedObjectIndex >= 0 && selectedObjectIndex < (int)objeckteListe.size()) {
        Objeckte& aktuellesObjekt = objeckteListe[selectedObjectIndex];
        ImGui::Text("Aktuelles Objekt: %s", aktuellesObjekt.name.c_str());

        static char nameBuffer[256];
        strncpy_s(nameBuffer, sizeof(nameBuffer), aktuellesObjekt.name.c_str(), _TRUNCATE);

        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            aktuellesObjekt.name = nameBuffer;
        }

        ImGui::Separator();
        ImGui::Text("Position");
        ImGui::DragFloat("Position X", &aktuellesObjekt.PositionX, 1.0f);
        ImGui::DragFloat("Position Y", &aktuellesObjekt.PositionY, 1.0f);

        ImGui::Separator();
        ImGui::Text("Groesse");
        // Diese beiden funktionieren jetzt - hoe und breite werden beim Zeichnen verwendet
        ImGui::DragFloat("Hoehe", &aktuellesObjekt.hoe, 1.0f, 1.0f, 2000.0f);
        ImGui::DragFloat("Breite", &aktuellesObjekt.breite, 1.0f, 1.0f, 2000.0f);

        ImGui::Separator();
        ImGui::Text("Farbe");
        float color[4] = {
            aktuellesObjekt.farbe.r / 255.0f,
            aktuellesObjekt.farbe.g / 255.0f,
            aktuellesObjekt.farbe.b / 255.0f,
            aktuellesObjekt.farbe.a / 255.0f
        };
        if (ImGui::ColorEdit4("Farbe", color)) {
            aktuellesObjekt.farbe.r = (unsigned char)(color[0] * 255);
            aktuellesObjekt.farbe.g = (unsigned char)(color[1] * 255);
            aktuellesObjekt.farbe.b = (unsigned char)(color[2] * 255);
            aktuellesObjekt.farbe.a = (unsigned char)(color[3] * 255);
        }
    }
    else {
        ImGui::Text("Kein Objekt ausgewaehlt");
    }

    ImGui::Separator();
    ImGui::End();
}