#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include "func_Server.h"

char out[MAX_OUT_SIZE];

char* processInput(char *input){
    char command[4], *out;
    strncpy(command, input, 3);
    command[3] = '\0';

    if (strcmp(command, "REG") == 0){
        if (input[3] == ' ')
            out = registerUserS(&input[4]);
        else{
            sprintf(out, "ERR\n");
        }
    }
    else if (strcmp(command, "UNR") == 0){
        if (input[3] == ' ')
            out = unregisterUserS(&input[4]);
        else{
            sprintf(out, "ERR\n");
        }
    }
    else if (strcmp(command, "LOG") == 0){
        if (input[3] == ' ')
            out = loginUserS(&input[4]);
        else{
            sprintf(out, "ERR\n");
        }
    }
    else if (strcmp(command, "OUT") == 0){
        if (input[3] == ' ')
            out = logoutUserS(&input[4]);
        else{
            sprintf(out, "ERR\n");
        }
    }
    else if (strcmp(command, "GLS") == 0){
        if (input[3] == '\n')
            out = showAvailableGroupsS();
        else{
            sprintf(out, "ERR\n");
        }
    }
    else if (strcmp(command, "GSR") == 0){
        if (input[3] == ' ')
            out = subscribeGroupS(&input[4]);
        else{
            sprintf(out, "ERR\n");
        }
    }/*
    else if (strcmp(command, "GUR") == 0){
        if (input[3] == ' ')
            out = unsubscribeGroupS(&input[4]);
        else{
            sprintf(out, "ERR\n");
        }
    }
    else if (strcmp(command, "GLM") == 0){
        out = showMyGroupsS();
    }
    else if (strcmp(command, "ULS") == 0){
        out = listUsers_GIDS();
    }
    else if (strcmp(command, "PST") == 0){
        out = postMessageS();
    }
    else if (strcmp(command, "RTV") == 0){
        out = retrieveMessagesS();
    }*/
    else{
        sprintf(out, "ERR\n");
    }
    return out;
}

int validUID(char *input){
    for (int i = 0; ((input[i] != ' ') && (input[i] != '\n'));i++){
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

int createDir(char *path){
    int ret = mkdir(path, 0070);
    if (ret == -1)
        return 0;
    return 1;
}

int createFile(char *path, char *content){
    FILE *fPtr = fopen(path, "w");
    if (fPtr == NULL)
        return 0;
    fputs(content, fPtr);
    fclose(fPtr);
    return 1;
}

int deleteFile(char *path){
    if(unlink(path)==0)
        return(1);
    else
        return(0);
}

int deleteDir(char *path){
    if(rmdir(path)==0)
        return(1);
    else
        return(0);
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

    strncpy(UID, input,5); UID[5] = '\0';
    strncpy(password, &input[6],8); password[8] = '\0';
    sprintf(user_dirname,"USERS/%s",UID);
    if(access(user_dirname, F_OK ) == 0 ){
        
        sprintf(out,"RRG DUP\n");
    }
    else{
        char user_password[30];
    
        mkdir(user_dirname,0700);
        sprintf(user_password,"USERS/%s/%s_pass.txt",UID,UID);
        FILE *password_file;
        password_file=fopen(user_password,"w");
        fputs(password,password_file);
        fclose(password_file);
        sprintf(out,"RRG OK\n");
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
    fscanf(pass_txt, "%s", realPassword);
    fclose(pass_txt);

    if(strcmp(password,realPassword)==0){
        if(access(loginPath, F_OK ) == 0 ){
            if (!deleteFile(loginPath)){
                return "RUN NOK\n";
            }
            if (deleteFile(passPath) && deleteDir(UIDPath)){
                printf("UID=%s: unregistered user\n", UID);
                return "RUN OK\n";
            }
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
            sprintf(out,"ERR\n");
        }
        fclose(password_ptr);
        file_password[8]='\0';

        if(memcmp(file_password,password,9)==0){
            sprintf(out,"RLO OK\n");
            sprintf(user_dirname,"USERS/%s/%s_login.txt",UID,UID);
            FILE *login_ptr = fopen(user_dirname,"w");
            if(login_ptr==NULL){
                sprintf(out,"ERR\n");
            }
            else{
                printf("UID=%s: login ok\n", UID);
            }
            fclose(login_ptr);
        }
        else{
            sprintf(out,"RLO NOK\n");
        }
    }
    else {
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
    fscanf(pass_txt, "%s", realPassword);
    fclose(pass_txt);

    sprintf(pathname,"USERS/%s/%s_login.txt",UID,UID);

    if(strcmp(password,realPassword)==0){
        if(deleteFile(pathname)){
            printf("UID=%s: logout ok\n", UID);
            return "ROU OK\n";
        }
        else{
            return "ROU NOK\n";
        }
    }
    else{

        return "ROU NOK\n";
    }
}

char *showAvailableGroupsS(){
    char list_groups[MAX_OUT_SIZE-6];
    char GNamepath[30], MIDpath[30], GID[3], GName[24], MID[5];
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
            if(strlen(dir->d_name)>2)
                continue;
            strcpy(GID, dir->d_name);
            GID[2] = '\0';
            sprintf(GNamepath,"GROUPS/%s/%s_name.txt", GID, GID);
            fp=fopen(GNamepath,"r");
            if(fp){
                fscanf(fp,"%s", GName);
                fclose(fp);
            }
            sprintf(MIDpath, "GROUPS/%s/MSG", GID);
            DIR *d_msg = opendir(MIDpath);
            struct dirent *msgdir;
            if (d_msg){
                strcpy(MID, "0000");
                while ((msgdir = readdir(d_msg)) != NULL){
                    strcpy(MID, msgdir->d_name);
                }
                MID[4] = '\0';
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
                printf("ok\n");
                printf("dir found, going to compare strings: %s %s\n", dir_grp->d_name, GID);
                if (strcmp(dir_grp->d_name, GID) == 0){
                    char *GNamepath;
                    sprintf("GROUPS/%s/%s_name.txt", GID, GID);
                    printf("sprintf groupname ok\n");
                    FILE *fp = fopen(GNamepath, "r");
                    char *content;
                    if (fp){
                        printf("before scanffile\n");
                        fscanf(fp,"%s", content);
                        fclose(fp);
                        printf("scanf ok\n");
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

            char *GUIDpath;
            sprintf(GUIDpath, "GROUPS/%s/%s.txt", GID, UID);
            if (!createFile(GUIDpath, UID))
                return "RGS NOK\n";
            return "RGS OK\n";
        }
        else{
            char newGID[3];
            strcpy(newGID, "01"); newGID[2] = '\0';
            while ((dir_grp = readdir(d_GRP)) != NULL){
                strncpy(newGID, dir_grp->d_name, 2);
                newGID[2] = '\0';
            }
            closedir(d_GRP);
            if (atoi(newGID) == 99)
                return "RGS E_FULL\n";
            int next = atoi(newGID) + 1;
            if (next < 10)
                sprintf(newGID, "0%d", next);
            else
                sprintf(newGID, "%d", next);

            char *GRPpath;
            sprintf(GRPpath,"GROUPS/%s",newGID);
            if (!createDir(GRPpath))
                return "RGS NOK\n";

            sprintf(GRPpath, "GROUPS/%s/%s_name.txt", newGID, newGID);
            if (!createFile(GRPpath, newGID))
                return "RGS NOK\n";

            sprintf(GRPpath, "GROUPS/%s/%s.txt", newGID, UID);
            if (!createFile(GRPpath, UID))
                return "RGS NOK\n";

            sprintf(out, "NEW %s\n", newGID);
            return out;
        }
    }

}
