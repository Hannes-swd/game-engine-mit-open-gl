#include "anzeigefenster.h"
#include "objeckte.h"
#include <cmath>

static float zoom = 1.0f;
static ImVec2 offset = { 0, 0 };

void zeichneObjecktImRenderTarget(const Objeckte& obj, float scale, ImVec2 offset, ImVec2 canvasPos, float canvasWidth, float canvasHeight) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    float x = (obj.PositionX + offset.x) * scale;
    float y = (obj.PositionY + offset.y) * scale;
    x += canvasPos.x;
    y += canvasPos.y;

    // Check if object is visible
    if (x + 50 * scale < canvasPos.x || x > canvasPos.x + canvasWidth ||
        y + 50 * scale < canvasPos.y || y > canvasPos.y + canvasHeight) {
        return;
    }

    ColorRGBA farbe = obj.farbe;
    ImU32 imFarbe = IM_COL32(farbe.r, farbe.g, farbe.b, farbe.a);
    float groesse = 50 * scale;

    switch (obj.form) {
    case Rechteck: {
        drawList->AddRectFilled(
            ImVec2(x, y),
            ImVec2(x + groesse, y + groesse),
            imFarbe
        );

        if (selectedObjectIndex >= 0 &&
            selectedObjectIndex < (int)objeckteListe.size() &&
            objeckteListe[selectedObjectIndex].name == obj.name) {
            drawList->AddRect(
                ImVec2(x, y),
                ImVec2(x + groesse, y + groesse),
                IM_COL32(255, 255, 0, 255),
                0.0f, 0, 3.0f
            );
        }
        break;
    }
    case Kreis: {
        float radius = 25 * scale;
        drawList->AddCircleFilled(
            ImVec2(x + radius, y + radius),
            radius,
            imFarbe
        );

        if (selectedObjectIndex >= 0 &&
            selectedObjectIndex < (int)objeckteListe.size() &&
            objeckteListe[selectedObjectIndex].name == obj.name) {
            drawList->AddCircle(
                ImVec2(x + radius, y + radius),
                radius,
                IM_COL32(255, 255, 0, 255),
                0, 3.0f
            );
        }
        break;
    }
    case Dreieck: {
        float halbeBreite = 25 * scale;
        float hoehe = 50 * scale;
        ImVec2 p1(x + halbeBreite, y);
        ImVec2 p2(x, y + hoehe);
        ImVec2 p3(x + hoehe, y + hoehe);

        drawList->AddTriangleFilled(p1, p2, p3, imFarbe);

        if (selectedObjectIndex >= 0 &&
            selectedObjectIndex < (int)objeckteListe.size() &&
            objeckteListe[selectedObjectIndex].name == obj.name) {
            drawList->AddTriangle(p1, p2, p3, IM_COL32(255, 255, 0, 255), 3.0f);
        }
        break;
    }
    }
}

void zeigeAnzeigeFenster() {
    ImGui::Begin("anzeigefenster", nullptr,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Zoom controls
    ImGui::SliderFloat("Zoom", &zoom, 0.5f, 3.0f);
    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        zoom = 1.0f;
        offset = ImVec2(0, 0);
    }

    ImGui::Separator();

    ImVec2 canvasPos = ImGui::GetCursorScreenPos();
    ImVec2 canvasSize = ImGui::GetContentRegionAvail();

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(canvasPos,
        ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y),
        IM_COL32(50, 50, 50, 255));

    // Draw grid
    float gridSpacing = 50 * zoom;
    float startX = canvasPos.x + (offset.x * zoom);
    float startY = canvasPos.y + (offset.y * zoom);

    for (float x = fmod(startX, gridSpacing); x < canvasPos.x + canvasSize.x; x += gridSpacing) {
        if (x >= canvasPos.x) {
            drawList->AddLine(ImVec2(x, canvasPos.y),
                ImVec2(x, canvasPos.y + canvasSize.y),
                IM_COL32(100, 100, 100, 100), 1.0f);
        }
    }

    for (float y = fmod(startY, gridSpacing); y < canvasPos.y + canvasSize.y; y += gridSpacing) {
        if (y >= canvasPos.y) {
            drawList->AddLine(ImVec2(canvasPos.x, y),
                ImVec2(canvasPos.x + canvasSize.x, y),
                IM_COL32(100, 100, 100, 100), 1.0f);
        }
    }

    ImGui::SetCursorScreenPos(canvasPos);
    ImGui::InvisibleButton("canvas", canvasSize);

    if (ImGui::IsItemHovered()) {
        // Zoom with mouse wheel
        float scrollDelta = ImGui::GetIO().MouseWheel;
        if (scrollDelta != 0) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 canvasMousePos(mousePos.x - canvasPos.x, mousePos.y - canvasPos.y);

            float oldZoom = zoom;
            zoom *= (scrollDelta > 0) ? 1.1f : 0.9f;
            zoom = std::max(0.5f, std::min(3.0f, zoom));

            if (zoom != oldZoom) {
                offset.x -= canvasMousePos.x / oldZoom - canvasMousePos.x / zoom;
                offset.y -= canvasMousePos.y / oldZoom - canvasMousePos.y / zoom;
            }
        }

        // Pan with right mouse button
        if (ImGui::IsMouseDragging(ImGuiMouseButton_Right)) {
            ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
            offset.x += delta.x / zoom;
            offset.y += delta.y / zoom;
            ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
        }

        // Object selection with left click
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            ImVec2 mousePos = ImGui::GetMousePos();
            ImVec2 canvasMousePos((mousePos.x - canvasPos.x) / zoom - offset.x,
                (mousePos.y - canvasPos.y) / zoom - offset.y);

            selectedObjectIndex = -1;
            for (int i = (int)objeckteListe.size() - 1; i >= 0; i--) {
                const auto& obj = objeckteListe[i];
                float x = obj.PositionX;
                float y = obj.PositionY;
                float groesse = 50;
                float radius = 25;

                bool getroffen = false;
                switch (obj.form) {
                case Rechteck:
                    getroffen = (canvasMousePos.x >= x && canvasMousePos.x <= x + groesse &&
                        canvasMousePos.y >= y && canvasMousePos.y <= y + groesse);
                    break;
                case Kreis: {
                    float dx = canvasMousePos.x - (x + radius);
                    float dy = canvasMousePos.y - (y + radius);
                    getroffen = (dx * dx + dy * dy <= radius * radius);
                    break;
                }
                case Dreieck: {
                    float x1 = x + 25, y1 = y;
                    float x2 = x, y2 = y + 50;
                    float x3 = x + 50, y3 = y + 50;

                    float denom = (y2 - y3) * (x1 - x3) + (x3 - x2) * (y1 - y3);
                    float a = ((y2 - y3) * (canvasMousePos.x - x3) + (x3 - x2) * (canvasMousePos.y - y3)) / denom;
                    float b = ((y3 - y1) * (canvasMousePos.x - x3) + (x1 - x3) * (canvasMousePos.y - y3)) / denom;
                    float c = 1 - a - b;
                    getroffen = (a >= 0 && a <= 1 && b >= 0 && b <= 1 && c >= 0 && c <= 1);
                    break;
                }
                }

                if (getroffen) {
                    selectedObjectIndex = i;
                    break;
                }
            }
        }
    }

    // Draw objects in canvas
    ImGui::SetCursorScreenPos(canvasPos);
    for (const auto& obj : objeckteListe) {
        zeichneObjecktImRenderTarget(obj, zoom, offset, canvasPos, canvasSize.x, canvasSize.y);
    }

    ImGui::End();
}