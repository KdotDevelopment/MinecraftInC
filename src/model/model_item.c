#include <model/model_item.h>

model_t model_item_create(int texture_id) {
    model_t model = model_create();
    model.type = MODEL_ITEM;

    float a = -2;
    float b = -2;
    float c = -2;

    model.head = cube_create(0, 0);

    model.head.vertices[0] = (vertex_t){ (vec3_t){ c, b, a }, 0, 0 };
    model.head.vertices[1] = (vertex_t){ (vec3_t){ 2, b, a }, 0, 0 };
    model.head.vertices[2] = (vertex_t){ (vec3_t){ 2, 2, a }, 0, 8 };
    model.head.vertices[3] = (vertex_t){ (vec3_t){ c, 2, a }, 8, 0 };
    model.head.vertices[4] = (vertex_t){ (vec3_t){ c, b, 2 }, 0, 0 };
    model.head.vertices[5] = (vertex_t){ (vec3_t){ 2, b, 2 }, 0, 8 };
    model.head.vertices[6] = (vertex_t){ (vec3_t){ 2, 2, 2 }, 8, 8 };
    model.head.vertices[7] = (vertex_t){ (vec3_t){ c, 2, 2 }, 8, 0 };

    a = 0.25;
    b = 0.25;
    float u1 = ((float)(texture_id % 16) + (1.0 - a)) / 16.0;
    float v1 = ((float)(texture_id / 16) + (1.0 - b)) / 16.0;
    float u2 = ((float)(texture_id % 16) + a) / 16.0;
    float v2 = ((float)(texture_id / 16) + b) / 16.0;

    model.head.polygons[0] = polygon_create( (vertex_t[]) { model.head.vertices[5], model.head.vertices[1], model.head.vertices[2], model.head.vertices[6] }, u1, v1, u2, v2);
    model.head.polygons[1] = polygon_create( (vertex_t[]) { model.head.vertices[0], model.head.vertices[4], model.head.vertices[7], model.head.vertices[3] }, u1, v1, u2, v2);
    model.head.polygons[2] = polygon_create( (vertex_t[]) { model.head.vertices[5], model.head.vertices[4], model.head.vertices[0], model.head.vertices[1] }, u1, v1, u2, v2);
    model.head.polygons[3] = polygon_create( (vertex_t[]) { model.head.vertices[2], model.head.vertices[3], model.head.vertices[7], model.head.vertices[6] }, u1, v1, u2, v2);
    model.head.polygons[4] = polygon_create( (vertex_t[]) { model.head.vertices[1], model.head.vertices[0], model.head.vertices[3], model.head.vertices[2] }, u1, v1, u2, v2);
    model.head.polygons[5] = polygon_create( (vertex_t[]) { model.head.vertices[4], model.head.vertices[5], model.head.vertices[6], model.head.vertices[7] }, u1, v1, u2, v2);

    return model;
}

void model_item_render(struct model_s *model) {
    cube_render(&model->head, 0.0625);
}