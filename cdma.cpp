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

    //칩 시퀀스 입력 및 저장
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
    pid_t tpid[NUM_TRANS];      //4개의 송신기 프로세스를 생성해야 함(transmitter)
    int tbit;                   //전송 비트
    int tcs[8];                 //송신 신호 (transmitter -> joiner)
    int buf[8];                 //수신 신호 (joiner <- transmitter)
    int acs[8]={0};             //신호들의 합
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

            //파이프로 joiner에게 전송 비트에 따른 시퀀스를 전송한다. 
            write(fd[1], tcs, sizeof(tcs));

            //해당 transmitter process 종료
            exit(1);

        } else { //부모 프로세스인 경우(joiner)
            
            //joiner process는 transmitter process가 전송한 시퀀스를 파이프를 통해 수신한다.
            read(fd[0], buf, sizeof(buf));

            //수신한 시퀀스들의 합을 계산하여 저장한다. 
            for(int j = 0; j < CS_SIZE; j++) {
                acs[j] += buf[j];
            }
        }
    }

    ///////////////////////
    ///// 결합 신호 출력 /////
    ///////////////////////
    cout << "-----What joiner will send to receivers-----" << endl;
    for(int i = 0; i < CS_SIZE; i++) {
        cout << acs[i] << " ";
    }
    cout << endl;

    ///////////////////////
    ///// 수신 결과 출력 /////
    ///////////////////////
    cout << "-----Output of "<<NUM_TRANS<<" receiver processes-----" << endl;
    pid_t rpid[NUM_TRANS];  //4개의 수신기 프로세스를 생성해야 함(receiver)

    for(int i = 0; i < NUM_TRANS; i++) {

        //자식 프로세스 생성(r0, r1, r2, r3) : 파이프 출구, 입구에 해당하는 파일 디스크립터 복사
        rpid[i]=fork(); 

        int rans = 0; //수신 결과 저장

        if(rpid[i]==0) { //자식 프로세스인 경우(receiver)

            //부모 프로세스(joiner)가 파이프에 시퀀스를 전달한 이후에 동작하도록 sleep
            sleep(1);

            //receiver process는 joiner process가 전송한 시퀀스를 파이프를 통해 수신한다.
            read(fd[0], buf, sizeof(buf));

            //수신 결과를 도출한다.
            for(int j = 0; j < CS_SIZE; j++) {
                rans += buf[j] * cs[i][j]; //inner product with chip sequence
            }
            rans /= CS_SIZE; //칩 시퀀스의 길이로 나눈다. 

            //수신 결과를 차례로 출력한다.
            cout << "Output " << i << " : "<< rans << endl;
            
            //수신 결과 변수 초기화
            rans = 0;

            //해당 receiver process 종료
            exit(1);

        } else { //부모 프로세스인 경우(joiner)

            //파이프로 receiver에게 joiner가 계산한(sum) 시퀀스를 전송한다.
            write(fd[1], acs, sizeof(acs));

            //자식 프로세스(receiver)가 동작을 모두 수행하도록 sleep
            sleep(2);
        }
    }

    return 0;
}