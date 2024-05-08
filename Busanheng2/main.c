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
int isHolding = 0; //붙잡기 여부. 0:아님, 1:붙잡고 있음
int stamina = 0; //체력

//입력 함수
int Input(char* string, int min, int max) {
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


//시민 전용 상태 출력함수
void print_CitizenState(int state) {
	if (state == 1) { //이동 (1은 결국 한발짝 움직였다는 것, 따라서 aggro가 무조건 1 상승한다.)
		printf("citizen: %d -> %d (aggro: %d -> %d)\n", (pos[0] + 1), pos[0], (aggro[0] - 1), aggro[0]);
	}
	else if (state == 2) { //대기
		//만약 대기가 0번 상태에서 대기라면?
		printf("citizen: stay %d (aggro: %d -> %d)\n", pos[0], (aggro[0] + 1), aggro[0]);
	}
}

//좀비 전용 상태 출력 함수
void print_ZombieState(int state) {
	if (state == 1) { //1 : 왼쪽 이동
		printf("zombie: %d -> %d\n", (pos[1] + 1), pos[1]);
	}
	else if (state == 2) { //2 : 오른쪽 이동
		printf("zombie: %d -> %d\n", (pos[1] - 1), pos[1]);
	}
	else if (state == 3) { //3 : 강제 정지(마동석 홀드)
		printf("zombie: Stay %d (cannot move caused by madongseok)\n", pos[1]);
	}
	else if (state == 4) { //4: 껴서 못움직임 또는 휴식 정지
		printf("zombie: Stay %d (cannot move)\n", pos[1]);
	}
}

//마동석 전용 출력함수 (마동석 state와 어그로 변화량을 받음)
void print_DongseokState(int state, int aggroDelta) {
	if (state == 0) { //대기 (어그로1 감소)
		if (aggro[0] == 0) { 
			//어그로가 1에서 0으로 감소할 때 0->0으로  표기 오류나는 문제의 범인이구나 너가
			//어그로가 0일때는 aggro[1], aggro[1]
			printf("madongseok: stay %d (aggro: %d->%d, stamina: %d)\n", pos[2], (aggro[1]), aggro[1], stamina);
		}
		else {
			printf("madongseok: stay %d (aggro: %d->%d, stamina: %d)\n", pos[2], (aggro[1] - aggroDelta), aggro[1], stamina);
		}
	}
	else if (state == 1) { //이동(어그로1 증가)
		printf("madongseok: %d -> %d (aggro: %d->%d, stamina: %d)\n", (pos[2] + 1), pos[2], (aggro[1] - aggroDelta), aggro[1], stamina);
	}
}

//시민 이동 구현 (확률계산까지)
int citizen_Move(int probability) {
	int action = ((rand() % 100) < (100 - probability)) ? 1 : 2; //(100-p)%확률로 시민 이동
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
//신규 기능 : isHolding이 1면 비활성화하고 이번턴 좀비 '이동' 불가
int zombie_Move(int probability) {
	if (!isSwitch) { //1턴일때
		if (!isHolding) { //holding == 0
			printf("좀비 : 이동 가능\n");
			isSwitch = 1; //턴스위칭
			return zombie_Dicision_Dir(); //이동 방향 결정 및 이동 실행. 왼쪽1, 오른쪽2
		}
		else { //holding == 1
			printf("좀비 : 홀딩으로 인한 이동 불가\n");
			isHolding = 0; //비활성화하고
			return 3; // 홀딩강제정지3
		}
	}
	else { //2턴일때
		if (isHolding) { isHolding = 0; } //2턴일때 홀딩시 증발
		printf("좀비 : 2턴 휴식\n");
		isSwitch = 0; //턴스위칭
		return 4; //휴식 정지
	}
}

//좀비 이동 방향 생성 함수
//어그로 수치를 기반으로 0:왼쪽, 1:오른쪽
int zombie_Dicision_Dir() {
	int dir;
	if (aggro[1] > aggro[0]) { //만약 마동석의 어그로수치가 시민보다 높다면
		dir = 1; //이동 방향을 오른쪽으로 결정한다. 1
		//만약 마동석과의 거리가 1이하라면(즉, 인접해있다면), 움직이지 않는다.
		if (pos[2] - pos[1] <= 1) { return 4; } //껴서못움직임4
		else {
			//아니라면, 이동방향으로 전진한다.
			pos[1] += 1; return 2; //오른쪽으로 1칸 전진한다.
		}
	}
	else { //만약 아니라면 (즉, 마동석의 어그로 수치가 시민보다 높지 않거나 마동석과 시민의 어그로 수치가 같으면.)
		dir = 0; //이동 방향을 왼쪽으로 결정한다. return 0
		//만약 시민과의 거리가 1이하라면(즉, 인접해있다면), 움직이지 않는다.
		if (pos[1] - pos[0] <= 1) { return 4; } //껴서못움직임4
		else { //아니라면, 이동방향으로 전진한다.
			pos[1] -= 1; return 1; // 왼쪽으로 1칸 전진한다. 
		}
	}
}

//마동석 이동 : 
// 0/1 입력받아서 행동, 기차 상태 출력
void dongseok_Move(int train_length, int dongseok_state) {
	printf("\n"); //좀비랑 인접해 있으면 '대기'만 가능
	if (!(pos[2] - pos[1] <= 1)) { //만약 현재 거리가 1 이하가 아니라면
		dongseok_state = Input("madongseok move(0:stay, 1:left)>>", MOVE_STAY, MOVE_LEFT);
	}
	else { dongseok_state = Input("madongseok move(0:stay)>>", MOVE_STAY, MOVE_STAY); }
	if (dongseok_state) { //1 입력시,
		pos[2] -= 1; //한칸 이동
		aggro_AddCal(1, 1); //어그로 1 증가
		print_TrainState(train_length); //기차 상태 출력
		print_DongseokState(dongseok_state, 1); //마동석 상태 출력
	}
	else { //0 입력시, 제자리에 대기 (바로 기차 상태 출력하니까 아무것도 안한다?)
		aggro_AddCal(1, -1); //어그로 1 감소
		print_TrainState(train_length); //열차 상태 출력
		print_ZombieState(dongseok_state); //좀비 상태 출력
		print_DongseokState(dongseok_state, -1); //마동석 상태 출력
	}
}

//시민 행동 함수
int action_Citizen() {
	if (pos[0] == 1) {
		//탈출 성공
		return 1;
	}
	else { //아무것도 하지 않음
		printf("citizen does nothing.\n");
		return 0;
	}
}

//좀비 행동 함수 - 공격
//class : 0시민 1마동석 2어그로기준
int action_ZombieAttack(int class) {
	switch (class)
	{
	case 0: //시민 공격
		printf("GAME OVER! citizen dead...\n"); exit(0); //게임오버
	case 1: //마동석 공격
		stamina -= 1;
		//만약, 공격으로 인해 체력이 STM_MIN이 되면 
		if (stamina == STM_MIN) { printf("GAME OVER! madongseok dead...(%d)\n", stamina); exit(0); } //게임 오버
		printf("Zombie attacked madongseok (aggro: %d vs. %d, madongseok stamina: %d -> %d)\n",
			aggro[0], aggro[1], stamina + 1, stamina);
		return stamina; //생존
		break;
	case 2: //어그로 기준. 
		//만약 시민 어그로가 마동석보다 더 높으면
		if (aggro[0] > aggro[1]) { exit(0); } //시민 공격하고 게임 오버
		else { return action_ZombieAttack(1); } //만약 마동석 어그로가 시민보다 더 높으면
	}
}

//좀비 행동 함수 : 인접한 인간을 공격
int action_Zombie() {
	int _distanceFromM = pos[2] - pos[1]; //마동석과의 거리
	int _distanceFromC = pos[1] - pos[0]; //시민과의 거리
	if ((_distanceFromM < _distanceFromC) && _distanceFromM <= 1) {
		//만약 마동석이 시민보다 가깝고 인접하다면
		return action_ZombieAttack(1); //마동석을 공격한다.
	}
	else if ((_distanceFromC < _distanceFromM) && _distanceFromC <= 1) {
		//만약 시민이 마동석보다 가깝고 인접하다면
		return action_ZombieAttack(0); //시민을 공격한다.
	}
	else if ((_distanceFromC == _distanceFromM) && _distanceFromC <= 1) {
		//만약 둘다 가까이 있다면 : 어그로가 높은 쪽을 공격
		return action_ZombieAttack(2);
	}
	else { //가까이 없다면 (안함)
		printf("zombie attacked nobody.\n");
	}
}

//마동석 행동 - 도발 (int로 바꿀 필요 없음)
void action_Madongseok_Proveke() {
	printf("madongseok provoked zombie...\n");
	int _delta = AGGRO_MAX - aggro[1];
	aggro[1] = AGGRO_MAX;
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n", pos[2], (aggro[1] - _delta), aggro[1], stamina);
}

//마동석 행동 - 휴식
int action_Madongseok_Rest() {
	int _delta = aggro[1];
	int _delta2 = stamina;
	aggro_AddCal(1, -1); //스태미나 추가가 가능한지 확인
	stamina += 1;
	if (stamina >= STM_MAX) {
		stamina = STM_MAX; //최대치로 고정
	}
	printf("madongseok rests...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", pos[2], _delta, aggro[1], _delta2, stamina);
	return stamina;
}

//마동석 행동 - 인접한 경우
int action_Madongseok_InputA(int probability) {
	int _input = Input("madongseok move(0.rest, 1.provoke, 2.pull)>>", ACTION_REST, ACTION_PULL);
	int _delta = aggro[1]; int _staDelta = stamina;
	switch (_input)
	{
	case 0: //휴식 (어그로 1 감소, 체력 1 증가)
		return action_Madongseok_Rest(); break;
	case 1: //도발 (어그로 AGGRO_MAX로 증가)
		action_Madongseok_Proveke(); break;
	case 2: //붙들기
		isHolding = ((rand() % 100) < (100 - probability)) ? 1 : 0; //(100-p)%확률로 성공1, 실패0
		aggro_AddCal(1, 2); stamina -= 1; //어그로 2 증가, 체력 1 감소
		if (isHolding) { printf("madongseok pulled zombie... Next turn, it can't move\n"); } //붙들기 성공시
		else { printf("madongseok failed to pull zombie\n"); } //붙들기 실패시
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", pos[2], _delta, aggro[1], _staDelta, stamina);
		break;
	}
}

//마동석 행동 - 인접하지 않은 경우
int action_Madongseok_InputB() {
	int _input = Input("madongseok move(0.rest, 1.provoke)>>", ACTION_REST, ACTION_PROVOKE);
	switch (_input)
	{
	case 0: //휴식 (어그로 1 감소, 체력 1 증가)
		return action_Madongseok_Rest(); break;
	case 1: //도발 (어그로 AGGRO_MAX로 증가)
		action_Madongseok_Proveke(); break;
	}
}

//마동석 행동
int action_Madongseok(int probability) {
	int _distanceFromZ = pos[2] - pos[1]; //좀비와의 거리
	if (_distanceFromZ <= 1) { //좀비와 인접한 경우
		//사용자 입력 발생 (0휴식, 1도발, 2붙들기)
		return action_Madongseok_InputA(probability);
	}
	else { //좀비와 인접하지 않은 경우
		//사용자 입력 발생 (0휴식, 1도발)
		action_Madongseok_InputB();
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
	int probability = 0;				// 이동 확률
	int gameover = 0;					// 0: 게임 오버 아님, 1 : 게임 오버
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
		//★=====<이동> 페이즈=====★
		//시민&좀비 이동
		state[0] = citizen_Move(probability); //이동시 state=1, 정지시 state=2
		state[1] = zombie_Move(probability);
		print_TrainState(train_length); //열차 상태 출력
		//시민, 좀비 상태 출력
		print_CitizenState(state[0]); //이동시 state[0] = 1; 정지시 state[0] = 
		print_ZombieState(state[1]);
		
		dongseok_Move(train_length, state[2]); //마동석 이동, 마동석 상태 출력

		//★=====<행동> 페이즈=====★
		printf("\n"); //시민 행동
		if (action_Citizen()) { break; }; 

		action_Zombie(); //좀비 행동
		action_Madongseok(probability); //마동석 행동
	}

	GameOver(); //아웃트로 - 종료상태 출력(성공/실패)
	return 0;
}