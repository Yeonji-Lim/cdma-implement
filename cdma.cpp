#include <iostream>
#include <sys/types.h> //시스템에서 사용하는 자료형에 관한 정보들
#include <unistd.h> //Unix에서 사용하는 C 컴파일러 헤더 파일
using namespace std;

#define BUF_SIZE 30

int main() {
    int fd[2];
    char str[] = "Who are you?";
    char buf[BUF_SIZE];
    pid_t pid;

    pipe(fd); //정수형 배열의 주소값을 전달 : fd[0]과 fd[1]에는 출구, 입구에 해당하는 파일 디스크립터가 할당된다.

    pid=fork(); //자식 프로세스 생성 : 파이프 출구, 입구에 해당하는 파일 디스크립터 복사

    if(pid==0) { //자식 프로세스인 경우 
        write(fd[1], str, sizeof(str)); //파이프의 입구로 메시지를 보냄
        cout <<"I'm child"<< endl;
    } else { //부모 프로세스인 경우
        read(fd[0], buf, BUF_SIZE); //파이프 출구로 부터 값을 읽어들이고 buf에 저장
        cout <<buf<< endl;
    }
    return 0;
}