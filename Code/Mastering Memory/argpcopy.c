#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

const char *argp_program_version = "copyfile 1.1";
const char *argp_program_bug_address = "<aple@domain.com>";

static char doc[] =  "Copies INPUT to OUTPUT with optional progress messages.\n";
static char args_doc[] = "INPUT OUTPUT";

static struct argp_option options[] = {
  {"verbose", 'v', 0, 0, "Produce verbose output"},
  {0}
};

struct arguments {
  int verbose;
  char *input;
  char *output;
};


static error_t parse_opt(int key, char *arg, struct argp_state *state) {
  struct arguments *arguments = state->input;

  switch (key) {
  case 'v':
    arguments->verbose = 1;
    break;
  case ARGP_KEY_ARG:
    if (state->arg_num == 0)
      arguments->input = arg;
    else if (state->arg_num == 1)
      arguments->output = arg;
    else
      argp_usage(state);  /* Too many arguments */
    break;
  case ARGP_KEY_END:
    if (state->arg_num < 2)
      argp_usage(state);  
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

int copy_file(const char *src, const char *dst, int verbose) {
  FILE *in = fopen(src, "rb");
  if (!in) {
    fprintf(stderr, "Error opening '%s': %s\n", src, strerror(errno));
    return 1;
  }

  FILE *out = fopen(dst, "wb");
  if (!out) {
    fprintf(stderr, "Error opening '%s': %s\n", dst, strerror(errno));
    fclose(in);
    return 1;
  }

  if (verbose)
    printf("[VERBOSE] Copying from '%s' to '%s'\n", src, dst);

  char buffer[4096];
  size_t total = 0;
  size_t n;

  while ((n = fread(buffer, 1, sizeof(buffer), in)) > 0) {
    if (fwrite(buffer, 1, n, out) != n) {
      fprintf(stderr, "Write error on '%s': %s\n", dst, strerror(errno));
      fclose(in);
      fclose(out);
      return 1;
    }
    total += n;
    if (verbose)
      printf("[VERBOSE] Copied %zu bytes...\n", total);
  }

  if (ferror(in))
    fprintf(stderr, "Read error on '%s'\n", src);

  fclose(in);
  fclose(out);

  if (verbose)
    printf("[VERBOSE] Finished copy: %zu bytes written.\n", total);
  else
    printf("File copied successfully.\n");

  return 0;
}

int main(int argc, char **argv) {
  struct arguments arguments = {0, NULL, NULL};
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  return copy_file(arguments.input, arguments.output, arguments.verbose);
}
