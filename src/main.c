#include "main.h"

SimulationState *sim_state;

int main() {
    sim_state = simulation_init();
    RenderContext *context = init_renderer();
    PerformanceMetrics *pm = init_performance_monitoring();

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

        pm_start_frame_time(pm);
        simulation_update();
        pm_end_simulation_time(pm);
        pm_start_render_time(pm);
        render(context);
        pm_end_render_time(pm);
        pm_end_frame_time(pm);
        print_performance_metrics(pm, sim_state);
        // print_memory_breakdown(sim_state);
        SDL_Delay(FRAME_DELAY);
    }

    simulation_cleanup();
    cleanup_renderer(context);
    return 0;
}
