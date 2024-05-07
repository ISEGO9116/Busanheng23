#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <time.h>
#define LEN_MIN		15 // ���� ����
#define LEN_MAX		50
#define PROB_MIN	10 // Ȯ��
#define PROB_MAX	90

//���� ����
int pos[3]; // 0: �ù�, 1: ����, 2: ������
char symbols[3] = { 'C', 'Z', 'M' }; //�ù�/����/�������� �ɺ�
int isSwitch = 0; //�� ����ġ

//���� ���� �ʱ�ȭ
void initTrain(int train_length) {
	pos[0] = train_length - 6; // �ù�
	pos[1] = train_length - 3; // ����
	pos[2] = train_length - 2; // ������
	isSwitch = 0; //�⺻ false
}

//���� �ʱ� ���� ��� -> (����) ���� ���� ��� �Լ�
void printTrainState(int train_length, int pos[], char symbols[]) {
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
//1 : �̵�
//2 : ����
//3 : ���� ����
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

//�ù� �̵� ���� (Ȯ��������)
int citizenMove(int probability) {
	//(100-p)%Ȯ���� �ù� �̵�
	printf("probability : %d\n", probability);
	int _chance = rand() % 100;
	if (_chance < (100-probability)) {
		pos[0] -= 1;
		printf("%d < %d\n", _chance, (100-probability));
		return 1; //state: �ù� �̵�
	}
	else {
		//p%Ȯ���� �ù� ���(=��, ��ġ���� �۾��� �Ѵ�)
		return 2; //state: �ù� ���
	}
}

//���� �̵� ����(Ȯ��������)
int zombieMove(int probability) {
	if (!isSwitch) { //isSwitch�� 0�϶�(ù����)
		//(100-p)%�� Ȯ���� ���ڸ��� ���
		//100 + 1�ϸ� 0~99 -> 1~100���� ����
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

void GameOver() { //��������.
	if (pos[0] == 1) {
		printf("SUCCESS!\n");
		printf("citizen(s) escaped to the next train");
	}
	else if (pos[1] - pos[0] <= 1) {
		printf("GAME OVER!\n");
		printf("Citizen(s) has(have) been attacked by a zombie\n");
	}
}



//����
int main() {
	//����
	int train_length, probability;		//(�Է�) ���� ����, �̵� Ȯ��
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
		printf("train length(%d~%d)>>", LEN_MIN, LEN_MAX);
		scanf_s("%d", &train_length);
		if (train_length < LEN_MIN || train_length > LEN_MAX) {
			continue;
		}
		//������
		//�̱���

		//Ȯ��
		while (1)
		{
			printf("percentile probability 'p'(%d~%d)>>", PROB_MIN, PROB_MAX);
			scanf_s("%d", &probability);
			if (probability < PROB_MIN || probability > PROB_MAX) {
				continue;
			}
			break;
		}
		//������� �����ѰŸ� ���
		break;
	}
	
	// ��ġ �ʱ�ȭ
	initTrain(train_length); //���� ���̿� ���� ��ġ �ʱ�ȭ

	//�ʱ� ���� ���� ���
	printTrainState(train_length, pos, symbols);

	//���η���(�ù��̵�, �����̵�, �������� ���, �ù�/���� ���� ���
	while (1)
	{
		if (pos[0] == 1 || pos[1] - pos[0] <= 1) {
			break; //���� üũ�� �ݺ��� Ż�� (�ù� Ż�� ���� ���� OR ����-�ù�(=�Ÿ�) 1����)
		}

		//��=====�ù� �̵�=====��
		//Ȯ�� �����Ͽ� �̵��� state=1, ������ state=2
		citizen_state = citizenMove(probability); 

		//��=====���� �̵�=====��
		zombie_state = zombieMove(probability);

		printTrainState(train_length, pos, symbols);

		//�ù�, ���� ���� ���
		printStatus("citizen", citizen_state, pos[0]);
		printStatus("zombie", zombie_state, pos[1]);
		Sleep(4000); //4�� ���
	}

	//�ƿ�Ʈ�� - ������� ���(����/����)
	//�̱��� 
	GameOver();
	//if (pos[0] == 1) {
	//	printf("SUCCESS!\n");
	//	printf("citizen(s) escaped to the next train");
	//}
	//else if (pos[1] - pos[0] <= 1) {
	//	printf("GAME OVER!\n");
	//	printf("Citizen(s) has(have) been attacked by a zombie\n");
	//}

	printf("\n============================");
	return 0;
}