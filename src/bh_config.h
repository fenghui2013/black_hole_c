#ifndef _BH_CONFIG_H_
#define _BH_CONFIG_H_

typedef struct bh_config bh_config;

bh_config *  bh_config_create();
void         bh_config_release(bh_config *config);
void         bh_config_load(bh_config *config, const char *config_file);
const char * bh_config_get_config_file(bh_config *config);
char *       bh_config_get_ip(bh_config *config);
int          bh_config_get_port(bh_config *config);
int          bh_config_get_threads(bh_config *config);
int          bh_config_get_lua_vms(bh_config *config);
char *       bh_config_get_server_type(bh_config *config);
char *       bh_config_get_lua_modules(bh_config *config);
#endif
