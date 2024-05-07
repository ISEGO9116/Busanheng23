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
int aggro[2]; //어그로. 0: 시민, 1:마동석
int isSwitch = 0; //턴 스위치

//입력 함수
// 파라미터로 문자열 train length(%d~%d)>> 넣고,
// 범위값 LEN_MIN, LEN_MAX 넣고,
// 자체적으로 do while로 오류없을때까지 입력받음
// 내부적으로 scanf_s에 담아서 return해야 할 듯
int Input(char *string, int min, int max) {
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
	printf("\n");
}

//열차 상태 출력 함수
void print_TrainState(int train_length) {
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
	if (state == 1) { //1 : 이동 (좀비 전용)
		printf("%s: %d -> %d\n", string, pos + 1, pos);
	}
	else if (state == 2) { //2 : 정지
		printf("%s: stay %d\n", string, pos);
	}
	else if (state == 3) { //3 : 강제 정지
		printf("%s: Stay %d (cannot move)\n", string, pos);
	}
}

//마동석 전용 출력함수 (마동석 state와 어그로 변화량을 받음)
// 1 : stay
// 2 : 
void print_DongseokPrint(int state, int aggroDelta) {
	if (state == 0) { //휴식
		printf("madongseok: stay %d (aggro: %d->%d)\n", pos[2], (aggro[1] + 1), aggro[1]);
	}
	else if (state == 1) { //이동(어그로1 증가)
		printf("madongseok: %d -> %d (aggro: %d->%d)\n", (pos[2]+1), pos[2], aggro[1], (aggro[1] + aggroDelta));
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
	else { //p%확률로 시민 대기
		return 2; //state: 시민 대기(2)
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

//마동석 이동 : 
// 0/1 입력받아서 행동, 기차 상태 출력
void dongseok_Move(int train_length) {
	int dongseok_state = Input("madongseok move(0:stay, 1:left)>>", 0, 1);
	if (dongseok_state) { //1 입력시,
		pos[2] -= 1; //한칸 이동
		print_TrainState(train_length); //기차 상태 출력
		print_DongseokPrint(dongseok_state, 1); //마동석 상태 출력
	}
	else { //0 입력시,
		// 제자리에 대기 (바로 기차 상태 출력하니까 아무것도 안한다?)
		print_TrainState(train_length);
		print_Status("madongseok", dongseok_state, pos[2]);
		print_DongseokPrint(dongseok_state, 0); //마동석 상태 출력
	}
}

//게임종료.
void GameOver() { 
	if (pos[0] == 1) {
		printf("SUCCESS!\n");
		printf("citizen(s) escaped to the next train");
		printf("\n============================");//
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
	int stamina = 0;					// 마동석 스태미나
	int probability = 0;				// 이동 확률
	int citizen_state = 0;				// 시민 상태
	int zombie_state = 0;				// 좀비 상태
	int citizen_aggro = 1;		// 시민 어그로
	int madongseok_aggro = 1;	// 마동석 어그로
	srand((unsigned int)time(NULL));	//랜덤 모듈 초기화

	//인트로
	printf("게임 시작\n============================\n");

	//★=====초기화=====★
	train_length = Input("train length(%d~%d)>>", LEN_MIN, LEN_MAX);
	stamina = Input("madongseok stamina(%d~%d)>>", STM_MIN, STM_MAX);
	probability = Input("percentile probability 'p'(%d~%d)>>", PROB_MIN, PROB_MAX);
	init_Train(train_length); //열차 길이에 따른 위치 초기화
	print_TrainState(train_length); //초기 열차 상태

	//★=====턴=====★
	while (1)
	{
		printf("\n");
		if (pos[0] == 1 || pos[1] - pos[0] <= 1) {
			break; //조건 체크후 반복문 탈출 (시민 탈출 조건 충족 OR 좀비-시민(=거리) 1이하)
		}

		//★=====<이동> 페이즈=====★
		//시민&좀비 이동
		citizen_state = citizen_Move(probability); //이동시 state=1, 정지시 state=2
		zombie_state = zombie_Move(probability);
		//열차 상태 출력
		print_TrainState(train_length);
		//시민, 좀비 상태 출력
		print_Status("citizen", citizen_state, pos[0]);
		print_Status("zombie", zombie_state, pos[1]);
		//마동석 이동
		dongseok_Move(train_length); //수치 변동까지 완료
		//마동석 상태 출력

		//★=====<행동> 페이즈=====★
		//시민 행동
		//좀비 행동
		//마동석 행동
	}

	GameOver(); //아웃트로 - 종료상태 출력(성공/실패)
	return 0;
}