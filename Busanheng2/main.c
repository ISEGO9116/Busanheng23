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
int pos[3]; // 0: �ù�, 1: ����, 2: ������
char symbols[3] = { 'C', 'Z', 'M' }; //�ù�/����/�������� �ɺ�
int isSwitch = 0; //�� ����ġ

//�Է� �Լ�
int Input(char *string, int min, int max) {
	//�����Ϸ��� 
	// �Ķ���ͷ� ���ڿ� train length(%d~%d)>> �ְ�,
	// ������ LEN_MIN, LEN_MAX �ְ�,
	// ��ü������ do while�� �������������� �Է¹���
	// ���������� scanf_s�� ��Ƽ� return�ؾ� �� ��
	int _input;
	do {
		printf(string, min, max); //string�� ����ִ� ��ȯ������ ǥ���� ���� �����.
		scanf_s("%d", &_input);
	} while (_input < min || _input > max);
	return _input;
}

//���� ���� �ʱ�ȭ
void init_Train(int train_length) {
	pos[0] = train_length - 6; // �ù�
	pos[1] = train_length - 3; // ����
	pos[2] = train_length - 2; // ������
	isSwitch = 0; //�⺻ false
}

//���� ���� ��� �Լ�
void print_TrainState(int train_length, int pos[], char symbols[]) {
	printf("\n");
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
}

//����ǥ
void print_Status(char *string, int state, int pos) {
	if (state == 1) { //1 : �̵�
		printf("%s: %d -> %d\n", string, pos + 1, pos);
	}
	else if (state == 2) { //2 : ����
		printf("%s: stay %d\n", string, pos);
	}
	else if (state == 3) { //3 : ���� ����
		printf("%s: Stay %d (cannot move)\n", string, pos);
	}
}

//�ù� �̵� ���� (Ȯ��������)
int citizen_Move(int probability) {
	//(100-p)%Ȯ���� �ù� �̵�
	int _chance = rand() % 100;
	if (_chance < (100-probability)) {
		pos[0] -= 1;
		return 1; //state: �ù� �̵�
	}
	else {
		//p%Ȯ���� �ù� ���(=��, ��ġ���� �۾��� �Ѵ�)
		return 2; //state: �ù� ���
	}
}

//���� �̵� ����(Ȯ��������)
int zombie_Move(int probability) {
	if (!isSwitch) { //isSwitch�� 0�϶�(ù����)
		//(100-p)%�� Ȯ���� ���ڸ��� ���
		if ((rand() % 100) < (100 - probability)) {
			isSwitch = 1; //�Ͻ���Ī
			return 2; //���� ���
		}
		else { //p%�� Ȯ���� 1ĭ�̵�
			pos[1] -= 1;
			isSwitch = 1; //�Ͻ���Ī
			return 1;
		}
	}
	else { //isSwitch�� 1�϶�
		isSwitch = 0; //�Ͻ���Ī
		return 3; //���� ����
		//���� : state
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
	int stamina = 0;					//
	int probability = 0;				//�̵� Ȯ��
	
	int citizen_state = 0;				// �ʱ�ȭ
	int zombie_state = 0;				// �ʱ�ȭ
	srand((unsigned int)time(NULL));	//���� ��� �ʱ�ȭ

	//��Ʈ��
	printf("���� ����\n");
	printf("============================\n");

	//������ ����, Ȯ�� �Է�
	while (1)
	{
		//train length
		train_length = Input("train length(%d~%d)>>", LEN_MIN, LEN_MAX);
		stamina = Input("madongseok stamina(%d~%d)>>", STM_MIN, STM_MAX);
		probability = Input("percentile probability 'p'(%d~%d)>>", PROB_MIN, PROB_MAX);

		//������
		//�̱���
		//������� �����ѰŸ� ���
		break;
	}
	
	// ��ġ �ʱ�ȭ
	init_Train(train_length); //���� ���̿� ���� ��ġ �ʱ�ȭ

	//�ʱ� ���� ���� ���
	print_TrainState(train_length, pos, symbols);

	//���η���(�ù��̵�, �����̵�, �������� ���, �ù�/���� ���� ���
	while (1)
	{
		if (pos[0] == 1 || pos[1] - pos[0] <= 1) {
			break; //���� üũ�� �ݺ��� Ż�� (�ù� Ż�� ���� ���� OR ����-�ù�(=�Ÿ�) 1����)
		}

		//��=====�ù� �̵�=====��
		citizen_state = citizen_Move(probability); //�̵��� state=1, ������ state=2

		//��=====���� �̵�=====��
		zombie_state = zombie_Move(probability);

		//���� ���� ���
		print_TrainState(train_length, pos, symbols);

		//�ù�, ���� ���� ���
		print_Status("citizen", citizen_state, pos[0]);
		print_Status("zombie", zombie_state, pos[1]);
	}

	GameOver(); //�ƿ�Ʈ�� - ������� ���(����/����)
	return 0;
}