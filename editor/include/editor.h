#include "core/app.h"

class EditorApp : public zaphod::App {
protected:
    bool onInitialize() override {
        // Editor-specific initialization
        return true;
    }

    void onUpdate(float deltaTime) override {
        // Editor update logic
    }

    void onRender() override {
        // Editor rendering
    }

    void onShutdown() override {
        // Editor cleanup
    }
};