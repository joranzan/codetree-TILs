//https://www.codetree.ai/training-field/frequent-problems/problems/maze-runner/description?page=1&pageSize=20

#include<iostream>
#include<vector>
#include<queue>

using namespace std;

struct pos {
	int row;
	int col;
};

struct P_Info {
	int row;
	int col;
	int index;
};


struct nextpos {
	int row;
	int col;
	int dist; //출구까지 최단 거리
	int dir;

	bool operator<(nextpos next) const {
		if (dist < next.dist) return false;
		else if (dist > next.dist) return true;
		else {
			if (dir < next.dir) return false;
			else if (dir > next.dir) return true;
			else return false;
		}
	}
};

struct spaceInfo {
	int arr[10] = { 0 };
};

//문제 분석
//참가자 : M명   N x N 격자 (1,1)

//미로 상태
//1. 빈칸 : 참가자 이동 가능
//2. 벽 : 참가자 이동 불가 (1~9)
//   - 회전 시 내구도 1 깎임 (0되면 빈칸으로 변경)
//3. 출구 : 참가자 도달 시 탈출!


// 1초에 참가자 한칸 씩 움직임
// 움직이는 조건
// 1) 최단 거리 : abs(x1-x2) + abs(y1-y2)
// 2) 참가자 동시에 움직임 -> 다 움직이고 처리할거 처리해야함
// 3) 상하좌우 (빈칸만)  우선순위: 상하
// 4) 출구까지 최단거리 더 가까워지게만 움직임 가능
// 5) 움직일 곳 없으면 Stay
// 6) 한칸에 여러명 있어도 됨 (참가자 현재 위치 배열 만들기)


//미로 회전
//1명 이상의 참가자와 출구 포함한 가장 작은 정사각형
//우선 순위 : 좌상단 Row좌표 작은 애 -> 좌상단 Col 좌표 작은애
//시계방향 90 회전 (사람, 출구, 내구도 다 같이 돌아감)
//내구도 1 깎임

//모두 탈출 시 종료 -> 참가자의 이동거리 합 + 출구 좌표
int N, M, K;
int Map[11][11] = { 0, };
spaceInfo Person[11][11] = { };  //사람 위치 -> 회전시킬때 바꿔주기 위함
pos Escape;  //출구 좌표
P_Info Info[10];  //사람들의 정보
bool Escaped[10] = { false, };  //탈출여부
int accDist[10] = { 0, };   //거리 누적

void move() {
	// 움직이는 조건
	// 1) 최단 거리 : abs(x1-x2) + abs(y1-y2)
	// 2) 참가자 동시에 움직임 -> 다 움직이고 처리할거 처리해야함
	// 3) 상하좌우 (빈칸만)  우선순위: 상하
	// 4) 출구까지 최단거리 더 가까워지게만 움직임 가능
	// 5) 움직일 곳 없으면 Stay
	// 6) 한칸에 여러명 있어도 됨 (참가자 현재 위치 배열 만들기) -> INFO

	const int dr[4] = { -1,1,0,0 };
	const int dc[4] = { 0,0,-1,1 };

	for (int i = 0; i < M; i++) {
		//이미 탈출했으면 무시
		if (Escaped[i]) continue;
		int nowRow = Info[i].row;
		int nowCol = Info[i].col;
		int nowDist = abs(nowRow - Escape.row) + abs(nowCol - Escape.col);
		priority_queue<nextpos> pq;
		for (int dir = 0; dir < 4; dir++) {
			int next_r = nowRow + dr[dir];
			int next_c = nowCol + dc[dir];
			if (next_r <= 0 || next_c <= 0 || next_r > N || next_c > N) continue;
			if (Map[next_r][next_c] > 0) continue;
			// 1) 최단 거리 : abs(x1-x2) + abs(y1-y2)
			// 3) 상하좌우 (빈칸만)  우선순위: 상하
			// 4) 출구까지 최단거리 더 가까워지게만 움직임 가능
			int next_dist = abs(next_r - Escape.row) + abs(next_c - Escape.col);
			if (next_dist >= nowDist) continue;
			pq.push({ next_r,next_c, next_dist, dir });
		}
		// 5) 움직일 곳 없으면 Stay
		if (pq.empty()) continue;
		int next_r = pq.top().row;
		int next_c = pq.top().col;
		Info[i].row = next_r;
		Info[i].col = next_c;
		accDist[i]++;
		Person[nowRow][nowCol].arr[i] = 0;
		Person[next_r][next_c].arr[i] = 1;
		if (Info[i].row == Escape.row && Info[i].col == Escape.col) {
			Escaped[i] = true;
		}
	}
}


void rotation(int start_r, int start_c, int squareSize) {
	//시계방향 90 회전 (사람, 출구, 내구도 다 같이 돌아감)
	//내구도 1 깎임
	int temp[11][11] = { 0, };
	bool escapeSwap = false;
	bool personSwap[10] = { false };
	//start_r   start_r+squareSize-1    start_c   start_c+squareSize-1
	//일단 회전
	for (int i = 0; i < squareSize; i++) {
		for (int j = 0; j < squareSize; j++) {
			//start_c + squareSize - 1 - i
			temp[start_r+j][start_c + squareSize - 1 - i] = Map[start_r + i][start_c + j];
			
			if ((start_r + i) == Escape.row && (start_c + j) == Escape.col&&!escapeSwap) {
				Escape.row = start_r + j;
				Escape.col = start_c + squareSize - 1 - i;
				escapeSwap = true;
				continue;
			}
			//Info 배열
			for (int k = 0; k < M; k++) {
				if (Escaped[k]) continue;
				if ((start_r + i) == Info[k].row && (start_c + j) == Info[k].col&&!personSwap[k]) {
					Info[k].row = start_r + j;
					Info[k].col = start_c + squareSize - 1 - i;
					personSwap[k] = true;
				}
			}
		}
	}

	for (int i = 0; i < squareSize; i++) {
		for (int j = 0; j < squareSize; j++) {
			Map[start_r + i][start_c + j] = temp[start_r + i][start_c + j];
			if (Map[start_r + i][start_c + j] > 0) {
				Map[start_r + i][start_c + j]--;
			}
		}
	}
}

void findSquare() {

	int squareSize = 2;
	bool flag = false;
	while (squareSize <= N) {
		int e_r = Escape.row;
		int e_c = Escape.col;

		vector<pos> squareCandi;

		int min_r = e_r - (squareSize - 1);    //탐색 시작 좌측 상단 좌표 r
		int min_c = e_c - (squareSize - 1);    //탐색 시작 좌측 상단 좌표 c
		if (min_r <= 0) min_r = 1;
		if (min_c <= 0) min_c = 1;

		for (int left_r = min_r; left_r <= e_r; left_r++) {
			if (left_r <= 0 || left_r > N) continue;
			for (int left_c = min_c; left_c <= e_c; left_c++) {
				if (left_c <= 0 || left_c > N) continue;
				squareCandi.push_back({ left_r, left_c });
			}
		}
		bool found = false;
		pos Result;
		for (int i = 0; i < squareCandi.size(); i++) {
			int start_r = squareCandi[i].row;
			int start_c = squareCandi[i].col;

			int personCnt = 0;
			//1명 이상의 참가자와 출구 포함한 가장 작은 정사각형
			//우선 순위 : 좌상단 Row좌표 작은 애 -> 좌상단 Col 좌표 작은애
			for (int r = 0; r < squareSize; r++) {
				for (int c = 0; c < squareSize; c++) {
					int now_r = start_r + r;
					int now_c = start_c + c;

					//참가자 좌표 탐색하는 코드
					//참가자 존재하면 personCnt++
					for (int k = 0; k < M; k++) {
						if (Escaped[k]) continue;
						if (Info[k].row == now_r && Info[k].col == now_c) {
							personCnt++;
							break;
						}
						
					}
					if (personCnt >= 1) {
						found = true;
						Result = { start_r, start_c };
						break;
					}
					
					//break
				}
				if (found) break;
			}
			if (found) break;
		}
		if (found) {
			rotation(Result.row, Result.col, squareSize);
			flag = true;
			break;
		}
		if (flag) break;
		squareSize++;
	}
}


void input() {
	cin >> N >> M >> K;
	for (int i = 1; i <= N; i++) {
		for (int j = 1; j <= N; j++) {
			cin >> Map[i][j];
			for (int k = 0; k < 10; k++) {
				Person[i][j].arr[k] = 0;
			}
		}
	}

	for (int i = 0; i < M; i++) {
		int r, c;
		cin >> r >> c;
		Info[i] = { r,c, i };
		Person[r][c].arr[i] = 1;
	}

	int target_r, target_c;
	cin >> target_r >> target_c;
	Escape = { target_r,target_c };

}




void solution() {
	for (int i = 0; i < K; i++) {
		move();
		findSquare();
	}
	int Ans = 0;
	for (int i = 0; i < M; i++) {
		Ans += accDist[i];
	}
	cout << Ans << "\n";
	cout << Escape.row << " " << Escape.col << "\n";
}


int main() {
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
	input();
	solution();

	return 0;
}