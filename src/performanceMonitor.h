#ifndef PERFORMANCE_MONITOR_H
#define PERFORMANCE_MONITOR_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "simulation.h"
#include "simulation.h"
#include "node.h"
#include "connection.h"
#include "pin.h"
#include "button.h"
#include "operation.h"

typedef struct PerformanceMetrics {
    struct timeval start_time;
    struct timeval end_time;
    struct timeval last_frame_time;
    struct timeval render_start_time;

    float frame_time;
    float render_time;
    float simulation_time;

    // Accumulated times for averaging
    float accumulated_frame_time;
    float accumulated_render_time;
    float accumulated_simulation_time;
    int timing_frame_count;

    float render_time_percentage;
    float simulation_time_percentage;

    float fps;
    int frame_count;
    int max_frame_count;
    float last_fps_update;
    float fps_update_interval;
    long last_fps_update_sec;
    long last_fps_update_usec;
} PerformanceMetrics;

PerformanceMetrics *init_performance_monitoring();
void pm_start_frame_time(PerformanceMetrics *pm);
void pm_start_render_time(PerformanceMetrics *pm);
void pm_end_frame_time(PerformanceMetrics *pm);
void pm_end_render_time(PerformanceMetrics *pm);
void pm_end_simulation_time(PerformanceMetrics *pm);
void print_performance_metrics(PerformanceMetrics *pm, SimulationState *sim_state);
void print_memory_breakdown(SimulationState *sim_state);

#endif // PERFORMANCE_MONITOR_H
