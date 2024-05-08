#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#define LEN_MIN		15	// ���� ����
#define LEN_MAX		50
#define STM_MIN		0	// ������ ü��
#define STM_MAX		5
#define PROB_MIN	10	// Ȯ��
#define PROB_MAX	90
#define AGGRO_MIN	0	// ��׷� ����
#define AGGRO_MAX	5

//������ �̵� ����
#define MOVE_LEFT	1
#define MOVE_STAY	0

//������ ���� ���
#define ATK_NONE		0
#define ATK_CITIZEN		1
#define ATK_DONGSEOK	2

//������ �ൿ
#define ACTION_REST		0
#define ACTION_PROVOKE	1 //�����ϱ�?
#define ACTION_PULL		2 //�ٵ��

//���� ����
int pos[3];			// 0:�ù�, 1:����, 2:������
int state[3];	//����. 0: �ù�, 1:����, 2:������
int aggro[2];		//��׷�. 0:�ù�, 1:������
char symbols[3] = { 'C', 'Z', 'M' }; //�ù�/����/�������� �ɺ�
int isSwitch = 0;	//�� ����ġ
int isHolding = 0; //����� ����. 0:�ƴ�, 1:����� ����
int stamina = 0; //ü��

//�Է� �Լ�
int Input(char* string, int min, int max) {
	int _input;
	do {
		printf(string, min, max); //string�� ����ִ� ��ȯ������ ǥ���� ���� �����.
		scanf_s("%d", &_input);
	} while (_input < min || _input > max);
	return _input;
}

//��׷θ� ����/���� ��Ű�� �Լ�(�ִ밪 ���ķδ� �ִ밪�� �����Ѵ�.)
void aggro_AddCal(int aggro_index, int delta_num) {
	aggro[aggro_index] += delta_num;
	if (aggro[aggro_index] > AGGRO_MAX) {
		aggro[aggro_index] = AGGRO_MAX; //�ִ�ġ�� ����
	}
	else if (aggro[aggro_index] < AGGRO_MIN) {
		aggro[aggro_index] = AGGRO_MIN;
	}
}

//���� ���� �ʱ�ȭ
void init_Variables(int train_length) {
	pos[0] = train_length - 6; // �ù�
	pos[1] = train_length - 3; // ����
	pos[2] = train_length - 2; // ������
	isSwitch = 0;	//�⺻ false
	aggro[0] = 1;	// �ù� ��׷�
	aggro[1] = 1;	// ������ ��׷�
	state[0] = 0;	//�ù� ����
	state[1] = 0;	//���� ����
	state[2] = 0;	//������ ����
	printf("\n");
}

//���� ���� ��� �Լ�
void print_TrainState(int train_length) {
	for (int i = 0; i < 3; i++) //3��
	{
		for (int j = 0; j < train_length; j++) //���� ����
		{
			//�߰����϶� + �ù�/����/������ ��ġ�϶�
			if (i == 1 && (j == pos[0] || j == pos[1] || j == pos[2]))
			{	//�ù�/����/������ ��ġ���� �ù�/����/������ �ɺ� ��ġ symbol (0 : (1:2))
				printf("%c", symbols[(j == pos[0]) ? 0 : (j == pos[1]) ? 1 : 2]);
			}
			else {
				//�߰����϶� + ùĭ�ƴ� + ��ĭ �ƴ� : ���̸� ����, �ƴϸ� #
				printf("%c", (i == 1 && (j != 0 && j != train_length - 1)) ? ' ' : '#');
			}
		}
		printf("\n"); //3�� ����� �� �ٲٱ�
	}
	printf("\n");
}


//�ù� ���� ���� ����Լ�
void print_CitizenState(int state) {
	if (state == 1) { //�̵� (1�� �ᱹ �ѹ�¦ �������ٴ� ��, ���� aggro�� ������ 1 ����Ѵ�.)
		printf("citizen: %d -> %d (aggro: %d -> %d)\n", (pos[0] + 1), pos[0], (aggro[0] - 1), aggro[0]);
	}
	else if (state == 2) { //���
		//���� ��Ⱑ 0�� ���¿��� �����?
		printf("citizen: stay %d (aggro: %d -> %d)\n", pos[0], (aggro[0] + 1), aggro[0]);
	}
}

//���� ���� ���� ��� �Լ�
void print_ZombieState(int state) {
	if (state == 1) { //1 : ���� �̵�
		printf("zombie: %d -> %d\n", (pos[1] + 1), pos[1]);
	}
	else if (state == 2) { //2 : ������ �̵�
		printf("zombie: %d -> %d\n", (pos[1] - 1), pos[1]);
	}
	else if (state == 3) { //3 : ���� ����(������ Ȧ��)
		printf("zombie: Stay %d (cannot move caused by madongseok)\n", pos[1]);
	}
	else if (state == 4) { //4: ���� �������� �Ǵ� �޽� ����
		printf("zombie: Stay %d (cannot move)\n", pos[1]);
	}
}

//������ ���� ����Լ� (������ state�� ��׷� ��ȭ���� ����)
void print_DongseokState(int state, int aggroDelta) {
	if (state == 0) { //��� (��׷�1 ����)
		if (aggro[0] == 0) { 
			//��׷ΰ� 1���� 0���� ������ �� 0->0����  ǥ�� �������� ������ �����̱��� �ʰ�
			//��׷ΰ� 0�϶��� aggro[1], aggro[1]
			printf("madongseok: stay %d (aggro: %d->%d, stamina: %d)\n", pos[2], (aggro[1]), aggro[1], stamina);
		}
		else {
			printf("madongseok: stay %d (aggro: %d->%d, stamina: %d)\n", pos[2], (aggro[1] - aggroDelta), aggro[1], stamina);
		}
	}
	else if (state == 1) { //�̵�(��׷�1 ����)
		printf("madongseok: %d -> %d (aggro: %d->%d, stamina: %d)\n", (pos[2] + 1), pos[2], (aggro[1] - aggroDelta), aggro[1], stamina);
	}
}

//�ù� �̵� ���� (Ȯ��������)
int citizen_Move(int probability) {
	int action = ((rand() % 100) < (100 - probability)) ? 1 : 2; //(100-p)%Ȯ���� �ù� �̵�
	if (action == 1) {
		pos[0] -= 1; //�̵�
		aggro_AddCal(0, 1); //��׷� 1 ����
		return 1;
	}
	else { //action == 2
		aggro_AddCal(0, -1); //�ù� ��� (��׷� 1 ����)
		return action; //p% Ȯ���� �ù� ���
	}
}

//���� �̵� ����(Ȯ��������)
//�ű� ��� : ���� ������ ���� ������ ��׷ΰ� ������, ���� ��쿡�� �ùι���
//�ű� ��� : isHolding�� 1�� ��Ȱ��ȭ�ϰ� �̹��� ���� '�̵�' �Ұ�
int zombie_Move(int probability) {
	if (!isSwitch) { //1���϶�
		if (!isHolding) { //holding == 0
			printf("���� : �̵� ����\n");
			isSwitch = 1; //�Ͻ���Ī
			return zombie_Dicision_Dir(); //�̵� ���� ���� �� �̵� ����. ����1, ������2
		}
		else { //holding == 1
			printf("���� : Ȧ������ ���� �̵� �Ұ�\n");
			isHolding = 0; //��Ȱ��ȭ�ϰ�
			return 3; // Ȧ����������3
		}
	}
	else { //2���϶�
		if (isHolding) { isHolding = 0; } //2���϶� Ȧ���� ����
		printf("���� : 2�� �޽�\n");
		isSwitch = 0; //�Ͻ���Ī
		return 4; //�޽� ����
	}
}

//���� �̵� ���� ���� �Լ�
//��׷� ��ġ�� ������� 0:����, 1:������
int zombie_Dicision_Dir() {
	int dir;
	if (aggro[1] > aggro[0]) { //���� �������� ��׷μ�ġ�� �ùκ��� ���ٸ�
		dir = 1; //�̵� ������ ���������� �����Ѵ�. 1
		//���� ���������� �Ÿ��� 1���϶��(��, �������ִٸ�), �������� �ʴ´�.
		if (pos[2] - pos[1] <= 1) { return 4; } //������������4
		else {
			//�ƴ϶��, �̵��������� �����Ѵ�.
			pos[1] += 1; return 2; //���������� 1ĭ �����Ѵ�.
		}
	}
	else { //���� �ƴ϶�� (��, �������� ��׷� ��ġ�� �ùκ��� ���� �ʰų� �������� �ù��� ��׷� ��ġ�� ������.)
		dir = 0; //�̵� ������ �������� �����Ѵ�. return 0
		//���� �ùΰ��� �Ÿ��� 1���϶��(��, �������ִٸ�), �������� �ʴ´�.
		if (pos[1] - pos[0] <= 1) { return 4; } //������������4
		else { //�ƴ϶��, �̵��������� �����Ѵ�.
			pos[1] -= 1; return 1; // �������� 1ĭ �����Ѵ�. 
		}
	}
}

//������ �̵� : 
// 0/1 �Է¹޾Ƽ� �ൿ, ���� ���� ���
void dongseok_Move(int train_length, int dongseok_state) {
	printf("\n"); //����� ������ ������ '���'�� ����
	if (!(pos[2] - pos[1] <= 1)) { //���� ���� �Ÿ��� 1 ���ϰ� �ƴ϶��
		dongseok_state = Input("madongseok move(0:stay, 1:left)>>", MOVE_STAY, MOVE_LEFT);
	}
	else { dongseok_state = Input("madongseok move(0:stay)>>", MOVE_STAY, MOVE_STAY); }
	if (dongseok_state) { //1 �Է½�,
		pos[2] -= 1; //��ĭ �̵�
		aggro_AddCal(1, 1); //��׷� 1 ����
		print_TrainState(train_length); //���� ���� ���
		print_DongseokState(dongseok_state, 1); //������ ���� ���
	}
	else { //0 �Է½�, ���ڸ��� ��� (�ٷ� ���� ���� ����ϴϱ� �ƹ��͵� ���Ѵ�?)
		aggro_AddCal(1, -1); //��׷� 1 ����
		print_TrainState(train_length); //���� ���� ���
		print_ZombieState(dongseok_state); //���� ���� ���
		print_DongseokState(dongseok_state, -1); //������ ���� ���
	}
}

//�ù� �ൿ �Լ�
int action_Citizen() {
	if (pos[0] == 1) {
		//Ż�� ����
		return 1;
	}
	else { //�ƹ��͵� ���� ����
		printf("citizen does nothing.\n");
		return 0;
	}
}

//���� �ൿ �Լ� - ����
//class : 0�ù� 1������ 2��׷α���
int action_ZombieAttack(int class) {
	switch (class)
	{
	case 0: //�ù� ����
		printf("GAME OVER! citizen dead...\n"); exit(0); //���ӿ���
	case 1: //������ ����
		stamina -= 1;
		//����, �������� ���� ü���� STM_MIN�� �Ǹ� 
		if (stamina == STM_MIN) { printf("GAME OVER! madongseok dead...(%d)\n", stamina); exit(0); } //���� ����
		printf("Zombie attacked madongseok (aggro: %d vs. %d, madongseok stamina: %d -> %d)\n",
			aggro[0], aggro[1], stamina + 1, stamina);
		return stamina; //����
		break;
	case 2: //��׷� ����. 
		//���� �ù� ��׷ΰ� ���������� �� ������
		if (aggro[0] > aggro[1]) { exit(0); } //�ù� �����ϰ� ���� ����
		else { return action_ZombieAttack(1); } //���� ������ ��׷ΰ� �ùκ��� �� ������
	}
}

//���� �ൿ �Լ� : ������ �ΰ��� ����
int action_Zombie() {
	int _distanceFromM = pos[2] - pos[1]; //���������� �Ÿ�
	int _distanceFromC = pos[1] - pos[0]; //�ùΰ��� �Ÿ�
	if ((_distanceFromM < _distanceFromC) && _distanceFromM <= 1) {
		//���� �������� �ùκ��� ������ �����ϴٸ�
		return action_ZombieAttack(1); //�������� �����Ѵ�.
	}
	else if ((_distanceFromC < _distanceFromM) && _distanceFromC <= 1) {
		//���� �ù��� ���������� ������ �����ϴٸ�
		return action_ZombieAttack(0); //�ù��� �����Ѵ�.
	}
	else if ((_distanceFromC == _distanceFromM) && _distanceFromC <= 1) {
		//���� �Ѵ� ������ �ִٸ� : ��׷ΰ� ���� ���� ����
		return action_ZombieAttack(2);
	}
	else { //������ ���ٸ� (����)
		printf("zombie attacked nobody.\n");
	}
}

//������ �ൿ - ���� (int�� �ٲ� �ʿ� ����)
void action_Madongseok_Proveke() {
	printf("madongseok provoked zombie...\n");
	int _delta = AGGRO_MAX - aggro[1];
	aggro[1] = AGGRO_MAX;
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d)\n", pos[2], (aggro[1] - _delta), aggro[1], stamina);
}

//������ �ൿ - �޽�
int action_Madongseok_Rest() {
	int _delta = aggro[1];
	int _delta2 = stamina;
	aggro_AddCal(1, -1); //���¹̳� �߰��� �������� Ȯ��
	stamina += 1;
	if (stamina >= STM_MAX) {
		stamina = STM_MAX; //�ִ�ġ�� ����
	}
	printf("madongseok rests...\n");
	printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", pos[2], _delta, aggro[1], _delta2, stamina);
	return stamina;
}

//������ �ൿ - ������ ���
int action_Madongseok_InputA(int probability) {
	int _input = Input("madongseok move(0.rest, 1.provoke, 2.pull)>>", ACTION_REST, ACTION_PULL);
	int _delta = aggro[1]; int _staDelta = stamina;
	switch (_input)
	{
	case 0: //�޽� (��׷� 1 ����, ü�� 1 ����)
		return action_Madongseok_Rest(); break;
	case 1: //���� (��׷� AGGRO_MAX�� ����)
		action_Madongseok_Proveke(); break;
	case 2: //�ٵ��
		isHolding = ((rand() % 100) < (100 - probability)) ? 1 : 0; //(100-p)%Ȯ���� ����1, ����0
		aggro_AddCal(1, 2); stamina -= 1; //��׷� 2 ����, ü�� 1 ����
		if (isHolding) { printf("madongseok pulled zombie... Next turn, it can't move\n"); } //�ٵ�� ������
		else { printf("madongseok failed to pull zombie\n"); } //�ٵ�� ���н�
		printf("madongseok: %d (aggro: %d -> %d, stamina: %d -> %d)\n", pos[2], _delta, aggro[1], _staDelta, stamina);
		break;
	}
}

//������ �ൿ - �������� ���� ���
int action_Madongseok_InputB() {
	int _input = Input("madongseok move(0.rest, 1.provoke)>>", ACTION_REST, ACTION_PROVOKE);
	switch (_input)
	{
	case 0: //�޽� (��׷� 1 ����, ü�� 1 ����)
		return action_Madongseok_Rest(); break;
	case 1: //���� (��׷� AGGRO_MAX�� ����)
		action_Madongseok_Proveke(); break;
	}
}

//������ �ൿ
int action_Madongseok(int probability) {
	int _distanceFromZ = pos[2] - pos[1]; //������� �Ÿ�
	if (_distanceFromZ <= 1) { //����� ������ ���
		//����� �Է� �߻� (0�޽�, 1����, 2�ٵ��)
		return action_Madongseok_InputA(probability);
	}
	else { //����� �������� ���� ���
		//����� �Է� �߻� (0�޽�, 1����)
		action_Madongseok_InputB();
	}
}

//��������.
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

//����
int main() {
	//����
	int train_length = 0;				// ���� ����
	int probability = 0;				// �̵� Ȯ��
	int gameover = 0;					// 0: ���� ���� �ƴ�, 1 : ���� ����
	srand((unsigned int)time(NULL));	//���� ��� �ʱ�ȭ

	//��Ʈ��
	printf("���� ����\n============================\n");

	//��=====�ʱ�ȭ=====��
	train_length = Input("train length(%d~%d)>>", LEN_MIN, LEN_MAX);
	stamina = Input("madongseok stamina(%d~%d)>>", STM_MIN, STM_MAX);
	probability = Input("percentile probability 'p'(%d~%d)>>", PROB_MIN, PROB_MAX);
	init_Variables(train_length); //���� ���̿� ���� ��ġ �ʱ�ȭ
	print_TrainState(train_length); //�ʱ� ���� ����
	//��=====��=====��
	while (1)
	{
		//��=====<�̵�> ������=====��
		//�ù�&���� �̵�
		state[0] = citizen_Move(probability); //�̵��� state=1, ������ state=2
		state[1] = zombie_Move(probability);
		print_TrainState(train_length); //���� ���� ���
		//�ù�, ���� ���� ���
		print_CitizenState(state[0]); //�̵��� state[0] = 1; ������ state[0] = 
		print_ZombieState(state[1]);
		
		dongseok_Move(train_length, state[2]); //������ �̵�, ������ ���� ���

		//��=====<�ൿ> ������=====��
		printf("\n"); //�ù� �ൿ
		if (action_Citizen()) { break; }; 

		action_Zombie(); //���� �ൿ
		action_Madongseok(probability); //������ �ൿ
	}

	GameOver(); //�ƿ�Ʈ�� - ������� ���(����/����)
	return 0;
}