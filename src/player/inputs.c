#include <player/inputs.h>

inputs_t inputs_create(game_settings_t *settings) {
    inputs_t inputs = { 0 };
    inputs.settings = settings;

    return inputs;
}

void inputs_set_key_state(inputs_t *inputs, uint8_t key, uint8_t state) {
    if(key == inputs->settings->forward_key.key) inputs->key_states[0] = state;
	if(key == inputs->settings->back_key.key) inputs->key_states[1] = state;
	if(key == inputs->settings->left_key.key) inputs->key_states[2] = state;
	if(key == inputs->settings->right_key.key) inputs->key_states[3] = state;
	if(key == inputs->settings->jump_key.key) inputs->key_states[4] = state;
}

void inputs_reset_keys(inputs_t *inputs) {
    for(int i = 0; i < 10; i++) {
        inputs->key_states[i] = 0;
    }
}

void inputs_update_movement(inputs_t *inputs) {
    inputs->x = 0.0;
    inputs->z = 0.0;

    if(inputs->key_states[0]) inputs->z -= 1.0;
    if(inputs->key_states[1]) inputs->z += 1.0;
    if(inputs->key_states[2]) inputs->x -= 1.0;
    if(inputs->key_states[3]) inputs->x += 1.0;
    inputs->jumping = inputs->key_states[4];
}