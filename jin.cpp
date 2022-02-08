#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<unistd.h>

int main(){

    int buffer[8]; //파이프에게 전송받은 값
    int pipes[2]; //파이프
    int bit,sum=0;

    if(pipe(pipes)==-1){ //파이프 생성&오류시 오류 메시지 출력
        
        perror("pipe failed");
        exit(1);
    }

    int chip[4][8]={0}; //칩 시퀀스 배열
    int add_chip[8]={0}; //칩 시퀀스 합 배열

    for(int i=0;i<4;i++){ //칩 시퀀스 입력
        printf("%d번째 칩 시퀀스\n",i);

        for(int j=0;j<8;j++){
            scanf("%d",&chip[i][j]);
        }
        
    }
    printf("\n");

    pid_t pid[8]; //프로세스 식별자(pid)/pid는 fork라는 시스템 호출에 의해 반환된다. 

    for(int i=0;i<4;i++){
        pid[i]=fork(); //자식 프로세스 생성

        if(pid[i]<0){ //생성 실패시 오류 메시지 출력
            perror("fork failed\n");
            exit(1);
        }
        else if(pid[i]==0){ //자식 프로세스

            printf("%d번째 전송 비트: ",i);
            scanf("%d",&bit);

            if(bit==0){ //전송받은 비트가 0이면 칩 시퀀스에 -1 곱한다
                for(int j=0;j<8;j++){
                    chip[i][j]*=-1;
                }
            }

            //부모 프로세스로 칩 시퀀스 값 전송
            write(pipes[1],chip[i],32);
            exit(1);
        }
        else{ //부모 프로세스

            //자식 프로세스에게 칩 시퀀스 수신(buffer에 저장)
            read(pipes[0],buffer,32);

            //칩 시퀀스 합을 저장한다
            for(int j=0;j<8;j++){
                add_chip[j]+=buffer[j];
            }
            wait(NULL); //자식 프로세스 종료될 때까지 대기
        }
        
    }

    printf("\n시퀀스 합: ");

    for(int i=0;i<8;i++){
            printf("%d ",add_chip[i]); //칩 시퀀스 합
    }
    printf("\n\n");

    for(int i=4;i<8;i++){

        pid[i]=fork(); //자식 프로세스 생성

        //printf("%d번째 pid:%d\n",i-4,pid[i]);

        if(pid[i]<0){ //생성 실패시 오류 메시지 출력
                perror("fork failed\n");
                exit(1);
        }
        else if(pid[i]==0){ //자식 프로세스

            //부모 프로세스에게 시퀀스 합 받음(buffer에 있음)
            read(pipes[0],buffer,32);

            sum=0;

            for(int j=0;j<8;j++){

                //각 칩 시퀀스*합 시퀀스
                buffer[j]*=chip[i-4][j];
                sum+=buffer[j]; //곱한 값을 다 합한다
            }

            printf("%d번째 수신 비트: %d\n\n",i-4,sum/8); //normalize
            printf("\n");
            exit(1);
        }
        else{

            printf("%d번째 부모 프로세스\n",i-4);
            
            //자식 프로세스에 합 시퀀스 값 전송
            write(pipes[1],add_chip,32);
            wait(NULL);
            
        }
    }
    
    return 0;

}