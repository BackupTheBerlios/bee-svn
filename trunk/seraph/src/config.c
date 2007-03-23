 /* */ struct Config {
        const void *class;
        int test_type;
        char *test_dir;
        char tmp_dir[FILENAME_MAX];
        char **argv;
        int argc;
        int refresh;
        char *config_file;
        char *cur_path;         /* retine variabila PATH inainte de rescriere */
        char cur_dir[FILENAME_MAX];     /* retine directorul curent, inainte k sa il schimb in /tmp/pid */
        char cur_test[FILENAME_MAX];
        char seraph_path[FILENAME_MAX]; /* Path to seraph dir */
        char *hostname;
        int port;
        char *axi_coreDir;      /* Path to core dir */
        char *axi_workDir;
        char axi_bin[FILENAME_MAX];
        char *axi_cfgFile;
        char *axi_dbgDir;
        char *axi_syslog;
        char *dest_coreDir;
        int script_tout;        /* script timeout in seconds. after this expires, the script is killed */
        bool verbose;
        bool allways_kill;
};

static void *Config_ctor(  )
{
}

static void *Config_dtor(  )
{
}
