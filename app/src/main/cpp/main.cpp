#include <jni.h>

#include "AndroidOut.h"
#include "TigorEngine.h"

#include "e_camera.h"

#include "primitive_object.h"

#include <game-activity/GameActivity.cpp>
#include <game-text-input/gametextinput.cpp>

PrimitiveObject obj;

Camera2D cam2D;
Camera3D cam3D;

extern "C" {

#include <game-activity/native_app_glue/android_native_app_glue.c>

/*!
 * Enable the motion events you want to handle; not handled events are
 * passed back to OS for further processing. For this example case,
 * only pointer and joystick devices are enabled.
 *
 * @param motionEvent the newly arrived GameActivityMotionEvent.
 * @return true if the event is from a pointer or joystick device,
 *         false for all other input devices.
 */
bool motion_event_filter_func(const GameActivityMotionEvent *motionEvent) {
    auto sourceClass = motionEvent->source & AINPUT_SOURCE_CLASS_MASK;
    return (sourceClass == AINPUT_SOURCE_CLASS_POINTER ||
            sourceClass == AINPUT_SOURCE_CLASS_JOYSTICK);
}

void Init(){
    Camera2DInit(&cam2D);
    Camera3DInit(&cam3D);

    DrawParam dParam;
    memset(&dParam, 0, sizeof(DrawParam));

    PrimitiveObjectInit(&obj,&dParam, TIGOR_PRIMITIVE3D_CUBE, NULL);

    Transform3DSetPosition((GameObject3D_T *)&obj, 0, 0, -5);
}

/*!
 * This the main entry point for a native activity
 */
 float rot = 0;
void android_main(struct android_app *pApp) {
    aout << "Welcome to android_main" << std::endl;

    TEngineInitSystem();
    TEngineSetInitFunc(Init);

    // Can be removed, useful to ensure your code is running
    //aout << "Welcome to android_main" << std::endl;

    // Register an event handler for Android events
    pApp->onAppCmd = TEngineHandleAppCommand;

    // Set input event filters (set it to NULL if the app wants to process all inputs).
    // Note that for key inputs, this example uses the default default_key_filter()
    // implemented in android_native_app_glue.c.
    android_app_set_motion_event_filter(pApp, motion_event_filter_func);

    // This sets up a typical game/event loop. It will run until the app is destroyed.
    do {
        // Process all pending events before running game logic.
        bool done = false;
        while (!done) {
            // 0 is non-blocking.
            int timeout = 0;
            int events;
            android_poll_source *pSource;
            int result = ALooper_pollOnce(timeout, nullptr, &events,
                                          reinterpret_cast<void**>(&pSource));
            switch (result) {
                case ALOOPER_POLL_TIMEOUT:
                    [[clang::fallthrough]];
                case ALOOPER_POLL_WAKE:
                    // No events occurred before the timeout or explicit wake. Stop checking for events.
                    done = true;
                    break;
                case ALOOPER_EVENT_ERROR:
                    aout << "ALooper_pollOnce returned an error" << std::endl;
                    break;
                case ALOOPER_POLL_CALLBACK:
                    break;
                default:
                    if (pSource) {
                        pSource->process(pApp, pSource);
                    }
            }
        }

        // Check if any user data is associated. This is assigned in handle_cmd
        if (pApp->userData) {
            // We know that our user data is a Renderer, so reinterpret cast it. If you change your
            // user data remember to change it here
            //auto *pRenderer = reinterpret_cast<Renderer *>(pApp->userData);

            // Process game input
            //pRenderer->handleInput();

            rot += 0.4f;

            Transform3DSetRotation((GameObject3D_T *)&obj, 0, rot, 0);

            TEngineDraw((GameObject *)&obj);

            // Render a frame
            TEngineRender();
        }
    } while (!pApp->destroyRequested);
}
}