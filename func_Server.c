#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char* processInput(char *input){
    char command[4], *out;
    strncpy(command, input, 3);
    command[3] = '\0';

    if (strcmp(command, "REG") == 0){
        out = registerUserS();
    }
    else if (strcmp(command, "UNR") == 0){
        out = unregisterUserS(input);
    }
    else if (strcmp(command, "LOG") == 0){
        out = loginUserS();
    }
    else if (strcmp(command, "OUT") == 0){
        out = logoutUserS();
    }
    else if (strcmp(command, "GLS") == 0){
        out = showAvailableGroupsS();
    }
    else if (strcmp(command, "GSR") == 0){
        out = subscribeGroupS();
    }
    else if (strcmp(command, "GUR") == 0){
        out = unsubscribeGroupS();
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
    }
    return out;
}

char* unregisterUserS(char *input){
    //ir a diretoria users e apagar pasta com UID dado

}

int DelUserDir(char *UID){
    char user_dirname[20];
    sprintf(user_dirname,"USERS/%s",UID);
    if(rmdir(user_dirname)==0)
        return(1);
    else
        return(0);
}

int ListGroupsDir(GROUPLIST *list){
    DIR *d;
    struct dirent *dir;
    int i=0;
    FILE *fp;
    char GIDname[30];
    list->no_groups=0;
    d = opendir("GROUPS");
    if (d){
        while ((dir = readdir(d)) != NULL){
            if(dir->d_name[0]==’.’)
                continue;
            if(strlen(dir->d_name)>2)
                continue;
            strcpy(list->group_no[i], dir->d_name);
            sprintf(GIDname,"GROUPS/%s/%s_name.txt",dir->d_name,dir->d_name);
            fp=fopen(GIDname,"r");
            if(fp){
                fscanf(fp,"%24s",list->group_name[i]);
                fclose(fp);
            }
            ++i;
            if(i==99)
                break;
        }
        list->no_groups=i;
        closedir(d);
    }
    else
        return(-1);

    if(list->no_groups>1)
        SortGList(list);

    return(list->no_groups);
}

int DelPassFile(char *UID){
    char pathname[50];
    sprintf(pathname,"USERS/%s/%s_pass.txt",UID,UID);
    if(unlink(pathname)==0)
        return(1);
    else
        return(0);
}