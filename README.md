# cdma-implement
cdma 프로그램 구현하기

근데 이제 시스템 콜 리뷰를 곁들인..(시스템 콜 설명 : System call 종류.docx)

## 조건
> 칩 시퀀스 길이 : 8
>
> 송신국 수 : 4
>
> 프로세스간의 통신은 파이프를 사용
>
> 프로세스 종류 수 : 3 
>
>> 송신기 프로세스(transmitter processes) : t0, t1, t2, t3
>>
>> 조이너 프로세스(joiner process) : main program
>>
>> 수신기 프로세스(receiver processes) : r0, r1, r2, r3
>
> 1. main(joiner process)에서 4개의 8비트 칩 시퀀스(bipolar notation)와 4개의 전송 비트를 입력 받음 (1 bit per transmitter process to be transmitted)
>
> 2. 각 송신기 프로세스는 입력받은 칩 시퀀스를 계산하여 main에게 전송
>
> 3. main에서 각 신호를 모두 결합한 신호를 수신기 프로세스로 전송
>
> 4. 수신기 프로세스는 전달 받은 시퀀스와 원래 가지고 있던 칩시퀀스를 inner product하고 칩 시퀀스의 길이로 나눈 값을 출력

## fork
~~~
#include <iostream>
#include <sys/types.h> //시스템에서 사용하는 자료형에 관한 정보들
#include <unistd.h> //Unix에서 사용하는 C 컴파일러 헤더 파일
using namespace std;

int main() {
    fork(); // 자식 프로세스를 생성함 -> 아래의 코드는 부모 프로세스와 자식 프로세스에서 두번 실행 됨
    cout << "Hello World!!" << endl;
    return 0;
}
~~~

## pipe
~~~
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
~~~
