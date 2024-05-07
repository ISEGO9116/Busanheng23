#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#define LEN_MIN		15	// 기차 길이
#define LEN_MAX		50
#define STM_MIN		0	// 마동석 체력
#define STM_MAX		5
#define PROB_MIN	10	// 확률
#define PROB_MAX	90
#define AGGRO_MIN	0	// 어그로 범위
#define AGGRO_MAX	5

//마동석 이동 방향
#define MOVE_LEFT	1
#define MOVE_STAY	0

//좀비의 공격 대상
#define ATK_NONE		0
#define ATK_CITIZEN		1
#define ATK_DONGSEOK	2

//마동석 행동
#define ACTION_REST		0
#define ACTION_PROVOKE	1 //도발하기?
#define ACTION_PULL		2 //붙들기

//전역 변수
int pos[3]; // 0: 시민, 1: 좀비, 2: 마동석
char symbols[3] = { 'C', 'Z', 'M' }; //시민/좀비/마동석의 심볼
int isSwitch = 0; //턴 스위치

//입력 함수
int Input(char *string, int min, int max) {
	//단축하려면 
	// 파라미터로 문자열 train length(%d~%d)>> 넣고,
	// 범위값 LEN_MIN, LEN_MAX 넣고,
	// 자체적으로 do while로 오류없을때까지 입력받음
	// 내부적으로 scanf_s에 담아서 return해야 할 듯
	int _input;
	do {
		printf(string, min, max); //string에 들어있는 변환문자의 표현을 위해 사용함.
		scanf_s("%d", &_input);
	} while (_input < min || _input > max);
	return _input;
}

//열차 상태 초기화
void init_Train(int train_length) {
	pos[0] = train_length - 6; // 시민
	pos[1] = train_length - 3; // 좀비
	pos[2] = train_length - 2; // 마동석
	isSwitch = 0; //기본 false
}

//열차 상태 출력 함수
void print_TrainState(int train_length, int pos[], char symbols[]) {
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
void print_Status(char *string, int state, int pos) {
	if (state == 1) { //1 : 이동
		printf("%s: %d -> %d\n", string, pos + 1, pos);
	}
	else if (state == 2) { //2 : 정지
		printf("%s: stay %d\n", string, pos);
	}
	else if (state == 3) { //3 : 강제 정지
		printf("%s: Stay %d (cannot move)\n", string, pos);
	}
}

//시민 이동 구현 (확률계산까지)
int citizen_Move(int probability) {
	//(100-p)%확률로 시민 이동
	int _chance = rand() % 100;
	if (_chance < (100-probability)) {
		pos[0] -= 1;
		return 1; //state: 시민 이동
	}
	else {
		//p%확률로 시민 대기(=즉, 수치변동 작업만 한다)
		return 2; //state: 시민 대기
	}
}

//좀비 이동 구현(확률계산까지)
int zombie_Move(int probability) {
	if (!isSwitch) { //isSwitch가 0일때(첫실행)
		//(100-p)%의 확률로 제자리에 대기
		if ((rand() % 100) < (100 - probability)) {
			isSwitch = 1; //턴스위칭
			return 2; //좀비 대기
		}
		else { //p%의 확률로 1칸이동
			pos[1] -= 1;
			isSwitch = 1; //턴스위칭
			return 1;
		}
	}
	else { //isSwitch가 1일때
		isSwitch = 0; //턴스위칭
		return 3; //강제 정지
		//리턴 : state
	}
}

//게임종료.
void GameOver() { 
	if (pos[0] == 1) {
		printf("SUCCESS!\n");
		printf("citizen(s) escaped to the next train");
		printf("\n============================");
	}
	else if (pos[1] - pos[0] <= 1) {
		printf("GAME OVER!\n");
		printf("Citizen(s) has(have) been attacked by a zombie\n");
		printf("\n============================");
	}
}

// =================================================================================

//메인
int main() {
	//변수
	int train_length = 0;				// 열차 길이
	int stamina = 0;					//
	int probability = 0;				//이동 확률
	
	int citizen_state = 0;				// 초기화
	int zombie_state = 0;				// 초기화
	srand((unsigned int)time(NULL));	//랜덤 모듈 초기화

	//인트로
	printf("게임 시작\n");
	printf("============================\n");

	//열차의 길이, 확률 입력
	while (1)
	{
		//train length
		train_length = Input("train length(%d~%d)>>", LEN_MIN, LEN_MAX);
		stamina = Input("madongseok stamina(%d~%d)>>", STM_MIN, STM_MAX);
		probability = Input("percentile probability 'p'(%d~%d)>>", PROB_MIN, PROB_MAX);

		//마동석
		//미구현
		//여기까지 도착한거면 통과
		break;
	}
	
	// 위치 초기화
	init_Train(train_length); //열차 길이에 따른 위치 초기화

	//초기 열차 상태 출력
	print_TrainState(train_length, pos, symbols);

	//메인로직(시민이동, 좀비이동, 열차상태 출력, 시민/좀비 상태 출력
	while (1)
	{
		if (pos[0] == 1 || pos[1] - pos[0] <= 1) {
			break; //조건 체크후 반복문 탈출 (시민 탈출 조건 충족 OR 좀비-시민(=거리) 1이하)
		}

		//★=====시민 이동=====★
		citizen_state = citizen_Move(probability); //이동시 state=1, 정지시 state=2

		//★=====좀비 이동=====★
		zombie_state = zombie_Move(probability);

		//열차 상태 출력
		print_TrainState(train_length, pos, symbols);

		//시민, 좀비 상태 출력
		print_Status("citizen", citizen_state, pos[0]);
		print_Status("zombie", zombie_state, pos[1]);
	}

	GameOver(); //아웃트로 - 종료상태 출력(성공/실패)
	return 0;
}