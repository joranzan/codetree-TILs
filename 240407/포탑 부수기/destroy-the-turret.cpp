//시작 시간 : 19:30
//종료 시간 : 
#include<iostream>
#include<algorithm>
#include<queue>
#include<vector>

using namespace std;

struct pos {
	int row;
	int col;
};

struct turret {
	int id;
	pos nowPos;
	int power;
	int attackTime = 0;

	bool operator<(turret next) const {
		if (power < next.power) return true;
		else if (power > next.power) return false;
		else {
			if (attackTime < next.attackTime) return false;
			else if (attackTime > next.attackTime) return true;
			else {
				if (nowPos.row + nowPos.col < next.nowPos.row + next.nowPos.col) return false;
				else if (nowPos.row + nowPos.col > next.nowPos.row + next.nowPos.col) return true;
				else {
					if (nowPos.col < next.nowPos.col) return false;
					else if (nowPos.col > next.nowPos.col) return true;
					else return false;
				}
			}
		}
	}
	/*
	1. 공격자 선정
		1) 공격자 : 가장 약한 포탑 (핸디캡 N+M 공격력 증가)
			- 선정기준
				- 공격력 가장 낮은 포탑
				- 동률인 경우 : 가장 최근 공격한 포탑
				- 동률인 경우 : 행 + 열 가장 큰 포탑
				- 동률인 경우 : 열 값이 가장 큰 포탑

	2. 공격 : 가장 강한 포탑 공격

		1) 공격대상 : 선정 기준
			- 공격력이 가장 높은 포탑
			- 동률인 경우 : 가장 오래 전 공격한 포탑
			- 동률인 경우 : 행 + 열 가장 작은 포탑
			- 동률인 경우 : 열 값이 가장 작은 포탑
	
	*/
};

int N, M, K;    // K : 1000 , 공격력 : 5000

int TurretMap[11][11] = { 0, };  //포탑 ID 관리 
int TurretNum = 0;   //포탑 개수
bool TurretAlive[101] = { false, };   //포탑 살아있는지 여부
int TurretIndexMap[101] = { 0, };
turret TurretOrigin[101];
turret TurretInfo[101];

int InputMap[11][11] = { 0, };
int minAliveTurretID = 0;

bool isAttacked[101] = { false, };    //이전 턴에 공격 받았는지

void init() {

	cin >> N >> M >> K;

	for (int i = 1; i <= N * M; i++) {
		TurretAlive[i] = true;
	}

	TurretNum = 0;
	minAliveTurretID = 1;

	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= M; c++) {
			cin >> InputMap[r][c];
			if (InputMap[r][c] == 0) continue;
			TurretNum++;
			TurretMap[r][c] = TurretNum;
			TurretInfo[TurretNum].attackTime = 0;
			TurretInfo[TurretNum].id = TurretNum;
			TurretInfo[TurretNum].nowPos.row = r;
			TurretInfo[TurretNum].nowPos.col = c;
			TurretInfo[TurretNum].power = InputMap[r][c];
			TurretOrigin[TurretNum] = TurretInfo[TurretNum];
		}
	}



}


bool LaserAttack(int AttackerId, int TargetId) {

	//cout << "Laser Attack\n";

	//2 - 1) 레이저 공격 먼저 시도(최단 경로로 공격)

	//	1) 상하좌우 4개의 방향 움직일 수 있음(우, 하, 좌, 상)
	//	2) 부서진 포탑 못지나감
	//	3) 격자 밖->반대편으로 나옴
	//	4) 경로 정한 뒤
	//	1) (공격대상 공격력 - 공격자의 공격력)
	//	2) (경로위 공격력 - (공격자의 공격력 / 2))


	//	Flag 필요(레이저 성공했는지 여부)

	const int dr[4] = { 0,1,0,-1 };
	const int dc[4] = { 1,0,-1,0 };

	pos Visited[11][11];

	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= M; c++) {
			Visited[r][c].row = -1;
			Visited[r][c].col = -1;
		}
	}

	int startRow = TurretInfo[AttackerId].nowPos.row;
	int startCol = TurretInfo[AttackerId].nowPos.col;
	int targetRow = TurretInfo[TargetId].nowPos.row;
	int targetCol = TurretInfo[TargetId].nowPos.col;

	queue<pos> q;
	q.push({ startRow, startCol });
	Visited[startRow][startCol] = { startRow, startCol };

	bool ret = false;
	vector<pos> path;

	while (!q.empty()) {
		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();

		if (nowRow == targetRow && nowCol == targetCol) {
			ret = true;
			int tempRow = Visited[targetRow][targetCol].row;
			int tempCol = Visited[targetRow][targetCol].col;
			while (tempRow != startRow || tempCol != startCol) {
				path.push_back({ tempRow, tempCol });
				int tempnextRow = Visited[tempRow][tempCol].row;
				int tempnextCol = Visited[tempRow][tempCol].col;

				if (tempnextRow == startRow && tempnextCol == startCol) break;
				tempRow = tempnextRow;
				tempCol = tempnextCol;


			}
			break;
		}

		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			//3) 격자 밖->반대편으로 나옴
			if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > M) {
				if (nextRow == 0) nextRow = N;
				if (nextCol == 0) nextCol = M;
				if (nextRow == N + 1) nextRow = 1;
				if (nextCol == M + 1) nextCol = 1;
			}
			//2) 부서진 포탑 못지나감
			if (!TurretAlive[TurretMap[nextRow][nextCol]]) continue;
			if (Visited[nextRow][nextCol].row != -1 || Visited[nextRow][nextCol].col != -1) continue;
			Visited[nextRow][nextCol].row = nowRow;
			Visited[nextRow][nextCol].col = nowCol;
			q.push({ nextRow, nextCol });
		}
	}

	if (!ret) {
		return false;
	}
	else {
		for (int i = 0; i < path.size(); i++) {
			if (path[i].row == startRow && path[i].col == startCol) continue;
			int id = TurretMap[path[i].row][path[i].col];
			isAttacked[TurretIndexMap[id]] = true;
			TurretInfo[TurretIndexMap[id]].power -= (TurretInfo[AttackerId].power / 2);
		}
		TurretInfo[TargetId].power -= TurretInfo[AttackerId].power;
		isAttacked[AttackerId] = true;
		isAttacked[TargetId] = true;
		return true;
	}

}


void BombAttack(int AttackerId, int TargetId) {

	//2 - 2) 포탄 공격 시도

	//1) 공격 대상에게 포탄 던짐(공격자의 공격력만큼 피해)
	//	2) 나머지 8방향의 포탑들(공격자의 공격력 / 2 만큼 피해)
	//	(격자 벗어나는 경우 반대편으로)

	int nowRow = TurretInfo[TargetId].nowPos.row;
	int nowCol = TurretInfo[TargetId].nowPos.col;
	
	int attackerRow = TurretInfo[AttackerId].nowPos.row;
	int attackerCol = TurretInfo[AttackerId].nowPos.col;

	const int dr[8] = { -1,1,0,0,-1,1,-1,1 };
	const int dc[8] = { 0,0,-1,1,-1,-1,1,1 };

	vector<pos> path;

	for (int dir = 0; dir < 8; dir++) {
		int nextRow = nowRow + dr[dir];
		int nextCol = nowCol + dc[dir];
		if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > M) {
			if (nextRow == 0) nextRow = N;
			if (nextCol == 0) nextCol = M;
			if (nextRow == N + 1) nextRow = 1;
			if (nextCol == M + 1) nextCol = 1;
		}
		//만약 자기 자신이라면 무시
		if (nextRow == attackerRow && nextCol == attackerCol) continue;
		//만약 이미 파괴되었다면 무시
		if (!TurretAlive[TurretMap[nextRow][nextCol]]) continue;
		path.push_back({ nextRow, nextCol });
	}

	//둘러쌓여있는 자리들 공격
	for (int i = 0; i < path.size(); i++) {
		int id = TurretMap[path[i].row][path[i].col];
		isAttacked[TurretIndexMap[id]] = true;
		TurretInfo[TurretIndexMap[id]].power -= (TurretInfo[AttackerId].power / 2);
	}
	//Target 공격
	TurretInfo[TargetId].power -= TurretInfo[AttackerId].power;
	isAttacked[AttackerId] = true;
	isAttacked[TargetId] = true;

}

void Attack(int AttackerId, int TargetId) {

	//레이저 먼저 판별
	bool laserSuccess = LaserAttack(AttackerId, TargetId);


	if (!laserSuccess) {
		BombAttack(AttackerId, TargetId);
		//cout << "Bomb Attack";
	}
}



void solution() {

	for (int k = 1; k <= K; k++) {

		int debugging = -1;
		for (int i = minAliveTurretID; i <= TurretNum; i++) {
			if (!TurretAlive[TurretInfo[i].id]) continue;
			isAttacked[i] = false;
		}

		sort(TurretInfo, TurretInfo + TurretNum);
		

		for (int i = 1; i <= TurretNum; i++) {
			TurretIndexMap[TurretInfo[i].id] = i;
		}

		int AttackerID = 0;
		int TargetID = 0;

		for (int i = minAliveTurretID; i <= TurretNum; i++) {
			if (!TurretAlive[TurretInfo[i].id]) continue;
			int minAliveTurretID = i;
			AttackerID = i;
			break;
		}

		for (int i = TurretNum; i >= 1; i--) {
			if (!TurretAlive[TurretInfo[i].id]) continue;
			TargetID = i;
			break;
		}

		if (AttackerID == TargetID) {
			int debugging = -1;
			continue;
		}

		TurretInfo[AttackerID].attackTime = k;
		TurretInfo[AttackerID].power += (N + M);


		Attack(AttackerID, TargetID);
		

		//3) 포탑 부서짐(공격력 0이하면 부서짐)
		for (int i = minAliveTurretID; i <= TurretNum; i++) {
			if (!TurretAlive[TurretInfo[i].id]) continue;
			if (TurretInfo[i].power <= 0) {
				TurretAlive[TurretInfo[i].id] = false;
				TurretInfo[i].power = 0;
			}
		}

		//4) 포탑 정비 (공격 안받은 포탑들은 공격력 + 1)
		//	(기준 : 공격자 x, 공격 대상 x, 그 주위 x)
		//isAttacked 로 판별

		for (int i = minAliveTurretID; i <= TurretNum; i++) {
			if (!TurretAlive[TurretInfo[i].id]) continue;
			if (!isAttacked[i]) {
				TurretInfo[i].power += 1;
			}
		}
	}

	sort(TurretInfo, TurretInfo + TurretNum);

	int Answer = 0;

	for (int i = TurretNum; i >= 1; i--) {
		if (!TurretAlive[TurretInfo[i].id]) continue;
		Answer = i;
		break;
	}

	cout << TurretInfo[Answer].power;
}

int main() {
	ios::sync_with_stdio(false);
	cin.tie(NULL); cout.tie(NULL);

	init();
	solution();

	return 0;
}


/*
문제 분석

맵 : N x M 
포탑 정보
	- 공격력 (상황에 따라 변경됨)
		- 0이하 부서짐 (공격 못함)
		- 최초 0 존재

4가지 액션 K번 반복

1. 공격자 선정
	1) 공격자 : 가장 약한 포탑 (핸디캡 N+M 공격력 증가)
		- 선정기준
			- 공격력 가장 낮은 포탑
			- 동률인 경우 : 가장 최근 공격한 포탑
			- 동률인 경우 : 행 + 열 가장 큰 포탑
			- 동률인 경우 : 열 값이 가장 큰 포탑

2. 공격 : 가장 강한 포탑 공격

1) 공격대상 : 선정 기준
	- 공격력이 가장 높은 포탑
	- 동률인 경우 : 가장 오래 전 공격한 포탑
	- 동률인 경우 : 행 + 열 가장 작은 포탑
	- 동률인 경우 : 열 값이 가장 작은 포탑


2-1) 레이저 공격 먼저 시도 (최단 경로로 공격) 

	1) 상하좌우 4개의 방향 움직일 수 있음 (우, 하, 좌, 상)
	2) 부서진 포탑 못지나감
	3) 격자 밖 -> 반대편으로 나옴 
	4) 경로 정한 뒤
		1) (공격대상 공격력 - 공격자의 공격력)
		2) (경로위 공격력 - (공격자의 공격력/2))


	Flag 필요 (레이저 성공했는지 여부)

2-2) 포탄 공격 시도
	
	1) 공격 대상에게 포탄 던짐 (공격자의 공격력만큼 피해)
	2) 나머지 8방향의 포탑들 (공격자의 공격력/2 만큼 피해)
		(격자 벗어나는 경우 반대편으로)




3) 포탑 부서짐 (공격력 0이하면 부서짐)


4) 포탑 정비

	(공격 안받은 포탑들은 공격력 +1)
	기준 : 공격자 x , 공격 대상 x , 그 주위 x




5) 포탑 중 가장 강한 포탑의 공격력을 출력
*/