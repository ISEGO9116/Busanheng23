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

//�Է� �Լ�
int Input(char *string, int min, int max) {
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

//���� ���� ���� ��� �Լ�
void print_ZombieState(int state) {
	if (state == 1) { //1 : �̵�
		printf("zombie: %d -> %d\n", (pos[1]+1), pos[1]);
	}
	else if (state == 2) { //2 : ����
		printf("zombie: stay %d\n", pos[1]);
	}
	else if (state == 3) { //3 : ���� ����
		printf("zombie: Stay %d (cannot move)\n", pos[1]);
	}
}

//�ù� ���� ���� ����Լ�
void print_CitizenState(int state) {
	if (state == 1) { //�̵�
		printf("citizen: %d -> %d (aggro: %d -> %d)\n", (pos[0] + 1), pos[0], (aggro[0] - 1), aggro[0]);
	}
	else if (state == 2) { //���
		printf("citizen: stay %d (aggro: %d -> %d)\n", pos[0], (aggro[0]+1), aggro[0]);
	}
}

//������ ���� ����Լ� (������ state�� ��׷� ��ȭ���� ����)
void print_DongseokState(int state, int aggroDelta, int stamina) {
	if (state == 0) { //��� (��׷�1 ����)
		printf("madongseok: stay %d (aggro: %d->%d, stamina: %d)\n", pos[2], (aggro[1] + 1), aggro[1], stamina);
	}
	else if (state == 1) { //�̵�(��׷�1 ����)
		printf("madongseok: %d -> %d (aggro: %d->%d, stamina: %d)\n", (pos[2] + 1), pos[2], (aggro[1] - 1), aggro[1], stamina);
	}
}

//�ù� �̵� ���� (Ȯ��������)
int citizen_Move(int probability) {
	int action = ((rand() % 100) < (100 - probability)) ? 1 : 2; //(100-p)%Ȯ���� �ù� �̵�
	//action ��ü�� �� �԰� ����
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
int zombie_Move(int probability) {
	if (!isSwitch) { //1���϶�
		int action = ((rand() % 100) < (100 - probability)) ? 2 : 1;//(100-p)%�� Ȯ���� ���ڸ��� ���
		isSwitch = 1; //�Ͻ���Ī
		if (action == 1) //�̵� ����
			return zombie_Dicision_Dir(); //�̵� ����
		return action; 
	}
	else { //2���϶�
		isSwitch = 0; //�Ͻ���Ī
		return 3; //���� ����
	}
}

//���� �̵� ���� ���� �Լ�
//��׷� ��ġ�� ������� 0:����, 1:������
int zombie_Dicision_Dir() {
	int dir;
	if (aggro[1] > aggro[0]) { //���� �������� ��׷μ�ġ�� �ùκ��� ���ٸ�
		dir = 1; //�̵� ������ ���������� �����Ѵ�. 1
		//���� ���������� �Ÿ��� 1���϶��(��, �������ִٸ�), �������� �ʴ´�.
		if (pos[2] - pos[1] <= 1) { return 2; }
		else {
			//�ƴ϶��, �̵��������� �����Ѵ�.
			pos[1] += 1; return 1; //���������� 1ĭ �����Ѵ�.
		}
	}
	else { //���� �ƴ϶��
		dir = 0; //�̵� ������ �������� �����Ѵ�. return 0
		//���� �ùΰ��� �Ÿ��� 1���϶��(��, �������ִٸ�), �������� �ʴ´�.
		if (pos[1] - pos[0] <= 1) { return 2; }
		else { //�ƴ϶��, �̵��������� �����Ѵ�.
			pos[1] -= 1; return 1; // �������� 1ĭ �����Ѵ�. 
		}
	}
}

//������ �̵� : 
// 0/1 �Է¹޾Ƽ� �ൿ, ���� ���� ���
void dongseok_Move(int train_length, int dongseok_state, int stamina) {
	printf("\n"); //����� ������ ������ '���'�� ����
	if (!(pos[2] - pos[1] <= 1)) { //���� ���� �Ÿ��� 1 ���ϰ� �ƴ϶��
		dongseok_state = Input("madongseok move(0:stay, 1:left)>>", 0, 1);
	}
	else { dongseok_state = Input("madongseok move(0:stay)>>", 0, 0); }
	if (dongseok_state) { //1 �Է½�,
		pos[2] -= 1; //��ĭ �̵�
		aggro_AddCal(1, 1); //��׷� 1 ����
		print_TrainState(train_length); //���� ���� ���
		print_DongseokState(dongseok_state, 1, stamina); //������ ���� ���
	}
	else { //0 �Է½�, ���ڸ��� ��� (�ٷ� ���� ���� ����ϴϱ� �ƹ��͵� ���Ѵ�?)
		aggro_AddCal(1, -1); //��׷� 1 ����
		print_TrainState(train_length); //���� ���� ���
		print_ZombieState(dongseok_state); //���� ���� ���
		print_DongseokState(dongseok_state, 0, stamina); //������ ���� ���
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
	int stamina = 0;					// ������ ���¹̳�
	int probability = 0;				// �̵� Ȯ��
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
		if (pos[0] == 1 || pos[1] - pos[0] <= 1) {
			break; //���� üũ�� �ݺ��� Ż�� (�ù� Ż�� ���� ���� OR ����-�ù�(=�Ÿ�) 1����)
		}

		//��=====<�̵�> ������=====��
		//�ù�&���� �̵�
		state[0] = citizen_Move(probability); //�̵��� state=1, ������ state=2
		state[1] = zombie_Move(probability);
		//���� ���� ���
		print_TrainState(train_length);
		//�ù�, ���� ���� ���
		print_CitizenState(state[0]); //�̵��� state[0] = 1; ������ state[0] = 
		print_ZombieState(state[1]);
		//������ �̵�
		dongseok_Move(train_length, state[2], stamina);
		//������ ���� ���

		//��=====<�ൿ> ������=====��
		//�ù� �ൿ
		//���� �ൿ
		//������ �ൿ
	}

	GameOver(); //�ƿ�Ʈ�� - ������� ���(����/����)
	return 0;
}