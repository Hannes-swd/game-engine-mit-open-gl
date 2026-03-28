#include "anzeigefenster.h"
#include "objeckte.h"
#include <cmath>

static float zoom = 1.0f;
static ImVec2 offset = { 0, 0 };

void zeichneObjecktImRenderTarget(const Objeckte& obj, float scale, ImVec2 offset, ImVec2 canvasPos, float canvasWidth, float canvasHeight) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    float x = (obj.PositionX + offset.x) * scale + canvasPos.x;
    float y = (obj.PositionY + offset.y) * scale + canvasPos.y;

    float w = obj.breite * scale;
    float h = obj.hoe * scale;

    // Culling
    if (x + w < canvasPos.x || x > canvasPos.x + canvasWidth ||
        y + h < canvasPos.y || y > canvasPos.y + canvasHeight) {
        return;
    }

    ImU32 imFarbe = IM_COL32(obj.farbe.r, obj.farbe.g, obj.farbe.b, obj.farbe.a);

    bool isSelected = (selectedObjectIndex >= 0 &&
        selectedObjectIndex < (int)objeckteListe.size() &&
        objeckteListe[selectedObjectIndex].name == obj.name);

    switch (obj.form) {
    case Rechteck: {
        drawList->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), imFarbe);
        if (isSelected)
            drawList->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), IM_COL32(255, 255, 0, 255), 0.0f, 0, 3.0f);
        break;
    }
    case Kreis: {
        float rx = w / 2.0f;
        float ry = h / 2.0f;
        float cx = x + rx;
        float cy = y + ry;
        // ImGui circles use single radius - use average for ellipse approximation
        float radius = (rx + ry) / 2.0f;
        drawList->AddCircleFilled(ImVec2(cx, cy), radius, imFarbe);
        if (isSelected)
            drawList->AddCircle(ImVec2(cx, cy), radius, IM_COL32(255, 255, 0, 255), 0, 3.0f);
        break;
    }
    case Dreieck: {
        ImVec2 p1(x + w / 2.0f, y);
        ImVec2 p2(x, y + h);
        ImVec2 p3(x + w, y + h);
        drawList->AddTriangleFilled(p1, p2, p3, imFarbe);
        if (isSelected)
            drawList->AddTriangle(p1, p2, p3, IM_COL32(255, 255, 0, 255), 3.0f);
        break;
    }
    }
}

void zeigeAnzeigeFenster() {
    ImGui::Begin("anzeigefenster", nullptr,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

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
        if (x >= canvasPos.x)
            drawList->AddLine(ImVec2(x, canvasPos.y), ImVec2(x, canvasPos.y + canvasSize.y),
                IM_COL32(100, 100, 100, 100), 1.0f);
    }
    for (float y = fmod(startY, gridSpacing); y < canvasPos.y + canvasSize.y; y += gridSpacing) {
        if (y >= canvasPos.y)
            drawList->AddLine(ImVec2(canvasPos.x, y), ImVec2(canvasPos.x + canvasSize.x, y),
                IM_COL32(100, 100, 100, 100), 1.0f);
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
                float w = obj.breite;
                float h = obj.hoe;

                bool getroffen = false;
                switch (obj.form) {
                case Rechteck:
                    getroffen = (canvasMousePos.x >= x && canvasMousePos.x <= x + w &&
                        canvasMousePos.y >= y && canvasMousePos.y <= y + h);
                    break;
                case Kreis: {
                    float rx = w / 2.0f;
                    float ry = h / 2.0f;
                    float dx = canvasMousePos.x - (x + rx);
                    float dy = canvasMousePos.y - (y + ry);
                    // Ellipse hit test
                    getroffen = ((dx * dx) / (rx * rx) + (dy * dy) / (ry * ry) <= 1.0f);
                    break;
                }
                case Dreieck: {
                    float x1 = x + w / 2.0f, y1 = y;
                    float x2 = x, y2 = y + h;
                    float x3 = x + w, y3 = y + h;
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

    // Draw objects
    ImGui::SetCursorScreenPos(canvasPos);
    for (const auto& obj : objeckteListe) {
        if (obj.visible)
            zeichneObjecktImRenderTarget(obj, zoom, offset, canvasPos, canvasSize.x, canvasSize.y);
    }

    ImGui::End();
}