#pragma once

void renderer_lighting_disable();
void renderer_lighting_enable();
float *renderer_lighting_set_color_buffer_d(double r, double g, double b, double a);
float *renderer_lighting_set_color_buffer(float r, float g, float b, float a);