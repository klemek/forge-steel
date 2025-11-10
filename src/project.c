#include <bsd/string.h>
#include <string.h>

#include "types.h"

#include "config.h"
#include "config_file.h"
#include "file.h"
#include "log.h"
#include "project.h"
#include "state.h"
#include "string.h"

static bool parse_fragment_shader_file(Project *project, unsigned int i) {
  File tmp_file;
  char file_path[STR_LEN];
  char *include_pos;
  char *include_end;
  char included_file[STR_LEN];
  char *new_content;

  project->sub_counts.values[i] = 0;

  include_pos = strstr(project->fragment_shaders[i][0].content, "\n#include ");

  while (include_pos != NULL && project->sub_counts.values[i] < MAX_SUB_FILE) {
    include_end = strstr(include_pos + 1, "\n");
    if (include_end == NULL) {
      log_error("Invalid '#include' directive in '%s'",
                project->fragment_shaders[i][0].path);
      return false;
    }

    strlcpy(included_file, include_pos + 10, include_end - include_pos - 9);

    snprintf(file_path, STR_LEN, "%s/%s", project->path, included_file);

    file_read(&tmp_file, file_path);

    if (tmp_file.error) {
      return false;
    }

    project->fragment_shaders[i][++project->sub_counts.values[i]] = tmp_file;

    new_content = string_replace_at(
        project->fragment_shaders[i][0].content,
        include_pos + 1 - project->fragment_shaders[i][0].content,
        include_end - project->fragment_shaders[i][0].content,
        tmp_file.content);

    project->fragment_shaders[i][0].content = new_content;

    file_free(&tmp_file);

    include_pos =
        strstr(project->fragment_shaders[i][0].content, "\n#include ");
  }

  return true;
}

static bool read_fragment_shader_file(Project *project, const char *frag_prefix,
                                      unsigned int i) {
  char file_path[STR_LEN];

  snprintf(file_path, STR_LEN, "%s/%s%d.glsl", project->path, frag_prefix,
           i + 1);

  file_read(&project->fragment_shaders[i][0], file_path);

  if (project->fragment_shaders[i][0].error) {
    return false;
  }

  return parse_fragment_shader_file(project, i);
}

void project_init(Project *project, char *project_path, char *config_file) {
  char config_path[STR_LEN];
  const char *frag_prefix;

  strlcpy(project->path, project_path, STR_LEN);

  snprintf(config_path, STR_LEN, "%s/%s", project_path, config_file);

  config_file_read(&project->config, config_path);

  state_parse_config(&project->state_config, &project->config);

  project->frag_count = config_file_get_int(&project->config, "FRAG_COUNT", 1);

  project->in_count = config_file_get_int(&project->config, "IN_COUNT", 0);
  frag_prefix =
      config_file_get_str(&project->config, "FRAG_FILE_PREFIX", "frag");

  if (project->frag_count > MAX_FRAG) {
    log_error("FRAG_COUNT over %d", MAX_FRAG);
    project->error = true;
    return;
  }

  project->sub_counts.length = project->frag_count;

  for (unsigned int i = 0; i < project->frag_count; i++) {
    project->sub_counts.values[i] = 0;
    if (!read_fragment_shader_file(project, frag_prefix, i)) {
      project_free(project);
      project->error = true;
      return;
    }
  }
}

void project_reload(Project *project, void (*reload_callback)(unsigned int)) {
  bool should_update;

  for (unsigned int i = 0; i < project->frag_count; i++) {
    should_update = file_should_update(&project->fragment_shaders[i][0]);

    for (unsigned int j = 0; j < project->sub_counts.values[i]; j++) {
      should_update = should_update ||
                      file_should_update(&project->fragment_shaders[i][j + 1]);
    }

    should_update =
        should_update && file_update(&project->fragment_shaders[i][0]);

    should_update = should_update && parse_fragment_shader_file(project, i);

    if (should_update) {
      reload_callback(i);
    }
  }
}

void project_free(Project *project) {
  for (unsigned int i = 0; i < project->frag_count; i++) {
    file_free(&project->fragment_shaders[i][0]);
  }

  config_file_free(&project->config);
}