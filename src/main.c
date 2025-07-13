#include "main.h"

SimulationState *sim_state;

int main() {
    sim_state = simulation_init();
    RenderContext *context = init_renderer();

    if (!sim_state || !context) {
        printf("Failed to initialize simulation or renderer\n");
        return 1;
    }

    while (sim_state->is_running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handle_input(&event);
        }

        if (sim_state->should_reset) {
            simulation_cleanup();
            next_pin_id = 0;
            sim_state = simulation_init();
            if (!sim_state) {
                printf("Failed to reinitialize simulation\n");
                break;
            }
            continue;
        }

        simulation_update();
        render(context);
        SDL_Delay(FRAME_DELAY);
    }

    simulation_cleanup();
    cleanup_renderer(context);
    return 0;
}
