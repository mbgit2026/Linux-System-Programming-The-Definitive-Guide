#include <argp.h>

const char *argp_program_version = "demo 1.0";
const char *argp_program_bug_address = "<aple@domain.com>";
static char doc[] = "Demo program using argp";
static char args_doc[] = "[FILENAME]";

static struct argp_option options[] = {
  {"verbose", 'v', 0, 0, "Produce verbose output"},
  {"output",  'o', "FILE", 0, "Output to FILE instead of stdout"},
  {0}
};

struct arguments {
  int verbose;
  char *output;
  char *input;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {

  struct arguments *arguments = state->input;

  switch (key) {
    case 'v': arguments->verbose = 1; break;
    case 'o': arguments->output = arg; break;
    case ARGP_KEY_ARG: arguments->input = arg; break;
    case ARGP_KEY_END: break;
    default: return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

int main(int argc, char **argv) {
  struct arguments arguments = {0, NULL, NULL};

  argp_parse(&argp, argc, argv, 0, 0, &arguments);
  return 0;
}
