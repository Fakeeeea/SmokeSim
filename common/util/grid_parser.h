//
// Created by aaa on 27/04/2026.
//

#ifndef SMOKESIM_GRID_PARSER_H
#define SMOKESIM_GRID_PARSER_H

#include "../physics/physics.h"
#include "../graphics/graphics.h"

#define MAX_SAVE_FILES 128
#define MAX_SAVE_FILENAME 256

enum SAVE_INFO_STATE {
    NOT_LOADED = 0,
    LOADED = 1,
};

typedef struct save_folder_info {
    char save_files[MAX_SAVE_FILES][MAX_SAVE_FILENAME];
    unsigned int save_files_count;
    unsigned int folders_loaded;
}save_folder_info;

save_folder_info init_save_folder_info();
void parse_save_folder(save_folder_info* s_f_info);

int load_simulation(grid* grid_data, physics_info* p_info, graphics_info* g_info, char* filename);
int export_simulation(grid* grid, physics_info* p_info, graphics_info* g_info, char* filename);

#endif //SMOKESIM_GRID_PARSER_H
