#include "pch.h"
#include "Input.h"

namespace Engine {

    void Input::init(android_app *app_) {
        this->app_ = app_;
        pointerDelta = glm::ivec2(0);
    }

    void Input::handleInput() {

        auto *inputBuffer = android_app_swap_input_buffers(app_);
        if (!inputBuffer)
            return;

        Input::handleMotionEvents(inputBuffer);
        Input::handleKeyEvents(inputBuffer);
    }

    void Input::handleMotionEvents(android_input_buffer* inputBuffer){

        // handle motion events (motionEventsCounts can be 0).
        for (auto i = 0; i < inputBuffer->motionEventsCount; i++) {
            auto &motionEvent = inputBuffer->motionEvents[i];
            auto action = motionEvent.action;

            // Find the pointer index, mask and bitshift to turn it into a readable value.
            auto pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK)
                    >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            //aout << "Pointer(s): ";

            // get the x and y position of this event if it is not ACTION_MOVE.
            auto &pointer = motionEvent.pointers[pointerIndex];
            auto x = GameActivityPointerAxes_getX(&pointer);
            auto y = GameActivityPointerAxes_getY(&pointer);

            // determine the action type and process the event accordingly.
            switch (action & AMOTION_EVENT_ACTION_MASK) {
                case AMOTION_EVENT_ACTION_DOWN:
                case AMOTION_EVENT_ACTION_POINTER_DOWN:{

                    pointerPosition = glm::ivec2(x,y);

                    //aout << "(" << pointer.id << ", " << x << ", " << y << ") "
                    //<< "Pointer Down";
                    break;
                }
                case AMOTION_EVENT_ACTION_CANCEL:
                    // treat the CANCEL as an UP event: doing nothing in the app, except
                    // removing the pointer from the cache if pointers are locally saved.
                    // code pass through on purpose.
                case AMOTION_EVENT_ACTION_UP:
                case AMOTION_EVENT_ACTION_POINTER_UP:

                    pointerPosition = glm::ivec2(x,y);
                    pointerDelta = glm::ivec2(0);
                    //aout << "(" << pointer.id << ", " << x << ", " << y << ") "
                    //<< "Pointer Up";
                    break;

                case AMOTION_EVENT_ACTION_MOVE:
                    // There is no pointer index for ACTION_MOVE, only a snapshot of
                    // all active pointers; app needs to cache previous active pointers
                    // to figure out which ones are actually moved.
                    for (auto index = 0; index < motionEvent.pointerCount; index++) {
                        pointer = motionEvent.pointers[index];
                        x = GameActivityPointerAxes_getX(&pointer);
                        y = GameActivityPointerAxes_getY(&pointer);

                        glm::ivec2 newPosition = glm::ivec2(x,y);
                        pointerDelta = newPosition - pointerPosition;
                        pointerPosition = newPosition;

                        //aout << "(" << pointer.id << ", " << x << ", " << y << ")";

                        //if (index != (motionEvent.pointerCount - 1)) aout << ",";
                        //aout << " ";
                    }
                    // << "Pointer Move";
                    break;
                default:
                    //aout << "Unknown MotionEvent Action: " << action;
            }
            //aout << std::endl;
        }
        // clear the motion input count in this buffer for main thread to re-use.
        android_app_clear_motion_events(inputBuffer);
    }

    void Input::handleKeyEvents(android_input_buffer* inputBuffer){

        // handle input key events.
        for (auto i = 0; i < inputBuffer->keyEventsCount; i++) {
            auto &keyEvent = inputBuffer->keyEvents[i];
            //aout << "Key: " << keyEvent.keyCode <<" ";
            switch (keyEvent.action) {
                case AKEY_EVENT_ACTION_DOWN:
                    //aout << "Key Down";
                    break;
                case AKEY_EVENT_ACTION_UP:
                    //aout << "Key Up";
                    break;
                case AKEY_EVENT_ACTION_MULTIPLE:
                    // Deprecated since Android API level 29.
                    //aout << "Multiple Key Actions";
                    break;
                default:
                    //aout << "Unknown KeyEvent Action: " << keyEvent.action;
            }
            //aout << std::endl;
        }
        // clear the key input count too.
        android_app_clear_key_events(inputBuffer);
    }

}