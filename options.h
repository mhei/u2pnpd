#ifndef OPTIONS_H
#define OPTIONS_H

typedef struct {
	char *friendlyName;
	char *manufacturer;
	char *manufacturerURL;
	char *modelDescription;
	char *modelName;
	char *modelNumber;
	char *modelURL;
	char *serialNumber;
	char *uuid;
	char *presentationURL;
	int use_https;
} config_options_t;

extern config_options_t *options;

config_options_t *options_init(void);
void options_free(config_options_t *);
int options_parse_cli(int argc, char *argv[], config_options_t *options);

#endif /* OPTIONS_H */
