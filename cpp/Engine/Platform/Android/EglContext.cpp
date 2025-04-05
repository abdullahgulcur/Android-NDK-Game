#include "pch.h"
#include "EglContext.h"
#include <game-activity/native_app_glue/android_native_app_glue.h>

namespace Engine {

    EglContext::EglContext() {

        display_ = EGL_NO_DISPLAY;
        surface_ = EGL_NO_SURFACE;
        context_ = EGL_NO_CONTEXT;
        width_ = 0;
        height_ = 0;
    }

    void EglContext::init(android_app *app_) {

        // Choose your render attributes
        constexpr EGLint attribs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, 24,
                EGL_NONE
        };

        // The default display is probably what you want on Android
        auto display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
        eglInitialize(display, nullptr, nullptr);

        // figure out how many configs there are
        EGLint numConfigs;
        eglChooseConfig(display, attribs, nullptr, 0, &numConfigs);

        // get the list of configurations
        std::unique_ptr<EGLConfig[]> supportedConfigs(new EGLConfig[numConfigs]);
        eglChooseConfig(display, attribs, supportedConfigs.get(), numConfigs, &numConfigs);

        // Find a config we like.
        // Could likely just grab the first if we don't care about anything else in the config.
        // Otherwise hook in your own heuristic
        auto config = *std::find_if(
                supportedConfigs.get(),
                supportedConfigs.get() + numConfigs,
                [&display](const EGLConfig &config) {
                    EGLint red, green, blue, depth;
                    if (eglGetConfigAttrib(display, config, EGL_RED_SIZE, &red)
                        && eglGetConfigAttrib(display, config, EGL_GREEN_SIZE, &green)
                        && eglGetConfigAttrib(display, config, EGL_BLUE_SIZE, &blue)
                        && eglGetConfigAttrib(display, config, EGL_DEPTH_SIZE, &depth)) {

                        //aout << "Found config with " << red << ", " << green << ", " << blue << ", "
                        //     << depth << std::endl;
                        return red == 8 && green == 8 && blue == 8 && depth == 24;
                    }
                    return false;
                });

        //aout << "Found " << numConfigs << " configs" << std::endl;
        //aout << "Chose " << config << std::endl;

        // create the proper window surface
        EGLint format;
        eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
        EGLSurface surface = eglCreateWindowSurface(display, config, app_->window, nullptr);

        // Create a GLES 3 context
        EGLint contextAttribs[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_CONTEXT_MINOR_VERSION, 2, EGL_NONE};
        EGLContext context = eglCreateContext(display, config, nullptr, contextAttribs);

        // get some window metrics
        auto madeCurrent = eglMakeCurrent(display, surface, surface, context);
        //assert(madeCurrent);

        display_ = display;
        surface_ = surface;
        context_ = context;

        // make width and height invalid so it gets updated the first frame in @a updateRenderArea()
        width_ = -1;
        height_ = -1;
    }

    void EglContext::updateRenderArea() {

        EGLint width;
        eglQuerySurface(display_, surface_, EGL_WIDTH, &width);

        EGLint height;
        eglQuerySurface(display_, surface_, EGL_HEIGHT, &height);

        if (width != width_ || height != height_) {
            width_ = width;
            height_ = height;
        }
    }

    void EglContext::swapBuffers() {
        eglSwapBuffers(display_, surface_);
    }

}