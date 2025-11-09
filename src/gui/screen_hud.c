#include <gui/screen_hud.h>
#include <gui/gui.h>
#include <renderer/entity/renderer_entity_item.h>
#include <renderer/renderer_camera.h>
#include <renderer/renderer_lighting.h>
#include <renderer/tesselator.h>
#include <player/player.h>
#include <minecraft.h>

#include <time.h>
#include <string.h>

screen_hud_t screen_hud_create(struct minecraft_s *minecraft, int width, int height) {
    screen_hud_t hud = { 0 };

    hud.minecraft = minecraft;
    hud.width = width;
    hud.height = height;
    hud.random = random_create(time(NULL));

    return hud;
}

void screen_hud_render(screen_hud_t *hud, float mx, float my, float partial_tick) {
    /*int x = hud->minecraft->width;
    int y = hud->minecraft->height;
    int w = x;
    int h = y;
    for(x = 1; w / (x + 1) >= 320 && h / (x + 1) >= 240; x++);
    w /= x;
    h /= x;
    hud->width = w;
    hud->height = h;*/
    renderer_camera_setup_gui(&hud->minecraft->renderer);
    glBindTexture(GL_TEXTURE_2D, textures_load(&hud->minecraft->textures, "gui/gui.png"));
    glColor4f(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    gui_blit(hud->width / 2 - 91, hud->height - 22, 0, 0, 182, 22, -90.0);
    gui_blit(hud->width / 2 - 92 + hud->minecraft->player.mob->player->inventory.selected * 20, hud->height - 23, 0, 22, 24, 22, -90.0);
    glBindTexture(GL_TEXTURE_2D, textures_load(&hud->minecraft->textures, "gui/icons.png"));
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
    gui_blit(hud->width / 2 - 7, hud->height / 2 - 7, 0, 0, 16, 15.99, -90.0);
    glDisable(GL_BLEND);
    int iframe = hud->minecraft->player.mob->invulnerable_time / 3 % 2 == 1;
    if(hud->minecraft->player.mob->invulnerable_time < 10) {
        iframe = 0;
    }

    int health = hud->minecraft->player.mob->health;
    int last_health = hud->minecraft->player.mob->last_health;
    if(hud->minecraft->gamemode.gamemode_type == GAMEMODE_SURVIVAL) {
        int armor_value = inventory_player_get_armor_value(&hud->minecraft->player.mob->player->inventory);
        for(int i = 0; i < 10; i++) {
            int h = hud->height - 32;
            if(armor_value > 0) {
                int armor_w = hud->width / 2 + 91 - (i << 3) - 9;
                if((i << 1) + 1 < armor_value) {
                    gui_blit(armor_w, h, 34, 9, 9, 9, -90);
                }

                if((i << 1) + 1 == armor_value) {
                    gui_blit(armor_w, h, 25, 9, 9, 9, -90);
                }

                if((i << 1) + 1 > armor_value) {
                    gui_blit(armor_w, h, 16, 9, 9, 9, -90);
                }
            }
            int8_t a = 0;
            if(iframe) {
                a = 1;
            }

            int w = hud->width / 2 - 91 + i * 8;
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

        if(entity_is_underwater(&hud->minecraft->player)) {
            int air1 = (int)ceil((hud->minecraft->player.mob->air_supply - 2.0) * 10.0 / 300.0);
            int air2 = (int)ceil(hud->minecraft->player.mob->air_supply * 10.0 / 300.0) - air1;

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
    glEnable(GL_NORMALIZE);
    glPushMatrix();
    glRotatef(180, 1, 0, 0);
    renderer_lighting_enable();
    glPopMatrix();

    for(int i = 0; i < 9; i++) {
        int x = hud->width / 2 - 90 + i * 20 + 2;
        int y = hud->height - 16 - 3;
        item_stack_t item = hud->minecraft->player.mob->player->inventory.inv[i];
        if(item.item_id != 0) {
            float a = item.animations_to_go - partial_tick;
            if(a > 0) {
                glPushMatrix();
                float bounce = 1.0 + a / 5.0;
                glTranslatef(x + 8, y + 12, 0);
                glScalef(1.0 / bounce, (bounce + 1.0) / 2.0, 1.0);
                glTranslatef(-(x + 8), -(y + 12), 0);
            }

            renderer_entity_item_render_gui(hud->minecraft, &item, x, y);
            if(a > 0) {
                glPopMatrix();
            }

            renderer_entity_item_render_overlay_gui(&hud->minecraft->font, &item, x, y);
        }
    }

    renderer_lighting_disable();
    glDisable(GL_NORMALIZE);
    font_render(&hud->minecraft->font, "Minecraft Infdev", 2, 2, 0xffffffff);
    if(hud->minecraft->settings.show_framerate) font_render(&hud->minecraft->font, hud->minecraft->debug, 2, 12, 0xffffffff);
        
    //chat screen

}

void screen_hud_destroy(screen_hud_t *hud) {
    //
}