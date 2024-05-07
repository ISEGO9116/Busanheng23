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
int pos[3];			// 0:시민, 1:좀비, 2:마동석
int state[3];	//상태. 0: 시민, 1:좀비, 2:마동석
int aggro[2];		//어그로. 0:시민, 1:마동석
char symbols[3] = { 'C', 'Z', 'M' }; //시민/좀비/마동석의 심볼
int isSwitch = 0;	//턴 스위치

//입력 함수
int Input(char *string, int min, int max) {
	int _input;
	do {
		printf(string, min, max); //string에 들어있는 변환문자의 표현을 위해 사용함.
		scanf_s("%d", &_input);
	} while (_input < min || _input > max);
	return _input;
}

//어그로를 증가/감소 시키는 함수(최대값 이후로는 최대값에 고정한다.)
void aggro_AddCal(int aggro_index, int delta_num) {
	aggro[aggro_index] += delta_num;
	if (aggro[aggro_index] > AGGRO_MAX) {
		aggro[aggro_index] = AGGRO_MAX; //최대치로 고정
	}
	else if (aggro[aggro_index] < AGGRO_MIN) {
		aggro[aggro_index] = AGGRO_MIN;
	}
}

//열차 상태 초기화
void init_Variables(int train_length) {
	pos[0] = train_length - 6; // 시민
	pos[1] = train_length - 3; // 좀비
	pos[2] = train_length - 2; // 마동석
	isSwitch = 0;	//기본 false
	aggro[0] = 1;	// 시민 어그로
	aggro[1] = 1;	// 마동석 어그로
	state[0] = 0;	//시민 상태
	state[1] = 0;	//좀비 상태
	state[2] = 0;	//마동석 상태
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
	printf("\n");
}

//좀비 전용 상태 출력 함수
void print_ZombieState(int state) {
	if (state == 1) { //1 : 이동
		printf("zombie: %d -> %d\n", (pos[1]+1), pos[1]);
	}
	else if (state == 2) { //2 : 정지
		printf("zombie: stay %d\n", pos[1]);
	}
	else if (state == 3) { //3 : 강제 정지
		printf("zombie: Stay %d (cannot move)\n", pos[1]);
	}
}

//시민 전용 상태 출력함수
void print_CitizenState(int state) {
	if (state == 1) { //이동
		printf("citizen: %d -> %d (aggro: %d -> %d)\n", (pos[0] + 1), pos[0], (aggro[0] - 1), aggro[0]);
	}
	else if (state == 2) { //대기
		printf("citizen: stay %d (aggro: %d -> %d)\n", pos[0], (aggro[0]+1), aggro[0]);
	}
}

//마동석 전용 출력함수 (마동석 state와 어그로 변화량을 받음)
void print_DongseokState(int state, int aggroDelta, int stamina) {
	if (state == 0) { //대기 (어그로1 감소)
		printf("madongseok: stay %d (aggro: %d->%d, stamina: %d)\n", pos[2], (aggro[1] + 1), aggro[1], stamina);
	}
	else if (state == 1) { //이동(어그로1 증가)
		printf("madongseok: %d -> %d (aggro: %d->%d, stamina: %d)\n", (pos[2] + 1), pos[2], (aggro[1] - 1), aggro[1], stamina);
	}
}

//시민 이동 구현 (확률계산까지)
int citizen_Move(int probability) {
	int action = ((rand() % 100) < (100 - probability)) ? 1 : 2; //(100-p)%확률로 시민 이동
	//action 자체는 잘 먹고 있음
	if (action == 1) {
		pos[0] -= 1; //이동
		aggro_AddCal(0, 1); //어그로 1 증가
		return 1;
	}
	else { //action == 2
		aggro_AddCal(0, -1); //시민 대기 (어그로 1 감소)
		return action; //p% 확률로 시민 대기
	}
}

//좀비 이동 구현(확률계산까지)
//신규 기능 : 좀비가 움직일 때는 무조건 어그로가 높은쪽, 같은 경우에는 시민방향
int zombie_Move(int probability) {
	if (!isSwitch) { //1턴일때
		int action = ((rand() % 100) < (100 - probability)) ? 2 : 1;//(100-p)%의 확률로 제자리에 대기
		isSwitch = 1; //턴스위칭
		if (action == 1) //이동 가능
			return zombie_Dicision_Dir(); //이동 실행
		return action; 
	}
	else { //2턴일때
		isSwitch = 0; //턴스위칭
		return 3; //강제 정지
	}
}

//좀비 이동 방향 생성 함수
//어그로 수치를 기반으로 0:왼쪽, 1:오른쪽
int zombie_Dicision_Dir() {
	int dir;
	if (aggro[1] > aggro[0]) { //만약 마동석의 어그로수치가 시민보다 높다면
		dir = 1; //이동 방향을 오른쪽으로 결정한다. 1
		//만약 마동석과의 거리가 1이하라면(즉, 인접해있다면), 움직이지 않는다.
		if (pos[2] - pos[1] <= 1) { return 2; }
		else {
			//아니라면, 이동방향으로 전진한다.
			pos[1] += 1; return 1; //오른쪽으로 1칸 전진한다.
		}
	}
	else { //만약 아니라면
		dir = 0; //이동 방향을 왼쪽으로 결정한다. return 0
		//만약 시민과의 거리가 1이하라면(즉, 인접해있다면), 움직이지 않는다.
		if (pos[1] - pos[0] <= 1) { return 2; }
		else { //아니라면, 이동방향으로 전진한다.
			pos[1] -= 1; return 1; // 왼쪽으로 1칸 전진한다. 
		}
	}
}

//마동석 이동 : 
// 0/1 입력받아서 행동, 기차 상태 출력
void dongseok_Move(int train_length, int dongseok_state, int stamina) {
	printf("\n"); //좀비랑 인접해 있으면 '대기'만 가능
	if (!(pos[2] - pos[1] <= 1)) { //만약 현재 거리가 1 이하가 아니라면
		dongseok_state = Input("madongseok move(0:stay, 1:left)>>", 0, 1);
	}
	else { dongseok_state = Input("madongseok move(0:stay)>>", 0, 0); }
	if (dongseok_state) { //1 입력시,
		pos[2] -= 1; //한칸 이동
		aggro_AddCal(1, 1); //어그로 1 증가
		print_TrainState(train_length); //기차 상태 출력
		print_DongseokState(dongseok_state, 1, stamina); //마동석 상태 출력
	}
	else { //0 입력시, 제자리에 대기 (바로 기차 상태 출력하니까 아무것도 안한다?)
		aggro_AddCal(1, -1); //어그로 1 감소
		print_TrainState(train_length); //열차 상태 출력
		print_ZombieState(dongseok_state); //좀비 상태 출력
		print_DongseokState(dongseok_state, 0, stamina); //마동석 상태 출력
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
	int stamina = 0;					// 마동석 스태미나
	int probability = 0;				// 이동 확률
	srand((unsigned int)time(NULL));	//랜덤 모듈 초기화

	//인트로
	printf("게임 시작\n============================\n");

	//★=====초기화=====★
	train_length = Input("train length(%d~%d)>>", LEN_MIN, LEN_MAX);
	stamina = Input("madongseok stamina(%d~%d)>>", STM_MIN, STM_MAX);
	probability = Input("percentile probability 'p'(%d~%d)>>", PROB_MIN, PROB_MAX);
	init_Variables(train_length); //열차 길이에 따른 위치 초기화
	print_TrainState(train_length); //초기 열차 상태

	//★=====턴=====★
	while (1)
	{
		if (pos[0] == 1 || pos[1] - pos[0] <= 1) {
			break; //조건 체크후 반복문 탈출 (시민 탈출 조건 충족 OR 좀비-시민(=거리) 1이하)
		}

		//★=====<이동> 페이즈=====★
		//시민&좀비 이동
		state[0] = citizen_Move(probability); //이동시 state=1, 정지시 state=2
		state[1] = zombie_Move(probability);
		//열차 상태 출력
		print_TrainState(train_length);
		//시민, 좀비 상태 출력
		print_CitizenState(state[0]); //이동시 state[0] = 1; 정지시 state[0] = 
		print_ZombieState(state[1]);
		//마동석 이동
		dongseok_Move(train_length, state[2], stamina);
		//마동석 상태 출력

		//★=====<행동> 페이즈=====★
		//시민 행동
		//좀비 행동
		//마동석 행동
	}

	GameOver(); //아웃트로 - 종료상태 출력(성공/실패)
	return 0;
}