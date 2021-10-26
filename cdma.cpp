#include <iostream>
#include <sys/types.h> //시스템에서 사용하는 자료형에 관한 정보들
#include <unistd.h> //Unix에서 사용하는 C 컴파일러 헤더 파일
using namespace std;

int main() {
    fork(); // 자식 프로세스를 생성함 -> 아래의 코드는 부모 프로세스와 자식 프로세스에서 두번 실행 됨
    cout << "Hello World!!" << endl;
    return 0;
}