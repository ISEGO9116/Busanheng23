#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#define LEN_MIN		15 // 기차 길이
#define LEN_MAX		50
#define PROB_MIN	10 // 확률
#define PROB_MAX	90

//전역 변수
int pos[3]; // 0: 시민, 1: 좀비, 2: 마동석
char symbols[3] = { 'C', 'Z', 'M' }; //시민/좀비/마동석의 심볼

//열차 초기 상태 출력 -> (개정) 열차 상태 출력 함수
void printTrainState(int train_length, int pos[], char symbols[]) {
	printf("\n");
	for (int i = 0; i < 3; i++) //3줄
	{
		for (int j = 0; j < train_length; j++) //길이 정의
		{
			//중간줄일때 + 시민/좀비/마동석 위치일때
			if (i == 1 && (j == pos[0] || j == pos[1] || j == pos[2]))
			{	//시민/좀비/마동석 위치에다 시민/좀비/마동석 심볼 배치 symbol (0 : (1:2))
				printf("%c", symbols[(j == pos[0]) ? 0 : (j == pos[1]) ? 1 : 2]);
			}
			else {
				//중간줄일때 + 첫칸아님 + 막칸 아님 : 참이면 공백, 아니면 #
				printf("%c", (i == 1 && (j != 0 && j != train_length - 1)) ? ' ' : '#');
			}
		}
		printf("\n"); //3줄 출력후 줄 바꾸기
	}
}

//상태표
//1 : 이동
//2 : 정지
//3 : 강제 정지
void printStatus(char *string, int state, int pos) {
	if (state == 1) {
		printf("%s: %d -> %d\n", string, pos + 1, pos);
	}
	else if (state == 2) {
		printf("%s: stay %d\n", string, pos);
	}
	else if (state == 3) {
		printf("%s: Stay %d (cannot move)\n", string, pos);
	}
}

//시민 이동 구현 (확률계산까지)
int citizen_move(int citizen_state, int probability) {
	//(100-p)%확률로 시민 이동
	if ((rand() % 100) < (100 - probability)) {
		pos[0] -= 1;
		return 1; //state: 시민 이동
	}
	else {
		//p%확률로 시민 대기(=즉, 수치변동 작업만 한다)
		return 2; //state: 시민 대기
	}
}

//좀비 이동 구현(확률계산까지)
void zombie_move(int isSwitch, int zombie_state) {

}

//메인
int main() {
	//변수
	int train_length, probability;		//(입력) 열차 길이, 이동 확률
	int isSwitch;						//턴 스위치
	int citizen_state;					// 초기화
	int zombie_state;					// 초기화
	srand((unsigned int)time(NULL));	//랜덤 모듈 초기화

	//인트로
	printf("게임 시작\n");
	printf("============================\n");

	//열차의 길이, 확률 입력
	while (1)
	{
		//train length
		printf("train length(%d~%d)>>", LEN_MIN, LEN_MAX);
		scanf_s("%d", &train_length);
		if (train_length < LEN_MIN || train_length > LEN_MAX) {
			continue;
		}
		//마동석
		//미구현

		//확률
		printf("percentile probability 'p'(%d~%d)>>", PROB_MIN, PROB_MAX);
		scanf_s("%d", &probability);
		if (probability < PROB_MIN || probability > PROB_MAX) {
			continue;
		}
		//여기까지 도착한거면 통과
		break;
	}
	

	// 위치 초기화
	pos[0] = train_length - 6; // 시민
	pos[1] = train_length - 3; // 좀비
	pos[2] = train_length - 2; // 마동석
	isSwitch = 0; //기본 false
	zombie_state = 0;
	citizen_state = 0;

	//초기 열차 상태 출력
	printTrainState(train_length, pos, symbols);

	//메인로직(시민이동, 좀비이동, 열차상태 출력, 시민/좀비 상태 출력
	while (1)
	{
		//조건 체크후 반복문 탈출 (시민 탈출 조건 충족 OR 좀비-시민(=거리) 1이하)
		if (pos[0] == 1 || pos[1] - pos[0] <= 1) {
			break;
		}

		//★=====시민 이동=====★ //100-p확률로 이동
		//미구현) 확률 적용하여 이동시 state=1, 정지시 state=2
		citizen_state = citizen_move(citizen_state, probability); 


		//★=====좀비 이동=====★
		//만약 1턴이고, (100 - p)% 
		if (!isSwitch) {
			if ((rand()%100) < (100-probability)) {
				//p%의 확률로 이동
				pos[1] -= 1;
				zombie_state = 2; //이동
			}
			else {
				//(100-p)% 확률로 제자리에 대기
				zombie_state = 1;
			}
			isSwitch = 1;
		}
		else{ //2턴
			zombie_state = 3; //강제 대기
			isSwitch = 0;
		}

		Sleep(4000);
		printTrainState(train_length, pos, symbols);

		//시민, 좀비 상태 출력
		printStatus("citizen", citizen_state, pos[0]);
		printStatus("zombie", zombie_state, pos[1]);
	}

	//아웃트로 - 종료상태 출력(성공/실패)
	if (pos[0] == 1) {
		printf("SUCCESS!\n");
		printf("citizen(s) escaped to the next train");
	}
	else if (pos[1] - pos[0] <= 1) {
		printf("GAME OVER!\n");
		printf("Citizen(s) has(have) been attacked by a zombie\n");
	}

	printf("\n============================");
	return 0;
}