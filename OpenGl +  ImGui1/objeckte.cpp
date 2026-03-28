#include "objeckte.h"
#include "imgui.h"
#include <vector>
#include <string>
#include <iostream>
#include <GLFW/glfw3.h>
#include <algorithm>

std::vector<Objeckte> objeckteListe;
int selectedObjectIndex = -1;
const char* items[] = { "Rechteck", "Kreis", "Dreieck" };
int selected = 0;

// Console logging
static std::vector<std::string> consoleLogs;
static void addConsoleLog(const std::string& message) {
    consoleLogs.push_back(message);
    if (consoleLogs.size() > 100) consoleLogs.erase(consoleLogs.begin());
}

void objecktaddbutton() {
    if (ImGui::BeginCombo("##form_auswahl", items[selected])) {
        for (int i = 0; i < 3; i++) {
            if (ImGui::Selectable(items[i], selected == i)) {
                selected = i;
                if (i == 0) addObjeckt(Rechteck);
                else if (i == 1) addObjeckt(Kreis);
                else if (i == 2) addObjeckt(Dreieck);
            }
        }
        ImGui::EndCombo();
    }
}

void addObjeckt(formen form) {
    Objeckte neuesObjekt;
    neuesObjekt.name = "Objekt_" + std::to_string(objeckteListe.size() + 1);
    neuesObjekt.form = form;
    neuesObjekt.PositionX = 400 + (objeckteListe.size() * 20);
    neuesObjekt.PositionY = 300 + (objeckteListe.size() * 20);
    neuesObjekt.hoe = 50;       // War 10 - jetzt sinnvoller Standardwert
    neuesObjekt.breite = 50;    // War 10 - jetzt sinnvoller Standardwert
    neuesObjekt.farbe = { 255, 0, 0, 255 };
    neuesObjekt.visible = true;
    neuesObjekt.locked = false;
    neuesObjekt.layer = 0;

    objeckteListe.push_back(neuesObjekt);
    addConsoleLog("Objekt hinzugefuegt: " + neuesObjekt.name);
}

void zeigeObjeckte() {
    for (size_t i = 0; i < objeckteListe.size(); i++) {
        Objeckte& obj = objeckteListe[i];

        ImGui::PushID(i);

        if (ImGui::Checkbox("##visible", &obj.visible)) {
            addConsoleLog(obj.name + " Sichtbarkeit: " + (obj.visible ? "An" : "Aus"));
        }
        ImGui::SameLine();

        if (ImGui::Checkbox("##locked", &obj.locked)) {
            addConsoleLog(obj.name + " Gesperrt: " + (obj.locked ? "Ja" : "Nein"));
        }
        ImGui::SameLine();

        std::string label = obj.name + "| ";
        switch (obj.form) {
        case Rechteck: label += "Rechteck"; break;
        case Kreis:    label += "Kreis";    break;
        case Dreieck:  label += "Dreieck";  break;
        }
        label += " (X: " + std::to_string((int)obj.PositionX) +
            ", Y: " + std::to_string((int)obj.PositionY) + ")";

        bool isSelected = (selectedObjectIndex == (int)i);

        if (ImGui::Button("X")) {
            addConsoleLog("Objekt geloescht: " + obj.name);
            objeckteListe.erase(objeckteListe.begin() + i);
            if (selectedObjectIndex == (int)i) selectedObjectIndex = -1;
            else if (selectedObjectIndex > (int)i) selectedObjectIndex--;
            ImGui::PopID();
            break;
        }
        ImGui::SameLine();

        float selectableWidth = ImGui::GetContentRegionAvail().x;
        if (ImGui::Selectable(label.c_str(), isSelected,
            ImGuiSelectableFlags_AllowDoubleClick, ImVec2(selectableWidth, 0))) {
            if (!obj.locked) {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isSelected)
                    selectedObjectIndex = -1;
                else if (!ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    selectedObjectIndex = (int)i;
                    addConsoleLog("Ausgewaehlt: " + obj.name);
                }
            }
        }

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("DND_OBJECT", &i, sizeof(size_t));
            ImGui::Text("Verschiebe: %s", obj.name.c_str());
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_OBJECT")) {
                size_t sourceIndex = *(const size_t*)payload->Data;
                if (sourceIndex != i) {
                    Objeckte temp = objeckteListe[sourceIndex];
                    objeckteListe.erase(objeckteListe.begin() + sourceIndex);
                    size_t targetIndex = i;
                    if (sourceIndex < targetIndex) targetIndex--;
                    objeckteListe.insert(objeckteListe.begin() + targetIndex, temp);
                    addConsoleLog("Objekt neu angeordnet");
                    if (selectedObjectIndex == (int)sourceIndex)
                        selectedObjectIndex = (int)targetIndex;
                    else if (selectedObjectIndex > (int)sourceIndex && selectedObjectIndex <= (int)targetIndex)
                        selectedObjectIndex--;
                    else if (selectedObjectIndex < (int)sourceIndex && selectedObjectIndex >= (int)targetIndex)
                        selectedObjectIndex++;
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
        ImGui::Separator();
    }
}

void zeigeErweiterteObjektListe() {
    ImGui::Text("Objekt Management");
    ImGui::Separator();

    static char filterBuffer[256] = "";
    ImGui::InputText("Filter", filterBuffer, sizeof(filterBuffer));

    int visibleCount = 0, lockedCount = 0;
    for (const auto& obj : objeckteListe) {
        if (obj.visible) visibleCount++;
        if (obj.locked) lockedCount++;
    }

    ImGui::Text("Gesamt: %zu | Sichtbar: %d | Gesperrt: %d", objeckteListe.size(), visibleCount, lockedCount);
    ImGui::Separator();

    if (ImGui::Button("Alle sichtbar")) { for (auto& obj : objeckteListe) obj.visible = true; }
    ImGui::SameLine();
    if (ImGui::Button("Alle unsichtbar")) { for (auto& obj : objeckteListe) obj.visible = false; }
    ImGui::SameLine();
    if (ImGui::Button("Alle entsperren")) { for (auto& obj : objeckteListe) obj.locked = false; }

    ImGui::Separator();

    for (size_t i = 0; i < objeckteListe.size(); i++) {
        Objeckte& obj = objeckteListe[i];
        std::string filterStr = filterBuffer;
        if (!filterStr.empty() && obj.name.find(filterStr) == std::string::npos) continue;

        ImGui::PushID(i);
        ImGui::ColorButton("##color",
            ImVec4(obj.farbe.r / 255.0f, obj.farbe.g / 255.0f, obj.farbe.b / 255.0f, obj.farbe.a / 255.0f),
            ImGuiColorEditFlags_NoTooltip, ImVec2(16, 16));
        ImGui::SameLine();

        bool isSelected = (selectedObjectIndex == (int)i);
        if (ImGui::Selectable(obj.name.c_str(), isSelected)) {
            if (!obj.locked) selectedObjectIndex = (int)i;
        }
        ImGui::PopID();
    }
}

void zeigeObjektBrowser() {
    ImGui::Text("Objekt Browser");
    ImGui::Separator();
    ImGui::Text("Schnell hinzufuegen:");
    if (ImGui::Button("Rechteck")) addObjeckt(Rechteck);
    ImGui::SameLine();
    if (ImGui::Button("Kreis")) addObjeckt(Kreis);
    ImGui::SameLine();
    if (ImGui::Button("Dreieck")) addObjeckt(Dreieck);
    ImGui::Separator();

    for (size_t i = 0; i < objeckteListe.size(); i++) {
        Objeckte& obj = objeckteListe[i];
        ImGui::PushID(i);
        const char* icon = (obj.form == Rechteck) ? "[R]" : (obj.form == Kreis) ? "[K]" : "[D]";
        ImGui::Text("%s", icon);
        ImGui::SameLine();
        bool isSelected = (selectedObjectIndex == (int)i);
        if (ImGui::Selectable(obj.name.c_str(), isSelected)) {
            if (!obj.locked) selectedObjectIndex = (int)i;
        }
        ImGui::PopID();
    }
}

void zeichneObjeckte() {
    int width, height;
    glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    std::vector<Objeckte> sortedObjects = objeckteListe;
    std::sort(sortedObjects.begin(), sortedObjects.end(),
        [](const Objeckte& a, const Objeckte& b) { return a.layer < b.layer; });

    for (const auto& obj : sortedObjects) {
        if (!obj.visible) continue;

        glColor4ub(obj.farbe.r, obj.farbe.g, obj.farbe.b, obj.farbe.a);

        float w = obj.breite;
        float h = obj.hoe;

        switch (obj.form) {
        case Rechteck:
            glBegin(GL_QUADS);
            glVertex2f(obj.PositionX, obj.PositionY);
            glVertex2f(obj.PositionX + w, obj.PositionY);
            glVertex2f(obj.PositionX + w, obj.PositionY + h);
            glVertex2f(obj.PositionX, obj.PositionY + h);
            glEnd();
            break;

        case Kreis: {
            float rx = w / 2.0f;
            float ry = h / 2.0f;
            float cx = obj.PositionX + rx;
            float cy = obj.PositionY + ry;
            int segments = 32;
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(cx, cy);
            for (int i = 0; i <= segments; i++) {
                float angle = 2.0f * 3.14159f * i / segments;
                glVertex2f(cx + rx * cosf(angle), cy + ry * sinf(angle));
            }
            glEnd();
            break;
        }

        case Dreieck:
            glBegin(GL_TRIANGLES);
            glVertex2f(obj.PositionX + w / 2.0f, obj.PositionY);
            glVertex2f(obj.PositionX, obj.PositionY + h);
            glVertex2f(obj.PositionX + w, obj.PositionY + h);
            glEnd();
            break;
        }
    }

    // Highlight selected object
    if (selectedObjectIndex >= 0 && selectedObjectIndex < (int)objeckteListe.size()) {
        const auto& obj = objeckteListe[selectedObjectIndex];
        if (obj.visible) {
            glColor4ub(255, 255, 0, 255);

            float w = obj.breite;
            float h = obj.hoe;

            switch (obj.form) {
            case Rechteck:
                glBegin(GL_LINE_LOOP);
                glVertex2f(obj.PositionX, obj.PositionY);
                glVertex2f(obj.PositionX + w, obj.PositionY);
                glVertex2f(obj.PositionX + w, obj.PositionY + h);
                glVertex2f(obj.PositionX, obj.PositionY + h);
                glEnd();
                break;

            case Kreis: {
                float rx = w / 2.0f;
                float ry = h / 2.0f;
                float cx = obj.PositionX + rx;
                float cy = obj.PositionY + ry;
                int segments = 32;
                glBegin(GL_LINE_LOOP);
                for (int i = 0; i <= segments; i++) {
                    float angle = 2.0f * 3.14159f * i / segments;
                    glVertex2f(cx + rx * cosf(angle), cy + ry * sinf(angle));
                }
                glEnd();
                break;
            }

            case Dreieck:
                glBegin(GL_LINE_LOOP);
                glVertex2f(obj.PositionX + w / 2.0f, obj.PositionY);
                glVertex2f(obj.PositionX, obj.PositionY + h);
                glVertex2f(obj.PositionX + w, obj.PositionY + h);
                glEnd();
                break;
            }
        }
    }
}

void zeigeConsole() {
    static std::vector<std::string>& logs = consoleLogs;

    ImGui::Text("Console Output");
    ImGui::Separator();

    if (ImGui::Button("Clear")) consoleLogs.clear();
    ImGui::SameLine();
    if (ImGui::Button("Scroll to bottom")) ImGui::SetScrollHereY(1.0f);

    ImGui::Separator();
    ImGui::BeginChild("ConsoleScroll");
    for (const auto& log : consoleLogs) {
        ImGui::TextWrapped("%s", log.c_str());
    }
    ImGui::EndChild();
}