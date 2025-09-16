#include "editor.h"

int main(int argc, char** argv) {
	// Initialize the editor application
	EditorApp app;
	if (!app.initialize(argc, argv)) {
		return -1; // Initialization failed
	}
	// Start the main event loop
	return app.run();
}
