# RC
gcc -O3 -Wall UDP_Client.c TCP_Client.c func_Client.c user.c -lm -o user

latest commit: all UDP comands done

sendTCP alternative

    while (1){
        n=read(fd_tcp, buffer, max);
        if(n==-1)
            exit(1);
        buffer[n] = '\0';
        buffer_number++;
        //printf("%ld\n",strlen(buffer));
        if(strlen(buffer) != MAX_OUTPUT_SIZE-1 && strlen(buffer)==0 ){
            saved_buffer[ptr]='\0';
          break;
        }
        else{
            //printf("buffer_number=%d\n",buffer_number);
            //printf("%s",buffer);
            for(int i=0;i<strlen(buffer);i++){
                saved_buffer[ptr]=buffer[i];
                ptr+=1;
            }
            saved_buffer[ptr]=' ';
            
        }
    }
    //printf("ptr=%d\n",ptr);
    //printf("%s",saved_buffer);
    close(fd_tcp);
    
    return saved_buffer;
