#define _CRT_SECURE_NO_WARNINGS
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "objeckte.h"
#include "anzeigefenster.h"
#include "edit.h"
#include "layers.h"

// OpenGL context and window
GLFWwindow* window = nullptr;

// Application state
static int currentTab = 0;
static bool showDemoWindow = false;
static bool showStyleEditor = false;

int main()
{
    // Initialize GLFW
    if (!glfwInit()) {
        return -1;
    }

    // Configure GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    window = glfwCreateWindow(1600, 900, "Game Engine - Professional Editor", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Initialize OpenGL
    glClearColor(0.2f, 0.2f, 0.25f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Viewports

    // Setup ImGui style
    ImGui::StyleColorsDark();

    // Custom style for professional look
    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 4.0f;
    style.FrameRounding = 2.0f;
    style.GrabRounding = 2.0f;
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.12f, 1.0f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.18f, 1.0f);
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.12f, 0.12f, 0.14f, 1.0f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_Header] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.35f, 0.35f, 0.4f, 1.0f);
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.25f, 1.0f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.35f, 0.35f, 0.4f, 1.0f);

    // Setup platform/renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize layers
    initLayers();

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Setup docking - Fixed version
        ImGuiID dockspace_id = ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

        // Draw main menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Datei")) {
                if (ImGui::MenuItem("Neues Projekt", "Ctrl+N")) {
                    // New project logic
                    objeckteListe.clear();
                    selectedObjectIndex = -1;
                }
                if (ImGui::MenuItem("Projekt laden", "Ctrl+O")) {
                    // Load project logic
                }
                if (ImGui::MenuItem("Projekt speichern", "Ctrl+S")) {
                    // Save project logic
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Beenden", "Alt+F4")) {
                    glfwSetWindowShouldClose(window, true);
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Bearbeiten")) {
                if (ImGui::MenuItem("Rueckgaengig", "Ctrl+Z")) {}
                if (ImGui::MenuItem("Wiederholen", "Ctrl+Y")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Ausschneiden", "Ctrl+X")) {}
                if (ImGui::MenuItem("Kopieren", "Ctrl+C")) {}
                if (ImGui::MenuItem("Einfuegen", "Ctrl+V")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Ansicht")) {
                ImGui::MenuItem("Demo Fenster", nullptr, &showDemoWindow);
                ImGui::MenuItem("Style Editor", nullptr, &showStyleEditor);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Objekte")) {
                if (ImGui::MenuItem("Rechteck hinzufuegen")) {
                    addObjeckt(Rechteck);
                }
                if (ImGui::MenuItem("Kreis hinzufuegen")) {
                    addObjeckt(Kreis);
                }
                if (ImGui::MenuItem("Dreieck hinzufuegen")) {
                    addObjeckt(Dreieck);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Alle loeschen")) {
                    objeckteListe.clear();
                    selectedObjectIndex = -1;
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Hilfe")) {
                if (ImGui::MenuItem("ueber")) {
                    // Show about dialog
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // === 3 Fenster ===

        // 1. Objekte & Layer
        ImGui::Begin("Objekte & Ebenen");

        ImGui::Text("Objekte");
        ImGui::Separator();

        if (ImGui::Button("+ Rechteck")) addObjeckt(Rechteck);
        ImGui::SameLine();
        if (ImGui::Button("+ Kreis")) addObjeckt(Kreis);
        ImGui::SameLine();
        if (ImGui::Button("+ Dreieck")) addObjeckt(Dreieck);

        ImGui::Separator();

        // Objektliste
        for (int i = 0; i < (int)objeckteListe.size(); i++) {
            Objeckte& obj = objeckteListe[i];
            ImGui::PushID(i);

            ImGui::Checkbox("##vis", &obj.visible);
            ImGui::SameLine();

            bool isSelected = (selectedObjectIndex == i);
            const char* formName = obj.form == Rechteck ? "[R]" : obj.form == Kreis ? "[K]" : "[D]";
            char label[128];
            snprintf(label, sizeof(label), "%s %s", formName, obj.name.c_str());

            if (ImGui::Selectable(label, isSelected)) {
                selectedObjectIndex = i;
            }

            if (isSelected) {
                ImGui::SameLine();
                ImGui::SetNextItemWidth(90);
                std::string layerPreview = (obj.layer >= 0 && obj.layer < (int)layers.size())
                    ? layers[obj.layer].name : "?";
                if (ImGui::BeginCombo("##lyr", layerPreview.c_str())) {
                    for (int l = 0; l < (int)layers.size(); l++) {
                        if (ImGui::Selectable(layers[l].name.c_str(), obj.layer == l))
                            obj.layer = l;
                    }
                    ImGui::EndCombo();
                }
            }

            ImGui::SameLine();
            if (ImGui::SmallButton("X")) {
                objeckteListe.erase(objeckteListe.begin() + i);
                if (selectedObjectIndex >= (int)objeckteListe.size())
                    selectedObjectIndex = (int)objeckteListe.size() - 1;
                ImGui::PopID();
                goto endObjList;
            }

            ImGui::PopID();
        }
    endObjList:;

        ImGui::Separator();
        ImGui::Text("Ebenen");
        ImGui::Separator();

        if (ImGui::Button("+ Ebene")) addLayer();
        ImGui::SameLine();
        if (ImGui::Button("- Ebene") && layers.size() > 1) {
            removeLayer(selectedLayerIndex);
            if (selectedLayerIndex >= (int)layers.size())
                selectedLayerIndex = (int)layers.size() - 1;
        }

        for (int i = 0; i < (int)layers.size(); i++) {
            Layer& layer = layers[i];
            ImGui::PushID(1000 + i);
            ImGui::Checkbox("##lvis", &layer.visible);
            ImGui::SameLine();
            bool isSel = (selectedLayerIndex == i);
            if (ImGui::Selectable(layer.name.c_str(), isSel))
                selectedLayerIndex = i;
            ImGui::PopID();
        }

        ImGui::End();

        // 2. Render / Canvas
        zeigeAnzeigeFenster();

        // 3. Einstellungen
        ImGui::Begin("Einstellungen");

        if (selectedObjectIndex >= 0 && selectedObjectIndex < (int)objeckteListe.size()) {
            Objeckte& obj = objeckteListe[selectedObjectIndex];

            ImGui::Text("Objekt: %s", obj.name.c_str());
            ImGui::Separator();

            // Name - using char array with safe string handling
            static char nameBuf[256] = "";
            strncpy_s(nameBuf, sizeof(nameBuf), obj.name.c_str(), _TRUNCATE);
            if (ImGui::InputText("Name", nameBuf, sizeof(nameBuf))) {
                obj.name = nameBuf;
            }

            // Position
            ImGui::DragFloat("Position X", &obj.PositionX, 1.0f);
            ImGui::DragFloat("Position Y", &obj.PositionY, 1.0f);

            ImGui::Separator();

            // Groesse
            ImGui::Text("Groesse");
            ImGui::DragFloat("Breite", &obj.breite, 1.0f, 1.0f, 2000.0f);
            ImGui::DragFloat("Hoehe", &obj.hoe, 1.0f, 1.0f, 2000.0f);

            ImGui::Separator();

            // Farbe
            float col[4] = {
                obj.farbe.r / 255.0f,
                obj.farbe.g / 255.0f,
                obj.farbe.b / 255.0f,
                obj.farbe.a / 255.0f
            };
            if (ImGui::ColorEdit4("Farbe", col)) {
                obj.farbe.r = (unsigned char)(col[0] * 255);
                obj.farbe.g = (unsigned char)(col[1] * 255);
                obj.farbe.b = (unsigned char)(col[2] * 255);
                obj.farbe.a = (unsigned char)(col[3] * 255);
            }

            ImGui::Separator();

            // Sichtbar / Gesperrt
            ImGui::Checkbox("Sichtbar", &obj.visible);
            ImGui::Checkbox("Gesperrt", &obj.locked);

            ImGui::Separator();

            // Ebene
            ImGui::Text("Ebene:");
            ImGui::SameLine();
            std::string layerPrev = (obj.layer >= 0 && obj.layer < (int)layers.size())
                ? layers[obj.layer].name : "?";
            if (ImGui::BeginCombo("##einst_lyr", layerPrev.c_str())) {
                for (int l = 0; l < (int)layers.size(); l++) {
                    if (ImGui::Selectable(layers[l].name.c_str(), obj.layer == l))
                        obj.layer = l;
                }
                ImGui::EndCombo();
            }

            ImGui::Separator();

            if (ImGui::Button("Duplizieren")) {
                Objeckte copy = obj;
                copy.name = obj.name + "_copy";
                copy.PositionX += 30;
                copy.PositionY += 30;
                objeckteListe.push_back(copy);
            }
            ImGui::SameLine();
            if (ImGui::Button("Loeschen")) {
                objeckteListe.erase(objeckteListe.begin() + selectedObjectIndex);
                selectedObjectIndex = -1;
            }
        }
        else {
            ImGui::TextDisabled("Kein Objekt ausgewaehlt");
        }

        ImGui::End();

        if (showDemoWindow) ImGui::ShowDemoWindow(&showDemoWindow);
        if (showStyleEditor) {
            ImGui::Begin("Style Editor", &showStyleEditor);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }

        // Render ImGui

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}