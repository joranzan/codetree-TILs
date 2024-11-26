#include<iostream>
#include<vector>
#include<queue>

using namespace std;



/*
필요한 자료구조

1. 정육면체 전개도 : 배열


*/

struct pos {
	int row;
	int col;
};

struct TimeDisaster {
	int row;
	int col;
	int dir;    //방향 (우좌하상)
	int valid;  //확산 상수
};

struct secondpos {
	pos nowPos;
	int nowTurn = 0;
};

int N, M, F;
int TimeWall[55][55] = { 0, }; //정육면체 전개도
int UnknownSpace[21][21] = { 0, };

pos EscapePos = { -1,-1 };  //미지의공간에서 탈출구
pos StartPosTimeWall;
pos EscapePosTimeWall; //시간의벽에서 탈출구랑 접해있는 면
pos EndPos;
TimeDisaster TimeDisasterInfo[1000];
int TurnNum = 0;

bool input() {

	TurnNum = 0;
	
	cin >> N >> M >> F;


	pos TimeWallStartPos;  //시간의벽 시작위치
	
	int started = 0; //시간의벽 위치 찾음

	for (int r = 0; r < N; r++) {
		for (int c = 0; c < N; c++) {
			cin >> UnknownSpace[r][c];
			if (UnknownSpace[r][c] == 3 && !started) {
				TimeWallStartPos = { r,c };
				started = 1;
			}
			if (UnknownSpace[r][c] == 4) {
				EndPos = { r,c };
			}
		}
	}

	//탈출구 찾기
	for (int r = -1; r <= M; r++) {
		for (int c = -1; c <= M; c++) {
			int nowRow = TimeWallStartPos.row + r;
			int nowCol = TimeWallStartPos.col + c;
			if (nowRow < 0 || nowCol < 0 || nowRow >= N || nowCol >= N) continue;
			if (UnknownSpace[nowRow][nowCol] != 0) continue;
			EscapePos.row = nowRow;
			EscapePos.col = nowCol;
		}
		if (EscapePos.row != -1 || EscapePos.col != -1) break;
	}
	
	

	//탈출구 없으면 false 반환
	if (EscapePos.row == -1 || EscapePos.col == -1) {
		return false;
	}

	//접해있는 면 찾기
	//상
	if (EscapePos.row < TimeWallStartPos.row) {
		EscapePosTimeWall.row = 0;
		EscapePosTimeWall.col = M + (EscapePos.col - TimeWallStartPos.col);
	}
	//하
	else if (EscapePos.row > TimeWallStartPos.row + M - 1) {
		EscapePosTimeWall.row = 3 * M - 1;
		EscapePosTimeWall.col = M + (EscapePos.col - TimeWallStartPos.col);
	}
	//좌
	else if (EscapePos.col < TimeWallStartPos.col) {
		EscapePosTimeWall.col = 0;
		EscapePosTimeWall.row = M + (EscapePos.row - TimeWallStartPos.row);
	}
	//우
	else if (EscapePos.col > TimeWallStartPos.col + M - 1) {
		EscapePosTimeWall.col = 3 * M - 1;
		EscapePosTimeWall.row = M + (EscapePos.row - TimeWallStartPos.row);
	}
	
	


	int debugging = -1;

	//시간의벽 전개도 초기화
	for (int r = 0; r < 3 * M; r++) {
		for (int c = 0; c < 3 * M; c++) {
			TimeWall[r][c] = -1;
		}
	}
	//동 : 좌로 90도 회전 후 가로(2M부터 3M-1까지), 세로 (M, 2M-1)까지

	for (int r = 0; r < M; r++) {
		for (int c = 0; c < M; c++) {
			cin >> TimeWall[2 * M - 1 - c][2 * M + r];
		}
	}

	//서 : 우로 90도 회전 후 가로 (M-1부터 0까지), 세로 (M, 2M-1)까지

	for (int r = 0; r < M; r++) {
		for (int c = 0; c < M; c++) {
			cin >> TimeWall[M + c][M - 1 - r];
		}
	}

	//남
	for (int r = 0; r < M; r++) {
		for (int c = 0; c < M; c++) {
			cin >> TimeWall[2 * M + r][M + c];
		}
	}

	//북 : 180도 회전 후 가로 : 
	for (int r = 0; r < M; r++) {
		for (int c = 0; c < M; c++) {
			cin >> TimeWall[M - 1 - r][2 * M -1 - c];
		}
	}

	//윗면
	for (int r = 0; r < M; r++) {
		for (int c = 0; c < M; c++) {
			cin >> TimeWall[M + r][M + c];
			if (TimeWall[M + r][M + c] == 2) {
				StartPosTimeWall.row = M + r;
				StartPosTimeWall.col = M + c;
			}
		}
	}

	for (int i = 0; i < F; i++) {
		cin >> TimeDisasterInfo[i].row >> TimeDisasterInfo[i].col >> TimeDisasterInfo[i].dir >> TimeDisasterInfo[i].valid;
	}


	return true;
}


pos Mapping(int nowRow, int nowCol, int nextRow, int nextCol) {

	pos ret = { -1,-1 };
	//현재 북면일때
	if (nowRow >= 0 && nowRow < M && nowCol >= M && nowCol < 2 * M) {
		if (nextCol < M) {
			ret.row = M;
			ret.col = nextRow;
		}
		else if (nextCol >= 2 * M) {
			ret.row = M;
			ret.col = 3 * M - 1 - nextRow;
		}
		else {
			ret = { nextRow, nextCol };
		}
	}
	//현재 남면일때
	else if (nowRow >= 2 * M && nowRow < 3 * M && nowCol >= M && nowCol < 2 * M) {
		if (nextCol < M) {
			ret.row = 2 * M - 1;
			ret.col = 3 * M - 1 - nextRow;
		}
		else if (nextCol >= 2 * M) {
			ret.row = 2 * M - 1;
			ret.col = nextRow;
		}
		else {
			ret = { nextRow, nextCol };
		}
	}
	//현재 서면일때
	else if (nowRow >= M && nowRow<2 * M && nowCol >= 0 && nowCol < M) {
		if (nextRow < M) {  //위로 벗어남
			ret.row = nextCol;
			ret.col = M;
		}
		else if (nextRow >= 2 * M) {  //아래로 벗어남
			ret.row = 3 * M - 1 - nextCol;
			ret.col = M;
		}
		else {
			ret = { nextRow, nextCol };
		}
	}
	//현재 동면일때
	else if (nowRow >= M && nowRow < 2 * M && nowCol >= 2*M && nowCol < 3 * M) {
		if (nextRow < M) {  //위로 벗어남
			ret.row = 3 * M - 1 - nextCol;
			ret.col = 2 * M - 1;
		}
		else if (nextRow >= 2 * M) {  //아래로 벗어남
			ret.row = nextCol;
			ret.col = 2 * M - 1;
		}
		else {
			ret = { nextRow, nextCol };
		}
	}
	else {
		ret.row = nextRow;
		ret.col = nextCol;
	}

	return ret;
}


bool findShortestTimeWall() {
	//StartPosTimeWall 부터 EscapePosTimeWall까지 최단 거리

	//*******************************
	//전개도에서 동서남북 면들이 이어져야함 Mapping 함수 짜야함

	queue<pos> q;
	int Visited[55][55] = {0,};
	int dr[4] = { -1,1,0,0 };
	int dc[4] = { 0,0,-1,1 };
	
	q.push(StartPosTimeWall);
	Visited[StartPosTimeWall.row][StartPosTimeWall.col] = 1;


	while (!q.empty()) {

		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();

		if (nowRow == EscapePosTimeWall.row && nowCol == EscapePosTimeWall.col) break;

		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			if (nextRow < 0 || nextCol < 0 || nextRow >= 3 * M || nextCol >= 3 * M) continue;
			pos nextPos = Mapping(nowRow, nowCol, nextRow, nextCol);
			nextRow = nextPos.row;
			nextCol = nextPos.col;
			if (TimeWall[nextRow][nextCol] != 0) continue;
			if (Visited[nextRow][nextCol]!=0) continue;
			Visited[nextRow][nextCol] = Visited[nowRow][nowCol] + 1;
			q.push({ nextRow,nextCol });
		}
	}


	if (Visited[EscapePosTimeWall.row][EscapePosTimeWall.col] == 0) {
		return false;
	}
	else {
		TurnNum = Visited[EscapePosTimeWall.row][EscapePosTimeWall.col] - 1;
		return true;
	}

}


bool MoveUnknownSpace() {

	queue <secondpos> q;
	q.push({ { EscapePos.row, EscapePos.col }, 1 });
	int CurrentTurn = 1;
	int Visited[20][20] = { 0, };
	Visited[EscapePos.row][EscapePos.col] = 1;

	while (!q.empty()) {
		

		//시간이상 이동
		int dr[4] = { 0,0,1,-1 };
		int dc[4] = { 1,-1,0,0 };
		for (int i = 0; i < F; i++) {
			if (TimeDisasterInfo[i].valid == -1) continue;
			if ((TurnNum + CurrentTurn) % TimeDisasterInfo[i].valid == 0) {
				int nextRow = TimeDisasterInfo[i].row + dr[TimeDisasterInfo[i].dir];
				int nextCol = TimeDisasterInfo[i].col + dc[TimeDisasterInfo[i].dir];
				//만약 시간이상이 맵 밖으로 확산된다면 종료
				if (nextRow < 0 || nextCol < 0 || nextRow >= N || nextCol >= N) {
					TimeDisasterInfo[i].valid = -1;
					continue;
				}
				//만약 빈공간이 아니라면 확산 종료
				if (UnknownSpace[nextRow][nextCol] > 0 && UnknownSpace[nextRow][nextCol] < 100) {
					TimeDisasterInfo[i].valid = -1;
					continue;
				}

				//확산 가능하다면
				TimeDisasterInfo[i].row = nextRow;
				TimeDisasterInfo[i].col = nextCol;
				UnknownSpace[nextRow][nextCol] = 100;
			}
			
		}
		while (q.front().nowTurn == CurrentTurn) {
			int nowRow = q.front().nowPos.row;
			int nowCol = q.front().nowPos.col;
			int nowTurn = q.front().nowTurn;
			q.pop();

			for (int dir = 0; dir < 4; dir++) {
				int nextRow = nowRow + dr[dir];
				int nextCol = nowCol + dc[dir];
				if (nextRow < 0 || nextCol < 0 || nextRow >= N || nextCol >= N) continue;
				if (Visited[nextRow][nextCol] != 0) continue;
				if (UnknownSpace[nextRow][nextCol] == 4) {
					TurnNum += nowTurn;
					return true;
				}
				if (UnknownSpace[nextRow][nextCol] != 0) continue;

				Visited[nextRow][nextCol] = nowTurn + 1;
				q.push({ {nextRow,nextCol}, nowTurn + 1 });

				
			}
			if (q.empty()) break;
		}
		CurrentTurn++;
		
	}

	

	/*
	1. 이동할 수 있는 시간이상현상 이동
	2. queue에 담긴 애들 중 현재단계 이동
	
	
	
	*/
	


	return false;

}

bool solution() {

	bool TimeWallisValid = findShortestTimeWall();
	int debugging = -1;
	if (TimeWallisValid) {
		//시간이상 TurnNum+1 만큼 옮기기 (현재 타임머신은 시간의 벽에 있음)

		for (int i = 0; i < F; i++) {
			UnknownSpace[TimeDisasterInfo[i].row][TimeDisasterInfo[i].col] = 100;
		}


		for (int nowtime = 1; nowtime <= TurnNum + 1; nowtime++) {
			//(우좌하상)
			int dr[4] = { 0,0,1,-1 };
			int dc[4] = { 1,-1,0,0 };
			for (int i = 0; i < F; i++) {
				if (TimeDisasterInfo[i].valid == -1) continue;
				if (nowtime % TimeDisasterInfo[i].valid == 0) {  //배수일때만
					int nextRow = TimeDisasterInfo[i].row + dr[TimeDisasterInfo[i].dir];
					int nextCol = TimeDisasterInfo[i].col + dc[TimeDisasterInfo[i].dir];
					//만약 시간이상이 맵 밖으로 확산된다면 종료
					if (nextRow < 0 || nextCol < 0 || nextRow >= N || nextCol >= N) {
						TimeDisasterInfo[i].valid = -1;
						continue;
					}
					//만약 빈공간이 아니라면 확산 종료
					if (UnknownSpace[nextRow][nextCol] > 0 && UnknownSpace[nextRow][nextCol]<100) {
						TimeDisasterInfo[i].valid = -1;
						continue;
					}

					//확산 가능하다면
					TimeDisasterInfo[i].row = nextRow;
					TimeDisasterInfo[i].col = nextCol;
					UnknownSpace[nextRow][nextCol] = 100;
				}
			}
		}

		int debugging = -1;
		//탈출구가 막혔다면 False
		if (UnknownSpace[EscapePos.row][EscapePos.col] == 100) {
			//cout << "-1";
			return false;
		}
		
		//평면으로 넘어왔음 이제
		TurnNum++; 

		//평면에서 이동하는 함수
		return MoveUnknownSpace();
	}
	else {
		//cout << "-1";
		return false;
	}

}

int main() {

	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	bool isValid = input();

	if (!isValid) {
		cout << "-1";
	}
	else {
		if (solution()) {
			cout << TurnNum;
		}
		else cout << "-1";
	}

	return 0;
}



/*
[미지의 공간 탈출 문제분석]

맵 : NxN
미션 : 타임머신 타고 공간 탈출

정보)
1. 맵 : NxN
2. 시간의 벽(정육면체) 한변 : M

타임머신 스캔기능

1) 공간평면도 : 위에서 본 관점
2) 시간의 벽 단면도 : 윗면, 동서남북
3) 0(빈 공간)과 1(장애물)
4) 시간의벽과 미지의공간 맞닿은 부분은 장애물 (하나만 탈출구)


타임머신 이동
1) 타임머신은 빈공간(0)만 이동 가능 (장애물 X)
2) 초기 위치 : 시간의 벽 윗면 (2로 표현)
3) 평면도에 시간의 벽 위치 (3)
4) 평면도에 탈출구 위치 (4) -> 시간의 벽 외부에 있는 미지의 공간의 바닥
5) 이동
	- 매 턴마다 상하좌우 이동
	- 시간이상현상 피해야함
	- 시간 이상 현상이 확산된 직후 타임머신이 이동
		-> 타임머신은 시간 이상 현상이 확산되는 곳으로 이동할 수 없음에 유의



시간이상현상 : 미지의공간 바닥에 F개의
1) 바닥의 빈 공간 r,c에서 시작
2) 매 v의 배수 턴 마다 방향 d로 확산
	- 기존 위치 사라지지 않고 유지
	- 빈공간으로만 확산
	- 더이상 확산할 수 없다면 멈춤
	- **동시에 확산
3) 방향 : 0123 동서남북 우좌하상


출력
	- 탈출 최소 시간(턴 수)
	- 탈출할 수 없다면 -1


입력
1) N M F
2) N개의 줄에 미지의공간 평면도
3) 동서남북위 평면도 각각 M줄
3) F줄에 걸쳐 (r,c) d v




**시뮬레이션

      1 1 1
      0 0 1
      0 0 0    
1 1 1 2 0 0 1 0 1
1 0 1 0 1 0 1 0 1
1 1 1 0 0 0 1 0 0
      0 0 1
	  1 0 0
	  1 0 1


탈출구와 맞닿아있는 면 찾기**
*/