#include<iostream>
#include<vector>
#include<queue>

using namespace std;

//Team 정보 어떻게 선언?

struct pos {
	int row;
	int col;
};

struct Team {

	int railID;
	pos nowHead;
	pos nowBeforeTail;
	pos nowTail;
};

const int dr[4] = { -1,1,0,0 };
const int dc[4] = { 0,0,-1,1 };

int N; // Map 크기 (3이상 20이하)
int M; // 팀의 개수 (1이상 5이하)
int K; // 라운드 수 (1이상 1000이하)

int Map[21][21] = { 0, };
int TeamNumInfo[21][21] = { 0, };
bool Visited[21][21] = { 0, };
int teamNum = 0;
long Answer = 0;
vector<Team> teamInfo;
vector<vector<pos>> RailInfo;

void findRail(int row, int col, int railNum) {
	
	//사람도 같이 찾아야함
	vector<pos> nowTeam;    //팀 멤버의 위치
	vector<pos> nowRail;    //해당 팀의 경로
	queue<pos> q;
	q.push({ row, col });
	Visited[row][col] = true;
	nowRail.push_back({ row,col });
	TeamNumInfo[row][col] = teamNum;

	while (!q.empty()) {
		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();

		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;
			if (Map[nextRow][nextCol] == 0 || Map[nextRow][nextCol]==4) continue;
			if (Visited[nextRow][nextCol]) continue;
			Visited[nextRow][nextCol] = true;
			TeamNumInfo[nextRow][nextCol] = teamNum;
			nowRail.push_back({ nextRow, nextCol });
			q.push({ nextRow, nextCol });
		}
	}

	


	Team t;
	t.railID = railNum;
	t.nowHead = nowRail[0];
	t.nowTail = nowRail[nowRail.size()-1];
	t.nowBeforeTail = nowRail[nowRail.size() - 2];
	//사람들의 1번부터 3번까지 순서대로 담겨있음
	//team 정보 넣기


	teamInfo.push_back(t);
	teamNum++;

	q.push(t.nowTail);

	while (!q.empty()) {
		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();

		for (int dir = 0; dir < 4; dir++) {
			for (int dir = 0; dir < 4; dir++) {
				int nextRow = nowRow + dr[dir];
				int nextCol = nowCol + dc[dir];
				if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;
				if (Map[nextRow][nextCol] != 4) continue;
				if (Visited[nextRow][nextCol]) continue;
				Visited[nextRow][nextCol] = true;
				nowRail.push_back({ nextRow, nextCol });
				q.push({ nextRow, nextCol });
			}
		}
	}

	RailInfo.push_back(nowRail);
	
	
}

void init() {

	RailInfo.clear();
	teamInfo.clear();
	teamNum = 0;
	Answer = 0;
	cin >> N >> M >> K;

	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			Visited[r][c] = false;
			cin >> Map[r][c];
			TeamNumInfo[r][c] = 0;
		}
	}

	int railNum = 0;
	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			if (Map[r][c] != 1) continue;
			if (Visited[r][c]) continue;
			findRail(r, c, railNum);
			railNum++;
		}
	}

	int debugging = -1;

}


void moveTeam() {

	for (int i = 0; i < teamInfo.size();i++) {

		pos nowHead = teamInfo[i].nowHead;
		pos nowTail = teamInfo[i].nowTail;
		pos nowBeforeTail = teamInfo[i].nowBeforeTail;

		//머리
		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowHead.row + dr[dir];
			int nextCol = nowHead.col + dc[dir];
			if (Map[nextRow][nextCol] == 4 || Map[nextRow][nextCol]==3) {

				Map[nowHead.row][nowHead.col] = 2;
				teamInfo[i].nowHead.row = nextRow;
				teamInfo[i].nowHead.col = nextCol;
				Map[nextRow][nextCol] = 1;
				break;
			}
		}

		//꼬리
		Map[nowBeforeTail.row][nowBeforeTail.col] = 3;
		if(Map[nowTail.row][nowTail.col]==3) Map[nowTail.row][nowTail.col] = 4;   //이미 1이면 Pass 꼬리물기
		teamInfo[i].nowTail.row = nowBeforeTail.row;
		teamInfo[i].nowTail.col = nowBeforeTail.col;


		//꼬리 바로 전
		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowBeforeTail.row + dr[dir];
			int nextCol = nowBeforeTail.col + dc[dir];
			if (Map[nextRow][nextCol] == 2) {

				teamInfo[i].nowBeforeTail.row = nextRow;
				teamInfo[i].nowBeforeTail.col = nextCol;
				break;
			}
		}

	}

	int debugging = -1;
}

void getScore(pos selected) {
	
	//몇번째인지 찾기
	bool VisitedTemp[21][21] = { false, };
	queue<pos> q;
	q.push({ selected });
	Visited[selected.row][selected.col] = true;
	long cnt = 1;
	while (!q.empty()) {
		int nowRow = q.front().row;
		int nowCol = q.front().col;
		q.pop();

		if (Map[nowRow][nowCol] == 1) {
			break;
		}

		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr[dir];
			int nextCol = nowCol + dc[dir];
			if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;
			if (Map[nextRow][nextCol] != 2 && Map[nextRow][nextCol] != 1) continue;
			if (VisitedTemp[nextRow][nextCol]) continue;
			VisitedTemp[nextRow][nextCol] = true;
			q.push({ nextRow, nextCol });
			cnt++;
		}
	}
	
	Answer += (cnt*cnt);

	int nowTeamNum = TeamNumInfo[selected.row][selected.col];
	pos nowTail = teamInfo[nowTeamNum].nowTail;
	pos nowHead = teamInfo[nowTeamNum].nowHead;
	pos nowBeforeTail = teamInfo[nowTeamNum].nowBeforeTail;

	//Head랑 Tail 바꾸고 Before Tail 변경해야함
	Map[nowTail.row][nowTail.col] = 1;
	Map[nowHead.row][nowHead.col] = 3;
	for (int dir = 0; dir < 4; dir++) {
		int nextRow = nowHead.row + dr[dir];
		int nextCol = nowHead.col + dc[dir];
		if (Map[nextRow][nextCol] == 2) {
			teamInfo[nowTeamNum].nowBeforeTail.row = nextRow;
			teamInfo[nowTeamNum].nowBeforeTail.col = nextCol;
			break;
		}
	}

	teamInfo[nowTeamNum].nowTail = nowHead;
	teamInfo[nowTeamNum].nowHead = nowTail;

	

}

void solution() {
	


	for (int round = 1; round <= K; round++) {

		moveTeam();
		int nowBallway = round % (4 * N);
		bool getHit = false;
		pos getHitPerson;
		if (nowBallway > 0 && nowBallway <= N) {
			

			for (int i = 1; i <= N; i++) {
				if (Map[nowBallway][i] != 0 && Map[nowBallway][i] != 4) {
					getHit = true;
					getHitPerson.row = nowBallway;
					getHitPerson.col = i;
					break;
				}
			}
		}
		else if (nowBallway > N && nowBallway <= 2 * N) {
			int startPoint = nowBallway % N;
			if (startPoint == 0) startPoint = N;
			for (int i = N; i >= 1; i--) {
				if (Map[i][startPoint] != 0 && Map[i][startPoint] != 4) {
					getHit = true;
					getHitPerson.row = i;
					getHitPerson.col = startPoint;
					break;
				}
			}
		}
		else if (nowBallway > 2 * N && nowBallway <= 3 * N) {
			int startPoint = nowBallway % (2*N);   //1~N까지 남음
			startPoint = N - startPoint + 1;
			for (int i = N; i >= 1; i--) {
				if (Map[startPoint][i] != 0 && Map[startPoint][i] != 4) {
					getHit = true;
					getHitPerson.row = startPoint;
					getHitPerson.col = i;
					break;
				}
			}
		}
		else if ((nowBallway > 3 * N && nowBallway < 4 * N) || nowBallway ==  0){
			
			
			int startPoint = nowBallway % (3*N);
			startPoint = N - startPoint + 1;
			for (int i = 1; i <= N; i++) {
				if (Map[i][startPoint] != 0 && Map[i][startPoint]!=4) {
					getHit = true;
					getHitPerson.row = i;
					getHitPerson.col = startPoint;
					break;
				}
			}
		}

		if (!getHit) continue;

		getScore(getHitPerson);

	}
}

int main() {
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	init();
	solution();
	cout << Answer;

	return 0;
}

/*
문제 분석

NxN Map

1. 팀 조건

팀 : 3명 이상 (앞사람의 허리 잡고 움직임)
	- 맨앞 : 머리
	- 맨뒤 : 꼬리

2. 이동
- 주어진 이동 선을 따라서만 이동 (순환식)
- 이동선은 겹치지 않음


3. 게임 시작 (라운드별)

1) 머리 먼저이동, 그 이후에 뒤에 다 한 칸씩 따라옴

2) 공의 이동 (반복)   
0    - (1 ~ N 번째 라운드) : 공이 좌측->우측 Row로 날아옴
1	- (N+1 ~ 2N 번째 라운드) : 공이 하단->상단 모든 Column으로 날아옴
2	- (2N+1 ~ 3N 번째 라운드) : 공이 우측->좌측 모든 Row로 날아옴
3	- (3N+1 ~ 4N 번쨰 라운드) : 공이 상단->하단 모든 Column으로 날아옴

3) 공 날라올 때 사람이 있으면 **최초**만 점수 얻음

점수 : 머리로부터 k번째 사람 : k^2

	- 점수를 얻은 팀
		1) 머리와 꼬리 변경 (방향을 바꿈)
			0이면 1 / 1이면 0

	- 아무도 없다면 Continue

4) 라운드 종료



*/

/*
문제 풀이

머리와 꼬리 후보는 2명 밖에 없음

1. Team 정보 관리
	- 몇 번 노선
	- 머리 어디 (최초 머리 :0, 최초 꼬리 : 1)
	- 최초 머리 : 
	- 최초 꼬리 : 

	- 멤버 관리 필요(머리로부터 몇번째인지 확인 필요)
		최초 머리 [머리, 허리 ........허리, 꼬리]
		최초 꼬리 [꼬리, 허리 ........허리, 머리]
	- 점수

2. (1~K)라운드별 공 방향 및 인덱스(공 날라오는 Row/Col) 로직 구현

3. 해당 행/열에 있는 최초 찾아서 점수 주기 (몇번째인지 확인 필요)

4. 해당 팀 방향 바꾸기

*/