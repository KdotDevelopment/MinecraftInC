#include <renderer/entity/renderer_entity.h>

#include <string.h>

renderer_entity_t renderer_entity_create() {
    renderer_entity_t renderer = { 0 };
    memset(&renderer, 0, sizeof(renderer_entity_t));

    renderer.shadow_size = 0.0;
    renderer.shadow_opaque = 1.0;

    renderer.render = renderer_entity_render;
    renderer.render_shadow = renderer_entity_render_shadow;
    
    return renderer;
}

void renderer_entity_render(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float rot_x, float partial_tick) {
    return;
}

void renderer_entity_render_shadow(renderer_entity_t *renderer, entity_t *entity, double x, double y, double z, float partial_tick) {
    if(renderer->shadow_size > 0) {
        
    }
}