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
#include "Client.h"

char out[MAX_OUT_SIZE];

char* processInput(char *input){
    char command[4], *out=NULL;
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
    }
    else if (strcmp(command, "GUR") == 0){
        if (input[3] == ' ')
            out = unsubscribeGroupS(&input[4]);
        else{
            sprintf(out, "ERR\n");
        }
    }
    else if (strcmp(command, "GLM") == 0){
        if (input[3] == ' ')
            out = showMyGroupsS(&input[4]);
        else{
            sprintf(out, "ERR\n");
        }
    }
    else{
        sprintf(out, "ERR\n");
    }
    if(out!=NULL)
        return out;
    else
        return "ERR\n";
}

void processInputTCP(int connfd, char *command){
    char input[5];

    if(strcmp(command, "PST ")==0){
        postMessageS(connfd);
    }
    else if(strcmp(command, "RTV ")==0){
        printf("RTV identificado\n");
    }
    else if(strcmp(command, "ULS ")==0){
        readTCP(connfd, 2, input);
        listUsers_GIDS(input,connfd);
    }
    else{
        printf("Error: wrong message format\n");
    }
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
    fputs(content, fPtr);
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
    if(fscanf(pass_txt, "%s", realPassword)==0){
        exit(0);
    }
    fclose(pass_txt);

    if(strcmp(password,realPassword)==0){
        if(access(loginPath, F_OK ) == 0 ){
            if (!deleteFile(loginPath)){
                return "RUN NOK\n";
            }
        }
        if (deleteFile(passPath) && deleteDir(UIDPath)){
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
    if(fscanf(pass_txt, "%s", realPassword)==0){
        exit(0);
    }
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
    char list_groups[MAX_OUT_SIZE-20];
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
                if(fscanf(fp,"%s", GName)==0){
                fclose(fp);
                exit(0);
                }
                fclose(fp);
            }
            sprintf(MIDpath, "GROUPS/%s/MSG", GID);
            DIR *d_msg = opendir(MIDpath);
            struct dirent *msgdir;
            if (d_msg){
                strcpy(MID, "0000");
                while ((msgdir = readdir(d_msg)) != NULL){
                    if(msgdir->d_name[0]=='.')
                        continue;
                    if(strlen(msgdir->d_name) != 4)
                        continue;
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

            printf("UID=%s: subscribed group: %s - \"%s\"\n", UID, GID, GName);
            return "RGS OK\n";
        }
        else{
            int nextGID = 1;
            short int max = 1;
            while ((dir_grp = readdir(d_GRP)) != NULL){
                nextGID = atoi(dir_grp->d_name);
                if (nextGID > max)
                    max = nextGID;
            }
            closedir(d_GRP);
            
            if (max == 99)
                return "RGS E_FULL\n";
            max++;
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
            if (strcmp(dir_grp->d_name, GID) == 0){
                valid = 1;
            }
            if (i == 99){
                closedir(d_GRP);
                return "RGU E_FULL\n";
            }
            i++;
        }
        closedir(d_GRP);
        if (!valid)
            return "RGU NOK\n";
        
        char GIDPath[24];
        sprintf(GIDPath, "GROUPS/%s/%s.txt", GID, UID);
        if (!deleteFile(GIDPath)){
            return "RGU NOK\n";
        }

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

    char GNamepath[30], MIDpath[30], UIDPath[30], GID[3], GName[24], MID[5];
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
            if (d_msg){
                strcpy(MID, "0000");
                while ((msgdir = readdir(d_msg)) != NULL){
                    if(msgdir->d_name[0]=='.')
                        continue;
                    if(strlen(msgdir->d_name) != 4)
                        continue;
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

void postMessageS(int connfd){
    char UID[6],GID[3],textSizeC[4],temp[MAX_TEXT_SIZE], text[MAX_TEXT_SIZE], FName[MAX_FNAME_SIZE];
    int i=0, textSize, withFile = 0, fileSize=0;

    //get UID
    readTCP(connfd, 5, UID);
    UID[5] = '\0';

    if(!validUID(UID)){
        sprintf(out, "RPT NOK\n");
        write(connfd,out,strlen(out));
        return;
    }

    //get whitespace
    readTCP(connfd,1,temp);
    if(temp[0] != ' '){
        sprintf(out, "RPT NOK\n");
        write(connfd,out,strlen(out));
        return;
    }
    temp[0] = '\0';

    //get GID
    readTCP(connfd, 2, GID);

    if(!validGID(GID)){
        sprintf(out, "RPT NOK\n");
        write(connfd,out,strlen(out));
        return;
    }
    
    //get whitespace
    readTCP(connfd,1,temp);
    if(temp[0] != ' '){
        sprintf(out, "RPT NOK\n");
        write(connfd,out,strlen(out));
        return;
    }
    temp[0] = '\0';

    //get textSize
    while(temp[0] != ' '){
        readTCP(connfd, 1, temp);
        //verification here
        textSizeC[i] = temp[0];
        i++;
    }
    textSizeC[i] = '\0';
    textSize = atoi(textSizeC);

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
        while(temp[0] != ' '){
            readTCP(connfd, 1, temp);
            //verification here
            FName[i] = temp[0];
            i++;
        }
        FName[i] = '\0';

        //get file size
        temp[0] = '\0';
        i=0;
        while(temp[0] != ' '){
            readTCP(connfd, 1, temp);
            //verification here
            if(temp[0] != ' '){
                if (i==0){
                    fileSize = atoi(temp);
                }
                else{
                    fileSize = fileSize*10 + atoi(temp);
                }
            }   
            i++;
        }
    }
    DIR *d_GRP, *d_GID, *d_MSG, *d_MID;
    struct dirent *dir_grp, *dir_gid, *dir_msg, *dir_mid;
    int MID=0, max=0;
    char UIDfname[20], MID_C[5],MIDpath[19],filePath[45],GIDpath[9],MSGpath[13],rcvFilePath[45];

    d_GRP = opendir("GROUPS");
    if (d_GRP){
        while ((dir_grp = readdir(d_GRP)) != NULL){
            //check if group exists
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
                        write(connfd,out,strlen(out));
                        return;
                    }
                    //open MSG folder
                    sprintf(MSGpath, "GROUPS/%s/MSG",GID);
                    //printf("msg path: %s\n",MSGpath);
                    d_MSG = opendir(MSGpath);
                    if (d_MSG){
                        //get next MID
                        while ((dir_grp = readdir(d_MSG)) != NULL){
                            MID = atoi(dir_grp->d_name);
                            if (MID > max)
                                max = MID;
                        }
                        if(max == 9999){
                            closedir(d_MSG);
                            closedir(d_GID);
                            closedir(d_GRP);
                            sprintf(out, "RPT NOK\n");
                            write(connfd,out,strlen(out));
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
                            write(connfd,out,strlen(out));
                            return;
                        }

                        //open MID folder
                        d_MID = opendir(MIDpath);
                        if (d_MID){
                            //create Author, Text (and File) files
                            sprintf(filePath, "GROUPS/%s/MSG/%s/A U T H O R.txt",GID, MID_C);
                            if (!createFile(filePath, UID)){
                                sprintf(out, "RPT NOK\n");
                                write(connfd,out,strlen(out));
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
                                write(connfd,out,strlen(out));
                                return;
                            }
                            filePath[0] = '\0';

                            if(withFile){
                                //if there's a file attached to the message
                                //read from tcp
                                ssize_t nbytes = fileSize, nread, nleft;
                                char buffer[512];
                                FILE *fileptr;

                                sprintf(rcvFilePath,"GROUPS/%s/MSG/%s/%s",GID,MID_C,FName);

                                fileptr = fopen(rcvFilePath, "wb");
                                if (fileptr == NULL){
                                    closedir(d_MID);
                                    closedir(d_MSG);
                                    closedir(d_GID);
                                    closedir(d_GRP);
                                    sprintf(out, "RPT NOK\n");
                                    write(connfd,out,strlen(out));
                                    return;
                                }

                                buffer[0] = '\0';

                                nleft = nbytes;
                                while(nleft>512){
                                    nread = readTCP(connfd, 512, buffer);
                                    if(nread==-1){
                                        closedir(d_MID);
                                        closedir(d_MSG);
                                        closedir(d_GID);
                                        closedir(d_GRP);
                                        sprintf(out, "RPT NOK\n");
                                        write(connfd,out,strlen(out));
                                        return;
                                    }
                                    else if(nread==0)
                                        break;

                                    fwrite(buffer, nread, 1, fileptr);
                                    nleft -= nread;
                                }
                                buffer[0] = '\0';
                                nread = read(connfd,buffer,nleft);
                                if(nread==-1){
                                        closedir(d_MID);
                                        closedir(d_MSG);
                                        closedir(d_GID);
                                        closedir(d_GRP);
                                        sprintf(out, "RPT NOK\n");
                                        write(connfd,out,strlen(out));
                                        return;
                                    }
                                fwrite(buffer, nread, 1, fileptr);
                                fclose(fileptr);
                            }
                            closedir(d_MID);
                        }
                        closedir(d_MSG);
                    }
                    closedir(d_GID);
                }
            }
        }
        closedir(d_GRP);
        if (withFile){
            printf("UID=%s: post group: %s - \"%s\" %s\n", UID, GID, text, FName);
        }
        else{
            printf("UID=%s: post group: %s - \"%s\"\n", UID, GID, text);
        }
        sprintf(out, "RPT %s\n",MID_C);
        write(connfd,out,strlen(out));
        return;
    }
    sprintf(out, "RPT NOK\n");
    write(connfd,out,strlen(out));
    return;
}

int validUser(char *user_file){
    for (int i=0;user_file[i]!='\0';i++){
        if(!isdigit(user_file[i]))
            return 0;
    }
    return 1;
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
        }
    }
    sprintf(G_Name_path,"GROUPS/%s/%s_name.txt",GID,GID);
    FILE *Gname_ptr;

    Gname_ptr = fopen(G_Name_path, "r");

    char G_Name[25];
    if(fscanf(Gname_ptr, "%s", G_Name)==0){
        exit(0);
    }
    sprintf(buff,"RUL OK %s",G_Name);
    buff[strlen(G_Name)+7]=' ';
    buff[strlen(G_Name)+8]='\0';
    int i=strlen(G_Name)+8;
    while ((entry = readdir(dirp)) != NULL) {
        
        if(entry->d_name[0]=='.')
                continue; 
        if(strlen(entry->d_name)<5)
            continue;
        else{
            char user_file[6];
            sscanf(entry->d_name,"%[^.]txt",user_file);
            user_file[5]='\0';
        
            if(validUser(user_file)){
                strncpy(&buff[i],user_file,6);
                buff[i+5]=' ';
                buff[i+6]='\0';
                i+=6;
            }
            
        if(i>20){
            buff[i]='\n';
            if(write(connfd,buff,(size_t) i)<0){
                exit(0);
            }
            i=0;
            memset(buff, 0, sizeof (buff));
        }
        }

    }
    buff[i]='\n';
    if(write(connfd,buff,strlen(buff))<0) {
        exit(0);
    }
    
    closedir(dirp);
}
