#include<iostream>

using namespace std;

//0:상, 1:하, 2:좌, 3:우
const int dr[4] = { -1,1,0,0 };  
const int dc[4] = { 0,0,-1,1 };
int Map[100][100] = { 0, };
int TreeMap[100][100] = { 0, };
int RunnerMap[100][100] = { 0, };
int taggerRow = -1;
int taggerCol = -1;
int taggerDir = -1;

struct pos {
	int row;
	int col;
};

struct tagger {
	pos nowPos;
	int dir;    //바라보는 시야
};

struct runner {
	int runnerId;
	pos nowPos;
	int moveType;    // 1: 좌우  2: 상하 
	int trigger = 0; //바라보는 방향 바뀌었는지 여부
	int dir[3][2] = { {0,0},{3,2}, {1,0} };  // dir[moveType][trigger] : 현재 방향
	bool alive = true;
};

int N, M, H, K;
runner RunnerInfo[10000];
tagger TaggerPath[10000];

void init() {

	cin >> N >> M >> H >> K;
	
	//초기화
	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			RunnerMap[r][c] = -1;
			Map[r][c] = 0;
			TreeMap[r][c] = 0;
		}
	}

	for (int i = 0; i < M; i++) {
		int r, c, t;
		cin >> r >> c >> t;
		RunnerInfo[i].runnerId = i;
		RunnerInfo[i].nowPos.row = r;
		RunnerInfo[i].nowPos.col = c;
		RunnerInfo[i].moveType = t;
		RunnerMap[r][c] = i;
	}

	for (int i = 0; i < H; i++) {
		int r, c;
		cin >> r >> c;
		TreeMap[r][c] = 1;
	}

	taggerRow = N / 2 + 1;
	taggerCol = N / 2 + 1;
	taggerDir = 0;
	//술래의 경로 미리 구해놓기 

	int TempMap[100][100] = { 0, };
	int pathIndex = 0;
	//0:상, 1:하, 2:좌, 3:우
	int startRow = N / 2 + 1;
	int startCol = N / 2 + 1;
	int startDir = 0;  //바라보는 시야

	// 0:우상 1:우하 2:좌하 3:좌상
	const int diag_dr[4] = { -1,1,1,-1 };
	const int diag_dc[4] = { 1,1,-1,-1 };

	/*
	밖으로 나갈 때
	1 3 3 3 1
	0 0 3 1 1
	0 0 0 1 1
	0 0 2 2 1
	0 2 2 2 2


	*/
	int temp_r[4] = { startRow,startRow, startRow, startRow};
	int temp_c[4] = { startCol, startCol, startCol, startCol+1 };
	int changedir[4] = { 2,3,1,4 };

	//방향 전환되는 칸 찾기
	for (int i = 0; i < 4; i++) {
		while (1) {
			temp_r[i] += diag_dr[i];
			temp_c[i] += diag_dc[i];
			if (temp_r[i] <= 0 || temp_c[i] <= 0 || temp_r[i] > N || temp_c[i] > N) break;
			TempMap[temp_r[i]][temp_c[i]] = changedir[i];
		}
	}
	TempMap[N / 2 + 1][N / 2 + 1] = 1;
	int nowRow = startRow;
	int nowCol = startCol;
	int nowDir = TempMap[nowRow][nowCol];

	//술래 경로에 추가
	while (nowRow != 1 || nowCol != 1) {
		
		if (TempMap[nowRow][nowCol] != 0){
			nowDir = TempMap[nowRow][nowCol];
		}

		TaggerPath[pathIndex].dir = nowDir-1;
		TaggerPath[pathIndex].nowPos.row = nowRow;
		TaggerPath[pathIndex].nowPos.col = nowCol;

		pathIndex++;

		nowRow += dr[nowDir - 1];
		nowCol += dc[nowDir - 1];

	}

	//(1,1) 칸도 추가해주기
	TempMap[nowRow][nowCol] = 2;
	nowDir = 2;
	TaggerPath[pathIndex].nowPos.row = nowRow;
	TaggerPath[pathIndex].nowPos.col = nowCol;
	TaggerPath[pathIndex].dir = 1;
	pathIndex++;

	//반대방향 경로 추가하기
	
	int temp_r_rv[4] = { startRow,startRow, startRow, startRow };
	int temp_c_rv[4] = { startCol, startCol, startCol, startCol + 1 };
	changedir[0] = 3;
	changedir[1] = 1;
	changedir[2] = 4;
	changedir[3] = 2;

	for (int i = 0; i < 4; i++) {
		while (1) {
			temp_r_rv[i] += diag_dr[i];
			temp_c_rv[i] += diag_dc[i];
			if (temp_r_rv[i] <= 0 || temp_c_rv[i] <= 0 || temp_r_rv[i] > N || temp_c_rv[i] > N) break;
			TempMap[temp_r_rv[i]][temp_c_rv[i]] = changedir[i];
		}
	}

	/*
	2 2 3 3 3
	2 2 2 3 1
	2 2 1 1 1
	2 4 4 1 1
	4 4 4 4 1
	
	*/


	//(2,1)부터 시작
	nowRow += dr[nowDir - 1];
	nowCol += dc[nowDir - 1];
	
	nowDir = 2;
	while (nowRow != N / 2 + 1 || nowCol != N / 2 + 1) {
		if (TempMap[nowRow][nowCol] != 0) {
			nowDir = TempMap[nowRow][nowCol];
		}

		TaggerPath[pathIndex].dir = nowDir - 1;
		TaggerPath[pathIndex].nowPos.row = nowRow;
		TaggerPath[pathIndex].nowPos.col = nowCol;
		pathIndex++;

		nowRow += dr[nowDir - 1];
		nowCol += dc[nowDir - 1];
	}

	int debugging = -1;
}

void runnerMove() {

	/*
	1) m명의 도망자 "동시"에 움직임  (바라본 방향 1칸)
	-  (|x1 - x2| + |y1 - y2| <=3) 조건 만족하는 도망자
	-  1) 격자를 벗어나지 않는 경우
		- 술래가 있다면 움직이지 않음
		- 나무있어도 그냥 감
	-  2) 격자 벗어나는 경우
		- 방향 반대로 틀기
		- 술래없다면 그 방향 1칸 이동
	*/




	for (int i = 0; i < M; i++) {
		if (!RunnerInfo[i].alive) continue;
		int nowRow = RunnerInfo[i].nowPos.row;
		int nowCol = RunnerInfo[i].nowPos.col;
		//(|x1 - x2| + |y1 - y2| <=3) 조건 만족하는 도망자
		int dist = abs(taggerRow - nowRow) + abs(taggerCol - nowCol);
		if (dist > 3) continue;

		int nextRow = nowRow + dr[RunnerInfo[i].dir[RunnerInfo[i].moveType][RunnerInfo[i].trigger]];
		int nextCol = nowCol + dc[RunnerInfo[i].dir[RunnerInfo[i].moveType][RunnerInfo[i].trigger]];
		
		//1) 격자를 벗어나지 않는 경우
		if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) {
			RunnerInfo[i].trigger = 1;
			nextRow = nowRow + dr[RunnerInfo[i].dir[RunnerInfo[i].moveType][RunnerInfo[i].trigger]];
			nextCol = nowCol + dc[RunnerInfo[i].dir[RunnerInfo[i].moveType][RunnerInfo[i].trigger]];
		}
		//술래 있다면 자리 유지
		if (taggerRow == nextRow && taggerCol == nowCol) {
			continue;
		}
		//술래 없다면 이동
		else {
			RunnerMap[nowRow][nowCol] = -1;
			RunnerInfo[i].nowPos.row = nextRow;
			RunnerInfo[i].nowPos.col = nextCol;
			RunnerMap[nextRow][nextCol] = RunnerInfo[i].runnerId;
		}
	}
}

void taggerMove(int turn) {
	int maxPathIndex = N * N * 2 - 2;
	turn %= maxPathIndex;

	taggerRow = TaggerPath[turn].nowPos.row;
	taggerCol = TaggerPath[turn].nowPos.col;
	taggerDir = TaggerPath[turn].dir;

}

int catchRunner() {

	/*
	3) 시야 내에 도망자 잡기
	- 무조건 3칸(격자밖 예외처리) *****
	- 바라보는 방향 3칸에 있는 도망자 잡음
	- 나무가 존재하는 경우 가려져서 안보임 (continue)
	- 점수 얻음 (t번째 턴 x 현재 턴에서 잡힌 도망자 수)
	*/
	int cnt = 0;

	for (int i = 0; i < 3; i++) {
		//무조건 3칸(격자밖 예외처리) * ****
		int nowRow = taggerRow + i * dr[taggerDir];
		int nowCol = taggerCol + i * dc[taggerDir];
		if (nowRow <= 0 || nowCol <= 0 || nowRow > N || nowCol > N) break;
		//도망장 없다면 무시
		if (RunnerMap[nowRow][nowCol] == -1) continue;
		if (!RunnerInfo[RunnerMap[nowRow][nowCol]].alive) continue;
		//나무가 존재하는 경우 가려져서 안보임 (continue)
		if (TreeMap[nowRow][nowCol] == 1) continue;


		//도망자 잡기
		RunnerInfo[RunnerMap[nowRow][nowCol]].alive = false;
		RunnerMap[nowRow][nowCol] = -1;
		cnt++;
	}

	return cnt;
}

void solution() {

	int score = 0;

	for (int k = 1; k <= K; k++) {
		//도망자 이동
		runnerMove();
		//술래 이동
		taggerMove(k);
		//도망자 잡기
		//점수 얻음 (t번째 턴 x 현재 턴에서 잡힌 도망자 수)
		score += catchRunner() * k;
		
		int debugging = -1;

		
	}
	cout << score << "\n";
}

int main() {
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	init();
	solution();

	return 0;
}

/*
문제 분석

맵 : NxN  (100미만) 반드시 홀수   (1,1) (N,N)
------------------------------------------------
도망자 : M명 (1이상 N^2 이하)

	- 중앙에서 시작 안함
	- (좌/우): 오른쪽 바라보고 시작
	- (상/하): 아래쪽 바라보고 시작
------------------------------------------------
나무위치 : H개 (1이상 N^2 이하)

	- 도망자와 초기에 겹쳐져서 주어지는 것 가능
-------------------------------------------------	
술래 : 정중앙 시작 (N/2+1)

=================================================

술래잡기 게임 순서
K번 반복(100이하)
1) m명의 도망자 "동시"에 움직임  (바라본 방향 1칸)
	-  (|x1 - x2| + |y1 - y2| <=3) 조건 만족하는 도망자
	-  1) 격자를 벗어나지 않는 경우
		- 술래가 있다면 움직이지 않음
		- 나무있어도 그냥 감
	-  2) 격자 벗어나는 경우
		- 방향 반대로 틀기
		- 술래없다면 그 방향 1칸 이동
========================================================

2) 술래 움직임 (달팽이 방향) -> Mapping 해야함
	- 달팽이 방향 1칸 이동 (dir 관리)
	- 현재 위치 (N/2+1,N/2+1)
	*******************
	- 1칸 이동 후 이동방향이 틀어지는 지점인 경우 -> 바로 방향 틀기
	- (1,1), (N/2+1,N/2+1) 에서도 방향 바로 틀어야함
	******************
==================================================================
3) 시야 내에 도망자 잡기
	- 무조건 3칸(격자밖 예외처리) *****
	- 바라보는 방향 3칸에 있는 도망자 잡음
	- 나무가 존재하는 경우 가려져서 안보임 (continue)
	- 점수 얻음 (t번째 턴 x 현재 턴에서 잡힌 도망자 수)
=========================================================================
3) 턴 종료

==============================================================

입력 : 
r, c , d (d: 1(좌우) 2(상하))


*/

/*
필요한 정보



*/