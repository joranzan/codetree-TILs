//시작 시간 : 15:26
//종료 시간 : 
#include<iostream>
#include<queue>

using namespace std;

//상좌우하
const int dr[4] = { -1,0,0,1 };
const int dc[4] = { 0,-1,1,0 };

struct pos {
	int row;
	int col;
};

struct person {
	int id;
	pos nowPos;    //현재 위치
	pos storePos;  //목표 편의점 위치 (한 턴 마다 BFS로 찾아야함 -> 최단거리 바뀔수있으니까)
	bool arrived = false;    //도착여부
};

struct base {
	int id;
	pos nowPos;           //Base위치
	bool valid = true;    //이미 사용된 Base인지
};


int N, M;
int ValidMap[16][16] = { 0, }; //0이면 가능한 곳, 1이면 불가능한 곳

int BaseNum = 0;   //베이스 개수
base BaseInfo[225];
int BaseMap[16][16] = { 0, }; //BFS용

person PersonInfo[31];

int Time = 0;

void init() {

	cin >> N >> M;

	int baseId = 0;
	int Time = 0;
	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			//초기화
			ValidMap[r][c] = 0;
			BaseMap[r][c] = 0;
			int temp;
			cin >> temp;
			if (temp == 1) {
				baseId++;
				//베이스 정보 입력
				BaseInfo[baseId].nowPos.row = r;
				BaseInfo[baseId].nowPos.col = c;
				BaseInfo[baseId].id = baseId;
				BaseMap[r][c] = baseId;
			}

		}
	}

	BaseNum = baseId;


	for (int i = 1; i <= M; i++) {
		int r, c;
		cin >> r >> c;
		PersonInfo[i].id = i;
		PersonInfo[i].storePos.row = r;
		PersonInfo[i].storePos.col = c;
	}

}

void moveShortestPath(int id) {
	/*
	1) 격자에 있는 사람들 모두 본인이 가고 싶은 편의점 방향으로 1칸 이동
		- 최단 거리(이동하여 도달하기까지 거쳐야하는 칸의 수 최소)
		- 여러가지면 상, 좌, 우, 하 우선순위로 이동(이건 그냥 위치에서 BFS 구해야할듯)
		- 인접한 칸 중 가능한 칸으로만 가능
	*/


	pos Visited[16][16];
	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			Visited[r][c].row = -1;
			Visited[r][c].col = -1;
		}
	}
	int startRow = PersonInfo[id].nowPos.row;
	int startCol = PersonInfo[id].nowPos.col;
	queue<pos> q;
	Visited[startRow][startCol].row = startRow;
	Visited[startRow][startCol].col = startCol;
	q.push({ startRow, startCol });

	while (!q.empty()) {
		
		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();

		if (nowRow == PersonInfo[id].storePos.row && nowCol == PersonInfo[id].storePos.col) {
			int tempRow = nowRow;
			int tempCol = nowCol;
			while (tempRow != startRow || tempCol != startCol) {
				int tempnextRow = Visited[tempRow][tempCol].row;
				int tempnextCol = Visited[tempRow][tempCol].col;
				if (tempnextRow == startRow && tempnextCol == startCol) {
					PersonInfo[id].nowPos.row = tempRow;
					PersonInfo[id].nowPos.col = tempCol;
					break;
				}
				tempRow = tempnextRow;
				tempCol = tempnextCol;
			}
			break;
		}

		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;
			if (ValidMap[nextRow][nextCol] == -1) continue;
			if (Visited[nextRow][nextCol].row != -1 || Visited[nextRow][nextCol].col != -1) continue;
			Visited[nextRow][nextCol].row = nowRow;
			Visited[nextRow][nextCol].col = nowCol;
			q.push({ nextRow, nextCol });

		}

	}

	int debugging = -1;
}

void selectBase() {
	//	1) t번 사람은 목표 편의점과 가장 가까운 베이스 캠프 들어감(시간 : 0)
	//	2) 가까이 기준 : 최단 거리(편의점으로부터 BFS 구하기)
	//	3) 동률인 경우 : 행이 작은 베이스 캠프
	//	4) 동률인 경우 : 열이 작은 베이스 캠프
	//	5)******* 베이스캠프는 지나갈 수 없는 칸(평생!)

	int minDist = 300;
	pos minBase = { 20,20 };

	int id = Time;
	int storeRow = PersonInfo[id].storePos.row;
	int storeCol = PersonInfo[id].storePos.col;

	queue<pos> q;
	int Visited[16][16] = { 0, };
	q.push({ storeRow, storeCol });
	Visited[storeRow][storeCol] = 1;

	while (!q.empty()) {
		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();

		bool foundBase = false;
		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;
			if (ValidMap[nextRow][nextCol] == -1) continue;
			if (Visited[nextRow][nextCol] == 1) continue;
			if (Visited[nowRow][nowCol] + 1 > minDist) continue;
			Visited[nextRow][nextCol] = Visited[nowRow][nowCol] + 1;
			
			if(BaseMap[nextRow][nextCol]==0) q.push({ nextRow, nextCol });
			else {
				if (minBase.row > nextRow) {
					minDist = Visited[nextRow][nextCol];
					minBase.row = nextRow;
					minBase.col = nextCol;
				}
				else if (minBase.row == nextRow) {
					if (minBase.col > nextCol) {
						minBase.row = nextRow;
						minBase.col = nextCol;
						minDist = Visited[nextRow][nextCol];
					}
				}
			}
		}
	}




	BaseInfo[BaseMap[minBase.row][minBase.col]].valid = false;
	ValidMap[minBase.row][minBase.col] = -1;
	PersonInfo[id].nowPos.row = minBase.row;
	PersonInfo[id].nowPos.col = minBase.col;
}

void solution() {


	bool personLeft = true;
	int newPersonNum = 0;
	while (personLeft) {
		//시간 지남 
		Time++;

		//격자에 들어가있는 사람들 이동
		for (int i = 1; i <= newPersonNum; i++) {
			if (PersonInfo[i].arrived) continue;
			moveShortestPath(i);
		}

		
		//편의점에 도착했는지 여부 확인

		for (int i = 1; i <= newPersonNum; i++) {
			if (PersonInfo[i].arrived) continue;
			if (PersonInfo[i].nowPos.row == PersonInfo[i].storePos.row && PersonInfo[i].nowPos.col == PersonInfo[i].storePos.col) {
				ValidMap[PersonInfo[i].nowPos.row][PersonInfo[i].nowPos.col] = -1;
				PersonInfo[i].arrived = true;
			}
		}
		

		
		//현재시간 Time이 M이하라면 사람 넣음
		if (Time <= M) {
			//Time==Id 가진 사람 Base 찾기
			selectBase();
			newPersonNum++;
		}
		//현재시간 Time이 M을 넘었으면 다 도착했는지 여부 조사
		else {
			bool Flag = false;
			for (int i = 1; i <= M; i++) {
				if (PersonInfo[i].arrived) continue;
				Flag = true;
				break;
			}
			personLeft = Flag;
		}
		
	}
	cout << Time;
}

int main() {
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	init();
	solution();



	return 0;
}

/*
문제 분석 

빵 구하는 사람 : M명
(1번 : 1분 .....M번 : M분  에 베이스캠프 출발해서 편의점 감)

초기조건
- 사람들은 격자 밖에 있음
- 사람마다 목표하는 편의점이 다름


- 게임 진행 순서 (반드시 순서 지켜야함)

1) 격자에 있는 사람들 모두 본인이 가고 싶은 편의점 방향으로 1칸 이동
	- 최단 거리 (이동하여 도달하기까지 거쳐야하는 칸의 수 최소)
		- 여러가지면 상,좌,우,하 우선순위로 이동 (이건 그냥 위치에서 BFS 구해야할듯)
		- 인접한 칸 중 가능한 칸으로만 가능

2) 편의점에 도착하면 해당 편의점에서 멈춤
	- 다른 사람들은 해당 편의점이 있는 칸 못지나감 
	- *****격자 사람 모두 이동 후 해당 칸을 지나갈 수 없게 됨

3) 현재 시간 : t분
	- (t<=M)을 만족한다면 
		1) t번 사람은 목표 편의점과 가장 가까운 베이스 캠프 들어감(시간 : 0)
		2) 가까이 기준 : 최단 거리 (편의점으로부터 BFS 구하기)
		3) 동률인 경우 : 행이 작은 베이스 캠프 
		4) 동률인 경우 : 열이 작은 베이스 캠프 
		5) *******베이스캠프는 지나갈 수 없는 칸 (평생!)


- 출: 몇분 후 도착?
*/

/*
편의점 좌표 관리 -> ID
베이스캠프 좌표 관리
칸마다 지나갈 수 있는지 여부
최단거리 그때마다 구해야함


*/