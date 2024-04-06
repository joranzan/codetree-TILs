#include <iostream>

using namespace std;

/*
입력
N : 맵의 크기(20이하)
M : 플레이어 수 (30이하)
K : 라운드 수

x y d s (위치, 방향, 능력치)

총 공격력 : 100000이하

능력치 : 100이하

*/

const int dr[4] = {-1,0,1,0};
const int dc[4] = {0,1,0,-1};

//반대방향으로 이동할 때 사용
const int dir_reverse[4] = {2,3,0,1};


struct pos{
    int row;
    int col;
};

struct player{
  pos nowPos;    //위치
  int playerId;  //플레이어 번호
  int initial_power;  //초기 능력치
  int weapon = 0;     //무기 공격력
  int dir;  // 방향
  int score = 0;
  
  bool operator<(player next) const{
      if(initial_power+weapon < next.initial_power + next.weapon) return true;
      else if(initial_power+weapon > next.initial_power + next.weapon) return false;
      else{
          if(initial_power<next.initial_power) return true;
          else return false;
      }
  }
  
};

int N,M,K;
int GunMap[21][21]={0,};
int PlayerMap[21][21]={0,};
player PlayerInfo[31];

void init(){
    cin >> N >> M >> K;
    
    //무기 입력 받기
    for(int r=1;r<=N;r++){
        for(int c=1;c<=N;c++){
            cin>>GunMap[r][c];
        }
    }
    
    for(int i=1;i<=N;i++){
        int x,y,d,s;
        cin>>x>>y>>d>>s;
        PlayerMap[x][y] = i;
        PlayerInfo[i].nowPos.row = x;
        PlayerInfo[i].nowPos.col = y;
        PlayerInfo[i].playerId = i;
        PlayerInfo[i].initial_power = s;
        PlayerInfo[i].dir = d;
    }
    
}


void movePlayer(int playerNum){

    

    int nowRow = PlayerInfo[playerNum].nowPos.row;
    int nowCol = PlayerInfo[playerNum].nowPos.col;
    int nowWeapon = PlayerInfo[playerNum].weapon;
    int nowDir = PlayerInfo[playerNum].dir;

    //1) 첫번째 플레이어부터 순차적으로 1칸 (본인이 향하고 있는 방향으로)
    
    int nextRow = nowRow + dr[nowDir];
    int nextCol = nowCol + dc[nowDir];
    //(격자 벗어나면 반대방향으로 1칸 이동)
    if(nextRow<=0||nextCol<=0||nextRow>N||nextCol>N){
        nextRow = nowRow + dr[dir_reverse[nowDir]];
        nextCol = nowCol + dc_reverse[nowDir];
    }

    //****갱신 똑바로 해주기*****
    //PlayerInfo[playerNum]에 정보 갱신
    PlayerInfo[playerNum].dir =  dir_reverse[nowDir];
    PlayerInfo[playerNum].nowPos.row = nextRow;
    PlayerInfo[playerNum].nowPos.col = nextCol;
    //PlayerMap 갱신
    //GunMap 갱신

    //2-1) 이동한 방향에 플레이어가 없는 경우
    if(PlayerMap[nextRow][nextCol]==0){

        //해당 칸에 총이 있는 경우
        if(GunMap[nextRow][nextCol]!=0){
            //1) 총을 가지고 있지 않는 경우 : 총 획득
            if(nowWeapon==0){
                GunMap[nextRow][nextCol] = 

            }
            swap(, PlayerInfo[playerNum].weapon);


            //2) 총을 가지고 있는 경우 : 공격력 max인 총 말고 내려놓음

        }
        //해당 칸에 총이 없는 경우 위치만 변함

        
    }
    //2-2) 이동한 방향에 플레이어가 있는 경우 전투
    else{

        //1) 초기 능력치 + 총의 공격력 큰 플레이어가 승리
        //2) 동률인 경우 : 초기 능력치가 더 큰 플레이어가 승리
    
        //3) 점수 획득 : 플레이어 간 (초기능력치 + 총의 공격력 차이) 획득
    }

    
        

}


void solution(){
    
    for(int p=1;p<=M;p++){
        movePlayer(p);
    }
    
    
}

int main()
{
    ios::sync_with_stdio(false); cin.tie(NULL); cout.tie(NULL);
    
    init();
    solution();
    
    return 0;
}


/*

맵 크기 : NxN

격자 정보

- 무기
    - 공격력
- 플레이어 (무기 없는 위치)
    - 플레이어 번호
    - 초기 능력치(모두 다름)
    
라운드 순서 (모든 플레이어를 대상으로 하면 1라운드 종료 )

1) 첫번째 플레이어부터 순차적으로 1칸 (본인이 향하고 있는 방향으로)
    (격자 벗어나면 반대방향으로 1칸 이동)

2-1) 이동한 방향에 플레이어가 없는 경우
     1) 총을 가지고 있지 않는 경우 : 총 획득 (능력치 증가)
     2) 총을 가지고 있는 경우 : 공격력 max인 총 말고 내려놓음

2-2) 이동한 방향에 플레이어가 있는 경우 전투
    1) 초기 능력치 + 총의 공격력 큰 플레이어가 승리
    2) 동률인 경우 : 초기 능력치가 더 큰 플레이어가 승리
    
    3) 점수 획득 : 플레이어 간 (초기능력치 + 총의 공격력 차이) 획득
    
    
    - 진 플레이어
        1) 총 내려놓음
        2) 바라보던 방향으로 1칸 이동
            1-1) 다른 플레이어 존재 또는 격자 밖 : 시계방향 90도 회전하여 빈칸 찾을 때 이동
        3) 총이 있으면 max 총 말고 내려놓음

    
    - 이긴 플레이어 
        1) 승리한 칸에 떨어진 max 총 획득 후 나머지 내려놓음
        

****점수를 플레이어별로 관리해야함******



**********************************
입력
N : 맵의 크기(20이하)
M : 플레이어 수 (30이하)
K : 라운드 수

x y d s (위치, 방향, 능력치)

총 공격력 : 100000이하

능력치 : 100이하

*/