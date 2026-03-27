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

                if (i == 0)
                    addObjeckt(Rechteck);
                else if (i == 1)
                    addObjeckt(Kreis);
                else if (i == 2)
                    addObjeckt(Dreieck);
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
    neuesObjekt.farbe = { 255, 0, 0, 255 };
    neuesObjekt.visible = true;
    neuesObjekt.locked = false;
    neuesObjekt.layer = 0;

    objeckteListe.push_back(neuesObjekt);
    addConsoleLog("Objekt hinzugefügt: " + neuesObjekt.name);
}

void zeigeObjeckte() {
    for (size_t i = 0; i < objeckteListe.size(); i++) {
        Objeckte& obj = objeckteListe[i];

        ImGui::PushID(i);

        // Visibility toggle
        if (ImGui::Checkbox("##visible", &obj.visible)) {
            addConsoleLog(obj.name + " Sichtbarkeit: " + (obj.visible ? "An" : "Aus"));
        }
        ImGui::SameLine();

        // Lock toggle
        if (ImGui::Checkbox("##locked", &obj.locked)) {
            addConsoleLog(obj.name + " Gesperrt: " + (obj.locked ? "Ja" : "Nein"));
        }
        ImGui::SameLine();

        std::string label = obj.name + "| ";

        switch (obj.form) {
        case Rechteck: label += "Rechteck"; break;
        case Kreis: label += "Kreis"; break;
        case Dreieck: label += "Dreieck"; break;
        }

        label += " (X: " + std::to_string((int)obj.PositionX) +
            ", Y: " + std::to_string((int)obj.PositionY) + ")";

        bool isSelected = (selectedObjectIndex == (int)i);

        // Delete button
        if (ImGui::Button("X")) {
            addConsoleLog("Objekt gelöscht: " + obj.name);
            objeckteListe.erase(objeckteListe.begin() + i);
            if (selectedObjectIndex == (int)i) {
                selectedObjectIndex = -1;
            }
            else if (selectedObjectIndex > (int)i) {
                selectedObjectIndex--;
            }
            ImGui::PopID();
            break;
        }

        ImGui::SameLine();

        // Selectable
        float selectableWidth = ImGui::GetContentRegionAvail().x;
        if (ImGui::Selectable(label.c_str(), isSelected,
            ImGuiSelectableFlags_AllowDoubleClick, ImVec2(selectableWidth, 0))) {
            if (!obj.locked) {
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left) && isSelected) {
                    selectedObjectIndex = -1;
                }
                else if (!ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                    selectedObjectIndex = (int)i;
                    addConsoleLog("Ausgewählt: " + obj.name);
                }
            }
        }

        // Drag & Drop source
        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("DND_OBJECT", &i, sizeof(size_t));
            ImGui::Text("Verschiebe: %s", obj.name.c_str());
            ImGui::EndDragDropSource();
        }

        // Drag & Drop target
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_OBJECT")) {
                size_t sourceIndex = *(const size_t*)payload->Data;
                if (sourceIndex != i) {
                    Objeckte temp = objeckteListe[sourceIndex];
                    objeckteListe.erase(objeckteListe.begin() + sourceIndex);

                    size_t targetIndex = i;
                    if (sourceIndex < targetIndex) {
                        targetIndex--;
                    }
                    objeckteListe.insert(objeckteListe.begin() + targetIndex, temp);
                    addConsoleLog("Objekt neu angeordnet");

                    if (selectedObjectIndex == (int)sourceIndex) {
                        selectedObjectIndex = (int)targetIndex;
                    }
                    else if (selectedObjectIndex > (int)sourceIndex && selectedObjectIndex <= (int)targetIndex) {
                        selectedObjectIndex--;
                    }
                    else if (selectedObjectIndex < (int)sourceIndex && selectedObjectIndex >= (int)targetIndex) {
                        selectedObjectIndex++;
                    }
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

    // Filter
    static char filterBuffer[256] = "";
    ImGui::InputText("Filter", filterBuffer, sizeof(filterBuffer));

    // Statistics
    int visibleCount = 0;
    int lockedCount = 0;
    for (const auto& obj : objeckteListe) {
        if (obj.visible) visibleCount++;
        if (obj.locked) lockedCount++;
    }

    ImGui::Text("Gesamt: %zu | Sichtbar: %d | Gesperrt: %d", objeckteListe.size(), visibleCount, lockedCount);
    ImGui::Separator();

    // Batch operations
    if (ImGui::Button("Alle sichtbar")) {
        for (auto& obj : objeckteListe) obj.visible = true;
        addConsoleLog("Alle Objekte sichtbar gemacht");
    }
    ImGui::SameLine();
    if (ImGui::Button("Alle unsichtbar")) {
        for (auto& obj : objeckteListe) obj.visible = false;
        addConsoleLog("Alle Objekte unsichtbar gemacht");
    }
    ImGui::SameLine();
    if (ImGui::Button("Alle entsperren")) {
        for (auto& obj : objeckteListe) obj.locked = false;
        addConsoleLog("Alle Objekte entsperrt");
    }

    ImGui::Separator();

    // List with filtering
    for (size_t i = 0; i < objeckteListe.size(); i++) {
        Objeckte& obj = objeckteListe[i];

        // Apply filter
        std::string filterStr = filterBuffer;
        if (!filterStr.empty() && obj.name.find(filterStr) == std::string::npos) {
            continue;
        }

        ImGui::PushID(i);

        // Color indicator
        ImGui::ColorButton("##color", ImVec4(obj.farbe.r / 255.0f, obj.farbe.g / 255.0f, obj.farbe.b / 255.0f, obj.farbe.a / 255.0f),
            ImGuiColorEditFlags_NoTooltip, ImVec2(16, 16));
        ImGui::SameLine();

        // Selection
        bool isSelected = (selectedObjectIndex == (int)i);
        if (ImGui::Selectable(obj.name.c_str(), isSelected)) {
            if (!obj.locked) {
                selectedObjectIndex = (int)i;
            }
        }

        ImGui::PopID();
    }
}

void zeigeObjektBrowser() {
    ImGui::Text("Objekt Browser");
    ImGui::Separator();

    // Quick add buttons
    ImGui::Text("Schnell hinzufügen:");
    if (ImGui::Button("Rechteck")) addObjeckt(Rechteck);
    ImGui::SameLine();
    if (ImGui::Button("Kreis")) addObjeckt(Kreis);
    ImGui::SameLine();
    if (ImGui::Button("Dreieck")) addObjeckt(Dreieck);

    ImGui::Separator();

    // Object list with icons
    for (size_t i = 0; i < objeckteListe.size(); i++) {
        Objeckte& obj = objeckteListe[i];

        ImGui::PushID(i);

        // Icon based on shape
        const char* icon = "x";
        switch (obj.form) {
        case Rechteck: icon = "x"; break;
        case Kreis: icon = "o"; break;
        case Dreieck: icon = "a"; break;
        }

        ImGui::Text("%s", icon);
        ImGui::SameLine();

        bool isSelected = (selectedObjectIndex == (int)i);
        if (ImGui::Selectable(obj.name.c_str(), isSelected)) {
            if (!obj.locked) {
                selectedObjectIndex = (int)i;
            }
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

    // Sort by layer
    std::vector<Objeckte> sortedObjects = objeckteListe;
    std::sort(sortedObjects.begin(), sortedObjects.end(),
        [](const Objeckte& a, const Objeckte& b) { return a.layer < b.layer; });

    for (const auto& obj : sortedObjects) {
        if (!obj.visible) continue;

        glColor4ub(obj.farbe.r, obj.farbe.g, obj.farbe.b, obj.farbe.a);

        switch (obj.form) {
        case Rechteck:
            glBegin(GL_QUADS);
            glVertex2f(obj.PositionX, obj.PositionY);
            glVertex2f(obj.PositionX + 50, obj.PositionY);
            glVertex2f(obj.PositionX + 50, obj.PositionY + 50);
            glVertex2f(obj.PositionX, obj.PositionY + 50);
            glEnd();
            break;

        case Kreis: {
            float radius = 25;
            float x = obj.PositionX + radius;
            float y = obj.PositionY + radius;
            int segments = 32;
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y);
            for (int i = 0; i <= segments; i++) {
                float angle = 2.0f * 3.14159f * i / segments;
                float dx = radius * cosf(angle);
                float dy = radius * sinf(angle);
                glVertex2f(x + dx, y + dy);
            }
            glEnd();
            break;
        }

        case Dreieck:
            glBegin(GL_TRIANGLES);
            glVertex2f(obj.PositionX + 25, obj.PositionY);
            glVertex2f(obj.PositionX, obj.PositionY + 50);
            glVertex2f(obj.PositionX + 50, obj.PositionY + 50);
            glEnd();
            break;
        }
    }

    // Highlight selected object
    if (selectedObjectIndex >= 0 && selectedObjectIndex < (int)objeckteListe.size()) {
        const auto& obj = objeckteListe[selectedObjectIndex];
        if (obj.visible) {
            glColor4ub(255, 255, 0, 255);

            switch (obj.form) {
            case Rechteck:
                glBegin(GL_LINE_LOOP);
                glVertex2f(obj.PositionX, obj.PositionY);
                glVertex2f(obj.PositionX + 50, obj.PositionY);
                glVertex2f(obj.PositionX + 50, obj.PositionY + 50);
                glVertex2f(obj.PositionX, obj.PositionY + 50);
                glEnd();
                break;

            case Kreis: {
                float radius = 25;
                float x = obj.PositionX + radius;
                float y = obj.PositionY + radius;
                int segments = 32;
                glBegin(GL_LINE_LOOP);
                for (int i = 0; i <= segments; i++) {
                    float angle = 2.0f * 3.14159f * i / segments;
                    float dx = radius * cosf(angle);
                    float dy = radius * sinf(angle);
                    glVertex2f(x + dx, y + dy);
                }
                glEnd();
                break;
            }

            case Dreieck:
                glBegin(GL_LINE_LOOP);
                glVertex2f(obj.PositionX + 25, obj.PositionY);
                glVertex2f(obj.PositionX, obj.PositionY + 50);
                glVertex2f(obj.PositionX + 50, obj.PositionY + 50);
                glEnd();
                break;
            }
        }
    }
}

void zeigeConsole() {
    ImGui::Text("Console Output");
    ImGui::Separator();

    if (ImGui::Button("Clear")) {
        consoleLogs.clear();
    }
    ImGui::SameLine();
    if (ImGui::Button("Scroll to bottom")) {
        ImGui::SetScrollHereY(1.0f);
    }

    ImGui::Separator();

    ImGui::BeginChild("ConsoleScroll");
    for (const auto& log : consoleLogs) {
        ImGui::TextWrapped("%s", log.c_str());
    }
    ImGui::EndChild();
}