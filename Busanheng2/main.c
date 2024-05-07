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
	if ((rand() % 100) < (100 - probability)) {
		pos[0] -= 1;
		return 1; //state: �ù� �̵�
	}
	else {
		//p%Ȯ���� �ù� ���(=��, ��ġ���� �۾��� �Ѵ�)
		return 2; //state: �ù� ���
	}
}

//���� �̵� ����(Ȯ��������)
int zombieMove(int zombie_state, int probability) {
	if (!isSwitch) { //isSwitch�� 0�϶�(ù����)
		//(100-p)%�� Ȯ���� ���ڸ��� ���
		//100 + 1�ϸ� 0~99 -> 1~100���� ����
		if ((rand() % 100) < (100 - probability)) {
			printf("[debug] IF isSwitch : %d\n", isSwitch);
			printf("[debug] IF isSwitch : %d\n", isSwitch);
			isSwitch = 1; //�Ͻ���Ī
			return 2; //���� ���
		}
		else { //p%�� Ȯ���� 1ĭ�̵�
			pos[1] -= 1;
			printf("[debug] IFELSE isSwitch : %d\n", isSwitch);
			isSwitch = 1; //�Ͻ���Ī
			return 1;
		}
	}
	else { //isSwitch�� 1�϶�
		isSwitch = 0; //�Ͻ���Ī
		printf("[debug] ELSE isSwitch : %d\n", isSwitch);
		return 3; //���� ����
		//���� : state
	}
}

//����
int main() {
	//����
	int train_length, probability;		//(�Է�) ���� ����, �̵� Ȯ��
	int citizen_state = 0;					// �ʱ�ȭ
	int zombie_state = 0;					// �ʱ�ȭ
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
		printf("percentile probability 'p'(%d~%d)>>", PROB_MIN, PROB_MAX);
		scanf_s("%d", &probability);
		if (probability < PROB_MIN || probability > PROB_MAX) {
			continue;
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
		//���� üũ�� �ݺ��� Ż�� (�ù� Ż�� ���� ���� OR ����-�ù�(=�Ÿ�) 1����)
		if (pos[0] == 1 || pos[1] - pos[0] <= 1) {
			break;
		}
		printf("[�� ����]\n");

		//��=====�ù� �̵�=====�� //100-pȮ���� �̵�
		//Ȯ�� �����Ͽ� �̵��� state=1, ������ state=2
		printf("[�ù� �̵�]\n");
		citizen_state = citizenMove(citizen_state, probability); 


		//��=====���� �̵�=====��
		// �̱��� zombie_move()
		printf("[���� �̵�]\n");
		zombie_state = zombieMove(zombie_state, probability);
		//���� 1���̰�, (100 - p)% 
		//if (!isSwitch) {
		//	if ((rand()%100) < (100-probability)) {
		//		//p%�� Ȯ���� �̵�
		//		pos[1] -= 1;
		//		zombie_state = 2; //�̵�
		//	}
		//	else {
		//		//(100-p)% Ȯ���� ���ڸ��� ���
		//		zombie_state = 1;
		//	}
		//	isSwitch = 1;
		//}
		//else{ //2��
		//	zombie_state = 3; //���� ���
		//	isSwitch = 0;
		//}

		printf("[���� ���� ���]\n");
		printTrainState(train_length, pos, symbols);

		//�ù�, ���� ���� ���
		printStatus("citizen", citizen_state, pos[0]);
		printStatus("zombie", zombie_state, pos[1]);
		Sleep(4000); //4�� ���
	}

	//�ƿ�Ʈ�� - ������� ���(����/����)
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