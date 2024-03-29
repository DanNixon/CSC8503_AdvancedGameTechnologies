// clang-format off
#include <winsock2.h>
#include <enet\enet.h>
#include <nclgl\Window.h>
#include <ncltech\NCLDebug.h>
#include <ncltech\PerfTimer.h>
#include <ncltech\PhysicsEngine.h>
#include <ncltech\SceneManager.h>
// clang-format on

#include "CourseworkScene.h"

const Vector4 status_colour = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
const Vector4 status_colour_header = Vector4(0.8f, 0.9f, 1.0f, 1.0f);

bool show_perf_metrics = false;
PerfTimer timer_total, timer_physics, timer_update, timer_render;

/**
 * @brief Releases all global components and memory.
 * @param error Flag indicating an exit due to error
 * @param reason Reason string
 *
 * Optionally prints out an error message and stalls the runtime if requested.
 */
void Quit(bool error = false, const string &reason = "")
{
  // Release Singletons
  SceneManager::Release();
  PhysicsEngine::Release();
  Window::Destroy();

  // Show console reason before exit
  if (error)
  {
    std::cerr << reason << std::endl;
    system("PAUSE");
    exit(-1);
  }
}

/**
 * @brief Generates all program wide components and enqueues all scenes for the SceneManager to display.
 */
void Initialize()
{
  // Init ENet
  if (enet_initialize() != 0)
    Quit(true, "An error occurred while initializing ENet!");

  // Initialise the Window
  if (!Window::Initialise("Game Technologies", 1280, 800, false))
    Quit(true, "Window failed to initialise!");

  // Initialise the PhysicsEngine
  PhysicsEngine::Instance();

  // Initialize Renderer
  SceneManager::Instance()->InitializeOGLContext(Window::GetWindow());
  if (!SceneManager::Instance()->HasInitialised())
    Quit(true, "Renderer failed to initialise!");

  // Enqueue All Scenes
  SceneManager::Instance()->EnqueueScene(new CourseworkScene());
}

/**
 * @brief Prints a list of status entries to the top left hand corner of the screen each frame.
 */
void PrintStatusEntries()
{
  // Print Current Scene Name
  NCLDebug::AddStatusEntry(status_colour_header, "%s", SceneManager::Instance()->GetCurrentScene()->GetSceneName().c_str());
  NCLDebug::AddStatusEntry(status_colour, "   \x01 R to reload scene");

  // Print Performance Timers
  NCLDebug::AddStatusEntry(status_colour, "   FPS: %5.2f  (Press G for %s info)", 1000.f / timer_total.GetAvg(),
                           show_perf_metrics ? "less" : "more");

  if (show_perf_metrics)
  {
    timer_total.PrintOutputToStatusEntry(status_colour, "      Total Time     :");
    timer_update.PrintOutputToStatusEntry(status_colour, "      Scene Update   :");
    timer_physics.PrintOutputToStatusEntry(status_colour, "      Physics Update :");
    timer_render.PrintOutputToStatusEntry(status_colour, "      Render Scene   :");
  }

  NCLDebug::AddStatusEntry(status_colour, "");
}

/**
 * @brief Handles all program wide keyboard inputs for things such toggling the physics engine.
 */
void HandleKeyboardInputs()
{
  if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_P))
    PhysicsEngine::Instance()->SetPaused(!PhysicsEngine::Instance()->IsPaused());

  if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_V))
    SceneManager::Instance()->SetVsyncEnabled(!SceneManager::Instance()->GetVsyncEnabled());

  if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_R))
  {
    uint sceneIdx = SceneManager::Instance()->GetCurrentSceneIndex();
    SceneManager::Instance()->JumpToScene(sceneIdx);
  }

  if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_G))
    show_perf_metrics = !show_perf_metrics;
}

/**
 * @brief Entry point.
 * @return Exit code
 */
int main()
{
  // Initialize our Window, Physics, Scenes etc
  Initialize();

  Window::GetWindow().GetTimer()->GetTimedMS();

  // Create main game-loop
  while (Window::GetWindow().UpdateWindow() && !SceneManager::Instance()->ExitFlag())
  {
    // Start Timing
    float dt = Window::GetWindow().GetTimer()->GetTimedMS() * 0.001f;
    timer_total.BeginTimingSection();

    // Print Status Entries
    PrintStatusEntries();

    // Handle Keyboard Inputs
    HandleKeyboardInputs();

    // Update Performance Timers (Show results every second)
    timer_total.UpdateRealElapsedTime(dt);
    timer_physics.UpdateRealElapsedTime(dt);
    timer_update.UpdateRealElapsedTime(dt);
    timer_render.UpdateRealElapsedTime(dt);

    // Update Scene
    timer_update.BeginTimingSection();
    SceneManager::Instance()->UpdateScene(dt);
    timer_update.EndTimingSection();

    // Update Physics
    timer_physics.BeginTimingSection();
    PhysicsEngine::Instance()->Update(dt);
    timer_physics.EndTimingSection();

    // Render Scene
    timer_render.BeginTimingSection();
    SceneManager::Instance()->RenderScene();
    {
      // Forces synchronisation if vsync is disabled
      // - This is solely to allow accurate estimation of render time
      // - We should NEVER normally lock our render or game loop!
      glClientWaitSync(glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, NULL), GL_SYNC_FLUSH_COMMANDS_BIT, 1000000);
    }
    timer_render.EndTimingSection();

    // Finish Timing
    timer_total.EndTimingSection();

    // Let other programs on the computer have some CPU time
    Sleep(0);
  }

  // Cleanup
  Quit();

  return 0;
}
