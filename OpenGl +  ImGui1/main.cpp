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
                if (ImGui::MenuItem("Rückgängig", "Ctrl+Z")) {}
                if (ImGui::MenuItem("Wiederholen", "Ctrl+Y")) {}
                ImGui::Separator();
                if (ImGui::MenuItem("Ausschneiden", "Ctrl+X")) {}
                if (ImGui::MenuItem("Kopieren", "Ctrl+C")) {}
                if (ImGui::MenuItem("Einfügen", "Ctrl+V")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Ansicht")) {
                ImGui::MenuItem("Demo Fenster", nullptr, &showDemoWindow);
                ImGui::MenuItem("Style Editor", nullptr, &showStyleEditor);
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Objekte")) {
                if (ImGui::MenuItem("Rechteck hinzufügen")) {
                    addObjeckt(Rechteck);
                }
                if (ImGui::MenuItem("Kreis hinzufügen")) {
                    addObjeckt(Kreis);
                }
                if (ImGui::MenuItem("Dreieck hinzufügen")) {
                    addObjeckt(Dreieck);
                }
                ImGui::Separator();
                if (ImGui::MenuItem("Alle löschen")) {
                    objeckteListe.clear();
                    selectedObjectIndex = -1;
                }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Hilfe")) {
                if (ImGui::MenuItem("über")) {
                    // Show about dialog
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }

        // Main workspace with tabs
        ImGui::Begin("Workspace", nullptr, ImGuiWindowFlags_MenuBar);

        if (ImGui::BeginTabBar("MainTabs")) {
            // Tab 1: Scene Editor
            if (ImGui::BeginTabItem("Scene Editor")) {
                // Split view within tab
                static bool showLeftPanel = true;
                static bool showRightPanel = true;

                if (showLeftPanel) {
                    ImGui::BeginChild("LeftPanel", ImVec2(280, 0), true);
                    zeigeObjektBrowser();
                    ImGui::EndChild();
                    ImGui::SameLine();
                }

                // Center area - Canvas
                ImGui::BeginChild("CanvasArea", ImVec2(0, 0), true);
                zeigeAnzeigeFenster();
                ImGui::EndChild();

                if (showRightPanel) {
                    ImGui::SameLine();
                    ImGui::BeginChild("RightPanel", ImVec2(320, 0), true);
                    zeichneEditFenster();
                    ImGui::EndChild();
                }

                ImGui::EndTabItem();
            }

            // Tab 2: Object List
            if (ImGui::BeginTabItem("Object List")) {
                zeigeErweiterteObjektListe();
                ImGui::EndTabItem();
            }

            // Tab 3: Layer Editor
            if (ImGui::BeginTabItem("Layer Editor")) {
                zeigeLayerEditor();
                ImGui::EndTabItem();
            }

            // Tab 4: Properties
            if (ImGui::BeginTabItem("Properties")) {
                zeigeErweiterteEigenschaften();
                ImGui::EndTabItem();
            }

            // Tab 5: Console
            if (ImGui::BeginTabItem("Console")) {
                zeigeConsole();
                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();

        // Additional floating windows
        if (showDemoWindow) {
            ImGui::ShowDemoWindow(&showDemoWindow);
        }

        if (showStyleEditor) {
            ImGui::Begin("Style Editor", &showStyleEditor);
            ImGui::ShowStyleEditor();
            ImGui::End();
        }

        // Status bar
        if (ImGui::BeginMainMenuBar()) {
            ImGui::Text("Status: Bereit | Objekte: %zu | Ausgewählt: %s",
                objeckteListe.size(),
                selectedObjectIndex >= 0 ? objeckteListe[selectedObjectIndex].name.c_str() : "Keins");
            ImGui::EndMainMenuBar();
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