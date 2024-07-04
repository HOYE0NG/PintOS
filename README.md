# PintOS
PintOS 운영체제 구현 : C 기반 PintOS 운영체제를 시스템콜, 스케쥴러, 가상 메모리 순으로 구현했던 프로젝트이다. 


*** 각각의 프로젝트 디렉터리 내부 20211531 워드 문서를 보면 각각의 프로젝트에 대해 자세한 설명을 확인할 수 있습니다. *** 

## Project 1
### User Program(1)
사용자가 OS와 상호작용할 수 있도록 Argument passing, 기본 시스템콜들과 추가 시스템 콜을 구현하였다.

### 구현 내용
- Argument Passing: 사용자 입력을 파싱하여 스택에 저장하는 기능 구현.
- User Memory Access: 사용자가 접근할 수 없는 메모리에 접근 시 예외 처리를 통해 프로그램 종료.
- System Calls: halt, exit, exec, wait, read, write 등의 시스템 콜 구현.
- 추가 시스템 콜: fibonacci 수열 계산과 네 개의 정수 중 최댓값 반환하는 새로운 시스템 콜 추가.

---

## Project 2
### User Program(2)
파일 시스템 관련 시스템 콜들인 open, close, create, remove, close, filesize, week, tell, read, write를 구현하였다. 또한 구현 중에 생기는 Sychronization 문제를 적절히 해결하여 파일 시스템 관련 시스템 콜들이 정상적으로 작동될 수 있도록 했다.

### 구현 내용
- File Descriptor: 각 프로세스마다 128개의 파일 디스크립터를 관리하는 자료구조를 구현
- System Calls: open, close, create, remove, close, filesize, week, tell, read, write과 같은 파일 시스템 관련 시스템 콜 구현
- Synchronization: 여러 프로세스가 동시에 파일에 접근할 때 발생할 수 있는 문제를 방지하기 위해 동기화 메커니즘 구현. 파일에 대한 읽기/쓰기 작업 시 적절한 lock_acquire와 lock_release 사용.
  
---

## Project 3
### Threads
운영체제의 쓰레드 관리를 위한 Alarm Clock, Priority Scheduling과 추가적으로 Advanced Scheduler(BSD Scheduler)를 구현하였다.

### 구현 내용
- Alarm Clock: 쓰레드를 sleep 상태에서 효율적으로 깨울 수 있도록 구현. 타이머 인터럽트를 통해 sleep 상태의 쓰레드를 깨우고, 남은 시간에 따라 적절히 블록 큐에서 레디 큐로 이동시킴.
- 우선순위 스케줄링: 쓰레드의 우선순위를 고려하여 스케줄링을 진행한다. 새로운 쓰레드가 레디 큐에 들어올 때, 현재 실행 중인 쓰레드보다 우선순위가 높으면 CPU를 양보. 또한 aging을 통해 Starvation을 방지하며 우선순위를 조정.
- BSD Scheduler: MLFQ(multi-level-feedback-queue)를 기반으로 하여 다중 우선순위 큐를 사용. 높은 우선순위의 큐에서 쓰레드가 CPU를 얻도록 하며, 내부에서는 Round-robin 방식을 채택.
- 
---

## Project 4
### Virutal Memory
Stack Growth와 Second Chance 기법을 이용하는 Disk Swap 기능을 구현하여 가상 메모리(Virtual Memory)를 개발하였다.

### 구현 내용
*** Stack Growth 부분만 구현했고 실제로 Page Table 과 Disk Swap 부분은 구현하지 못하였습니다.  ㅠㅠ ***
- ~~Page Table & Page Fault Handler: 페이지 폴트 발생 시 SPT(Supplement Page Table)를 구현하여 처리.~~
- ~~Disk Swap: 물리 메모리에 공간이 부족할 때 Second Chance 기법을 사용하여 스왑 아웃 후 스왑 인을 구현. 스왑 테이블을 관리하여 스왑 디스크를 활용.~~
- Stack Growth: 스택이 할당된 영역을 넘어서 확장할 경우 추가 페이지 할당을 구현. 스택 영역은 8MB로 제한되며, 유효한 스택 확장 요청인지 확인.
