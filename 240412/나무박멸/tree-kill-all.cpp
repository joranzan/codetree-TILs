//시작 시간 : 07:59
//종료 시간 :
#include<iostream>
#include<vector>

using namespace std;

struct pos {
	int row;
	int col;
};

int N, M, K, C;
int TreeMap[21][21] = { 0, };  //벽: -1
int PoisonMap[21][21] = { 0, };  //제초제를 뿌릴 때
int Answer = 0;
const int tree_dr[4] = { -1,1,0,0 };
const int tree_dc[4] = { 0,0,-1,1 };
const int poison_dr[4] = { -1,-1,1,1 };
const int poison_dc[4] = { -1,1,-1,1 };

int totalTreeCnt = 0;

void init() {
	cin >> N >> M >> K >> C;

	Answer = 0;
	totalTreeCnt = 0;

	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			cin >> TreeMap[r][c];
			if (TreeMap[r][c] >= 1) totalTreeCnt++;

			PoisonMap[r][c] = 0;
		}
	}

	
}

void growTree() {
	//1) 나무의 성장
	//1) 인접한 4개의 칸 중 나무가 있는 칸의 수만큼 성장
	//2)***** 모든 나무가 동시에 일어남

	int tempMap[21][21] = { 0, };

	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			if (TreeMap[r][c] < 1) continue;
			int cnt = TreeMap[r][c];
			for (int dir = 0; dir < 4; dir++) {
				int nextRow = r + tree_dr[dir];
				int nextCol = c + tree_dc[dir];
				if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;   //격자 밖 무시
				if (TreeMap[nextRow][nextCol] < 1) continue;  //벽이거나 나무 없으면 무시
				cnt++;
			}
			tempMap[r][c] = cnt;
		}
	}

	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			if (tempMap[r][c] < 1) continue;
			TreeMap[r][c] = tempMap[r][c];
		}
	}

	//디버깅용
	// cout << "나무의 성장\n";
	// for (int r = 1; r <= N; r++) {
	// 	for (int c = 1; c <= N; c++) {
	// 		cout << TreeMap[r][c] << " ";
	// 	}
	// 	cout << "\n";
	// }
	// cout << "\n";

}

void reproduction() {
	//2) 번식

	//1) 기존에 있는 나무들은 인접한 4개의 칸 성장
	//1) 벽이면 무시
	//2) 다른 나무면 무시
	//3) 제초제면 무시
	//4) 가능한 칸으로 나눈 값으로 번식

	int tempMap[21][21] = { 0, };

	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			if (TreeMap[r][c] < 1) continue;
			int nowTreeCnt = TreeMap[r][c];

			int cnt = 0;
			vector<int> targetDir;

			for (int dir = 0; dir < 4; dir++) {
				int nextRow = r + tree_dr[dir];
				int nextCol = c + tree_dc[dir];
				if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;   //격자 밖 무시
				if (TreeMap[nextRow][nextCol] != 0) continue;  //벽이거나 나무면 무시
				if (PoisonMap[nextRow][nextCol] != 0) continue;
				cnt++;
				targetDir.push_back(dir);
			}

			if (cnt == 0) continue;
			int nextCnt = nowTreeCnt / cnt;

			for (int i = 0; i < cnt; i++) {
				int nextRow = r + tree_dr[targetDir[i]];
				int nextCol = c + tree_dc[targetDir[i]];
				tempMap[nextRow][nextCol] += nextCnt;
			}
			tempMap[r][c] = nowTreeCnt;
		}

		
	}


	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			if (tempMap[r][c] < 1) continue;
			TreeMap[r][c] = tempMap[r][c];
		}
	}

	//디버깅용
	// cout << "나무의 번식\n";
	// for (int r = 1; r <= N; r++) {
	// 	for (int c = 1; c <= N; c++) {
	// 		cout << TreeMap[r][c] << " ";
	// 	}
	// 	cout << "\n";
	// }
	// cout << "\n";

}
void scatterPoison() {
	//3) 제초제 뿌림
	//1) 각 칸 중 제초제를 뿌렸을 때 가장 많이 박멸되는 칸에 뿌림
	//2) 제초제 전파 조건
	//2 - 1) 나무가 없는 곳 : 아무 일도 안 일어남
	//2 - 2) 나무가 있는 곳
	//1) K칸 만큼 대각선으로 전파
	//2) 전파되는 도중 벽이거나 나무 없으면 멈춤
	//3) C년만큼 제초제가 남아있음(C + 1)년에 없어짐
	//	4) 이미 있는데 뿌려지면 다시 C년동안 유지

	pos ResultIndex;
	int maxCnt = -1;
	// 20x20x4x20 = O(32000000)
	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			if (TreeMap[r][c] < 1) continue;
			int cnt = TreeMap[r][c];
			for (int dir = 0; dir < 4; dir++) {
				for (int k = 1; k <= K; k++) {
					int nextRow = r + poison_dr[dir] * k;
					int nextCol = c + poison_dc[dir] * k;
					if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) break;
					if (TreeMap[nextRow][nextCol] < 1) break;
					cnt += TreeMap[nextRow][nextCol];
				}
			}
			if (maxCnt > cnt) continue;
			//아직 안찾았다면
			if (maxCnt == -1) {
				ResultIndex.row = r;
				ResultIndex.col = c;
				maxCnt = cnt;
			}
			else if (maxCnt < cnt) {
				ResultIndex.row = r;
				ResultIndex.col = c;
				maxCnt = cnt;
			}
			//행이 작은 순서대로, 만약 행이 같은 경우에는 열이 작은 칸에 제초제를 뿌림
			else if (maxCnt == cnt) {
				if (ResultIndex.row > r) {
					ResultIndex.row = r;
					ResultIndex.col = c;
					maxCnt = cnt;
				}
				else if (ResultIndex.row == r) {
					if (ResultIndex.col > c) {
						ResultIndex.row = r;
						ResultIndex.col = c;
						maxCnt = cnt;
					}
				}
			}
		}
	}

	if (maxCnt == -1) return;

	//cout << "제초제 위치 : (" << ResultIndex.row << "," << ResultIndex.col << ")\n박멸수 : " << maxCnt << "\n\n";

	Answer += maxCnt;
	TreeMap[ResultIndex.row][ResultIndex.col] = 0;
	PoisonMap[ResultIndex.row][ResultIndex.col] = C;

	for (int dir = 0; dir < 4; dir++) {
		for (int k = 1; k <= K; k++) {
			int nextRow = ResultIndex.row + poison_dr[dir] * k;
			int nextCol = ResultIndex.col + poison_dc[dir] * k;
			if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) break;
			if (TreeMap[nextRow][nextCol] < 1) break;
			TreeMap[nextRow][nextCol] = 0;
			PoisonMap[nextRow][nextCol] = C;
		}
	}

}

void solution() {

	for (int turn = 1; turn <= M; turn++) {

		// cout << "\n----------------------------------------------------------------";
		// cout << "\n" << turn << "번 째 턴\n\n";
		//나무의 성장
		growTree();
		reproduction();

		for (int r = 1; r <= N; r++) {
			for (int c = 1; c <= N; c++) {
				if (PoisonMap[r][c] == 0) continue;
				PoisonMap[r][c]--;
			}
		}

		scatterPoison();

		int treeLeft = 0;
		for (int r = 1; r <= N; r++) {
			for (int c = 1; c <= N; c++) {
				if (TreeMap[r][c] < 1) continue;
				treeLeft++;
			}
		}

		if (treeLeft == 0) {
			break;
		}
	}
	cout << Answer;
}

int main() {
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	init();
	solution();

	return 0;
}

/*
문제 분석

맵 정보 : NxN
제초제
	1) K의 범위만큼 대각선으로 퍼짐
	2) 벽이 있는 경우 막힘

게임 순서

1) 나무의 성장
	1) 인접한 4개의 칸 중 나무가 있는 칸의 수만큼 성장
	2) *****모든 나무가 동시에 일어남

2) 번식

	1) 기존에 있는 나무들은 인접한 4개의 칸 성장
		1) 벽이면 무시
		2) 다른 나무면 무시
		3) 제초제면 무시
		4) 가능한 칸으로 나눈 값으로 번식

3) 제초제 뿌림
	1) 각 칸 중 제초제를 뿌렸을 때 가장 많이 박멸되는 칸에 뿌림
	2) 제초제 전파 조건
		2-1) 나무가 없는 곳 : 아무 일도 안 일어남
		2-2) 나무가 있는 곳
			1) K칸 만큼 대각선으로 전파
			2) 전파되는 도중 벽이거나 나무 없으면 멈춤
			3) C년만큼 제초제가 남아있음 (C+1)년에 없어짐
			4) 이미 있는데 뿌려지면 다시 C년동안 유지



*/

/*

제초제 뿌릴 때 대각선 K칸 탐색해서 나무 수 최대인 곳 뿌리기

*/