#include <iostream>
#include <sys/types.h>      //시스템에서 사용하는 자료형에 관한 정보들
#include <unistd.h>         //Unix에서 사용하는 C 컴파일러 헤더 파일
using namespace std;

#define NUM_TRANS 4         //전송 횟수
#define CS_SIZE 8           //각 칩 시퀀스의 길이

int main() {

    ///////////////////////
    ///// 칩 시퀀스 입력 /////
    ///////////////////////
    cout << "-----Enter " << NUM_TRANS << " chip sequences-----" << endl;
    int cs[NUM_TRANS][CS_SIZE]; //칩 시퀀스

    for(int i = 0; i < NUM_TRANS; i++) {
        cout << "Input " << i << " : ";
        for (int j = 0; j < CS_SIZE; j++) {
            cin >> cs[i][j];
        }
        cout << endl;
    }

    ///////////////////////
    ///// 전송 비트 입력 /////
    ///////////////////////
    cout << "-----Enter " << NUM_TRANS << " transmission bits-----" << endl;
    pid_t tpid[NUM_TRANS];      //4개의 송신기 프로세스를 생성해야 함
    int tbit;                   //전송비트 저장
    int tcs[8];                 //송신 신호 (transmitter -> joiner)
    int buf[8];                 //수신 신호 (joiner <- transmitter)
    int acs[8]={0};             //신호들의 합 계산
    int fd[2];                  //파이프에 들어갈 파일 디스크립터
    pipe(fd);                   //정수형 배열의 주소값을 전달 : fd[0]과 fd[1]에는 출구, 입구에 해당하는 파일 디스크립터가 할당된다.

    for(int i = 0; i < NUM_TRANS; i++) {

        //자식 프로세스 생성(t0, t1, t2, t3) : 파이프 출구, 입구에 해당하는 파일 디스크립터 복사
        tpid[i]=fork(); 

        if(tpid[i]==0) { //자식 프로세스인 경우(transmitter)

            //전송 비트를 차례로 입력받는다. 
            cout << "Input " << i << " : ";
            cin.clear();
            cin >> tbit;

            //전송 비트가 0인 경우 1's compliment 수행, 아닌 경우 해당 칩 시퀀스를 그대로 가져온다.
            if(tbit == 0) tbit = -1;
            for(int j = 0; j < CS_SIZE; j++) {
                tcs[j] = cs[i][j] * tbit;
            }

            //파이프라인으로 joiner에게 전송 비트에 따른 시퀀스를 전송한다. 
            write(fd[1], tcs, sizeof(tcs));

            //해당 transmitter process 종료
            exit(1);

        } else { //부모 프로세스인 경우(joiner)
            
            //joiner process는 transmitter process가 전송한 시퀀스를 파이프라인을 통해 수신한다.
            read(fd[0], buf, sizeof(buf));

            //수신한 시퀀스들의 합을 계산하여 저장한다. 
            for(int j = 0; j < CS_SIZE; j++) {
                acs[j] += buf[j];
            }
        }
    }

    return 0;
}