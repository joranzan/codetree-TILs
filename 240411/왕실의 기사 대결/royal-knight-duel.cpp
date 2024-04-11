//시작 시간 : 09:00
//종료 시간 : 
#include<iostream>
#include<vector>
#include<set>

using namespace std;

/*
입력)
L : 맵 크기(3~40)
N : 기사 수(1~30)
Q : 턴 수(1~100)

맵 정보 :  0(빈칸) 1(함정) 2(벽)

r c h w k
k: (1~100)


i d : i번 기사 d 방향 (상 우 하 좌)
*/

const int dr[4] = { -1,0,1,0 };
const int dc[4] = { 0,1,0, -1 };


struct pos {
	int row;
	int col;
};

struct knight {
	int id;
	pos nowPos;
	int height;
	int width;
	int hp;
	bool alive = true;
};

int L, N, Q;
knight KnightInfo[31];  //기사 정보 관리
int KnightMap[41][41] = { 0, }; //ID로 관리
int InputMap[41][41] = { 0, }; //함정&벽관리
int OriginHP[31] = { 0, };
bool Moved[31] = { false, };
vector<int> NeedtoMoveKnights;


void init() {
	cin >> L >> N >> Q;

	for (int r = 1; r <= L; r++) {
		for (int c = 1; c <= L; c++) {
			KnightMap[r][c] = 0;
			cin >> InputMap[r][c]; //0(빈칸) 1(함정) 2(벽)
		}
	}

	for (int i = 1; i <= N; i++) {
		KnightInfo[i].id = i;
		cin >> KnightInfo[i].nowPos.row >> KnightInfo[i].nowPos.col >> KnightInfo[i].height >> KnightInfo[i].width >> KnightInfo[i].hp;
		OriginHP[i] = KnightInfo[i].hp;
		for (int r = KnightInfo[i].nowPos.row; r < KnightInfo[i].nowPos.row + KnightInfo[i].height; r++) {
			for (int c = KnightInfo[i].nowPos.col; c < KnightInfo[i].nowPos.col + KnightInfo[i].width; c++) {
				KnightMap[r][c] = i;
			}
		}
	}
}

bool isValid(int id, int dir) {

	int nowKnightRow = KnightInfo[id].nowPos.row;
	int nowKnightCol = KnightInfo[id].nowPos.col;

	set<int> nextKnightId;

	bool flag = true;

	//옆면 체크
	switch (dir) {
	case 0: {     //상

		for (int c = nowKnightCol; c < nowKnightCol + KnightInfo[id].width; c++) {
			int nextRow = nowKnightRow + dr[dir];
			int nextCol = c;

			//판별하기
			if (nextRow <= 0 || nextCol <= 0 || nextRow > L || nextCol > L) {
				flag = false;
				break;
			}
			if (InputMap[nextRow][nextCol] == 2) {
				flag = false;
				//cout << "\n벽!!!  False\n";
				break;
			}
			//기사가 존재한다면
			if (KnightMap[nextRow][nextCol] != 0) {
				if (!KnightInfo[KnightMap[nextRow][nextCol]].alive) continue;
				nextKnightId.insert(KnightMap[nextRow][nextCol]);
			}
		}


		break;
	}
	case 1: {     //우

		for (int r = nowKnightRow; r < nowKnightRow + KnightInfo[id].height; r++) {
			int nextRow = r;
			int nextCol = nowKnightCol + KnightInfo[id].width - 1 + dc[dir];

			//판별하기
			if (nextRow <= 0 || nextCol <= 0 || nextRow > L || nextCol > L) {
				flag = false;
				break;
			}
			if (InputMap[nextRow][nextCol] == 2) {
				flag = false;
				//cout << "\n벽!!!  False\n";
				break;
			}
			//기사가 존재한다면
			if (KnightMap[nextRow][nextCol] != 0) {
				if (!KnightInfo[KnightMap[nextRow][nextCol]].alive) continue;
				nextKnightId.insert(KnightMap[nextRow][nextCol]);
			}
		}

		break;
	}
	case 2: {     //하

		for (int c = nowKnightCol; c < nowKnightCol + KnightInfo[id].width; c++) {

			int nextRow = nowKnightRow + KnightInfo[id].height - 1 + dr[dir];
			int nextCol = c;

			//판별하기
			if (nextRow <= 0 || nextCol <= 0 || nextRow > L || nextCol > L) {
				flag = false;
				break;
			}
			if (InputMap[nextRow][nextCol] == 2) {
				flag = false;
				//cout << "\n벽!!!  False\n";
				break;
			}

			//기사가 존재한다면
			if (KnightMap[nextRow][nextCol] != 0) {
				if (!KnightInfo[KnightMap[nextRow][nextCol]].alive) continue;
				nextKnightId.insert(KnightMap[nextRow][nextCol]);
			}
		}
		break;
	}
	case 3: {     //좌

		for (int r = nowKnightRow; r < nowKnightRow + KnightInfo[id].height; r++) {

			int nextRow = r;
			int nextCol = nowKnightCol + dc[dir];

			//판별하기
			if (nextRow <= 0 || nextCol <= 0 || nextRow > L || nextCol > L) {
				flag = false;
				break;
			}
			if (InputMap[nextRow][nextCol] == 2) {
				flag = false;
				//cout << "\n벽!!!  False\n";
				break;
			}

			//기사가 존재한다면
			if (KnightMap[nextRow][nextCol] != 0) {
				if (!KnightInfo[KnightMap[nextRow][nextCol]].alive) continue;
				nextKnightId.insert(KnightMap[nextRow][nextCol]);
			}
		}
		break;
	}
	}

	if (!flag) {
		//cout << "이동 불가능\n";

		return flag;
	}
	int debugging = -1;
	for (auto it = nextKnightId.begin(); it != nextKnightId.end(); it++) {
		//cout << *it << " ";
		NeedtoMoveKnights.push_back(*it);
		flag = isValid(*it, dir);
		
		if (!flag) break;
	}
	//cout << "\n";


	//만약 불가능하다면 false 반환
	if (!flag) {
		return flag;
	}

	//true 반환
	return flag;
}


void moveKnight(int dir) {

	for (int i = NeedtoMoveKnights.size() - 1; i >=0; i--) {
		int id = NeedtoMoveKnights[i];
		Moved[id] = true;
		//cout<<id<<" ";
		int nowKnightRow = KnightInfo[id].nowPos.row;
		int nowKnightCol = KnightInfo[id].nowPos.col;
		//이동
		switch (dir) {
		case 0: {     //상

			for (int r = nowKnightRow; r < nowKnightRow + KnightInfo[id].height; r++) {
				for (int c = nowKnightCol; c < nowKnightCol + KnightInfo[id].width; c++) {
					int nextRow = r + dr[dir];
					int nextCol = c + dc[dir];
					if (InputMap[nextRow][nextCol] == 2) {
						int debugging = -1;
					}
					KnightMap[nextRow][nextCol] = KnightMap[r][c];
					KnightMap[r][c] = 0;
				}
			}

			KnightInfo[id].nowPos = { nowKnightRow + dr[dir], nowKnightCol + dc[dir] };

			break;
		}
		case 1: {     //우

			for (int c = nowKnightCol + KnightInfo[id].width - 1; c >= nowKnightCol; c--) {
				for (int r = nowKnightRow + KnightInfo[id].height - 1; r >= nowKnightRow; r--) {
					int nextRow = r + dr[dir];
					int nextCol = c + dc[dir];
					if (InputMap[nextRow][nextCol] == 2) {
						int debugging = -1;
					}
					KnightMap[nextRow][nextCol] = KnightMap[r][c];
					KnightMap[r][c] = 0;
				}
			}

			KnightInfo[id].nowPos = { nowKnightRow + dr[dir], nowKnightCol + dc[dir] };

			break;
		}
		case 2: {     //하



			for (int r = nowKnightRow + KnightInfo[id].height - 1; r >= nowKnightRow; r--) {
				for (int c = nowKnightCol + KnightInfo[id].width - 1; c >= nowKnightCol; c--) {
					int nextRow = r + dr[dir];
					int nextCol = c + dc[dir];
					if (InputMap[nextRow][nextCol] == 2) {
						int debugging = -1;
					}
					KnightMap[nextRow][nextCol] = KnightMap[r][c];
					KnightMap[r][c] = 0;
				}
			}

			KnightInfo[id].nowPos = { nowKnightRow + dr[dir], nowKnightCol + dc[dir] };

			break;
		}
		case 3: {     //좌

			for (int c = nowKnightCol; c < nowKnightCol + KnightInfo[id].width; c++) {
				for (int r = nowKnightRow; r < nowKnightRow + KnightInfo[id].height; r++) {
					int nextRow = r + dr[dir];
					int nextCol = c + dc[dir];
					if (InputMap[nextRow][nextCol] == 2) {
						int debugging = -1;
					}
					KnightMap[nextRow][nextCol] = KnightMap[r][c];
					KnightMap[r][c] = 0;
				}
			}

			KnightInfo[id].nowPos = { nowKnightRow + dr[dir], nowKnightCol + dc[dir] };

			break;
		}
		}
	}
	
}

void solution() {

	for (int q = 1; q <= Q; q++) {  //100
		int id, dir;
		cin >> id >> dir;

		NeedtoMoveKnights.clear();
		
		for (int i = 1; i <= N; i++) {
			Moved[i] = false;
		}

		//cout << q << "번째 명령어: id(" << id << ") 방향(" << dir << ")\n";

		//만약 이미 사라졌다면 무시
		if (!KnightInfo[id].alive) continue;

		// cout << "\n***********변경전***********\n";
		// for (int r = 1; r <= L; r++) {
		// 	for (int c = 1; c <= L; c++) {
		// 		cout << KnightMap[r][c] << " ";
		// 	}
		// 	cout << "\n";
		// }

		NeedtoMoveKnights.push_back(id);

		bool flag = isValid(id, dir);

		if (!flag) {
			//cout << "변경사항 없음 XXXXXXXXXXXXXX\n\n\n";
			continue;
		};

		moveKnight(dir);

		// for (int i = 0; i < NeedtoMoveKnights.size(); i++) {
		// 	cout << NeedtoMoveKnights[i] << " ";
		// }

		// cout << "\n***********변경후***********\n";
		// for (int r = 1; r <= L; r++) {
		// 	for (int c = 1; c <= L; c++) {
		// 		cout << KnightMap[r][c] << " ";
		// 	}
		// 	cout << "\n";
		// }

		// cout << "\n\n\n";


		vector<int> nowDeadKnight;

		//체력 깎기 
		for (int i = 1; i <= N; i++) {  //30
			if (!KnightInfo[i].alive) continue;
			if (id == i) continue;
			if (!Moved[i]) continue;
			int trapCnt = 0;

			int Row = KnightInfo[i].nowPos.row;
			int Col = KnightInfo[i].nowPos.col;

			for (int r = Row; r < Row + KnightInfo[i].height; r++) {
				for (int c = Col; c < Col + KnightInfo[i].width; c++) {
					if (InputMap[r][c] == 1) trapCnt++;
				}
			}


			KnightInfo[i].hp -= trapCnt;

			//cout << "$$$$$$$$데미지$$$$$$$$ " << trapCnt << "\n\n\n";


			if (KnightInfo[i].hp <= 0) {
				//cout << i << "번 기사 죽었음----------\n";
				KnightInfo[i].hp = 0;
				KnightInfo[i].alive = false;
				nowDeadKnight.push_back(i);
			}
		}
		//KnightMap 갱신
		for (int i = 0; i < nowDeadKnight.size(); i++) {
			int nowID = nowDeadKnight[i];
			int Row = KnightInfo[i].nowPos.row;
			int Col = KnightInfo[i].nowPos.col;
			for (int r = Row; r < Row + KnightInfo[i].height; r++) {
				for (int c = Col; c < Col + KnightInfo[i].width; c++) {
					KnightMap[r][c] = 0;
				}
			}
		}
		int debugging = -1;
	}

	int Answer = 0;

	for (int i = 1; i <= N; i++) {
		if (!KnightInfo[i].alive) continue;
		Answer += (OriginHP[i] - KnightInfo[i].hp);
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

Map : LxL (1,1)

- 빈칸
- 함정
- 벽

왕실의 기사들은 마력으로 상대방 밀쳐낼 수 있음
기사 정보)

	(r,c) : 기사의 초기 위치
	(hxw) 방패
	 k : 체력

게임 순서

1) 기사 이동
- 왕의 명령 받은 기사 : 상하좌우 중 하나로 한 칸 이동
	1) 이동 칸에 다른 기사 있다면 연쇄적으로 한칸 밀려남
		- 기사들은 이동한 곳에 wxh 직사각형에 함정 수만큼 데미지 얻음
		- 체력 깎임
		- 체력 < 0 이면 사라짐
		- 명령을 받은 기사는 피해 안입음
		- *****기사들은 모두 밀린 후에 피해 입음*****

	2) 마지막 연쇄에서 벽이 있다면 모든 기사 이동할 수 없음
	3) 체스판에서 사라진 기사는 명령 무시

2) 대결 Demage


*/


/*
Test Case

4 3 3
0 0 1 0
0 0 1 0
1 1 0 1
0 0 2 0
1 2 2 1 5
2 1 2 1 1
3 2 1 2 3
1 2
2 1
3 3

1번 명령)
id = 1
dir = 2 (하)

이동할 수 있는지 판별

1번 기사가 dir방향으로 이동 시 칸 판별
-----> 3번 기사가 접해있음
	3번 기사가 dir 방향으로 이동시 칸 판별
	----> 벽이 있으므로 false return

밀 수 없음

2번 명령)
id = 2
dir = 1 (우)
1.  ***************************************

2번 기사가 dir방향으로 이동시 칸 판별
-----> 1번기사, 3번 기사 접해있음 (set에 담음)
	--> 1번 기사 dir방향 이동 시 칸 판별 : true
	--> 3번 기사 dir방향 이동 시 칸 판별 : true

2.***********************************************

move() --> 마지막부터 이동 시킴 (재귀 마지막 부분에 이동 시킴)

3. **********************************************

함정 판별 후 체력 깎기

4. ********************************************

죽었는지 판별 후 죽임



*/