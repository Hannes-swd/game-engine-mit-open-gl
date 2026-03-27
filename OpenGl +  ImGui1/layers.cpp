#include "layers.h"
#include "objeckte.h"
#include <iostream>

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
        // Move objects from this layer to layer 0
        for (auto& obj : objeckteListe) {
            if (obj.layer == index) {
                obj.layer = 0;
            }
        }
        layers.erase(layers.begin() + index);

        // Update layer indices
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

    // Add layer button
    if (ImGui::Button("+ Neue Ebene")) {
        addLayer();
    }
    ImGui::SameLine();
    if (ImGui::Button("- Ebene löschen") && selectedLayerIndex >= 0) {
        removeLayer(selectedLayerIndex);
        if (selectedLayerIndex >= (int)layers.size()) {
            selectedLayerIndex = layers.size() - 1;
        }
    }

    ImGui::Separator();

    // Layer list
    for (size_t i = 0; i < layers.size(); i++) {
        Layer& layer = layers[i];

        ImGui::PushID(i);

        // Visibility toggle
        bool visible = layer.visible;
        if (ImGui::Checkbox("##visible", &visible)) {
            layer.visible = visible;
        }
        ImGui::SameLine();

        // Lock toggle
        bool locked = layer.locked;
        if (ImGui::Checkbox("##locked", &locked)) {
            layer.locked = locked;
        }
        ImGui::SameLine();

        // Layer color indicator
        ImVec4 color(layer.color.r / 255.0f, layer.color.g / 255.0f, layer.color.b / 255.0f, layer.color.a / 255.0f);
        ImGui::ColorButton("##color", color, ImGuiColorEditFlags_NoTooltip, ImVec2(16, 16));
        ImGui::SameLine();

        // Layer name and selection
        bool isSelected = (selectedLayerIndex == (int)i);
        if (ImGui::Selectable(layer.name.c_str(), isSelected)) {
            selectedLayerIndex = (int)i;
        }

        // Drag & Drop for reordering layers
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

                    // Update layer indices
                    for (size_t j = 0; j < layers.size(); j++) {
                        layers[j].index = j;
                    }
                }
            }
            ImGui::EndDragDropTarget();
        }

        ImGui::PopID();
        ImGui::Separator();
    }

    ImGui::Separator();
    ImGui::Text("Layer Info");
    ImGui::Text("Anzahl Objekte in ausgewählter Ebene: %d",
        std::count_if(objeckteListe.begin(), objeckteListe.end(),
            [&](const Objeckte& obj) { return obj.layer == selectedLayerIndex; }));
}

void zeigeErweiterteEigenschaften() {
    ImGui::Text("Erweiterte Objekt Eigenschaften");
    ImGui::Separator();

    if (selectedObjectIndex >= 0 && selectedObjectIndex < (int)objeckteListe.size()) {
        Objeckte& obj = objeckteListe[selectedObjectIndex];

        // Basic info
        ImGui::Text("Name: %s", obj.name.c_str());
        ImGui::Text("Typ: %s",
            obj.form == Rechteck ? "Rechteck" :
            obj.form == Kreis ? "Kreis" : "Dreieck");

        ImGui::Separator();

        // Layer selection
        ImGui::Text("Ebene:");
        ImGui::SameLine();

        std::string layerPreview = "Keine";
        if (obj.layer >= 0 && obj.layer < (int)layers.size()) {
            layerPreview = layers[obj.layer].name;
        }

        if (ImGui::BeginCombo("##layer", layerPreview.c_str())) {
            for (size_t i = 0; i < layers.size(); i++) {
                bool isSelected = (obj.layer == (int)i);
                if (ImGui::Selectable(layers[i].name.c_str(), isSelected)) {
                    obj.layer = (int)i;
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }

        ImGui::Separator();

        // Tags
        static char tagBuffer[256];
        strcpy_s(tagBuffer, obj.tag.c_str());
        if (ImGui::InputText("Tag", tagBuffer, sizeof(tagBuffer))) {
            obj.tag = tagBuffer;
        }

        ImGui::Separator();

        // Transform
        ImGui::Text("Transform");
        ImGui::DragFloat("X", &obj.PositionX, 1.0f);
        ImGui::DragFloat("Y", &obj.PositionY, 1.0f);

        // Scale (future feature)
        static float scale = 1.0f;
        ImGui::DragFloat("Scale", &scale, 0.1f, 0.1f, 5.0f);

        // Rotation (future feature)
        static float rotation = 0.0f;
        ImGui::DragFloat("Rotation", &rotation, 1.0f, -180.0f, 180.0f);

        ImGui::Separator();

        // Appearance
        ImGui::Text("Appearance");
        float color[4] = {
            obj.farbe.r / 255.0f,
            obj.farbe.g / 255.0f,
            obj.farbe.b / 255.0f,
            obj.farbe.a / 255.0f
        };

        if (ImGui::ColorEdit4("Farbe", color)) {
            obj.farbe.r = (unsigned char)(color[0] * 255);
            obj.farbe.g = (unsigned char)(color[1] * 255);
            obj.farbe.b = (unsigned char)(color[2] * 255);
            obj.farbe.a = (unsigned char)(color[3] * 255);
        }

        ImGui::Separator();

        // Status
        ImGui::Text("Status");
        ImGui::Checkbox("Sichtbar", &obj.visible);
        ImGui::Checkbox("Gesperrt", &obj.locked);

        // Advanced options
        if (ImGui::CollapsingHeader("Erweitert")) {
            ImGui::Text("Objekt ID: %d", selectedObjectIndex);
            ImGui::Text("Layer Index: %d", obj.layer);
            ImGui::Text("Speicheradresse: %p", (void*)&obj);
        }

        // Duplicate button
        ImGui::Separator();
        if (ImGui::Button("Objekt duplizieren")) {
            Objeckte newObj = obj;
            newObj.name = obj.name + "_copy";
            newObj.PositionX += 30;
            newObj.PositionY += 30;
            objeckteListe.push_back(newObj);
        }

        ImGui::SameLine();

        // Delete button
        if (ImGui::Button("Objekt löschen")) {
            objeckteListe.erase(objeckteListe.begin() + selectedObjectIndex);
            selectedObjectIndex = -1;
        }

    }
    else {
        ImGui::Text("Kein Objekt ausgewählt");
        ImGui::Text("Wählen Sie ein Objekt in der Objektliste oder im Canvas aus");
    }
}