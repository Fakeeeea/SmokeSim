#include "shader.h"

unsigned int get_graphics_program(const char* vertex_path, const char* fragment_path) {
    unsigned int shader_program;

    char info_log[MAX_LEN];
    int success;

    const char* vertex_source = parse_file(vertex_path);
    const char* fragment_source = parse_file(fragment_path);

    unsigned int vertex_shader, fragment_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(vertex_shader), glCompileShader(fragment_shader);

    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex_shader, MAX_LEN, NULL, info_log);
        printf("Vertex shader (%s) compilation failed!\n%s\n", vertex_path, info_log);
        free_sources(vertex_source, fragment_source);
        exit(-1);
    }

    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment_shader, MAX_LEN, NULL, info_log);
        printf("Fragment shader (%s) compilation failed!\n%s\n", fragment_path, info_log);
        free_sources(vertex_source, fragment_source);
        exit(-1);
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader), glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, MAX_LEN, NULL, info_log);
        printf("Shader (%s, %s) linking failed!\n%s\n", vertex_path, fragment_path, info_log);
        free_sources(vertex_source, fragment_source), delete_shaders(vertex_shader, fragment_shader);
        exit(-1);
    }

    free_sources(vertex_source, fragment_source), delete_shaders(vertex_shader, fragment_shader);

    return shader_program;
}

char* parse_file(const char* filename) {
    FILE *fptr = NULL;
    if (fopen_s(&fptr, filename, "rb")) {
        printf("Cannot open %s for reading: %s (errno = %d)\n", filename, strerror(errno), errno);
        exit(errno);
    }

    fseek(fptr, 0, SEEK_END);
    long size = ftell(fptr);
    if (size < 0) {
        printf("Ftell for file %s failed!\n", filename);
        fclose(fptr);
        exit(-1);
    }
    fseek(fptr, 0, SEEK_SET);

    char *parsedFile = malloc(size + 1);
    if (parsedFile) {
        size_t bytesRead = fread(parsedFile, 1, size, fptr);
        parsedFile[bytesRead] = '\0';
    } else {
        printf("Malloc for file %s failed!\n", filename);
        fclose(fptr);
        exit(-1);
    }

    fclose(fptr);

    return parsedFile;
}

void free_sources(const char* vertex_source, const char* fragment_source) {
    free( (void*) vertex_source), free( (void*) fragment_source);
}
void delete_shaders(unsigned int vertexID, unsigned int fragmentID) {
    glDeleteShader(vertexID), glDeleteShader(fragmentID);
}

unsigned int get_compute_program(const char* compute_path) {
    unsigned int shader_program;

    char info_log[MAX_LEN];
    int success;

    const char* compute_source = parse_file(compute_path);
    unsigned int compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader, 1, &compute_source,  NULL);
    glCompileShader(compute_shader);

    glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(compute_shader, MAX_LEN, NULL, info_log);
        printf("Compute shader (%s) compilation failed!\n%s\n", compute_path, info_log);
        free_source(compute_source);
        exit(-1);
    }

    shader_program = glCreateProgram();
    glAttachShader(shader_program, compute_shader);
    glLinkProgram(shader_program);

    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shader_program, MAX_LEN, NULL, info_log);
        printf("Shader (%s) linking failed!\n%s\n", compute_path, info_log);
        free_source(compute_source), delete_shader(compute_shader);
        exit(-1);
    }

    free_source(compute_source), delete_shader(compute_shader);

    return shader_program;
}

void free_source(const char* compute_shader) {
    free( (void*) compute_shader);
}
void delete_shader(unsigned int computeID) {
    glDeleteShader(computeID);
}

physics_shaders3d compile_physics_shaders3d() {
    physics_shaders3d shaders;

    shaders.update_vx = get_compute_program("../shaders3d/compute/update_vx.comp");
    shaders.update_vy = get_compute_program("../shaders3d/compute/update_vy.comp");
    shaders.update_vz = get_compute_program("../shaders3d/compute/update_vz.comp");

    shaders.advect_vx = get_compute_program("../shaders3d/compute/advect_vx.comp");
    shaders.advect_vy = get_compute_program("../shaders3d/compute/advect_vy.comp");
    shaders.advect_vz = get_compute_program("../shaders3d/compute/advect_vz.comp");

    shaders.advect_smoke = get_compute_program("../shaders3d/compute/advect_smoke.comp");

    shaders.apply_buoyancy = get_compute_program("../shaders3d/compute/apply_buoyancy.comp");

    shaders.handle_emitters = get_compute_program("../shaders3d/compute/handle_emitters.comp");
    shaders.update_emitters_status = get_compute_program("../shaders3d/compute/update_emitters_status.comp");

    shaders.calculate_vorticity = get_compute_program("../shaders3d/compute/calculate_vorticity.comp");
    shaders.apply_vorticity_x = get_compute_program("../shaders3d/compute/apply_vorticity_x.comp");
    shaders.apply_vorticity_y = get_compute_program("../shaders3d/compute/apply_vorticity_y.comp");
    shaders.apply_vorticity_z = get_compute_program("../shaders3d/compute/apply_vorticity_z.comp");

    shaders.init_solid_map = get_compute_program("../shaders3d/compute/init_solid_map.comp");

    shaders.obstacle_update = get_compute_program("../shaders3d/compute/obstacle_update.comp");
    shaders.update_solid_map = get_compute_program("../shaders3d/compute/update_solid_map.comp");

    shaders.compute_residual = get_compute_program("../shaders3d/compute/pressure_solve/compute_residual.comp");
    shaders.prolong_pressure = get_compute_program("../shaders3d/compute/pressure_solve/prolong_pressure.comp");
    shaders.restrict_residual = get_compute_program("../shaders3d/compute/pressure_solve/restrict_residual.comp");
    shaders.restrict_solid = get_compute_program("../shaders3d/compute/pressure_solve/restrict_solid.comp");
    shaders.smooth = get_compute_program("../shaders3d/compute/pressure_solve/smooth.comp");

    return shaders;
}

graphics_shaders3d compile_graphics_shaders3d() {
    graphics_shaders3d g_shaders;

    g_shaders.direction_lines = get_graphics_program("../shaders3d/graphics/direction_vectors.vert",
                                                     "../shaders3d/graphics/direction_vectors.frag");
    g_shaders.grid_lines = get_graphics_program("../shaders3d/graphics/grid_lines.vert",
                                                "../shaders3d/graphics/grid_lines.frag");
    g_shaders.arrows = get_graphics_program("../shaders3d/graphics/velocity_arrows.vert",
                                            "../shaders3d/graphics/velocity_arrows.frag");
    g_shaders.smoke = get_graphics_program("../shaders3d/graphics/ray_march_smoke.vert",
                                           "../shaders3d/graphics/ray_march_smoke.frag");
    g_shaders.vorticity = get_graphics_program("../shaders3d/graphics/draw_vorticity.vert",
                                               "../shaders3d/graphics/draw_vorticity.frag");
    g_shaders.obstacles = get_graphics_program("../shaders3d/graphics/draw_obstacles.vert",
                                               "../shaders3d/graphics/draw_obstacles.frag");

    return g_shaders;
}

physics_shaders2d compile_physics_shaders2d() {
    physics_shaders2d shaders;

    shaders.update_vx = get_compute_program("../shaders2d/compute/update_vx.comp");
    shaders.update_vy = get_compute_program("../shaders2d/compute/update_vy.comp");

    shaders.advect_vx = get_compute_program("../shaders2d/compute/advect_vx.comp");
    shaders.advect_vy = get_compute_program("../shaders2d/compute/advect_vy.comp");

    shaders.advect_smoke = get_compute_program("../shaders2d/compute/advect_smoke.comp");

    shaders.apply_buoyancy = get_compute_program("../shaders2d/compute/apply_buoyancy.comp");

    shaders.handle_emitters = get_compute_program("../shaders2d/compute/handle_emitters.comp");
    shaders.update_emitters_status = get_compute_program("../shaders2d/compute/update_emitters_status.comp");

    shaders.calculate_vorticity = get_compute_program("../shaders2d/compute/calculate_vorticity.comp");
    shaders.apply_vorticity_x = get_compute_program("../shaders2d/compute/apply_vorticity_x.comp");
    shaders.apply_vorticity_y = get_compute_program("../shaders2d/compute/apply_vorticity_y.comp");

    shaders.init_solid_map = get_compute_program("../shaders2d/compute/init_solid_map.comp");

    shaders.obstacle_update = get_compute_program("../shaders2d/compute/obstacle_update.comp");
    shaders.update_solid_map = get_compute_program("../shaders2d/compute/update_solid_map.comp");

    shaders.compute_residual = get_compute_program("../shaders2d/compute/pressure_solve/compute_residual.comp");
    shaders.prolong_pressure = get_compute_program("../shaders2d/compute/pressure_solve/prolong_pressure.comp");
    shaders.restrict_residual = get_compute_program("../shaders2d/compute/pressure_solve/restrict_residual.comp");
    shaders.restrict_solid = get_compute_program("../shaders2d/compute/pressure_solve/restrict_solid.comp");
    shaders.smooth = get_compute_program("../shaders2d/compute/pressure_solve/smooth.comp");

    return shaders;
}

graphics_shaders2d compile_graphics_shaders2d() {
    graphics_shaders2d g_shaders;

    g_shaders.draw_smoke = get_graphics_program("../shaders2d/graphics/draw_smoke.vert",
                                                "../shaders2d/graphics/draw_smoke.frag");

    return g_shaders;
}