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
	bool activated = true;

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
		return false;
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

turret TurretMap[11][11] = { 0, };  //포탑 ID 관리 
bool isChangedMap[11][11] = { false, };

void init() {

	cin >> N >> M >> K;
	int TurretNum = 0;
	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= M; c++) {
			TurretNum++;
			cin >> TurretMap[r][c].power;
			TurretMap[r][c].id = TurretNum;
			TurretMap[r][c].nowPos.row = r;
			TurretMap[r][c].nowPos.col = c;
			if (TurretMap[r][c].power <= 0) TurretMap[r][c].activated = false;
		}
	}
}


bool LaserAttack(pos attackerPos, pos targetPos) {


	int AttackRow = attackerPos.row;
	int AttackCol = attackerPos.col;
	int TargetRow = targetPos.row;
	int TargetCol = targetPos.col;

	//(우, 하, 좌, 상)
	const int dr[4] = { 0,1,0,-1 };
	const int dc[4] = { 1,0,-1,0 };
	bool foundPath = false;
	//BFS로 탐색

	//방문 배열 초기화
	pos Visited[11][11];
	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= M; c++) {
			Visited[r][c].row = -1;
			Visited[r][c].col = -1;
		}
	}

	queue<pos> q;
	q.push({ AttackRow, AttackCol });
	Visited[AttackRow][AttackCol] = { AttackRow, AttackCol };

	while (!q.empty()) {
		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();

		

		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			if (nextRow <= 0) nextRow = N;
			if (nextCol <= 0) nextCol = M;
			if (nextRow > N) nextRow = 1;
			if (nextCol > M) nextCol = 1;
			//죽은 포탑 못지나감
			if (TurretMap[nextRow][nextCol].power <= 0) continue;
			//방문 노드 무시
			if (!(Visited[nextRow][nextCol].row == -1 && Visited[nextRow][nextCol].col == -1)) continue;

			Visited[nextRow][nextCol] = { nowRow, nowCol };
			if (nextRow == TargetRow && nextCol == TargetCol) {
				//cout << "Laser Attack 경로\n";
				//cout << "(" << nextRow << ", " << nextCol << ") ";
				int tempRow = nextRow;
				int tempCol = nextCol;
				foundPath = true;
				while (!(tempRow == AttackRow && tempCol == AttackCol)) {
					int tempnextRow = Visited[tempRow][tempCol].row;
					int tempnextCol = Visited[tempRow][tempCol].col;
					//cout << "(" << tempnextRow << ", " << tempnextCol << ") ";
					if (tempnextRow == AttackRow && tempnextCol == AttackCol) {
						//cout << "\n";
						break;
					}
					TurretMap[tempnextRow][tempnextCol].power -= (TurretMap[AttackRow][AttackCol].power / 2);
					isChangedMap[tempnextRow][tempnextCol] = true;
					tempRow = tempnextRow;
					tempCol = tempnextCol;
				}
				if (foundPath) break;
			}
			q.push({ nextRow, nextCol });
		}

		if (foundPath) break;
	}

	if (foundPath) {
		
		isChangedMap[AttackRow][AttackCol] = true;
		isChangedMap[TargetRow][TargetCol] = true;
		TurretMap[TargetRow][TargetCol].power -= TurretMap[AttackRow][AttackCol].power;
		return true;
	}

	return false;
}


void BombAttack(pos attackerPos, pos targetPos) {

	const int dr[8] = { -1,1,0,0,-1,-1,1,1 };
	const int dc[8] = { 0,0,-1,1,-1,1,-1,1 };

	int TargetRow = targetPos.row;
	int TargetCol = targetPos.col;

	for (int dir = 0; dir < 8; dir++) {
		int nextRow = TargetRow + dr[dir];
		int nextCol = TargetCol + dc[dir];
		if (nextRow <= 0) nextRow = N;
		if (nextCol <= 0) nextCol = M;
		if (nextRow > N) nextRow = 1;
		if (nextCol > M) nextCol = 1;

		if (TurretMap[nextRow][nextCol].power <= 0) continue;
		if (!TurretMap[nextRow][nextCol].activated) continue;
		if (nextRow == attackerPos.row && nextCol == attackerPos.col) continue;
		//cout << "(" << nextRow << ", " << nextCol << ") ";
		TurretMap[nextRow][nextCol].power -= (TurretMap[attackerPos.row][attackerPos.col].power / 2);
		isChangedMap[nextRow][nextCol] = true;
	}

	isChangedMap[attackerPos.row][attackerPos.col] = true;
	isChangedMap[TargetRow][TargetCol] = true;
	//cout << "(" << TargetRow << ", " << TargetCol << ")\n";
	TurretMap[TargetRow][TargetCol].power -= TurretMap[attackerPos.row][attackerPos.col].power;

}

void Attack(pos attackerPos, pos targetPos) {

	//레이저 먼저 판별
	bool laserSuccess = LaserAttack(attackerPos, targetPos);


	if (!laserSuccess) {
		BombAttack(attackerPos, targetPos);
		//cout << "Bomb Attack\n";
	}
}



void solution() {

	
	int minRow = 1;
	int minCol = 1;
	int maxRow = 1;
	int maxCol = 1;


	for (int k = 1; k <= K; k++) {
		//cout << "---------------------------------------------------------------------\n";
		//cout << k << "번째 턴\n";

		for (int r = 1; r <= N; r++) {
			for (int c = 1; c <= M; c++) {
				//cout << TurretMap[r][c].power << " ";
			}
			//cout << "\n";
		}

		//cout << "\n";

		//공격자 선정
		//공격 대상 선정
		turret attacker = TurretMap[0][0];
		turret target = TurretMap[0][0];
		attacker.power = 2112345678;
		target.power = -2100000000;

		bool findMin = true;
		bool findMax = true;

		for (int r = 1; r <= N; r++) {
			for (int c = 1; c <= M; c++) {
				isChangedMap[r][c] = false;
				if (!TurretMap[r][c].activated) continue;
				if (TurretMap[r][c].power <= 0) continue;
				if (TurretMap[r][c] < attacker) attacker = TurretMap[r][c];
				if (target < TurretMap[r][c]) target = TurretMap[r][c];
			}
		}
		
		if (target.power == -2100000000 || attacker.power == 2112345678) {
			int debuggging = -1;
		}

		if (attacker.id == target.id) break;

		//핸디캡 & 공격시기 저장
		TurretMap[attacker.nowPos.row][attacker.nowPos.col].power += (N + M);
		TurretMap[attacker.nowPos.row][attacker.nowPos.col].attackTime = k;

		//cout << "공격자 위치: (" << attacker.nowPos.row << ", " << attacker.nowPos.col << ")\n";
		//cout << "공격대상 위치 : (" << target.nowPos.row << ", " << target.nowPos.col << ")\n";
		//cout << "공격력 : " << TurretMap[attacker.nowPos.row][attacker.nowPos.col].power << "\n";
		//cout << "주변 공격력 : " << TurretMap[attacker.nowPos.row][attacker.nowPos.col].power / 2 << "\n";
		//공격
		Attack(attacker.nowPos, target.nowPos);

		//죽은 포탑 처리
		for (int r = 1; r <= N; r++) {
			for (int c = 1; c <= M; c++) {
				if (!TurretMap[r][c].activated) continue;
				if (TurretMap[r][c].power <= 0) {
					TurretMap[r][c].activated = false;
					TurretMap[r][c].power = 0;
				}
			}
		}
		
		//cout << "\n정비된 포탑" << "\n";
		//정비
		for (int r = 1; r <= N; r++) {
			for (int c = 1; c <= M; c++) {
				if (!TurretMap[r][c].activated) continue;
				if (isChangedMap[r][c]) continue;
				//cout << "(" << r << ", " << c << ") ";
				TurretMap[r][c].power++;
			}
		}

		//cout << "\n\n\n";
	}

	int Answer = 0;

	

	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= M; c++) {
			//cout << TurretMap[r][c].power << " ";
			if (!TurretMap[r][c].activated) continue;
			Answer = max(Answer, TurretMap[r][c].power);
		}
		//cout << "\n";
	}

	cout << Answer;

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