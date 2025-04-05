#include <gui/screen_hud.h>
#include <gui/gui.h>
#include <renderer/game_renderer.h>
#include <renderer/tesselator.h>
#include <player/player.h>
#include <minecraft.h>

#include <time.h>
#include <string.h>

screen_hud_t screen_hud_create(struct minecraft_s *minecraft, int width, int height) {
    screen_hud_t hud = { 0 };

    hud.minecraft = minecraft;
    hud.width = width * 240 / height;
    hud.height = height * 240 / height;
    hud.random = random_create(time(NULL));

    return hud;
}

void screen_hud_render(screen_hud_t *hud, float delta, float mx, float my) {
    renderer_setup_gui_camera(&hud->minecraft->renderer);
    glBindTexture(GL_TEXTURE_2D, textures_load(&hud->minecraft->textures, "gui/gui.png"));
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    gui_blit(hud->width / 2 - 91, hud->height - 22, 0, 0, 182, 22, -90.0);
    gui_blit(hud->width / 2 - 92 + hud->minecraft->player.inventory.selected * 20, hud->height - 23, 0, 22, 24, 22, -90.0);
    glBindTexture(GL_TEXTURE_2D, textures_load(&hud->minecraft->textures, "gui/icons.png"));
    gui_blit(hud->width / 2 - 7, hud->height / 2 - 7, 0, 0, 16, 15.99, -90.0);
    int iframe = hud->minecraft->player.invulnerable_time / 3 % 2 == 1;
    if(hud->minecraft->player.invulnerable_time < 10) {
        iframe = 0;
    }

    int health = hud->minecraft->player.health;
    int last_health = hud->minecraft->player.last_health;
    if(hud->minecraft->gamemode.gamemode_type == GAMEMODE_SURVIVAL) {
        for(int i = 0; i < 10; i++) {
            int8_t a = 0;
            if(iframe) {
                a = 1;
            }

            int w = hud->width / 2 - 91 + i * 8;
            int h = hud->height - 32;
            if(health <= 4) {
                h += random_next_int_range(&hud->random, 0, 2);
            }

            gui_blit(w, h, 16 + a * 9, 0, 9, 9, -90.0);
            if(iframe) {
                if((i * 2) + 1 < last_health) {
                    gui_blit(w, h, 70, 0, 9, 9, -90.0);
                }

                if((i * 2) + 1 == last_health) {
                    gui_blit(w, h, 79, 0, 9, 9, -90.0);
                }
            }

            if((i * 2) + 1 < health) {
                gui_blit(w, h, 52, 0, 9, 9, -90.0);
            }
            if((i * 2) + 1 == health) {
                gui_blit(w, h, 61, 0, 9, 9, -90.0);
            }
        }

        if(entity_is_underwater(&hud->minecraft->player.entity)) {
            int air1 = (int)ceil((hud->minecraft->player.air_supply - 2.0) * 10.0 / 300.0);
            int air2 = (int)ceil(hud->minecraft->player.air_supply * 10.0 / 300.0) - air1;

            for(int j = 0; j < air1 + air2; j++) {
                if(j < air1) {
                    gui_blit(hud->width / 2 - 91 + (j * 8), hud->height - 32 - 9, 16, 18, 9, 9, -90.0);
                }else {
                    gui_blit(hud->width / 2 - 91 + (j * 8), hud->height - 32 - 9, 25, 18, 9, 9, -90.0);
                }
            }
        }
    }

    glDisable(GL_BLEND);
    for(int i = 0; i < 9; i++) {
        int x = hud->width / 2 - 90 + i * 20;
        int y = hud->height - 16;
        uint8_t block_id = hud->minecraft->player.inventory.slots[i];
        block_t *block = &block_list[block_id];
        if(block_id != -1 && block_id != 0) {
            glPushMatrix();
            glTranslatef(x, y, -50.0);
            if(hud->minecraft->player.inventory.pop_times[i] > 0) {
                float a = (hud->minecraft->player.inventory.pop_times[i] - delta) / 5.0;
                float b = -tsin(a * a * M_PI) * 8.0;
                float c = tsin(a * a * M_PI) + 1.0;
                float d = tsin(a * M_PI) + 1.0;
                glTranslatef(10, b + 10, 0);
                glScalef(c, d, 1);
                glTranslatef(-10, -10, 0);
            }
            glScalef(10.0, 10.0, 10.0);
            glTranslatef(1.0, 0.5, 0.0);
            glRotatef(-30.0, 1.0, 0.0, 0.0);
            glRotatef(45.0, 0.0, 1.0, 0.0);
            glTranslatef(-1.5, 0.5, 0.5);
            glScalef(-1.0, -1.0, -1.0);
            glBindTexture(GL_TEXTURE_2D, textures_load(&hud->minecraft->textures, "terrain.png"));
            tesselator_begin();
            block->render_full_brightness(&block_list[block_id]);
            tesselator_end();
            glPopMatrix();
            if(hud->minecraft->player.inventory.counts[i] > 1) {
                char number[4];
                sprintf(number, "%d", hud->minecraft->player.inventory.counts[i]);
                font_render(&hud->minecraft->font, number, x + 19 - font_get_width(&hud->minecraft->font, number), y + 6, 0xffffffff);
            }
        }
    }

    font_render(&hud->minecraft->font, "0.30", 2, 2, 0xffffffff);
    if(hud->minecraft->settings.show_framerate) font_render(&hud->minecraft->font, hud->minecraft->debug, 2, 12, 0xffffffff);

    if(hud->minecraft->gamemode.gamemode_type == GAMEMODE_SURVIVAL) {
        char buffer[32];
        char number[5];
        sprintf(number, "%d", hud->minecraft->player.score);
        strcpy(buffer, "Score: &e");
        strcat(buffer, number);
        font_render(&hud->minecraft->font, buffer, hud->width - font_get_width(&hud->minecraft->font, buffer) - 2, 2, 0xffffffff);
        sprintf(number, "%d", hud->minecraft->player.arrows);
        strcpy(buffer, "Arrows: ");
        strcat(buffer, number);
        font_render(&hud->minecraft->font, buffer, hud->width / 2 + 8, hud->height - 33, 0xffffffff);
    }
        
    //chat screen

}

void screen_hud_destroy(screen_hud_t *hud) {
    //
}