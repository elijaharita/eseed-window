#include <eseed/window/window.hpp>

int main() {
    esd::window::Window window("A Window", { 1366, 768 });

    while (!window.isCloseRequested()) {
        window.poll();
    }
}