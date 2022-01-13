#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "func_Server.h"

char out[MAX_OUT_SIZE];
int v_mode = 0;

char* processInput(char *input){
    char command[5], *out=NULL;

    int size = strlen(input);
    if (size < 4)
        return "ERR\n";

    strncpy(command, input, 4);
    command[4] = '\0';

    if (strcmp(command, "REG ") == 0){
        out = registerUserS(&input[4]);
    }
    else if (strcmp(command, "UNR ") == 0){
        out = unregisterUserS(&input[4]);
    }
    else if (strcmp(command, "LOG ") == 0){
        out = loginUserS(&input[4]);
    }
    else if (strcmp(command, "OUT ") == 0){
        out = logoutUserS(&input[4]);
    }
    else if (strcmp(command, "GLS\n") == 0){
        out = showAvailableGroupsS();
    }
    else if (strcmp(command, "GSR ") == 0){
        out = subscribeGroupS(&input[4]);
    }
    else if (strcmp(command, "GUR ") == 0){
        out = unsubscribeGroupS(&input[4]);
    }
    else if (strcmp(command, "GLM ") == 0){
        out = showMyGroupsS(&input[4]);
    }
    else{
        return "ERR\n";
    }
    return out;
}

void processInputTCP(int connfd, char *command){
    char input[15];

    if(strcmp(command, "PST ")==0){
        postMessageS(connfd);
    }
    else if(strcmp(command, "RTV ")==0){
        int nread = readTCP(connfd, 14, input);
        if (nread == 14)
            retrieveMessagesS(input, connfd);
        else{
            writeTCP(connfd, 5, "ERR\n");
        }
    }
    else if(strcmp(command, "ULS ")==0){
        int nread = readTCP(connfd, 3, input);
        if (nread == 3)
            listUsers_GIDS(input, connfd);
        else{
            writeTCP(connfd, 5, "ERR\n");
        }
    }
    else{
        if (v_mode)
            printf("Error: wrong message format\n");
        writeTCP(connfd, 5, "ERR\n");
    }
}

void initSession(int verbose_mode){
    if (verbose_mode)
        v_mode = 1;
}

int readTCP(int connfd, int n_bytes, char *content){
    ssize_t nread;

    nread=read(connfd, content, n_bytes);
    if(nread<1){
        exit(1);
    } 
    content[nread] = '\0';
    return nread;
}

int writeTCP(int connfd, int n_bytes, char *content){
    ssize_t nwrite;

    nwrite=write(connfd, content, n_bytes);
    if(nwrite<1){
        exit(1);
    } 
    return nwrite;
}

int validUID(char *input){
    for (int i = 0; ((input[i] != ' ') && (input[i] != '\n') && (input[i] != '\0'));i++){
        if ((!isdigit(input[i])) || (i >= 5))
            return 0;
    }
    return 1;
}

int validPassword(char *input){
    if (strlen(input) != 9)
        return 0;
    for (int i = 0; input[i] != '\n';i++){
        if (((!isdigit(input[i])) && (!isalpha(input[i]))) || (i >= 8))
            return 0;
    }
    return 1;
}

int validGID(char *input){
    if (isdigit(input[0]) && isdigit(input[1]))
        return 1;
    return 0;
}

int validGName(char *input){
    int i;
    if (strlen(input) >= 26)
        return 0;
    for (i = 0; input[i] != '\n';i++){
        if (((!isdigit(input[i])) && (!isalpha(input[i])) && (input[i] != '-') && (input[i] != '_')) || (i >= 24))
            return 0;
    }
    return i; // returns size of Group Name
}

int validMID(char *input){
    for (int i = 0; input[i] != '\n'; i++){
        if ((!isdigit(input[i])) || (i >= 4))
            return 0;
    }
    return 1;
}

int createDir(char *path){
    int ret = mkdir(path, 0700);
    if (ret == -1)
        return 0;
    return 1;
}

int createFile(char *path, char *content){
    FILE *fPtr = fopen(path, "w");
    if (fPtr == NULL)
        return 0;
    fwrite(content, 1, strlen(content), fPtr);
    fclose(fPtr);
    return 1;
}

int deleteFile(char *path){
    if(unlink(path)==0)
        return 1;
    else
        return 0;
}

int deleteDir(char *path){
    if(rmdir(path)==0)
        return 1;
    else
        return 0;
}

int getIntLength(int i){
    return (floor(log10(abs(i))) + 1);
}

char* registerUserS(char *input){
    char UID[6], password[9], user_dirname[15];

    int file_count = 0;
    DIR * dirp;
    struct dirent * entry;
    
    dirp = opendir("USERS");
   
    while ((entry = readdir(dirp)) != NULL) {
        if (strlen(entry->d_name)==5) { 
            file_count++;
        }   
    }
    closedir(dirp);
    
    if(file_count>99999){
        sprintf(out,"RRG NOK\n");
    }

    if (!validUID(input)){
        return "RRG NOK\n";
    }
    if (input[5] != ' '){
        return "RRG NOK\n";
    }
    if (!validPassword(&input[6])){
        return "RRG NOK\n";
    }

    strncpy(UID, input, 5); UID[5] = '\0';
    strncpy(password, &input[6], 8); password[8] = '\0';
    sprintf(user_dirname,"USERS/%s",UID);
    if(access(user_dirname, F_OK ) == 0 ){
        if (v_mode)
            printf("UID=%s already registered\n", UID);
        sprintf(out,"RRG DUP\n");
    }
    else{
        char user_password[30];
        if (!createDir(user_dirname)){
            if (v_mode)
                printf("Register error: Could not create dir\n");
            return "RRG NOK\n";
        }

        sprintf(user_password,"USERS/%s/%s_pass.txt",UID,UID);
        if (!createFile(user_password, password)){
            if (v_mode)
                printf("Register error: Could not create pass .txt\n");
            return "RRG NOK\n";
        }
        
        sprintf(out,"RRG OK\n");
        if (v_mode)
            printf("UID=%s: new user\n", UID);
    }
    return out;
}

char* unregisterUserS(char *input){
    char UID[MAX_UID_SIZE], password[MAX_PASS_SIZE],realPassword[MAX_PASS_SIZE];
    char passPath[50], UIDPath[50], loginPath[50];
    FILE *pass_txt;

    //get UID and password from input
    //get real password from user
    //check if input password is correct

    if (!validUID(input)){
        return "RUN NOK\n";
    }
    if (input[5] != ' '){
        return "RUN NOK\n";
    }
    if (!validPassword(&input[6])){
        return "RUN NOK\n";
    }

    strncpy(UID,input,5); UID[5] = '\0';
    strncpy(password,&input[6],8); password[8] = '\0';

    sprintf(loginPath,"USERS/%s/%s_login.txt",UID,UID);
    sprintf(passPath,"USERS/%s/%s_pass.txt",UID,UID);
    sprintf(UIDPath,"USERS/%s",UID);
    pass_txt = fopen(passPath, "r");
    if (pass_txt){
        if(fscanf(pass_txt, "%s", realPassword)==0){
            exit(EXIT_FAILURE);
        }
        fclose(pass_txt);
    }
    else{
        if (v_mode)
            printf("UID=%s not registered\n", UID);
        return "RUN NOK\n";
    }

    if(strcmp(password,realPassword)==0){
        if(access(loginPath, F_OK ) == 0 ){
            if (!deleteFile(loginPath)){
                return "RUN NOK\n";
            }
        }
        if (deleteFile(passPath) && deleteDir(UIDPath)){
            if (v_mode)
                printf("UID=%s: unregistered user\n", UID);
            return "RUN OK\n";
        }
    }
    return "RUN NOK\n";
}

char* loginUserS(char *input){
    char UID[6],password[9],pass_dir[40],file_password[10], user_dirname[31];
    FILE *password_ptr;

    if (!validUID(input)){
        return "RLO NOK\n";
    }
    if (input[5] != ' '){
        return "RLO NOK\n";
    }
    if (!validPassword(&input[6])){
        return "RLO NOK\n";
    }

    strncpy(UID,input,5); UID[5] = '\0';
    strncpy(password,&input[6],8); password[8] = '\0';

    sprintf(user_dirname,"USERS/%s",UID);

    sprintf(pass_dir,"USERS/%s/%s_pass.txt",UID,UID);
    if(access(user_dirname, F_OK ) == 0 ){
        password_ptr=fopen(pass_dir,"r");
        if(fgets(file_password,9,password_ptr)==NULL){
            sprintf(out,"RLO NOK\n");
        }
        fclose(password_ptr);
        file_password[8]='\0';

        if(memcmp(file_password,password,9)==0){
            sprintf(out,"RLO OK\n");
            sprintf(user_dirname,"USERS/%s/%s_login.txt",UID,UID);
            FILE *login_ptr = fopen(user_dirname,"w");
            if(login_ptr==NULL){
                sprintf(out,"RLO NOK\n");
            }
            else{
                if (v_mode)
                    printf("UID=%s: login ok\n", UID);
            }
            fclose(login_ptr);
        }
        else{
            if (v_mode)
                printf("Login error: incorrect password\n");
            sprintf(out,"RLO NOK\n");
        }
    }
    else{
        if (v_mode)
            printf("Login error: UID=%s not registered", UID);
        sprintf(out,"RLO NOK\n");
    }
    return out;
}

char *logoutUserS(char* input){
    char UID[MAX_UID_SIZE], password[MAX_PASS_SIZE],realPassword[MAX_PASS_SIZE],pathname[50];
    FILE *pass_txt;
    //get UID and password from input
    //get real password from user
    //check if input password is correct

    if (!validUID(input)){
        return "ROU NOK\n";
    }
    if (input[5] != ' '){
        return "ROU NOK\n";
    }
    if (!validPassword(&input[6])){
        return "ROU NOK\n";
    }

    strncpy(UID,input,5); UID[5] = '\0';
    strncpy(password,&input[6],8); password[8] = '\0';

    sprintf(pathname,"USERS/%s/%s_pass.txt",UID,UID);
    pass_txt = fopen(pathname, "r");
    if (pass_txt){
        if(fscanf(pass_txt, "%s", realPassword)==0){
            exit(0);
        }
        fclose(pass_txt);
    }
    else{
        if (v_mode)
            printf("Logout error: User not registered");
        return "ROU NOK\n";
    }

    sprintf(pathname,"USERS/%s/%s_login.txt",UID,UID);

    if(strcmp(password,realPassword)==0){
        if(deleteFile(pathname)){
            if (v_mode)
                printf("UID=%s: logout ok\n", UID);
            return "ROU OK\n";
        }
        else{
            return "ROU NOK\n";
        }
    }
    else{
        if (v_mode)
            printf("Logout error: Incorrect password\n");
        return "ROU NOK\n";
    }
}

char *showAvailableGroupsS(){
    char list_groups[MAX_OUT_SIZE-20];
    char GNamepath[30], MIDpath[30], GID[3], GName[24], MID[12];
    DIR *d;
    struct dirent *dir;
    int n_groups = 0;
    FILE *fp;
    
    char *p = list_groups;
    d = opendir("GROUPS");

    if (d){
        sprintf(out, "RGL 0\n");
        while ((dir = readdir(d)) != NULL){
            if(dir->d_name[0]=='.')
                continue;
            if(strlen(dir->d_name)!=2)
                continue;
            strcpy(GID, dir->d_name);
            GID[2] = '\0';
            sprintf(GNamepath,"GROUPS/%s/%s_name.txt", GID, GID);
            fp=fopen(GNamepath,"r");
            if(fp){
                if(fscanf(fp,"%s", GName)==0){
                fclose(fp);
                exit(0);
                }
                fclose(fp);
            }
            sprintf(MIDpath, "GROUPS/%s/MSG", GID);
            DIR *d_msg = opendir(MIDpath);
            struct dirent *msgdir;
            short int latest = 0;
            if (d_msg){
                while ((msgdir = readdir(d_msg)) != NULL){
                    if(msgdir->d_name[0]=='.')
                        continue;
                    if(strlen(msgdir->d_name) != 4)
                        continue;
                    if (atoi(msgdir->d_name) > latest){
                        latest = atoi(msgdir->d_name);
                    }
                }
                if (latest < 10)
                    sprintf(MID, "000%d", latest);
                else if (latest < 100)
                    sprintf(MID, "00%d", latest);
                else if (latest < 1000)
                    sprintf(MID, "0%d", latest);
                else if (latest < 10000)
                    sprintf(MID, "%d", latest);

                closedir(d_msg);
            }
            size_t offset = sprintf(p, " %s %s %s", GID, GName, MID);
            p += offset;
            ++n_groups;
            if(n_groups==99)
                break;
        }
        closedir(d);

        sprintf(out, "RGL %d%s\n", n_groups, list_groups);
        return out;
    }
    return " ";
}

char *subscribeGroupS(char *input){
    if (!validUID(input)){
        return "RGS E_USR\n";
    }
    if (input[5] != ' '){
        return "RGS NOK\n";
    }
    if (!validGID(&input[6])){
        return "RGS E_GRP\n";
    }
    if (input[8] != ' '){
        return "RGS NOK\n";
    }
    int size;
    if ((size = validGName(&input[9])) == 0){
        return "RGS E_GNAME\n";
    }

    char UID[6], GID[3], GName[25];

    strncpy(UID, input, 5); UID[5] = '\0';
    strncpy(GID, &input[6], 2); GID[2] = '\0';
    strncpy(GName, &input[9], size); GName[size] = '\0';

    DIR *d_USR, *d_GRP;
    struct dirent *dir_usr, *dir_grp;
    int valid = 0, i = 0;
    d_USR = opendir("USERS");
    if (d_USR){
        while ((dir_usr = readdir(d_USR)) != NULL){
            if(dir_usr->d_name[0]=='.')
                continue;
            if (strcmp(dir_usr->d_name, UID) == 0){
                valid = 1;
                break;
            }
            if (i == 99999)
                break;
            i++;
        }
        closedir(d_USR);
        if (!valid)
            return "RGS E_USR\n";
    }
    valid = 0;
    d_GRP = opendir("GROUPS");
    if (d_GRP){
        if (strcmp(GID, "00") != 0){
            while ((dir_grp = readdir(d_GRP)) != NULL){
                if(dir_grp->d_name[0]=='.')
                    continue;
                if (strcmp(dir_grp->d_name, GID) == 0){
                    char GNamepath[24];
                    sprintf(GNamepath, "GROUPS/%s/%s_name.txt", GID, GID);
                    FILE *fp = fopen(GNamepath, "r");
                    char content[26];
                    if (fp){
                        if(fscanf(fp,"%s", content)==0){
                            exit(0);
                        }
                        fclose(fp);
                        if (strcmp(content, GName) == 0){
                            valid = 1;
                        }
                    }
                    break;
                                    
                }
                if (i == 99){
                    closedir(d_GRP);
                    return "RGS E_FULL\n";
                }
                i++;
            }
            closedir(d_GRP);
            if (!valid)
                return "RGS NOK\n";

            char GUIDpath[20];
            sprintf(GUIDpath, "GROUPS/%s/%s.txt", GID, UID);
            if (!createFile(GUIDpath, UID))
                return "RGS NOK\n";

            if (v_mode)
                printf("UID=%s: subscribed group: %s - \"%s\"\n", UID, GID, GName);
            return "RGS OK\n";
        }
        else{
            int nextGID = 1;
            short int max = 0;
            while ((dir_grp = readdir(d_GRP)) != NULL){
                if(dir_grp->d_name[0]=='.')
                    continue;
                nextGID = atoi(dir_grp->d_name);
                if (nextGID > max)
                    max = nextGID;
            }
            closedir(d_GRP);

            max++;
            
            if (max == 99){
                if (v_mode)
                    printf("Subscribe error: Maximum number of groups exceeded\n");
                return "RGS E_FULL\n";
            }

            int gid = 1;
            while (gid < max){
                char GNamepath[40], GID_tmp[10], real_gname[26];
                if (gid < 10)
                    sprintf(GID_tmp, "0%d", gid);
                else
                    sprintf(GID_tmp, "%d", gid);

                sprintf(GNamepath, "GROUPS/%s/%s_name.txt", GID_tmp, GID_tmp);
                FILE *fp = fopen(GNamepath, "r");
                if (fp){
                    if (!fscanf(fp, "%s", real_gname)){
                        return "RGS NOK\n";
                    }
                    fclose(fp);
                    if (strcmp(GName, real_gname) == 0){
                        if (v_mode)
                            printf("Subscribe error: Group name already being used\n");
                        return "RGS E_GRP\n";
                    }
                }
                else{
                    if (v_mode)
                        printf("Error: cannot open Group name file\n");
                    return "RGS NOK\n";
                }
                gid++;
            }
            
            char newGID[8];
            if (max < 10)
                sprintf(newGID, "0%d", max);
            else
                sprintf(newGID, "%d", max);

            char GRPpath[37];
            sprintf(GRPpath,"GROUPS/%s",newGID);
            if (!createDir(GRPpath))
                return "RGS NOK\n";

            sprintf(GRPpath, "GROUPS/%s/%s_name.txt", newGID, newGID);
            if (!createFile(GRPpath, GName))
                return "RGS NOK\n";

            sprintf(GRPpath, "GROUPS/%s/%s.txt", newGID, UID);
            if (!createFile(GRPpath, UID))
                return "RGS NOK\n";

            sprintf(GRPpath, "GROUPS/%s/MSG", newGID);
            if (!createDir(GRPpath))
                return "RGS NOK\n";

            if (v_mode)
                printf("UID=%s: new group: %s - \"%s\"\n", UID, newGID, GName);
            sprintf(out, "RGS NEW %s\n", newGID);
            return out;
        }
    }
    return "RGS NOK\n";
}

char *unsubscribeGroupS(char *input){
    if (!validUID(input)){
        return "RGU E_USR\n";
    }
    if (input[5] != ' '){
        return "RGU NOK\n";
    }
    if (!validGID(&input[6])){
        return "RGU E_GRP\n";
    }
    if (input[8] != '\n'){
        return "RGU NOK\n";
    }

    char UID[6], GID[3];

    strncpy(UID, input, 5); UID[5] = '\0';
    strncpy(GID, &input[6], 2); GID[2] = '\0';

    DIR *d_USR, *d_GRP;
    struct dirent *dir_usr, *dir_grp;
    int valid = 0, i = 0;
    d_USR = opendir("USERS");
    if (d_USR){
        while ((dir_usr = readdir(d_USR)) != NULL){
            if(dir_usr->d_name[0]=='.')
                continue;
            if (strcmp(dir_usr->d_name, UID) == 0){
                valid = 1;
                break;
            }
            if (i == 99999)
                break;
            i++;
        }
        closedir(d_USR);
        if (!valid)
            return "RGU E_USR\n";
    }

    valid = 0;
    d_GRP = opendir("GROUPS");
    if (d_GRP){
        while ((dir_grp = readdir(d_GRP)) != NULL){
            if(dir_grp->d_name[0]=='.')
                continue;
            if (strcmp(dir_grp->d_name, GID) == 0){
                valid = 1;
            }
            i++;
        }
        closedir(d_GRP);
        if (!valid){
            if (v_mode)
                printf("Unsubscribe error: Non-existing group GID=%s\n", GID);
            return "RGU NOK\n";
        }
        
        char GIDPath[24];
        sprintf(GIDPath, "GROUPS/%s/%s.txt", GID, UID);
        if (!deleteFile(GIDPath)){
            return "RGU NOK\n";
        }

        if (v_mode)
            printf("UID=%s: unsubscribed group: %s\n", UID, GID);
        return "RGU OK\n";
    }
    return "RGU NOK\n";
}

char *showMyGroupsS(char *input){

    if (!validUID(input)){
        return "RGM E_USR\n";
    }

    char UID[6];
    strncpy(UID, input, 5); UID[5] = '\0';

    DIR *d_USR, *d_GRP;
    struct dirent *dir_usr, *dir_grp;
    int valid = 0, i = 0;

    d_USR = opendir("USERS");
    if (d_USR){
        while ((dir_usr = readdir(d_USR)) != NULL){
            if(dir_usr->d_name[0]=='.')
                continue;
            if (strcmp(dir_usr->d_name, UID) == 0){
                valid = 1;
                break;
            }
            if (i == 99999)
                break;
            i++;
        }
        closedir(d_USR);
        if (!valid)
            return "RGM E_USR\n";
    }

    char GNamepath[30], MIDpath[30], UIDPath[30], GID[3], GName[24], MID[12];
    char list_groups[MAX_OUT_SIZE-20];
    int n_groups = 0;
    FILE *fp;
    char *p = list_groups;
    d_GRP = opendir("GROUPS");

    if (d_GRP){
        while ((dir_grp = readdir(d_GRP)) != NULL){
            if(dir_grp->d_name[0]=='.')
                continue;
            if(strlen(dir_grp->d_name)>2)
                continue;
            
            strcpy(GID, dir_grp->d_name); GID[2] = '\0';
            sprintf(UIDPath,"GROUPS/%s/%s.txt", GID, UID);
            if(access(UIDPath, F_OK ) != 0 )
                continue;
            
            sprintf(GNamepath,"GROUPS/%s/%s_name.txt", GID, GID);
            fp=fopen(GNamepath,"r");
            if(fp){
                if(fscanf(fp,"%s", GName)==0){
                    exit(0);
                }
                fclose(fp);
            }

            sprintf(MIDpath, "GROUPS/%s/MSG", GID);
            DIR *d_msg = opendir(MIDpath);
            struct dirent *msgdir;
            short int latest = 0;
            if (d_msg){
                while ((msgdir = readdir(d_msg)) != NULL){
                    if(msgdir->d_name[0]=='.')
                        continue;
                    if(strlen(msgdir->d_name) != 4)
                        continue;
                    if (atoi(msgdir->d_name) > latest){
                        latest = atoi(msgdir->d_name);
                    }
                }
                if (latest < 10)
                    sprintf(MID, "000%d", latest);
                else if (latest < 100)
                    sprintf(MID, "00%d", latest);
                else if (latest < 1000)
                    sprintf(MID, "0%d", latest);
                else if (latest < 10000)
                    sprintf(MID, "%d", latest);
            
                closedir(d_msg);
            }

            size_t offset = sprintf(p, " %s %s %s", GID, GName, MID);
            p += offset;
            ++n_groups;
            if(n_groups==99)
                break;
        }
        closedir(d_GRP);

        if (n_groups == 0)
            sprintf(out, "RGM 0\n");
        else{
            sprintf(out, "RGM %d%s\n", n_groups, list_groups);
        }
        return out;
    }
    return "RGM E_USR\n";
}

void listUsers_GIDS(char *input,int connfd){
    char GID[3], group_dirname[10],G_Name_path[29];
    
    char buff[506];
    DIR * dirp;
    struct dirent * entry;
    strncpy(GID,input,2); GID[2] = '\0';
    sprintf(group_dirname,"GROUPS/%s",GID); group_dirname[9]='\0';
    dirp = opendir(group_dirname);
    if(dirp==NULL){
        if(write(connfd,"RUL NOK\n",strlen("RUL NOK\n"))<0){
            fprintf(stderr,"ERROR");
            return;
        }
    }

    sprintf(G_Name_path,"GROUPS/%s/%s_name.txt",GID,GID);
    char G_Name[25];
    FILE *Gname_ptr = fopen(G_Name_path, "r");
    if (Gname_ptr){
        if(fscanf(Gname_ptr, "%s", G_Name)==0){
            exit(EXIT_FAILURE);
        }
        fclose(Gname_ptr);
    }
    else{
        writeTCP(connfd, 9, "RUL NOK\n");
        return;
    }

    char *p = buff;
    size_t offset = sprintf(buff,"RUL OK %s",G_Name);
    p += offset;

    int i=strlen(G_Name)+8;
    while ((entry = readdir(dirp)) != NULL) {
        if(entry->d_name[0]=='.')
            continue; 
        if(strlen(entry->d_name)<5)
            continue;
        else{
            char user_file[6];
            sscanf(entry->d_name,"%[^.]txt",user_file);
        
            if(validUID(user_file)){
                offset = sprintf(p, " %s", user_file);
                p += offset;
                i+=6;
            }
            else{
                continue;
            }
            
            if(i>470){
                writeTCP(connfd, i, buff);
                i=0;
                p = buff;
            }
        }

    }
    sprintf(p, "\n");
    writeTCP(connfd, strlen(buff), buff);
    
    closedir(dirp);
}

void postMessageS(int connfd){
    char UID[6],GID[3],textSizeC[4],temp[MAX_TEXT_SIZE], text[MAX_TEXT_SIZE], FName[MAX_FNAME_SIZE];
    int i=0, textSize, withFile = 0, fileSize=0;

    //get UID
    readTCP(connfd, 5, UID);
    UID[5] = '\0';

    if(!validUID(UID)){
        sprintf(out, "RPT NOK\n");
        writeTCP(connfd,strlen(out), out);
        return;
    }

    //get whitespace
    readTCP(connfd,1,temp);
    if(temp[0] != ' '){
        sprintf(out, "RPT NOK\n");
        writeTCP(connfd,strlen(out), out);
        return;
    }
    temp[0] = '\0';

    //get GID
    readTCP(connfd, 2, GID);

    if(!validGID(GID)){
        sprintf(out, "RPT NOK\n");
        writeTCP(connfd,strlen(out), out);
        return;
    }
    
    //get whitespace
    readTCP(connfd,1,temp);
    if(temp[0] != ' '){
        sprintf(out, "RPT NOK\n");
        writeTCP(connfd,strlen(out), out);
        return;
    }
    temp[0] = '\0';

    readTCP(connfd, 1, temp);
    while(temp[0] != ' '){
        if(i >= 3){
            sprintf(out, "RPT NOK\n");
            writeTCP(connfd,strlen(out), out);
            return;
        }
        textSizeC[i] = temp[0];
        i++;
        readTCP(connfd, 1, temp);
    }
    textSizeC[i] = '\0';
    textSize = atoi(textSizeC);
    if(textSize > 240){
        sprintf(out, "RPT NOK\n");
        writeTCP(connfd,strlen(out), out);
        return;
    }

    //get text
    readTCP(connfd, textSize, text);
    text[textSize] = '\0';
    
    //get whitespace to see if it has a file or not
    readTCP(connfd, 1, temp);
    if (temp[0] == ' '){
        withFile=1;
    }
    temp[0] = '\0';

    if(withFile){
        //get FName
        i=0;
        readTCP(connfd, 1, temp);
        while(temp[0] != ' '){
            if (i > 23){
                sprintf(out, "RPT NOK\n");
                writeTCP(connfd,strlen(out), out);
                return;
            }
            FName[i] = temp[0];
            i++;
            readTCP(connfd, 1, temp);
        }
        FName[i] = '\0';

        //get file size
        temp[0] = '\0';
        i=0;
        while(temp[0] != ' '){
            if (i > 9){
                sprintf(out, "RPT NOK\n");
                writeTCP(connfd,strlen(out), out);
                return;
            }
            if (i==0){
                fileSize = atoi(temp);
            }
            else{
                fileSize = fileSize*10 + atoi(temp);
            }
            readTCP(connfd, 1, temp);
            i++;
        }
    }

    DIR *d_GRP, *d_GID, *d_MSG, *d_MID;
    struct dirent *dir_grp;
    int MID=0, max=0;
    char UIDfname[20], MID_C[14],MIDpath[28],filePath[45],GIDpath[10],MSGpath[14],rcvFilePath[53];

    d_GRP = opendir("GROUPS");
    if (d_GRP){
        while ((dir_grp = readdir(d_GRP)) != NULL){
            //check if group exists
            if(dir_grp->d_name[0]=='.')
                continue;
            if (strcmp(dir_grp->d_name, GID) == 0){
                //open group folder
                sprintf(GIDpath, "GROUPS/%s",GID);
                d_GID = opendir(GIDpath);
                if (d_GID){
                    //check if user is subscribed to group
                    sprintf(UIDfname, "GROUPS/%s/%s.txt", GID, UID);
                    if(access(UIDfname,F_OK)){
                        closedir(d_GID);
                        closedir(d_GRP);
                        sprintf(out, "RPT NOK\n");
                        writeTCP(connfd, strlen(out), out);
                        return;
                    }
                    //open MSG folder
                    sprintf(MSGpath, "GROUPS/%s/MSG",GID);
                    d_MSG = opendir(MSGpath);
                    if (d_MSG){
                        //get next MID
                        while ((dir_grp = readdir(d_MSG)) != NULL){
                            if(dir_grp->d_name[0]=='.')
                                continue;
                            MID = atoi(dir_grp->d_name);
                            if (MID > max)
                                max = MID;
                        }
                        if(max == 9999){
                            closedir(d_MSG);
                            closedir(d_GID);
                            closedir(d_GRP);
                            sprintf(out, "RPT NOK\n");
                            writeTCP(connfd,strlen(out), out);
                            return;
                        }
                        MID = max+1;

                        if(getIntLength(MID) == 1){
                            sprintf(MID_C, "000%d", MID);
                        }
                        else if(getIntLength(MID) == 2){
                            sprintf(MID_C, "00%d", MID);
                        }
                        else if(getIntLength(MID) == 3){
                            sprintf(MID_C, "0%d", MID);
                        }
                        else if(getIntLength(MID) == 4){
                            sprintf(MID_C, "%d", MID);
                        }

                        sprintf(MIDpath, "GROUPS/%s/MSG/%s", GID, MID_C);

                        //create MID folder
                        if(!createDir(MIDpath)){
                            closedir(d_MSG);
                            closedir(d_GID);
                            closedir(d_GRP);
                            sprintf(out, "RPT NOK\n");
                            writeTCP(connfd,strlen(out), out);
                            return;
                        }

                        //open MID folder
                        d_MID = opendir(MIDpath);
                        if (d_MID){
                            //create Author, Text (and File) files
                            sprintf(filePath, "GROUPS/%s/MSG/%s/A U T H O R.txt",GID, MID_C);
                            if (!createFile(filePath, UID)){
                                sprintf(out, "RPT NOK\n");
                                writeTCP(connfd,strlen(out), out);
                                return;
                            }
                            filePath[0] = '\0';

                            sprintf(filePath, "GROUPS/%s/MSG/%s/T E X T.txt",GID, MID_C);
                            if (!createFile(filePath, text)){
                                closedir(d_MID);
                                closedir(d_MSG);
                                closedir(d_GID);
                                closedir(d_GRP);
                                sprintf(out, "RPT NOK\n");
                                writeTCP(connfd,strlen(out), out);
                                return;
                            }
                            filePath[0] = '\0';

                            if(withFile){
                                //if there's a file attached to the message
                                //read from tcp
                                ssize_t nbytes = fileSize, nread, nleft;
                                char buffer[513];
                                FILE *fileptr;

                                sprintf(rcvFilePath,"GROUPS/%s/MSG/%s/%s",GID,MID_C,FName);

                                fileptr = fopen(rcvFilePath, "wb");
                                if (fileptr == NULL){
                                    closedir(d_MID);
                                    closedir(d_MSG);
                                    closedir(d_GID);
                                    closedir(d_GRP);
                                    sprintf(out, "RPT NOK\n");
                                    writeTCP(connfd,strlen(out), out);
                                    return;
                                }

                                nleft = nbytes;
                                while(nleft>0){
                                    if (nleft > 512){
                                        buffer[0] = '\0';
                                        nread = readTCP(connfd, 512, buffer);
                                        buffer[nread] = '\0';
                                        if(nread==-1){
                                            closedir(d_MID);
                                            closedir(d_MSG);
                                            closedir(d_GID);
                                            closedir(d_GRP);
                                            sprintf(out, "RPT NOK\n");
                                            writeTCP(connfd,strlen(out), out);
                                            return;
                                        }
                                        else if(nread==0)
                                            break;

                                        if (!fwrite(buffer, 1, nread, fileptr))
                                            return;
                                        nleft -= nread;
                                    }
                                    else{
                                        buffer[0] = '\0';
                                        nread = readTCP(connfd,nleft, buffer);
                                        buffer[nread] = '\0';
                                        if(nread==-1){
                                            closedir(d_MID);
                                            closedir(d_MSG);
                                            closedir(d_GID);
                                            closedir(d_GRP);
                                            sprintf(out, "RPT NOK\n");
                                            writeTCP(connfd,strlen(out), out);
                                            return;
                                        }
                                        if (!fwrite(buffer, 1, nread, fileptr))
                                            return;
                                        nleft -= nread;
                                    }
                                }
                                fclose(fileptr);
                            }
                            closedir(d_MID);
                        }
                        closedir(d_MSG);
                    }
                    closedir(d_GID);
                }
                if (v_mode){
                    if (withFile){
                        printf("UID=%s: post group: %s - \"%s\" %s\n", UID, GID, text, FName);
                    }
                    else{
                        printf("UID=%s: post group: %s - \"%s\"\n", UID, GID, text);
                    }
                }
                sprintf(out, "RPT %s\n",MID_C);
                writeTCP(connfd, strlen(out), out);
                closedir(d_GRP);
                return;
            }
        }
        closedir(d_GRP);
    }
    sprintf(out, "RPT NOK\n");
    writeTCP(connfd,strlen(out), out);
    return;
}

void retrieveMessagesS(char *input, int connfd){
    if (!validUID(input) || (input[5] != ' ')){
        writeTCP(connfd, 9, "RRT NOK\n");
        return;
    }
    if (!validGID(&input[6]) || (input[8] != ' ')){
        writeTCP(connfd, 9, "RRT NOK\n");
        return;
    }
    if (!validMID(&input[9]) || (input[13] != '\n')){
        writeTCP(connfd, 9, "RRT NOK\n");
        return;
    }

    char UID[6], GID[3], MID[5];

    strncpy(UID, input, 5); UID[5] = '\0';
    strncpy(GID, &input[6], 2); GID[2] = '\0';
    strncpy(MID, &input[9], 4); MID[4] = '\0';

    DIR *d_USR, *d_GRP;
    struct dirent *dir_usr, *dir_grp;
    int valid = 0, i = 0;
    d_USR = opendir("USERS");
    if (d_USR){
        while ((dir_usr = readdir(d_USR)) != NULL){
            if(dir_usr->d_name[0]=='.')
                continue;
            if (strcmp(dir_usr->d_name, UID) == 0){
                valid = 1;
                break;
            }
            if (i == 99999)
                break;
            i++;
        }
        closedir(d_USR);
        if (!valid){
            writeTCP(connfd, 9, "RRT NOK\n");
            return;
        }
    }
    else{
        writeTCP(connfd, 9, "RRT NOK\n");
        return;
    }

    valid = 0;

    d_GRP = opendir("GROUPS");
    if (d_GRP){
        while ((dir_grp = readdir(d_GRP)) != NULL){
            if(dir_grp->d_name[0]=='.')
                continue;
            if (strcmp(dir_grp->d_name, GID) == 0){
                valid = 1;
            }
            if (i == 99){
                closedir(d_GRP);
                writeTCP(connfd, 9, "RRT NOK\n");
                return;
            }
            i++;
        }
        closedir(d_GRP);
        if (!valid){
            writeTCP(connfd, 9, "RRT NOK\n");
            return;
        }
    }
    else{
        writeTCP(connfd, 9, "RRT NOK\n");
        return;
    }

    char GMIDpath[30];
    sprintf(GMIDpath, "GROUPS/%s/MSG", GID);
    
    struct dirent *dir_msg;
    int n_msgs = 0;
    DIR *d_MID = opendir(GMIDpath);
    if (d_MID){
        while ((dir_msg = readdir(d_MID)) != NULL){
            if(dir_msg->d_name[0]=='.')
                continue;
            if (atoi(dir_msg->d_name) >= atoi(MID)){
                n_msgs += 1;
            }
            if (i == 9999){
                closedir(d_MID);
                writeTCP(connfd, 9, "RRT NOK\n");
                return;
            }
            if (n_msgs == 20){
                break;
            }
            i++;
        }
        closedir(d_MID);
    }
    else{
        writeTCP(connfd, 9, "RRT NOK\n");
        return;
    }

    if (n_msgs == 0){
        if (v_mode)
            printf("UID=%s: retrieve group %s, no messages to retrieve\n", UID, GID);
        writeTCP(connfd, 11, "RRT EOF 0\n");
        return;
    }

    if (v_mode)
        printf("UID=%s: retrieve group %s, %d message(s):\n", UID, GID, n_msgs);

    char *p = out;
    size_t offset = sprintf(p, "RRT OK %d", n_msgs);
    p += offset;

    d_MID = opendir(GMIDpath);
    if (d_MID){
        while ((dir_msg = readdir(d_MID)) != NULL){
            if(dir_msg->d_name[0]=='.')
                continue;
            if (n_msgs == 0){
                writeTCP(connfd, 1, "\n");
                break;
            }
            if (atoi(dir_msg->d_name) >= atoi(MID)){
                char MUIDpath[40], MTEXTpath[40], MID_tmp[5];

                strncpy(MID_tmp, dir_msg->d_name, 4);
                MID_tmp[4] = '\0';
                
                sprintf(MUIDpath, "GROUPS/%s/MSG/%s/A U T H O R.txt", GID, MID_tmp);
                FILE *fp = fopen(MUIDpath, "r");
                if (fp){
                    if (!fscanf(fp, "%s", UID))
                        return;
                    fclose(fp);
                }
                else{
                    writeTCP(connfd, 9, "RRT NOK\n");
                    return;
                }

                sprintf(MTEXTpath, "GROUPS/%s/MSG/%s/T E X T.txt", GID, MID_tmp);
                fp = fopen(MTEXTpath, "rb");
                char text[MAX_TEXT_SIZE];
                if (fp){
                    fseek(fp, 0, SEEK_END);
                    int filesize = ftell(fp);
                    fseek(fp,0,SEEK_SET);
                    if (!fread(text, 1, filesize, fp))
                        return;
                    text[filesize] = '\0';
                    fclose(fp);
                    sprintf(p, " %s %s %d %s", MID_tmp, UID, filesize, text);
                    writeTCP(connfd, strlen(out), out);
                    p = out;
                }
                else{
                    writeTCP(connfd, 9, "RRT NOK\n");
                    return;
                }
                char MIDpath[40], FName[260];
                sprintf(MIDpath, "GROUPS/%s/MSG/%s", GID, MID_tmp);
                DIR *d_msg = opendir(MIDpath);
                int withFile = 0;
                while ((dir_msg = readdir(d_msg)) != NULL){
                    if(dir_msg->d_name[0]=='.')
                        continue;
                    if (dir_msg->d_type == DT_REG){
                        if (strcmp(dir_msg->d_name, "A U T H O R.txt") != 0){
                            if (strcmp(dir_msg->d_name, "T E X T.txt") != 0){
                                char MFILEpath[300];
                                sprintf(MFILEpath, "GROUPS/%s/MSG/%s/%s", GID, MID_tmp, dir_msg->d_name);
                                FILE *fptr = fopen(MFILEpath, "rb");
                                sprintf(FName, "%s", dir_msg->d_name);
                                if (fptr){
                                    withFile = 1;
                                    fseek(fptr, 0, SEEK_END);
                                    int filesize = ftell(fptr);
                                    sprintf(out, " / %s %d ", dir_msg->d_name, filesize);
                                    writeTCP(connfd, strlen(out), out);
                                    fseek(fptr,0,SEEK_SET);
                                    while (filesize > 0){
                                        int nwritten;
                                        if (filesize > 512){
                                            if (!fread(out, 1, 512, fptr))
                                                return;
                                            out[512] = '\0';
                                            nwritten = writeTCP(connfd, 512, out);
                                        }
                                        else{
                                            if (!fread(out, 1, filesize, fptr))
                                                return;
                                            out[filesize] = '\0';
                                            nwritten = writeTCP(connfd, filesize, out);
                                        }
                                        filesize -= nwritten;
                                    }
                                    fclose(fptr);
                                }
                            }
                        }
                    }
                }
                if (v_mode){
                    if (withFile){
                        printf("%s - %s \"%s\" %s\n", MID_tmp, UID, text, FName);
                    }
                    else{
                        printf("%s - %s \"%s\"\n", MID_tmp, UID, text);
                    }
                }
                n_msgs--;
            }
        }
        closedir(d_MID);
    }
    else{
        writeTCP(connfd, 9, "RRT NOK\n");
        return;
    }

    return;
    
}
