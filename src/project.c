#include "types.h"

#include "config_file.h"
#include "file.h"
#include "project.h"
#include "state.h"

static File read_fragment_shader_file(char *frag_path, char *frag_prefix,
                                      unsigned int i) {
  File fragment_shader;
  char file_path[STR_LEN];

  snprintf(file_path, STR_LEN, "%s/%s%d.glsl", frag_path, frag_prefix, i);
  fragment_shader = file_read(file_path);
  if (fragment_shader.error) {
    exit(EXIT_FAILURE);
  }

  return fragment_shader;
}

static void init_files(Project *output, char *frag_path, char *frag_prefix,
                       unsigned int frag_count) {
  unsigned int i;

  output->fragment_shaders.length = frag_count;

  for (i = 0; i < frag_count + 1; i++) {
    if (i == 0) {
      output->common_shader_code =
          read_fragment_shader_file(frag_path, frag_prefix, i);
    } else {
      output->fragment_shaders.values[i - 1] =
          read_fragment_shader_file(frag_path, frag_prefix, i);

      file_prepend(&output->fragment_shaders.values[i - 1],
                   output->common_shader_code);
    }
  }
}

Project project_init(char *project_path, char *config_file) {
  Project project;
  char config_path[STR_LEN];
  char *frag_prefix;

  snprintf(config_path, STR_LEN, "%s/%s", project_path, config_file);

  project.config = config_file_read(config_path);

  project.state_config = state_parse_config(project.config);

  project.frag_count = config_file_get_int(project.config, "FRAG_COUNT", 1);
  project.in_count = config_file_get_int(project.config, "IN_COUNT", 0);
  frag_prefix = config_file_get_str(project.config, "FRAG_FILE_PREFIX", "frag");

  init_files(&project, project_path, frag_prefix, project.frag_count);

  return project;
}

void project_reload(Project *project, void (*reload_callback)(unsigned int)) {
  unsigned int i;
  bool force_update;

  force_update = false;

  if (file_should_update(project->common_shader_code)) {
    file_update(&project->common_shader_code);
    force_update = true;
  }

  for (i = 0; i < project->frag_count; i++) {
    if (force_update ||
        file_should_update(project->fragment_shaders.values[i])) {
      file_update(&project->fragment_shaders.values[i]);
      file_prepend(&project->fragment_shaders.values[i],
                   project->common_shader_code);

      reload_callback(i);
    }
  }
}

void project_free(Project project) {
  unsigned int i;

  for (i = 0; i < project.frag_count; i++) {
    file_free(&project.fragment_shaders.values[i]);
  }

  file_free(&project.common_shader_code);

  config_file_free(project.config);
}