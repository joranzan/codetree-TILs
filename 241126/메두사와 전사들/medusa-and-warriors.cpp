#include<iostream>
#include<vector>
#include<queue>

using namespace std;

/*
범위
N : 4이상 50이하
M : 0이상 300이하
*/
struct pos {
	int row;
	int col;
};

struct Warrior {
	int id;   //전사 ID 번호
	pos wpos; //전사 현재 위치
	bool stone = false;  //돌이 되었는지 여부
	bool dead = false;   //사라졌는지 여부
};


int N, M;   //N(맵 크기), M(전사 수)
pos Snake;  //메두사 집 위치
pos Park;   //메두사 공원 위치
vector<Warrior> WarriorInfo;     //전사들의 정보
int RoadMap[51][51] = { 0, };    //도로 정보 (도로(0), 비도로(1))
vector<int> WarriorMap[51][51];  //전사 위치 정보 ID(1~M)로 관리(전사 같이 있을 수도 있음)->메두사 시야각 따질 때 DAT
int SightMap[51][51] = { 0, };   //시야각 배열 (1 : 보임) (-1 : 안보임)
int ans_dist = 0;
int ans_stone = 0;
int ans_attack = 0;
bool arrived = false;

void input() {
	//N , M : 마을 크기, 전사 수
	//(sr, sc), (er, ec) : 집의 위치, 공원의 위치
	//(r1,c1) ~ : 전사의 위치
	//마을 정보
	cin >> N >> M;
	cin >> Snake.row >> Snake.col;
	cin >> Park.row >> Park.col;

	Warrior Dummy = { 0,{0,0} };
	WarriorInfo.push_back(Dummy);

	for(int i = 1; i <= M; i++) {
		Warrior temp;
		temp.id = i;
		cin >> temp.wpos.row >> temp.wpos.col;
		WarriorInfo.push_back(temp);
		WarriorMap[temp.wpos.row][temp.wpos.col].push_back(temp.id);
	}

	for (int r = 0; r < N; r++) {
		for (int c = 0; c < N; c++) {
			cin >> RoadMap[r][c];
		}
	}

	

}

bool check_valid() {

	queue<pos> q;
	q.push(Snake);
	int Visited[51][51] = { 0, };
	Visited[Snake.row][Snake.col] = 1;

	int dr[4] = { -1,1,0,0 };
	int dc[4] = { 0,0,-1,1 };

	while (!q.empty()) {
		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();
		if (nowRow == Park.row && nowCol == Park.col) {
			return true;
		}
		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			if (nextRow < 0 || nextCol < 0 || nextRow >= N || nextCol >= N) continue;
			if (Visited[nextRow][nextCol] == 1) continue;
			if (RoadMap[nextRow][nextCol] == 1) continue;
			Visited[nextRow][nextCol] = 1;
			q.push({ nextRow, nextCol });
		}
	}
	return false;
}

void snake_move() {
	queue<pos> q;    //{현재 위치, 이전 위치}
	q.push(Snake);
	int Visited[51][51] = { 0, };
	Visited[Snake.row][Snake.col] = 1;

	pair<pos, int> temp[51][51];

	int dr[4] = { -1,1,0,0 };
	int dc[4] = { 0,0,-1,1 };

	int SelectedDir = -1;

	while (!q.empty()) {
		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();

		if (nowRow == Park.row && nowCol == Park.col) {
			int nowDir = -1;
			while (nowRow != Snake.row || nowCol != Snake.col) {
				nowDir = temp[nowRow][nowCol].second;
				int tempRow = nowRow;
				int tempCol = nowCol;
				nowRow = temp[tempRow][tempCol].first.row;
				nowCol = temp[tempRow][tempCol].first.col;
			}
			SelectedDir = nowDir;
			//SelectedDir = temp[Snake.row][Snake.col].second;
			break;
		}

		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			if (nextRow < 0 || nextCol < 0 || nextRow >= N || nextCol >= N) continue;
			if (Visited[nextRow][nextCol] == 1) continue;
			if (RoadMap[nextRow][nextCol] == 1) continue;
			Visited[nextRow][nextCol] = 1;
			temp[nextRow][nextCol].first = { nowRow, nowCol };
			temp[nextRow][nextCol].second = dir;
			q.push({ nextRow, nextCol });
		}
	}

	//메두사 이동
	Snake.row += dr[SelectedDir];
	Snake.col += dc[SelectedDir];

	//1-1) 이동한 칸에 전사 있을 경우 : 전사는 사라짐
	for (int i = 0; i < WarriorMap[Snake.row][Snake.col].size(); i++) {
		int nowID = WarriorMap[Snake.row][Snake.col][i];
		WarriorInfo[nowID].dead = true;
	}

	if (Snake.row == Park.row && Snake.col == Park.col) arrived = true;

}



void snake_sight() {
	//2. 메두사의 시선
	//	1) 상하좌우 중 하나만 바라봄(전사를 많이 볼 수 있는 방향 바라봄, 동률일 때 상하좌우 순)
	//	2) 90도 시야각(처리 어떻게 할지 고민->대각선) * *********************************
	//	2 - 1) 전사가 시야각에 있을 경우 돌이 됨
	//	- 다른 전사에 가려진 경우 메두사에게 보이지 않음(상대적인 위치) ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? ? 그럼 대각선과 상하좌우 사이 방향은 ?
	//	-메두사가 바라보는 방향에 따라서 Mapping 필요 or 시야각 Map 만들어서 가까운 칸들부터 순서대로 탐색(cnt : 그 줄이 모두 막혀있으면 그만)
	//	- 하(아래)인 경우 Col기준으로 왼쪽 오른쪽

	//	2 - 2) 돌로 변한 전사는 현재 턴은 움직일 수 없음
	//	2 - 3) 두 명 이상의 전사들이 같은 칸인 경우 둘다 돌!

	vector<int> result[4];  //방향별 바라볼 수 있는 전사의 ID
	int diag_dr[4] = { -1,-1,1,1 };
	int diag_dc[4] = { -1,1,-1,1 };
	int tempMap[51][51] = { 0, };
	int maxDist = max(max(max(N - 1 - Snake.row, Snake.row), N - 1 - Snake.col), Snake.col);


	//상
	for (int r = 1; r <= Snake.row; r++) {
		int nowRow = Snake.row - r;
		int lineCnt = 0;
		int invalidCnt = 0;
		for (int nowCol = Snake.col - r; nowCol <= Snake.col + r; nowCol++) {
			if (nowCol < 0 || nowCol >= N) continue;
			lineCnt++;
			if (tempMap[nowRow][nowCol] == -1) {
				invalidCnt++;
				continue;
			}
			
			tempMap[nowRow][nowCol] = 1;
			bool warrior_exist = false;
			for (int i = 0; i < WarriorMap[nowRow][nowCol].size(); i++) {
				if (!WarriorInfo[WarriorMap[nowRow][nowCol][i]].dead) {
					result[0].push_back(WarriorMap[nowRow][nowCol][i]);
					warrior_exist = true;
				}
			}

			if (warrior_exist) {
				for (int t_r = 1; t_r <= nowRow; t_r++) {
					int tempRow = nowRow - t_r;
					if (nowCol == Snake.col) tempMap[tempRow][nowCol] = -1;
					else if (nowCol < Snake.col) {
						for (int tempCol = nowCol - t_r; tempCol <= nowCol; tempCol++) {
							if (tempCol < 0 || tempCol >= N) continue;
							tempMap[tempRow][tempCol] = -1;
						}
					}
					else {
						for (int tempCol = nowCol; tempCol <= nowCol + t_r; tempCol++) {
							if (tempCol < 0 || tempCol >= N) continue;
							tempMap[tempRow][tempCol] = -1;
						}
					}
				}
			}
			
		}

		if (invalidCnt == lineCnt) break;
	}
	//대각선 4방향 초기화
	for (int i = 1; i <= maxDist; i++) {
		for (int dir = 0; dir < 4; dir++) {
			int nowRow = Snake.row + diag_dr[dir] * i;
			int nowCol = Snake.col + diag_dc[dir] * i;
			if (nowRow < 0 || nowCol < 0 || nowRow >= N || nowCol >= N) continue;
			tempMap[nowRow][nowCol] = 0;
		}
	}

	//하
	for (int r = 1; r <= N - 1 - Snake.row; r++) {
		int nowRow = Snake.row + r;
		int lineCnt = 0;
		int invalidCnt = 0;
		for (int nowCol = Snake.col - r; nowCol <= Snake.col + r; nowCol++) {
			if (nowCol < 0 || nowCol >= N) continue;
			lineCnt ++;
			if (tempMap[nowRow][nowCol] == -1) {
				invalidCnt++;
				continue;
			}

			tempMap[nowRow][nowCol] = 1;
			bool warrior_exist = false;

			for (int i = 0; i < WarriorMap[nowRow][nowCol].size(); i++) {
				if (!WarriorInfo[WarriorMap[nowRow][nowCol][i]].dead) {
					result[1].push_back(WarriorMap[nowRow][nowCol][i]);
					warrior_exist = true;
				}
				
			}
			if (warrior_exist) {
				for (int t_r = 1; t_r <= N - 1 - nowRow; t_r++) {
					int tempRow = nowRow + t_r;
					if (nowCol == Snake.col) tempMap[tempRow][nowCol] = -1;
					else if (nowCol < Snake.col) {
						for (int tempCol = nowCol - t_r; tempCol <= nowCol; tempCol++) {
							if (tempCol < 0 || tempCol >= N) continue;
							tempMap[tempRow][tempCol] = -1;
						}
					}
					else {
						for (int tempCol = nowCol; tempCol <= nowCol + t_r; tempCol++) {
							if (tempCol < 0 || tempCol >= N) continue;
							tempMap[tempRow][tempCol] = -1;
						}
					}
					
				}
			}
			
		}

		if (invalidCnt == lineCnt) break;
	}
	//대각선 4방향 초기화
	for (int i = 1; i <= maxDist; i++) {
		for (int dir = 0; dir < 4; dir++) {
			int nowRow = Snake.row + diag_dr[dir] * i;
			int nowCol = Snake.col + diag_dc[dir] * i;
			if (nowRow < 0 || nowCol < 0 || nowRow >= N || nowCol >= N) continue;
			tempMap[nowRow][nowCol] = 0;
		}
	}

	//좌
	for (int c = 1; c <= Snake.col; c++) {
		int nowCol = Snake.col - c;
		int lineCnt = 0;
		int invalidCnt = 0;
		for (int nowRow = Snake.row - c; nowRow <= Snake.row + c; nowRow++) {
			if (nowRow < 0 || nowRow >= N) continue;
			lineCnt++;
			if (tempMap[nowRow][nowCol] == -1) {
				invalidCnt++;
				continue;
			}

			tempMap[nowRow][nowCol] = 1;
			bool warrior_exist = false; 

			for (int i = 0; i < WarriorMap[nowRow][nowCol].size(); i++) {
				if (!WarriorInfo[WarriorMap[nowRow][nowCol][i]].dead) {
					result[2].push_back(WarriorMap[nowRow][nowCol][i]);
					warrior_exist = true;
				}
				
			}
			if (warrior_exist) {
				for (int t_c = 1; t_c <= nowCol; t_c++) {
					int tempCol = nowCol - t_c;
					if (nowRow == Snake.row) tempMap[nowRow][tempCol] = -1;
					else if (nowRow < Snake.row) {
						for (int tempRow = nowRow - t_c; tempRow <= nowRow; tempRow++) {
							if (tempRow < 0 || tempRow >= N) continue;
							tempMap[tempRow][tempCol] = -1;
						}
					}
					else {
						for (int tempRow = nowRow; tempRow <= nowRow + t_c; tempRow++) {
							if (tempRow < 0 || tempRow >= N) continue;
							tempMap[tempRow][tempCol] = -1;
						}
					}
					
				}
			}
		}

		if (invalidCnt == lineCnt) break;
	}
	
	//대각선 4방향 초기화
	for (int i = 1; i <= maxDist; i++) {
		for (int dir = 0; dir < 4; dir++) {
			int nowRow = Snake.row + diag_dr[dir] * i;
			int nowCol = Snake.col + diag_dc[dir] * i;
			if (nowRow < 0 || nowCol < 0 || nowRow >= N || nowCol >= N) continue;
			tempMap[nowRow][nowCol] = 0;
		}
	}

	//우
	for (int c = 1; c <= N - 1 - Snake.col; c++) {
		int nowCol = Snake.col + c;
		int lineCnt = 0;
		int invalidCnt = 0;
		for (int nowRow = Snake.row - c; nowRow <= Snake.row + c; nowRow++) {
			if (nowRow < 0 || nowRow >= N) continue;
			lineCnt++;
			if (tempMap[nowRow][nowCol] == -1) {
				invalidCnt++;
				continue;
			}

			tempMap[nowRow][nowCol] = 1;
			bool warrior_exist = false;

			for (int i = 0; i < WarriorMap[nowRow][nowCol].size(); i++) {
				if (!WarriorInfo[WarriorMap[nowRow][nowCol][i]].dead) {
					result[3].push_back(WarriorMap[nowRow][nowCol][i]);
					warrior_exist = true;
				}
				
			}
			if (warrior_exist) {
				for (int t_c = 1; t_c <= N - 1 - nowCol; t_c++) {
					int tempCol = nowCol + t_c;
					if (nowRow == Snake.row) tempMap[nowRow][tempCol] = -1;
					else if (nowRow < Snake.row) {
						for (int tempRow = nowRow - t_c; tempRow <= nowRow; tempRow++) {
							if (tempRow < 0 || tempRow >= N) continue;
							tempMap[tempRow][tempCol] = -1;
						}
					}
					else {
						for (int tempRow = nowRow; tempRow <= nowRow + t_c; tempRow++) {
							if (tempRow < 0 || tempRow >= N) continue;
							tempMap[tempRow][tempCol] = -1;
						}
					}
				}
			}
		}

		if (invalidCnt == lineCnt) break;
	}
	//대각선 4방향 초기화
	for (int i = 1; i <= maxDist; i++) {
		for (int dir = 0; dir < 4; dir++) {
			int nowRow = Snake.row + diag_dr[dir] * i;
			int nowCol = Snake.col + diag_dc[dir] * i;
			if (nowRow < 0 || nowCol < 0 || nowRow >= N || nowCol >= N) continue;
			tempMap[nowRow][nowCol] = 0;
		}
	}


	//방향 정하기
	int maxDir = 0;
	int maxWarrior = 0;
	for (int i = 0; i < 4; i++) {
		int debugging = -1;
		//cout << "result[" << i << "].size(): " << result[i].size() << ", maxWarrior: " << maxWarrior << endl;
		int result_size = result[i].size();
		if (maxWarrior < result_size) {
			maxWarrior = result_size;
			maxDir = i;
		}
	}

	int debugging = -1;

	//최종 시야각 처리
	switch (maxDir) {
	case 0:
		for (int r = 1; r <= Snake.row; r++) {
			int nowRow = Snake.row - r;
			for (int nowCol = Snake.col - r; nowCol <= Snake.col + r; nowCol++) {
				if (nowCol < 0 || nowCol >= N) continue;
				if (SightMap[nowRow][nowCol] == -1) {
					continue;
				}

				SightMap[nowRow][nowCol] = 1;

				if (WarriorMap[nowRow][nowCol].size() > 0) {
					for (int t_r = 1; t_r <= nowRow; t_r++) {
						int tempRow = nowRow - t_r;
						if (nowCol == Snake.col) SightMap[tempRow][nowCol] = -1;
						else if (nowCol < Snake.col) {
							for (int tempCol = nowCol - t_r; tempCol <= nowCol; tempCol++) {
								if (tempCol < 0 || tempCol >= N) continue;
								SightMap[tempRow][tempCol] = -1;
							}
						}
						else {
							for (int tempCol = nowCol; tempCol <= nowCol + t_r; tempCol++) {
								if (tempCol < 0 || tempCol >= N) continue;
								SightMap[tempRow][tempCol] = -1;
							}
						}
						
					}
				}

			}
		}
		break;
	case 1:
		for (int r = 1; r <= N - 1 - Snake.row; r++) {
			int nowRow = Snake.row + r;
			for (int nowCol = Snake.col - r; nowCol <= Snake.col + r; nowCol++) {
				if (nowCol < 0 || nowCol >= N) continue;
				if (SightMap[nowRow][nowCol] == -1) {
					continue;
				}

				SightMap[nowRow][nowCol] = 1;


				if (WarriorMap[nowRow][nowCol].size() > 0) {
					for (int t_r = 1; t_r <= N - 1 - nowRow; t_r++) {
						int tempRow = nowRow + t_r;
						if (nowCol == Snake.col) SightMap[tempRow][nowCol] = -1;
						else if (nowCol < Snake.col) {
							for (int tempCol = nowCol - t_r; tempCol <= nowCol; tempCol++) {
								if (tempCol < 0 || tempCol >= N) continue;
								SightMap[tempRow][tempCol] = -1;
							}
						}
						else {
							for (int tempCol = nowCol; tempCol <= nowCol + t_r; tempCol++) {
								if (tempCol < 0 || tempCol >= N) continue;
								SightMap[tempRow][tempCol] = -1;
							}
						}
						
					}
				}

			}
		}
		break;
	case 2:
		for (int c = 1; c <= Snake.col; c++) {
			int nowCol = Snake.col - c;
			for (int nowRow = Snake.row - c; nowRow <= Snake.row + c; nowRow++) {
				if (nowRow < 0 || nowRow >= N) continue;
				if (SightMap[nowRow][nowCol] == -1) {
					continue;
				}

				SightMap[nowRow][nowCol] = 1;

				if (WarriorMap[nowRow][nowCol].size() > 0) {
					for (int t_c = 1; t_c <= nowCol; t_c++) {
						int tempCol = nowCol - t_c;
						if (nowRow == Snake.row) SightMap[nowRow][tempCol] = -1;
						else if (nowRow < Snake.row) {
							for (int tempRow = nowRow - t_c; tempRow <= nowRow; tempRow++) {
								if (tempRow < 0 || tempRow >= N) continue;
								SightMap[tempRow][tempCol] = -1;
							}
						}
						else {
							for (int tempRow = nowRow; tempRow <= nowRow + t_c; tempRow++) {
								if (tempRow < 0 || tempRow >= N) continue;
								SightMap[tempRow][tempCol] = -1;
							}
						}
						
					}
				}
			}
		}
		break;
	case 3 :
		for (int c = 1; c <= N - 1 - Snake.col; c++) {
			int nowCol = Snake.col + c;
			for (int nowRow = Snake.row - c; nowRow <= Snake.row + c; nowRow++) {
				if (nowRow < 0 || nowRow >= N) continue;
				if (SightMap[nowRow][nowCol] == -1) {
					continue;
				}

				SightMap[nowRow][nowCol] = 1;

				if (WarriorMap[nowRow][nowCol].size() > 0) {
					for (int t_c = 1; t_c <= N - 1 - nowCol; t_c++) {
						int tempCol = nowCol + t_c;
						if (nowRow == Snake.row) SightMap[nowRow][tempCol] = -1;
						else if (nowRow < Snake.row) {
							for (int tempRow = nowRow - t_c; tempRow <= nowRow; tempRow++) {
								if (tempRow < 0 || tempRow >= N) continue;
								SightMap[tempRow][tempCol] = -1;
							}
						}
						else {
							for (int tempRow = nowRow ; tempRow <= nowRow + t_c; tempRow++) {
								if (tempRow < 0 || tempRow >= N) continue;
								SightMap[tempRow][tempCol] = -1;
							}
						}
					}
				}
			}
		}
		break;
	}


	//시야각 내의 전사들 돌로 변함
	for (int i = 0; i < result[maxDir].size(); i++) {
		int id = result[maxDir][i];
		ans_stone++;
		WarriorInfo[id].stone = true;
	}
}

void warriors_move() {


	for (int i = 1; i <= M; i++) {
		if (WarriorInfo[i].dead) continue;
		if (WarriorInfo[i].stone) continue;

		//2) 메두사를 향해 최대 2칸(한칸씩) 이동
		//	2-1) 첫 번째 이동
		//	- 메두사와 거리를 줄일 수 있는 방향으로 한 칸 이동(상하좌우 순)
		//	- 격자 밖, 메두사의 시야에 들어오는 곳으로 이동 금지-------- > 시야각 배열도 전역으로 관리해야함
		//	2-2) 두 번째 이동
		//	- 메두사와 거리를 줄일 수 있는 방향으로 한 칸 이동(좌우상하 순)
		//	- 격자 밖, 메두사의 시야에 들어오는 곳으로 이동 금지------->시야각 배열도 전역으로 관리해야함

		int dr1[4] = { -1,1,0,0 };
		int dc1[4] = { 0,0,-1,1 };

		int nowRow = WarriorInfo[i].wpos.row;
		int nowCol = WarriorInfo[i].wpos.col;

		int minDist = abs(Snake.row - nowRow) + abs(Snake.col - nowCol);
		int minDir = -1;

		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr1[dir];
			int nextCol = nowCol + dc1[dir];
			if (nextRow < 0 || nextCol < 0 || nextRow >= N || nextCol >= N) continue;
			if (SightMap[nextRow][nextCol] == 1) continue;
			int dist = abs(Snake.row - nextRow) + abs(Snake.col - nextCol);
			if (dist < minDist) {
				minDist = dist;
				minDir = dir;
			}
		}

		if (minDir == -1) continue;

		ans_dist++;
		WarriorInfo[i].wpos.row += dr1[minDir];
		WarriorInfo[i].wpos.col += dc1[minDir];


		//두번째 이동

		//minDist = 5000;
		minDir = -1;

		nowRow = WarriorInfo[i].wpos.row;
		nowCol = WarriorInfo[i].wpos.col;
		if (nowRow == Snake.row && nowCol == Snake.col) {
			WarriorInfo[i].dead = true;
			ans_attack++;
			continue;
		}

		int dr2[4] = { 0,0,-1,1 };
		int dc2[4] = { -1,1,0,0 };

		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr2[dir];
			int nextCol = nowCol + dc2[dir];
			if (nextRow < 0 || nextCol < 0 || nextRow >= N || nextCol >= N) continue;
			if (SightMap[nextRow][nextCol] == 1) continue;
			int dist = abs(Snake.row - nextRow) + abs(Snake.col - nextCol);
			if (dist < minDist) {
				minDist = dist;
				minDir = dir;
			}
		}
		if (minDir == -1) continue;
		ans_dist++;
		WarriorInfo[i].wpos.row += dr2[minDir];
		WarriorInfo[i].wpos.col += dc2[minDir];

		nowRow = WarriorInfo[i].wpos.row;
		nowCol = WarriorInfo[i].wpos.col;
		if (nowRow == Snake.row && nowCol == Snake.col) {
			WarriorInfo[i].dead = true;
			ans_attack++;
		}
	}

	
}

void setting_back() {



	for (int r = 0; r < N; r++) {
		for (int c = 0; c < N; c++) {
			SightMap[r][c] = 0;
			WarriorMap[r][c].clear();
		}
	}

	for (int i = 1; i <= M; i++) {
		if (WarriorInfo[i].dead) continue;
		if (WarriorInfo[i].stone) WarriorInfo[i].stone = false;
		WarriorMap[WarriorInfo[i].wpos.row][WarriorInfo[i].wpos.col].push_back(i);
	}
}

void solution() {
	input();
	//메두사가 공원에 도달할 수 있는지 검사
	bool valid = check_valid();
	if (!valid) {
		cout << "-1";
		return;
	}

	while (Snake.row != Park.row || Snake.col != Park.col) {
		ans_dist = 0;
		ans_stone = 0;
		ans_attack = 0;
		//메두사 이동
		snake_move();
		//메두사 시선 처리
		if (arrived) break;
		snake_sight();
		//전사 이동 + 전사 위치정보 초기화 & 갱신 + 전사 공격
		warriors_move();
		//정답 출력 + 초기화(시야각 배열, 전사들 돌 여부)
		setting_back();
		cout << ans_dist << " " << ans_stone << " " << ans_attack << "\n";
	}
	
	cout << "0";
}

int main() {
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	solution();

	return 0;
}





/*
문제 분석 08:37

정보)
1. NxN 맵 (0 ~ N-1)
    - 도로:0
    - 도로 아닌 곳: 1

전사 : M 명
1) 초기 위치 : (ri, ci)
2) 메두사를 향한 최단 경로
3) 도로/비도로 모두 이동 가능


메두사 좌표 : (Sr, Sc)
공원 좌표 : (Er, Ec)
1) 메두사는 도로만 이동 가능
2) 집, 공원 모두 항상 도로 위
3) 집, 공원 모두 좌표 다름
4) 초기부터 집에 전사들이 위치하는 경우 없음
5) 전사들이 움직이기 전에 돌로 만들어 멈출 수 있음



설명)


1. 메두사의 이동
1) 도로를 따라 공원까지 최단경로로 한칸 이동
	1-1) 이동한 칸에 전사 있을 경우 : 전사는 사라짐
	1-2) 최단경로 우선순위 : 상하좌우
	1-3) 집->공원까지 경로가 없을 수도 있음 ******************************

2. 메두사의 시선
1) 상하좌우 중 하나만 바라봄 (전사를 많이 볼 수 있는 방향 바라봄, 동률일 때 상하좌우 순)
2) 90도 시야각 (처리 어떻게 할지 고민 -> 대각선) **********************************
	2-1) 전사가 시야각에 있을 경우 돌이 됨
		- 다른 전사에 가려진 경우 메두사에게 보이지 않음 (상대적인 위치) ??????????????????? 그럼 대각선과 상하좌우 사이 방향은?
		- 메두사가 바라보는 방향에 따라서 Mapping 필요 or 시야각 Map 만들어서 가까운 칸들부터 순서대로 탐색 (cnt :그 줄이 모두 막혀있으면 그만)
			- 하(아래)인 경우 Col기준으로 왼쪽 오른쪽

	2-2) 돌로 변한 전사는 현재 턴은 움직일 수 없음
	2-3) 두 명 이상의 전사들이 같은 칸인 경우 둘다 돌!

		  
3. 전사들의 이동
1) 돌로 변하지 않은 전사들 이동
2) 메두사를 향해 최대 2칸(한칸씩) 이동
	2-1) 첫 번째 이동 
		- 메두사와 거리를 줄일 수 있는 방향으로 한 칸 이동 (상하좌우 순)
		- 격자 밖, 메두사의 시야에 들어오는 곳으로 이동 금지 --------> 시야각 배열도 전역으로 관리해야함
	2-2) 두 번째 이동
		- 메두사와 거리를 줄일 수 있는 방향으로 한 칸 이동 (좌우상하 순)
		- 격자 밖, 메두사의 시야에 들어오는 곳으로 이동 금지  -------> 시야각 배열도 전역으로 관리해야함


4. 전사의 공격
1) 메두사와 같은 칸에 도달한 전사는 메두사를 공격
2) 공격한 전사는 사라짐
3) 각 턴마다
	- 모든 전사가 이동한 거리의 합
	- 돌이 된 전사의 수
	- 메두사를 공격한 전사의 수
	- 메두사가 공원에 도착한다면 0 출력 후 프로그램 종료


*/


/*
자료구조

 Map1 : 도로 정보
 Map2 : 전사 위치 정보 (전사 같이 있을 수도 있음) -> 메두사 시야각 따질 때 DAT
 Map3 : 시야각 배열 (1 : 보임) (-1 : 안보임)

 N(맵 크기), M(전사)
메두사 집 위치
메두사 공원 위치

전사들의 정보를 담는 자료구조
{
1) 현재 위치 r,c
2) 돌 여부
3) 사라짐 여부
}
*/