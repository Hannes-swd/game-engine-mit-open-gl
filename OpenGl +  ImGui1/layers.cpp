#include "layers.h"
#include "objeckte.h"
#include <iostream>
#include <cstring>

std::vector<Layer> layers;
int selectedLayerIndex = 0;

void initLayers() {
    if (layers.empty()) {
        Layer defaultLayer;
        defaultLayer.name = "Layer 0";
        defaultLayer.visible = true;
        defaultLayer.locked = false;
        defaultLayer.index = 0;
        defaultLayer.color = { 100, 100, 100, 255 };
        layers.push_back(defaultLayer);
    }
}

void addLayer() {
    Layer newLayer;
    newLayer.name = "Layer " + std::to_string(layers.size());
    newLayer.visible = true;
    newLayer.locked = false;
    newLayer.index = layers.size();
    newLayer.color = { (unsigned char)(rand() % 200 + 55),
                       (unsigned char)(rand() % 200 + 55),
                       (unsigned char)(rand() % 200 + 55), 255 };
    layers.push_back(newLayer);
}

void removeLayer(int index) {
    if (index >= 0 && index < (int)layers.size() && layers.size() > 1) {
        for (auto& obj : objeckteListe) {
            if (obj.layer == index) obj.layer = 0;
        }
        layers.erase(layers.begin() + index);
        for (size_t i = 0; i < layers.size(); i++) {
            layers[i].index = i;
        }
    }
}

void moveObjectToLayer(int objectIndex, int layerIndex) {
    if (objectIndex >= 0 && objectIndex < (int)objeckteListe.size() &&
        layerIndex >= 0 && layerIndex < (int)layers.size()) {
        objeckteListe[objectIndex].layer = layerIndex;
    }
}

void zeigeLayerEditor() {
    ImGui::Text("Layer Management");
    ImGui::Separator();

    if (ImGui::Button("+ Neue Ebene")) addLayer();
    ImGui::SameLine();
    if (ImGui::Button("- Ebene loeschen") && selectedLayerIndex >= 0) {
        removeLayer(selectedLayerIndex);
        if (selectedLayerIndex >= (int)layers.size())
            selectedLayerIndex = layers.size() - 1;
    }

    ImGui::Separator();

    for (size_t i = 0; i < layers.size(); i++) {
        Layer& layer = layers[i];
        ImGui::PushID(i);

        ImGui::Checkbox("##visible", &layer.visible);
        ImGui::SameLine();
        ImGui::Checkbox("##locked", &layer.locked);
        ImGui::SameLine();

        ImVec4 color(layer.color.r / 255.0f, layer.color.g / 255.0f,
            layer.color.b / 255.0f, layer.color.a / 255.0f);
        ImGui::ColorButton("##color", color, ImGuiColorEditFlags_NoTooltip, ImVec2(16, 16));
        ImGui::SameLine();

        bool isSelected = (selectedLayerIndex == (int)i);
        if (ImGui::Selectable(layer.name.c_str(), isSelected))
            selectedLayerIndex = (int)i;

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload("DND_LAYER", &i, sizeof(size_t));
            ImGui::Text("Verschiebe: %s", layer.name.c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget()) {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_LAYER")) {
                size_t sourceIndex = *(const size_t*)payload->Data;
                if (sourceIndex != i) {
                    Layer temp = layers[sourceIndex];
                    layers.erase(layers.begin() + sourceIndex);
                    size_t targetIndex = i;
                    if (sourceIndex < targetIndex) targetIndex--;
                    layers.insert(layers.begin() + targetIndex, temp);
                    for (size_t j = 0; j < layers.size(); j++)
                        layers[j].index = j;
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
        ImGui::Separator();
    }

    ImGui::Separator();
    ImGui::Text("Layer Info");
    ImGui::Text("Anzahl Objekte in ausgewaehlter Ebene: %d",
        std::count_if(objeckteListe.begin(), objeckteListe.end(),
            [&](const Objeckte& obj) { return obj.layer == selectedLayerIndex; }));
}

void zeigeErweiterteEigenschaften() {
    ImGui::Text("Erweiterte Objekt Eigenschaften");
    ImGui::Separator();

    if (selectedObjectIndex >= 0 && selectedObjectIndex < (int)objeckteListe.size()) {
        Objeckte& obj = objeckteListe[selectedObjectIndex];

        ImGui::Text("Name: %s", obj.name.c_str());
        ImGui::Text("Typ: %s",
            obj.form == Rechteck ? "Rechteck" :
            obj.form == Kreis ? "Kreis" : "Dreieck");

        ImGui::Separator();

        ImGui::Text("Ebene:");
        ImGui::SameLine();
        std::string layerPreview = (obj.layer >= 0 && obj.layer < (int)layers.size())
            ? layers[obj.layer].name : "Keine";
        if (ImGui::BeginCombo("##layer", layerPreview.c_str())) {
            for (size_t i = 0; i < layers.size(); i++) {
                bool isSelected = (obj.layer == (int)i);
                if (ImGui::Selectable(layers[i].name.c_str(), isSelected))
                    obj.layer = (int)i;
                if (isSelected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        static char tagBuffer[256];
        strncpy_s(tagBuffer, sizeof(tagBuffer), obj.tag.c_str(), _TRUNCATE);
        if (ImGui::InputText("Tag", tagBuffer, sizeof(tagBuffer)))
            obj.tag = tagBuffer;

        ImGui::Separator();

        ImGui::Text("Transform");
        ImGui::DragFloat("X", &obj.PositionX, 1.0f);
        ImGui::DragFloat("Y", &obj.PositionY, 1.0f);

        static float scale = 1.0f;
        ImGui::DragFloat("Scale", &scale, 0.1f, 0.1f, 5.0f);
        static float rotation = 0.0f;
        ImGui::DragFloat("Rotation", &rotation, 1.0f, -180.0f, 180.0f);

        ImGui::Separator();

        ImGui::Text("Appearance");
        float color[4] = {
            obj.farbe.r / 255.0f, obj.farbe.g / 255.0f,
            obj.farbe.b / 255.0f, obj.farbe.a / 255.0f
        };
        if (ImGui::ColorEdit4("Farbe", color)) {
            obj.farbe.r = (unsigned char)(color[0] * 255);
            obj.farbe.g = (unsigned char)(color[1] * 255);
            obj.farbe.b = (unsigned char)(color[2] * 255);
            obj.farbe.a = (unsigned char)(color[3] * 255);
        }

        ImGui::Separator();

        ImGui::Text("Status");
        ImGui::Checkbox("Sichtbar", &obj.visible);
        ImGui::Checkbox("Gesperrt", &obj.locked);

        if (ImGui::CollapsingHeader("Erweitert")) {
            ImGui::Text("Objekt ID: %d", selectedObjectIndex);
            ImGui::Text("Layer Index: %d", obj.layer);
            ImGui::Text("Speicheradresse: %p", (void*)&obj);
        }

        ImGui::Separator();
        if (ImGui::Button("Objekt duplizieren")) {
            Objeckte newObj = obj;
            newObj.name = obj.name + "_copy";
            newObj.PositionX += 30;
            newObj.PositionY += 30;
            objeckteListe.push_back(newObj);
        }
        ImGui::SameLine();
        if (ImGui::Button("Objekt loeschen")) {
            objeckteListe.erase(objeckteListe.begin() + selectedObjectIndex);
            selectedObjectIndex = -1;
        }
    }
    else {
        ImGui::Text("Kein Objekt ausgewaehlt");
        ImGui::Text("Waehlen Sie ein Objekt in der Objektliste oder im Canvas aus");
    }
}