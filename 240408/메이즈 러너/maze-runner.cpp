//시작 시간 : 19:21
//종료 시간 :
#include<iostream>
#include<vector>

using namespace std;

struct pos {
	int row;
	int col;
};

struct person {
	int id;
	bool escaped = false;
	pos nowPos;
	int path = 0;
};

/*
입력)
N : 미로 크기 (4이상 10이하)
M : 참가자 수 (1이상 10이하)
K : 게임 반복 수 (1이상 100이하)
Map 정보 : 0(빈칸) , 1~9(내구도)
참가자 좌표
출구 좌표
*/

int N, M, K;
int InputMap[11][11] = { 0, };
pos EscapePos;
person PersonInfo[11];
vector<int> PersonMap[11][11];

void init() {
	cin >> N >> M >> K;

	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			cin >> InputMap[r][c];
		}
	}

	for (int i = 1; i <= M; i++) {
		PersonInfo[i].id = i;
		cin >> PersonInfo[i].nowPos.row >> PersonInfo[i].nowPos.col;
	}

	cin >> EscapePos.row >> EscapePos.col;
}

void movePerson() {

//1. 모든 참가자 **동시에** 1칸 이동
//2. 상하좌우 
//3. 현재 칸에서 출구 최단 거리 > 움직인 칸 출구 최단거리
//	(최단거리 : abs(x1-x2) + abs(y1-y2)
//4. 우선순위 : 상하 좌우
//5. 못움직이면 움직이지 않음 (벽 or 최단거리 안가까워짐)
//6. **한칸에 2명 참가자 가능**

	//가까워지는 방향 계산
	//	1) nearPath = abs(출구좌표 - 사람좌표) 구하기
	//	2) nextnearPath = abs(출구 좌표 - 네방향 좌표)
	//	3) nextnearPath 최소 여러개면 dir 상하 먼저
	//	4) 이동

	const int dr[4] = { -1,1,0,0 };
	const int dc[4] = { 0,0,-1,1 };

	

	for (int i = 1; i <= M; i++) {
		//이미 탈출했다면 무시
		if (PersonInfo[i].escaped) continue;

		//현재 참가자의 위치
		int nowRow = PersonInfo[i].nowPos.row;
		int nowCol = PersonInfo[i].nowPos.col;
		//현재 참가자의 위치에서 출구까지 거리
		int nowNearPath = abs(nowRow - EscapePos.row) + abs(nowCol - EscapePos.col);



		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;
			if (InputMap[nextRow][nextCol] != 0) continue;
			//못움직이면 움직이지 않음(벽 or 최단거리 안가까워짐)
			int nextNearPath = abs(nextRow - EscapePos.row) + abs(nextCol - EscapePos.col);
			if (nextNearPath >= nowNearPath) continue;

			//만약 가까워지는 칸 찾았다면 이동
			PersonInfo[i].nowPos = { nextRow, nextCol };
			PersonInfo[i].path += 1;
			//이동한 칸이 탈출구라면 탈출!
			if (nextRow == EscapePos.row && nextCol == EscapePos.col) {
				PersonInfo[i].escaped = true;
			}
			break;
		}
	}



}

pair<pos, int> findSquare() {

	

	//	출구(escapeRow, escapeCol) 일 때

	//	정사각형의 변 길이 : h라고 했을 때 좌측 좌표

	//	h = 2)

	//	escapeRow - (h - 1) <= 좌측 Row <= escapeRow
	//	escapeCol - (h - 1) <= 좌측 Col <= excapeCol

	//	만약 사람이 하나라도 존재한다면
	//	반환)
	//	- 좌측좌표
	//	- 크기
	bool foundPoint = false;

	for (int h = 2; h <= N; h++) {
		for (int r = EscapePos.row - (h - 1); r <= EscapePos.row; r++) {
			if (r <= 0 || r > N) continue;
			if (r + h - 1 <= 0 || r + h - 1 > N) continue;
			for (int c = EscapePos.col - (h - 1); c <= EscapePos.col; c++) {
				if (c <= 0 || c > N) continue;
				if (c + h - 1 <= 0 || c + h - 1 > N) continue;
				//좌측 상단 좌표 : (r,c) ~ (r+h-1, c+h-1)
				for (int Row = r; Row < r + h; Row++) {
					for (int Col = c; Col < c + h; Col++) {
						if (PersonMap[Row][Col].size() != 0) {
							// cout << "사각형 크기 : " << h << "\n";
							// cout << "좌측 좌표 : (" << r << ", " << c << ")\n";
							foundPoint = true;
							return { {r,c},h };
							break;
						}
					}
					if (foundPoint) break;
				}
				if (foundPoint) break;
			}
			if (foundPoint) break;
		}
		if (foundPoint) break;
	}

}

void rotationSquare(pos squarePos, int squareSize) {

	/*
	3. 회전

		시계방향 90도 회전 알고리즘 구현
		(회전할 때 한칸에 사람 여러명 어떻게 할건지)

		- PersonInfo에서 사각형 내에 있는 사람의 ID 및(r, c) 찾아놓기
		- 돌릴 때 만약 r, c에 사람 있으면 해당 ID값들 위치 옮겨주기
	*/

	int tempMap[11][11] = { 0, };
	int escapeRow = EscapePos.row;
	int escapeCol = EscapePos.col;

	for (int r = squarePos.row; r < squarePos.row + squareSize; r++) {
		for (int c = squarePos.col; c < squarePos.col + squareSize; c++) {
			tempMap[squarePos.row - squarePos.col + c][squarePos.col + squareSize - 1 - (r - squarePos.row)] = InputMap[r][c];
			//변경 열) squarePos.row + squareSize - r  vs   N - r + 1  vs squarePos.col+squareSize -1 -(r-squarePos.row)   
			//변경 행)c vs squarePos.row - squarePos.col + c
			bool debug1 = false;
			bool debug2 = false;

			//출구도 바꿔줘야함
			if (EscapePos.row == r && EscapePos.col == c) {
				escapeRow = squarePos.row - squarePos.col + c;
				escapeCol = squarePos.col + squareSize - 1 - (r - squarePos.row);
				//cout << "출구 변경!\n";
				debug1 = true;
				//continue;
			}

			//사람 있으면 
			for (int i = 0; i < PersonMap[r][c].size(); i++) {
				debug2 = true;
				PersonInfo[PersonMap[r][c][i]].nowPos.row = squarePos.row - squarePos.col + c;
				PersonInfo[PersonMap[r][c][i]].nowPos.col = squarePos.col + squareSize - 1 - (r - squarePos.row);
			}

			if (debug1 && debug2) {
				int debugging = -1;
			}

		}
	}

	EscapePos.row = escapeRow;
	EscapePos.col = escapeCol;

	for (int r = squarePos.row; r < squarePos.row + squareSize; r++) {
		for (int c = squarePos.col; c < squarePos.col + squareSize; c++) {
			InputMap[r][c] = tempMap[r][c];
			if (InputMap[r][c] != 0) InputMap[r][c]--;
		}
	}


}

void solution() {

	for (int k = 1; k <= K; k++) {

		// cout << k << "번째 턴\n";

		//PersonMap 초기화
		for (int r = 1; r <= N; r++) {
			for (int c = 1; c <= N; c++) {
				PersonMap[r][c].clear();
			}
		}

		bool allPlayerDone = true;

		//1. 참가자 이동
		movePerson();

		//PersonMap 갱신
		for (int i = 1; i <= M; i++) {
			if (PersonInfo[i].escaped) continue;
			allPlayerDone = false;
			PersonMap[PersonInfo[i].nowPos.row][PersonInfo[i].nowPos.col].push_back(PersonInfo[i].id);
		}

		// //PersonMap 초기화
		// for (int r = 1; r <= N; r++) {
		// 	for (int c = 1; c <= N; c++) {
		// 		cout << PersonMap[r][c].size() << "명 ";
		// 	}
		// 	cout << "\n";
		// }

		
		if (allPlayerDone) break;

		// cout << "\n현재 Map\n";

		// for (int r = 1; r <= N; r++) {
		// 	for (int c = 1; c <= N; c++) {
		// 		cout << InputMap[r][c] << " ";
		// 	}
		// 	cout << "\n";
		// }


		//2. 회전할 사각형 찾기
		pair<pos,int> ret = findSquare();
		
		pos squarePos = ret.first;
		int squareSize = ret.second;

		//시계방향 90도 회전
		rotationSquare(squarePos, squareSize);
		

		// cout << "\n회전 후 Map\n";
		// for (int r = 1; r <= N; r++) {
		// 	for (int c = 1; c <= N; c++) {
		// 		cout << InputMap[r][c] << " ";
		// 	}
		// 	cout << "\n";
		// }

		// cout << "\n출구 위치 : ";
		// cout <<"(" << EscapePos.row << ", " << EscapePos.col << ")\n";
		// cout << "******************************************************************";
		// cout << "\n";
	}

	int sum = 0;

	for (int i = 1; i <= M; i++) {
		sum += PersonInfo[i].path;
	}

	cout << sum << "\n";
	cout << EscapePos.row << " " << EscapePos.col;

}

int main() {
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	init();
	solution();

	return 0;
}

/*
문제 분석

참가자 : M명
맵 : NxN (1,1)

미로 칸 정보

1) 빈칸 : 이동 가능
2) 벽 :
	- 이동 불가능
	- (1~9) 내구도 (회전할 때 -1씩 깎임)
	- 내구도 0 되면 빈칸 (이동 가능)

3) 출구 : 탈출

----------------------------------------------------------------------
K초 동안 반복 (모든 참가자가 탈출에 성공한다면)

순서)
---------------------------------------------------------
1. 참가자 한칸 식 이동

조건)

1. 모든 참가자 **동시에**
2. 상하좌우 
3. 현재 칸에서 출구 최단 거리 > 움직인 칸 출구 최단거리
	(최단거리 : abs(x1-x2) + abs(y1-y2)
4. 우선순위 : 상하 좌우
5. 못움직이면 움직이지 않음 (벽 or 최단거리 안가까워짐)
6. **한칸에 2명 참가자 가능**

-----------------------------------------------------------

이동 후 미로 회전 (회전 알고리즘 필요)

1) 한 명 이상의 참가자와 출구를 포함한 가장 작은 정사각형
2) 우선순위
	- 좌상단 좌표의 r값 작은것
	- 동률인 경우 : c값 작은것
3) 시계방향 90도 회전
4) 내구도 --;


--------------------------------------------------------

*******모든 참가자들의 이동거리 합!*************
*******출구 위치

-----------------------------------------------------------------
*/

/*
예제

5 3 8
0 0 0 0 1
9 2 2 0 0
0 1 0 1 0
0 0 0 1 0
0 0 0 0 0
1 3
3 1
3 5
3 3


1. 참가자 이동

가까워지는 방향 계산 
1) nearPath = abs(출구좌표 - 사람좌표) 구하기
2) nextnearPath = abs(출구 좌표 - 네방향 좌표)
3) nextnearPath 최소 여러개면 dir 상하 먼저
4) 이동


2-0) 참가자의 위치 판별하여 사람 존재하는지 여부 조사해놓기


2. 회전할 사각형 찾기

출구 (escapeRow, escapeCol) 일 때 

정사각형의 변 길이 : h라고 했을 때 좌측 좌표

h = 2)

escapeRow-(h-1) <= 좌측 Row <= escapeRow
escapeCol-(h-1) <= 좌측 Col <= excapeCol

만약 사람이 하나라도 존재한다면
반환) 
- 좌측좌표
- 크기 



3. 회전

시계방향 90도 회전 알고리즘 구현
(회전할 때 한칸에 사람 여러명 어떻게 할건지)

- PersonInfo에서 사각형 내에 있는 사람의 ID 및 (r,c) 찾아놓기
- 돌릴 때 만약 r,c에 사람 있으면 해당 ID값들 위치 옮겨주기

vector<int> v[r][c];  //id 저장


int tempMap[11][11]={0,};

for(int r=1;r<=N;r++){
	for(int c=1;c<=N;c++){   
		tempMap[c][N-r+1] = Map[r][c];
		//출구도 바꿔줘야함
		for(int i=0;i<v[r][c].size();i++){
			PersonMap[v[i]].row = c;
			PersonMap[v[i]].col = N-r+1;
		}
	}
}

for(int r=1;r<=N;r++){
	for(int c=1;c<=N;c++){
		Map[r][c] = temp[r][c];
		if(Map[r][c]!=0) Map[r][c]--;
	}
}

*/

/*
사람 여러명 있을 수 있는데 사람 있는지 판별 어떻게 할지 정하기

PersonMap?
아니면 PersonInfo 다 탐색하기?


이동 후 회전하기 전에 미리 사람 있는 좌표 다 파악해놓기 
----> 얘가 제일 편할 듯 (대신 초기화 매번 해야함)
*/