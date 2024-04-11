//시작 시간 : 18:35
//종료 시간 : 
#include<iostream>
#include<queue>
#include<vector>
#include<cmath>

using namespace std;

/*
입력 조건

N : 맵의 크기 (3~50)
M : 게임 턴의 수 (1~1000)
P : 산타의 수 (1 ~ 30)
C : 루돌프의 힘
D : 산타의 힘
Pn : 산타의 번호 (1~P)

*/

const int dr[8] = { -1,1,0,0,-1,-1,1,1 };
const int dc[8] = { 0,0,-1,1,-1,1,-1,1 };

//상우하좌
const int dr_s[4] = { -1,0,1,0 };
const int dc_s[4] = { 0,1,0,-1 };
const int rv_dir[4] = { 2,3,0,1 };


struct pos {
	int row;
	int col;
};

struct rudolph {
	pos nowPos; //현재 위치
	int dir;    //루돌프의 방향
};

struct santa {
	int id;
	pos nowPos;
	int dir;
	int stunned = 0;  //1이면 기절 , 0이면 활성화
	bool alive = true;
	int score = 0;
};

int N, M, P, C, D;
santa SantaInfo[31];  //각 산타의 정보 
int SantaMap[51][51]; //산타의 위치 정보
rudolph RudolphInfo;

void init() {
	
	cin >> N >> M >> P >> C >> D;

	//SantaMap 초기화
	for (int r = 1; r <= N; r++) {
		for (int c = 1; c <= N; c++) {
			SantaMap[r][c] = 0;
		}
	}

	//루돌프 정보 초기화
	cin >> RudolphInfo.nowPos.row >> RudolphInfo.nowPos.col;
	RudolphInfo.dir = -1;

	for (int i = 1; i <= P; i++) {
		int id, r, c;
		cin >> id >> r >> c;
		//SantaMap 초기화
		SantaMap[r][c] = id;

		//SantaInfo 초기화
		SantaInfo[id].id = id;
		SantaInfo[id].nowPos.row = r;
		SantaInfo[id].nowPos.col = c;
		SantaInfo[id].dir = -1;
		SantaInfo[id].stunned = 0;
		SantaInfo[id].alive = true;
		SantaInfo[id].score = 0;
	}
}


void Interaction(int SantaID) {
	//4. 상호작용

	//	1) 조건 : 루돌프와 충돌 후 날라간 자리에 다른 산타 있는 경우
	//	2) 원래 있던 산타는 해당 방향으로 1칸 밀려남(연쇄)
	//	3) 더이상 최종 위치에 산타가 없다면 끝
	//	4) 격자밖으로 나갔다면 탈락
	

	//상호작용 시작점
	int nowRow = SantaInfo[SantaID].nowPos.row;
	int nowCol = SantaInfo[SantaID].nowPos.col;
	int nowDir = SantaInfo[SantaID].dir;
	int nowSantaID = SantaID;

	while (1) {
		//cout <<nowSantaID<<"번 산타 날라와서 상호작용!" << "\n";
		int nextSantaID = SantaMap[nowRow][nowCol];
		//cout << nextSantaID << "번 산타 기존에 있었는데 날라갈 준비!" << "\n";
		SantaMap[nowRow][nowCol] = nowSantaID;   //날라온 산타로 갱신

		int nextSantaRow = SantaInfo[nextSantaID].nowPos.row + dr[nowDir];
		int nextSantaCol = SantaInfo[nextSantaID].nowPos.col + dc[nowDir];

		//상호작용 중 루돌프 만날 일 없음 --> 항상 반대방향이므로

		if (nextSantaRow <= 0 || nextSantaCol <= 0 || nextSantaRow > N || nextSantaCol > N) {
			SantaInfo[nextSantaID].alive = false;
			//cout <<nextSantaID<< "번 산타 죽어서 상호작용 끝!\n";
			break;
		}


		//다음 도착지에 산타있다면
		if (SantaMap[nextSantaRow][nextSantaCol] != 0) {
			SantaInfo[nextSantaID].nowPos.row = nextSantaRow;
			SantaInfo[nextSantaID].nowPos.col = nextSantaCol;
			SantaInfo[nextSantaID].dir = nowDir;
			nowRow = nextSantaRow;
			nowCol = nextSantaCol;
			nowSantaID = nextSantaID;

		}
		else {
			SantaInfo[nextSantaID].nowPos.row = nextSantaRow;
			SantaInfo[nextSantaID].nowPos.col = nextSantaCol;
			SantaInfo[nextSantaID].dir = nowDir;
			SantaMap[nextSantaRow][nextSantaCol] = nextSantaID;
			//cout << "상호작용 끝!\n";
			break;
		}

	}

	//cout << "\n";

}


void moveRudolph() {

	int nowRow = RudolphInfo.nowPos.row;
	int nowCol = RudolphInfo.nowPos.col;

	

	//1. 루돌프 움직임

	//	1) 가장 가까운 산타를 향해 1칸 돌진
	//	1) 이동방향 : 상하좌우, 대각선
	//	2) 8방향 중 가장 가까워지는 방향으로 한 칸 돌진
	//	2) 최단거리 동률일 때
	//	2 - 1) r좌표가 큰 산타(더 밑에 있는 산타)
	//	2 - 2) 동률일 때 : c좌표가 큰 산타(더 오른쪽에 있는 산타)

	//*****************************************************
	//현재 루돌프의 위치에서 최단거리의 산타 찾기 

	int targetSantaID = 2112345678;   //가장 가까운 산타 ID
	int minDist = 2112345678;
	


	//방향 찾기
	for (int i = 1; i <= P; i++) {
		//1) 탈락한 산타 제외
		if (!SantaInfo[i].alive) continue;
		int santaRow = SantaInfo[i].nowPos.row;
		int santaCol = SantaInfo[i].nowPos.col;
		int nowDist = pow(santaRow - nowRow, 2) + pow(santaCol - nowCol, 2);
		if (minDist < nowDist) continue;

		if (targetSantaID == 2112345678) {
			targetSantaID = i;
			minDist = nowDist;
			continue;
		}
		if (nowDist == minDist) {
			if (santaRow > SantaInfo[targetSantaID].nowPos.row) {
				targetSantaID = i;
				minDist = nowDist;
			}
			else if (santaRow == SantaInfo[targetSantaID].nowPos.row) {
				if (santaCol > SantaInfo[targetSantaID].nowPos.col) {
					targetSantaID = i;
					minDist = nowDist;
				}
			}
		}
		else {
			targetSantaID = i;
			minDist = nowDist;
		}
	}


	if (targetSantaID == 2112345678 || minDist == 2112345678) {
		return;
	}
	
	int RudolphDir = -1;

	int targetRow = SantaInfo[targetSantaID].nowPos.row;
	int targetCol = SantaInfo[targetSantaID].nowPos.col;
	int nowDist = 2112345678;
	//	2) 8방향 중 가장 가까워지는 방향으로 한 칸 돌진
	for (int dir = 0; dir < 8; dir++) {
		int nextRow = nowRow + dr[dir];
		int nextCol = nowCol + dc[dir];
		if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;
		int nextDist = pow(targetRow - nextRow, 2) + pow(targetCol - nextCol, 2);
		//가장 가까워지는 방향으로 돌진
		if (nextDist > minDist) continue;
		if (nextDist >= nowDist) continue;
		//이전에 찾은 산타의 거리보다 크면 무시
		
		nowDist = nextDist;
		RudolphDir = dir;
	}

	

	//루돌프 이동
	RudolphInfo.dir = RudolphDir;
	RudolphInfo.nowPos.row = nowRow + dr[RudolphDir];
	RudolphInfo.nowPos.col = nowCol + dc[RudolphDir];


	//충돌 여부 판별
	int RudolphRow = RudolphInfo.nowPos.row;
	int RudolphCol = RudolphInfo.nowPos.col;

	//3. 충돌
	//	1) 산타와 루돌프 같은 칸
	if (SantaMap[RudolphRow][RudolphCol] != 0) {
		
		int SantaID = SantaMap[RudolphRow][RudolphCol];
		int nowSantaRow = SantaInfo[SantaID].nowPos.row;
		int nowSantaCol = SantaInfo[SantaID].nowPos.col;
		int nextSantaRow = nowSantaRow + dr[RudolphDir] * C;
		int nextSantaCol = nowSantaCol + dc[RudolphDir] * C;

		//cout << SantaID << "번 산타와 충돌! \n";
		//기존 칸에 산타 없애기
		SantaMap[nowSantaRow][nowSantaCol] = 0;
		SantaInfo[SantaID].score += C;//산타는 C만큼 점수 얻음
		
		//격자 밖 벗어난 경우
		if (nextSantaRow <= 0 || nextSantaCol <= 0 || nextSantaRow > N || nextSantaCol > N) {
			SantaInfo[SantaID].alive = false;
		}
		//격자 밖 안벗어난 경우
		else {
			//산타 정보 갱신
			SantaInfo[SantaID].dir = RudolphDir;
			SantaInfo[SantaID].nowPos.row = nextSantaRow;
			SantaInfo[SantaID].nowPos.col = nextSantaCol;
			SantaInfo[SantaID].stunned = 2;

			//다음 위치에 산타가 존재한다면
			if (SantaMap[nextSantaRow][nextSantaCol] != 0) {
				Interaction(SantaID);
			}
			else {
				SantaMap[nextSantaRow][nextSantaCol] = SantaID;
			}
		}

	}
	//	1 - 1) 루돌프가 움직여서 충돌 경우
	//	1) 산타는 C만큼 점수 얻음
	//	2) 루돌프가 이동한 방향으로 C칸만큼 밀려남
			//2-1) 충돌 경우 포물선----->날라가는 도중에 충돌 안함
			//2-2) 밀려난 위치가 격자 밖 : 탈락
			//2-3) 밀려난 칸에 다른 산타 : 상호작용 발생


	//5. 기절

	//	1) 조건 : 산타는 루돌프와 충돌 후 기절
	//	2) k번째 턴에 기절---- > k + 1에 기절 상태 유지
	//	3) k + 2번째 턴에서 정상상태
	//	4)************** 기절 상태에도 충돌, 상호작용 가능!!!!!
	//	5)************** 루돌프는 기절한 산타를 돌진 대상으로 선택 가능!!
}

void moveSanta() {

	//2. 산타의 움직임

	//	1) 1번부터 P번까지 순서대로 움직임
	//	1 - 1) 기절한 산타 무시
	//	1 - 2) 탈락한 산타 무시
	//	2) 루돌프에 가장 가까워지는 방향으로 이동
	//	2 - 1) 상하좌우 4방향
	//	2 - 2) 우선순위 : 상하좌우
	//	2 - 3) 격자 밖 무시
	//	2 - 4) 다른 산타 있다면 무시
	//	2 - 5) 방향 없다면 움직이지 않음
	//	2 - 6) 움직일 수 있어도 가까워지지 않으면 무시

	for (int i = 1; i <= P; i++) {
		if (!SantaInfo[i].alive) continue;  //탈락한 산타 무시
		if (SantaInfo[i].stunned!=0) {      //기절한 산타 무시
			SantaInfo[i].stunned--;
			continue;
		}

		int nowRow = SantaInfo[i].nowPos.row;
		int nowCol = SantaInfo[i].nowPos.col;
		int nowDist = pow(nowRow - RudolphInfo.nowPos.row, 2) + pow(nowCol - RudolphInfo.nowPos.col, 2);
		int minDist = 2112345678;
		int minDir = -1;
		
		for (int dir = 0; dir < 4; dir++) {
			int nextRow = nowRow + dr_s[dir];
			int nextCol = nowCol + dc_s[dir];
			if (nextRow <= 0 || nextCol <= 0 || nextRow > N || nextCol > N) continue;
			if (SantaMap[nextRow][nextCol] != 0) continue;
			int nextDist = pow(nextRow - RudolphInfo.nowPos.row, 2) + pow(nextCol - RudolphInfo.nowPos.col, 2);
			if (nextDist >= nowDist) continue;

			if (nextDist < minDist) {
				minDir = dir;
				minDist = nextDist;
			}
		}

		if (minDir == -1) continue;

		int nextRow = nowRow + dr_s[minDir];
		int nextCol = nowCol + dc_s[minDir];

		//이동할 수 있는 칸 존재
		SantaMap[nowRow][nowCol] = 0; //기존 칸 갱신
		//만약 루돌프가 존재한다면 
		if (RudolphInfo.nowPos.row == nextRow && RudolphInfo.nowPos.col == nextCol) {
			

			SantaInfo[i].score += D;    //산타는 D만큼 점수 얻음
			//cout << i << "번 산타 이동 중 루돌프와 충돌!\n";
			//자신이 이동한 방향(반대로) D칸만큼 이동
			int nextNextRow = nextRow + dr_s[rv_dir[minDir]] * D;
			int nextNextCol = nextCol + dc_s[rv_dir[minDir]] * D;
			if (nextNextRow <= 0 || nextNextCol <= 0 || nextNextRow > N || nextNextCol > N) {
				SantaInfo[i].alive = false;
			}
			else {
				//상하좌우
				  //상우하좌
				int DirMap[4] = { 0,3,1,2 };
				SantaInfo[i].dir = DirMap[rv_dir[minDir]];
				SantaInfo[i].nowPos.row = nextNextRow;
				SantaInfo[i].nowPos.col = nextNextCol;
				SantaInfo[i].stunned = 1;

				if (SantaMap[nextNextRow][nextNextCol] != 0) {
					Interaction(i);
				}
				else {
					SantaMap[nextNextRow][nextNextCol] = i;
				}
			}
		}
		//루돌프 없다면
		else {
			SantaMap[nextRow][nextCol] = i;
			SantaInfo[i].dir = minDir;
			SantaInfo[i].nowPos.row = nextRow;
			SantaInfo[i].nowPos.col = nextCol;
		}

	}


	//3. 충돌
	//	1) 산타와 루돌프 같은 칸

	//	1-2) 산타가 움직여서 충돌 경우
	//	1) 산타는 D만큼 점수 얻음
	//	2) 자신이 이동한 방향(반대로) D칸만큼 이동

	//	2) 충돌 경우 포물선----->날라가는 도중에 충돌 안함
	//	3) 밀려난 위치가 격자 밖 : 탈락
	//	4) 밀려난 칸에 다른 산타 : 상호작용 발생



	//5. 기절

	//	1) 조건 : 산타는 루돌프와 충돌 후 기절
	//	2) k번째 턴에 기절---- > k + 1에 기절 상태 유지
	//	3) k + 2번째 턴에서 정상상태
	//	4)************** 기절 상태에도 충돌, 상호작용 가능!!!!!
	//	5)************** 루돌프는 기절한 산타를 돌진 대상으로 선택 가능!!
}

void solution() {

	for (int turn = 1; turn <= M; turn++) {

		// cout << "--------------------------------------------------------------------------\n";
		// cout << turn << "번째 턴 \n\n";

		// cout << "이동 전 루돌프의 위치: (" << RudolphInfo.nowPos.row << " " << RudolphInfo.nowPos.col << ")\n\n";



		// cout << "루돌프 이동 전 산타들의 위치\n";
		// for (int r = 1; r <= N; r++) {
		// 	for (int c = 1; c <= N; c++) {
		// 		cout << SantaMap[r][c] << " ";
		// 	}
		// 	cout << "\n";
		// }
		// cout << "\n";

		// if (turn == 7) {
		// 	int debug = -1;
		// }
		//루돌프 움직임 (충돌 및 상호작용 가능)
		moveRudolph();

		// cout << "이동 후 루돌프의 위치: (" << RudolphInfo.nowPos.row << " " << RudolphInfo.nowPos.col << ")\n\n";


		// cout << "루돌프 이동 후 산타들의 위치\n";
		// for (int r = 1; r <= N; r++) {
		// 	for (int c = 1; c <= N; c++) {
		// 		cout << SantaMap[r][c] << " ";
		// 	}
		// 	cout << "\n";
		// }
		// cout << "\n";

		//산타 움직임 (충돌 및 상호작용 가능)
		moveSanta();


		// cout << "산타 이동 후 산타들의 위치\n";
		// for (int r = 1; r <= N; r++) {
		// 	for (int c = 1; c <= N; c++) {
		// 		cout << SantaMap[r][c] << " ";
		// 	}
		// 	cout << "\n";
		// }
		// cout << "\n";

		//6. 게임 종료

		bool isDone = true;

		for (int i = 1; i <= P; i++) {
			if (!SantaInfo[i].alive) {
				//cout << "죽음(" << SantaInfo[i].score << ") ";
				continue;
			}
			isDone = false;
			SantaInfo[i].score++;
			//cout << SantaInfo[i].score << " ";
		}

		//cout << "\n";

		//	1) M번의 턴이 끝나면 종료
		//	2) 산타 모두 탈락이면 종료
		//	3) 각 턴마다 탈락하지 않은 산타들에게 1점씩 부여

		if (isDone) break;
	}

	for (int i = 1; i <= P; i++) {
		cout << SantaInfo[i].score << " ";
	}

}

int main() {
	ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);

	init();
	solution();

	return 0;
}

/*
문제 분석

산타 : P명 
루돌프 : 산타들을 박치기 하여 산타의 선물을 방해


게임 정보

1. NxN (1,1) 
2. 턴 수 : M 

	1) 루돌프 한번 움직임 
	2) 1번 ~ P번 산타까지 순서대로 움직임

3. 거리 계산 (r차이)^2 + (c차이)^2


게임 순서

1. 루돌프 움직임 

	1) 가장 가까운 산타를 향해 1칸 돌진 
		1) 이동방향 : 상하좌우, 대각선 
		2) 8방향 중 가장 가까워지는 방향으로 한 칸 돌진 
	2) 최단거리 동률일 때
		2-1) r좌표가 큰 산타 (더 밑에 있는 산타)
		2-2) 동률일 때: c좌표가 큰 산타 (더 오른쪽에 있는 산타)
	
2. 산타의 움직임

	1) 1번부터 P번까지 순서대로 움직임
		1-1) 기절한 산타 무시
		1-2) 탈락한 산타 무시
	2) 루돌프에 가장 가까워지는 방향으로 이동
		2-1) 상하좌우 4방향
		2-2) 우선순위 : 상우하좌
		2-3) 격자 밖 무시
		2-4) 다른 산타 있다면 무시
		2-5) 방향 없다면 움직이지 않음
		2-6) 움직일 수 있어도 가까워지지 않으면 무시
	
3. 충돌 
	1) 산타와 루돌프 같은 칸

		1-1) 루돌프가 움직여서 충돌 경우
			1) 산타는 C만큼 점수 얻음
			2) 루돌프가 이동한 방향으로 C칸만큼 밀려남
			

		1-2) 산타가 움직여서 충돌 경우
			1) 산타는 D만큼 점수 얻음
			2) 자신이 이동한 방향 (반대로) D칸만큼 이동

	2) 충돌 경우 포물선 -----> 날라가는 도중에 충돌 안함
	3) 밀려난 위치가 격자 밖 : 탈락
	4) 밀려난 칸에 다른 산타 : 상호작용 발생


4. 상호작용

	1) 조건 : 루돌프와 충돌 후 날라간 자리에 다른 산타 있는 경우
	2) 원래 있던 산타는 해당 방향으로 1칸 밀려남 (연쇄)
	3) 더이상 최종 위치에 산타가 없다면 끝
	4) 격자밖으로 나갔다면 탈락


5. 기절 

	1) 조건 : 산타는 루돌프와 충돌 후 기절
	2) k번째 턴에 기절 ----> k+1에 기절 상태 유지
	3) k+2번째 턴에서 정상상태
	4) **************기절 상태에도 충돌, 상호작용 가능!!!!!
	5) **************루돌프는 기절한 산타를 돌진 대상으로 선택 가능!!


6. 게임 종료

	1) M번의 턴이 끝나면 종료
	2) 산타 모두 탈락이면 종료
	3) 각 턴마다 탈락하지 않은 산타들에게 1점씩 부여



목표 : 각 산타가 얻은 최종 점수
*/

/*
예외 처리

1. 산타 동작
	1) 기절한 산타
	2) 격자 밖에 빠져나가 탈락한 산타

2. 루돌프의 방향 정할 때 
	1) 탈락한 산타 제외



*/