//
// Created by aaa on 27/04/2026.
//

#include "grid_parser.h"
#include <stdio.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#include <sys/stat.h>
#endif

#define SAVE_MAGIC "SMKSAVE\0"
#define SAVE_MAGIC_SIZE 8
#define SAVE_VERSION 1u

static int write_block(FILE* fptr, const void* p, size_t n) {
    return fwrite(p, 1, n, fptr) == n ? 0 : -1;
}

static int read_block(FILE* fptr, const void* p, size_t n) {
    return fread((void*)p, 1, n, fptr) == n ? 0 : -1;
}

int load_simulation(grid* grid_data, physics_info* p_info, graphics_info* g_info, char* filename) {
    FILE* fptr;
    fptr = fopen(filename, "rb");
    if(!fptr) return -1;

    char magic[SAVE_MAGIC_SIZE];
    if(read_block(fptr, magic, SAVE_MAGIC_SIZE) || memcmp(magic, SAVE_MAGIC, SAVE_MAGIC_SIZE) != 0) {fclose(fptr); return -1;}

    uint32_t version = 0;
    if(read_block(fptr, &version, sizeof(version))) { if(version != SAVE_VERSION) {fclose(fptr); return -1;}}

    uint32_t is_2d = 0, e_count = 0, o_count = 0;
    int err = 0;

    err |= read_block(fptr, &is_2d, sizeof(is_2d));
    grid_data->is_2d = (int)is_2d;

    err |= read_block(fptr, &grid_data->cell_size, sizeof(grid_data->cell_size));

    if(is_2d) err |= read_block(fptr, grid_data->grid2d_data.size, sizeof(grid_data->grid2d_data.size));
    else err |= read_block(fptr, grid_data->grid3d_data.size, sizeof(grid_data->grid3d_data.size));

    err |= read_block(fptr, &p_info->p_settings, sizeof(p_info->p_settings));
    err |= read_block(fptr, &p_info->p_s_settings, sizeof(p_info->p_s_settings));
    err |= read_block(fptr, &p_info->enclosed, sizeof(p_info->enclosed));
    err |= read_block(fptr, &p_info->wind_tunnel, sizeof(p_info->wind_tunnel));

    err |= read_block(fptr, &e_count, sizeof(e_count));
    p_info->e_info.emitters_count = (int) e_count;

    if(e_count > 0) {
        p_info->e_info.emitters_array = malloc(e_count * sizeof(emitter));
        err |= read_block(fptr, p_info->e_info.emitters_array, e_count * sizeof(emitter));
    }

    err |= read_block(fptr, &o_count, sizeof(o_count));
    p_info->o_info.obstacles_count = (int) o_count;

    if(o_count) {
        p_info->o_info.obstacles_array = malloc(o_count * sizeof(obstacle));
        err |= read_block(fptr, p_info->o_info.obstacles_array, o_count * sizeof(obstacle));
    }

    if(is_2d) err |= read_block(fptr, &g_info->g_info2d.g_s_settings, sizeof(g_info->g_info2d.g_s_settings));
    else err |= read_block(fptr, &g_info->g_info3d.g_s_settings, sizeof(g_info->g_info3d.g_s_settings));
    err |= read_block(fptr, &g_info->g_settings, sizeof(g_info->g_settings));

    fclose(fptr);
    return err;
}

int export_simulation(grid* grid_data, physics_info* p_info, graphics_info* g_info, char* filename) {
    FILE* fptr;
    fptr = fopen(filename, "wb");
    if(!fptr) return -1;

    uint32_t version = SAVE_VERSION;
    uint32_t is_2d = grid_data->is_2d;
    uint32_t e_count = (uint32_t) p_info->e_info.emitters_count;
    uint32_t o_count = (uint32_t) p_info->o_info.obstacles_count;

    int err = 0;
    err |= write_block(fptr, SAVE_MAGIC, SAVE_MAGIC_SIZE);
    err |= write_block(fptr, &version, sizeof(version));
    err |= write_block(fptr, &is_2d, sizeof(is_2d));
    err |= write_block(fptr, &grid_data->cell_size, sizeof(grid_data->cell_size));

    if(is_2d) err |= write_block(fptr, grid_data->grid2d_data.size, sizeof(grid_data->grid2d_data.size));
    else err |= write_block(fptr, grid_data->grid3d_data.size, sizeof(grid_data->grid3d_data.size));

    err |= write_block(fptr, &p_info->p_settings, sizeof(p_info->p_settings));
    err |= write_block(fptr, &p_info->p_s_settings, sizeof(p_info->p_s_settings));
    err |= write_block(fptr, &p_info->enclosed, sizeof(p_info->enclosed));
    err |= write_block(fptr, &p_info->wind_tunnel, sizeof(p_info->wind_tunnel));

    err |= write_block(fptr, &e_count, sizeof(e_count));
    if(e_count) err |= write_block(fptr, p_info->e_info.emitters_array, e_count * sizeof(emitter));

    err |= write_block(fptr, &o_count, sizeof(o_count));
    if(o_count) err |= write_block(fptr, p_info->o_info.obstacles_array, o_count * sizeof(obstacle));

    if(is_2d) err |= write_block(fptr, &g_info->g_info2d.g_s_settings, sizeof(g_info->g_info2d.g_s_settings));
    else err |= write_block(fptr, &g_info->g_info3d.g_s_settings, sizeof(g_info->g_info3d.g_s_settings));
    err |= write_block(fptr, &g_info->g_settings, sizeof(g_info->g_settings));

    fclose(fptr);
    return err;
}

save_folder_info init_save_folder_info() {
    save_folder_info out;

    out.folders_loaded = NOT_LOADED;
    out.save_files_count = 0;

    return out;
}

void parse_save_folder(save_folder_info* s_f_info) {
    if(s_f_info->folders_loaded == LOADED) return;

    s_f_info->save_files_count = 0;

#ifdef _WIN32
    WIN32_FIND_DATAA fd;
    char search_path[MAX_SAVE_FILENAME];
    snprintf(search_path, sizeof(search_path), "../saves/*.dat");

    HANDLE hFind = FindFirstFileA(search_path, &fd);
    if(hFind != INVALID_HANDLE_VALUE) {
        do {
            if(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;

            snprintf(s_f_info->save_files[s_f_info->save_files_count], MAX_SAVE_FILENAME, "%s", fd.cFileName);
            s_f_info->save_files_count++;

            if(s_f_info->save_files_count >= MAX_SAVE_FILES) break;

        }while(FindNextFileA(hFind, &fd));

        FindClose(hFind);
    }
#else
    DIR *dir = opendir("../saves");
    if(dir) {
        struct dirent *entry;
        while((entry = readdir(dir)) != NULL) {
            size_t name_len = strlen(entry->d_name);
            if(name_len < 4 || strcmp(entry->d_name + name_len - 4, ".dat") != 0) continue;

#ifdef DT_REG
            if(entry->d_type != DT_REG && entry->d_type != DT_UNKNOWN) continue;
#else
            char fullpath[MAX_SAVE_FILENAME];
            struct stat st;
            snprintf(fullpath, sizeof(fullpath), "../saves/%s", entry->d_name);
            if(stat(fullpath, &st) != 0 || !S_ISREG(st.st_mode)) continue;
#endif
            snprintf(s_f_info->save_files[s_f_info->save_files_count], MAX_SAVE_FILENAME, "%s", entry->d_name);
            s_f_info->save_files_count++;

            if(s_f_info->save_files_count >= MAX_SAVE_FILES) break;
        }

        closedir(dir);
    }
#endif
    s_f_info->folders_loaded = LOADED;
}

