#include "performanceMonitor.h"

PerformanceMetrics *init_performance_monitoring() {
    PerformanceMetrics *pm = (PerformanceMetrics *)malloc(sizeof(PerformanceMetrics));
    if (pm == NULL) {
        return NULL;
    }

    pm->fps = 0.0f;
    pm->frame_time = 0.0f;
    pm->frame_count = 0;
    pm->fps_update_interval = 0.5f;

    pm->max_frame_count = 0;
    pm->accumulated_frame_time = 0.0f;
    pm->accumulated_render_time = 0.0f;
    pm->accumulated_simulation_time = 0.0f;
    pm->timing_frame_count = 0;

    gettimeofday(&pm->start_time, NULL);
    pm->last_frame_time = pm->start_time;
    pm->last_fps_update_sec = pm->start_time.tv_sec;
    pm->last_fps_update_usec = pm->start_time.tv_usec;

    return pm;
}

void pm_start_frame_time(PerformanceMetrics *pm) {
    if (pm == NULL) return;
    gettimeofday(&pm->start_time, NULL);
}

void pm_start_render_time(PerformanceMetrics *pm) {
    if (pm == NULL) return;
    gettimeofday(&pm->render_start_time, NULL);
}

void pm_end_render_time(PerformanceMetrics *pm) {
    if (pm == NULL) return;
    gettimeofday(&pm->end_time, NULL);

    long seconds = pm->end_time.tv_sec - pm->render_start_time.tv_sec;
    long microseconds = pm->end_time.tv_usec - pm->render_start_time.tv_usec;

    pm->accumulated_render_time += (float)(seconds * 1000 + microseconds / 1000.0);
}

void pm_end_simulation_time(PerformanceMetrics *pm) {
    if (pm == NULL) return;
    gettimeofday(&pm->end_time, NULL);

    long seconds = pm->end_time.tv_sec - pm->start_time.tv_sec;
    long microseconds = pm->end_time.tv_usec - pm->start_time.tv_usec;

    pm->accumulated_simulation_time += (float)(seconds * 1000 + microseconds / 1000.0);
}

void pm_end_frame_time(PerformanceMetrics *pm) {
    if (pm == NULL) return;

    gettimeofday(&pm->end_time, NULL);

    long seconds = pm->end_time.tv_sec - pm->start_time.tv_sec;
    long microseconds = pm->end_time.tv_usec - pm->start_time.tv_usec;

    pm->accumulated_frame_time += (float)(seconds * 1000 + microseconds / 1000.0);
    pm->timing_frame_count++;
    pm->frame_count++;

    long total_seconds = pm->end_time.tv_sec - pm->last_fps_update_sec;
    long total_microseconds = pm->end_time.tv_usec - pm->last_fps_update_usec;
    float total_time = (float)(total_seconds + total_microseconds / 1000000.0);

    if (total_time >= pm->fps_update_interval) {
        pm->fps = (float)pm->frame_count / total_time;
        pm->max_frame_count = pm->frame_count;
        pm->frame_count = 0;
        pm->last_fps_update_sec = pm->end_time.tv_sec;
        pm->last_fps_update_usec = pm->end_time.tv_usec;

        if (pm->timing_frame_count > 0) {
            pm->frame_time = pm->accumulated_frame_time / pm->timing_frame_count;
            pm->render_time = pm->accumulated_render_time / pm->timing_frame_count;
            pm->simulation_time = pm->accumulated_simulation_time / pm->timing_frame_count;

            pm->render_time_percentage = (pm->render_time / pm->frame_time) * 100;
            pm->simulation_time_percentage = (pm->simulation_time / pm->frame_time) * 100;

            pm->accumulated_frame_time = 0.0f;
            pm->accumulated_render_time = 0.0f;
            pm->accumulated_simulation_time = 0.0f;
            pm->timing_frame_count = 0;
        }
    }
}

void print_performance_metrics(PerformanceMetrics *pm, SimulationState *sim_state) {
    if (pm == NULL || sim_state == NULL) return;

    printf("=======PERFORMANCE METRICS=======\n");
    if (pm->fps > 30) {
        printf("\033[32mFPS: %.1f\033[0m\n", pm->fps);
    } else if (pm->fps > 15) {
        printf("\033[33mFPS: %.1f\033[0m\n", pm->fps);
    } else {
        printf("\033[31mFPS: %.1f\033[0m\n", pm->fps);
    }
    printf("Frame time: %.5f ms\n", pm->frame_time);
    printf("Frame count: %d\n", pm->max_frame_count);
    printf("Render time: %.5f ms (%.2f%%)\n", pm->render_time, pm->render_time_percentage);
    printf("Simulation time: %.5f ms (%.2f%%)\n", pm->simulation_time, pm->simulation_time_percentage);
}

void print_memory_breakdown(SimulationState *sim_state) {
    if (sim_state == NULL) return;

    printf("// todo: print memory breakdown\n");
}

/*

Performance Metrics Display
  - Number of nodes being rendered
  - Number of connections being rendered
  - Number of nodes being simulated
  - Memory usage (nodes, connections, textures)

Debug Overlays
  - Show render calls per frame
  - Display spatial partitioning info
  - Show culling statistics (objects skipped)

*/
