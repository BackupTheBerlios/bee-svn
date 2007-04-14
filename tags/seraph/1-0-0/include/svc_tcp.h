
typedef struct {
        char *keyword;
        int ( *call ) ( int socket, char *buf );
} Callbacks;

int t_startCallback(int sock, char* buf);
int t_stopCallback(int sock, char* buf);
int t_refreshCallback(int sock, char* buf);
int t_copyCallback(int sock, char* buf);
int t_rmCallback(int sock, char* buf);
int t_mkdirCallback(int sock, char* buf);
int t_executeCallback(int sock, char* buf);
int t_checkCoreCallback(int sock, char* buf);
