//01星球牛逼
#include <graphics.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdio>
#include <windows.h>

#define WIN_WIDTH        1000
#define WIN_HEIGHT       800

//获取全局消息
ExMessage msg = { 0 };


//游戏帧率
#define FPS              60

//玩家初始属性
#define PLAYER_INIT_HP   100
#define PLAYER_INIT_ATK  1
//speed，用帧率实现速度
#define PLAYER_SPEED     4
#define BULLET_SPEED     8
#define MONSTER_SPEED    2

//升级所需经验值
#define EXP_PER_LEVEL    20

//每隔多少级生成一次小BOSS
#define MINI_BOSS_LV     5

//多少级生成大BOSS
#define FINAL_BOSS_LV    20

//玩家受伤后无敌帧（毫秒）
#define INVINCIBLE_TIME  800

//游戏界面枚举，标识当前处于哪个界面
enum GameUI
{
    START,        //开始界面
    HELP,         //玩法介绍界面
    SETTING,      //设置界面
    TEAM,         //团队介绍界面
    PLAY,         //游戏游玩主界面
    PAUSE,        //游戏暂停界面
    SETTLEMENT    //游戏结算界面（胜利/失败）
};

//实体类型枚举，区分玩家、怪物、子弹、BOSS等对象
enum EntityType
{
    PLAYER,      //玩家
    MONSTER,     //普通怪物
    MINI_BOSS,   //小BOSS
    FINAL_BOSS,  //大BOSS
    BULLET       //子弹
};

// 按钮类
class Button
{
public:
    int x;              // 按钮左上角X坐标
    int y;              // 按钮左上角Y坐标
    int w;              // 按钮宽度
    int h;              // 按钮高度
};

//玩家类
class Player
{
public:
    int x;              //玩家X坐标
    int y;              //玩家Y坐标
    int w;              //玩家贴图宽度
    int h;              //玩家贴图高度

    int hp;             //当前生命值
    int maxHp;          //最大生命值
    int atk;            //攻击力
    int moveSpeed;      //移动速度

    int level;          //当前等级
    int exp;            //当前经验值
    int expNeed;        //升级所需经验
    int score;          //当前游戏分数

    bool isInvincible;  //是否处于无敌状态
    DWORD invincibleTimer; //无敌帧计时时间

public:
    Player();           //构造函数
    void Init();        //属性初始化
    void Reset();       //重置玩家状态
    void Move();        //玩家移动
    void LimitBorder(); //限制玩家在屏幕边界里
    void Attack();      //玩家攻击，发射子弹
    void TakeDamage(int dmg); //玩家受伤处理
    void LevelUp();     //玩家升级处理
};

//子弹类
class Bullet
{
public:
    int x;              //子弹X坐标
    int y;              //子弹Y坐标
    int w;              //子弹贴图宽度
    int h;              //子弹贴图高度
    int speed;          //子弹移动速度
    int atk;            //子弹攻击力
    bool active;        //子弹是否存在

public:
    Bullet();           //构造函数
    void Init(int px, int py); //初始化子弹位置
    void Move();        //子弹移动
    bool CheckBorder(); //检测子弹是否出界，出界则销毁
};

//怪物类
class Monster
{
public:
    int x;              //怪物X坐标
    int y;              //怪物Y坐标
    int w;              //怪物贴图宽度
    int h;              //怪物贴图高度

    int hp;             //当前生命值
    int maxHp;          //最大生命值
    int speed;          //移动速度
    int expDrop;        //死亡掉落经验值
    int score;          //击杀获得分数

    bool active;        //怪物是否存活
    EntityType type;    //怪物类型（普通/BOSS）

public:
    Monster();          //构造函数
    void RandomSpawn(); //随机位置生成怪物
    void TrackPlayer(Player& player); //自动追踪玩家
	void ShootMonsterBullet();//怪物攻击，发射子弹
    void TakeDamage(int dmg); //怪物受伤处理
    void OnDead();      //怪物死亡处理
};

//图片类
class GameRes
{
public:
    IMAGE imgPlayer;         //玩家图片
    IMAGE imgBullet;         //子弹图片
    IMAGE imgMonster;        //普通怪物图片
    IMAGE imgMiniBoss;       //小BOSS图片
    IMAGE imgFinalBoss;      //大BOSS图片

    IMAGE bgStart;           //开始界面
    IMAGE bgHelp;            //玩法介绍
    IMAGE bgSetting;         //设置界面
    IMAGE bgTeam;            //团队介绍
    IMAGE bgGame;            //游戏界面
    IMAGE bgPause;           //暂停界面
    IMAGE bgSettlement;      //结算界面

public:
    void Load();     //加载所有图片
    void Free();     //释放图片
};

// 当前显示的游戏界面
GameUI     g_curUI;

// 游戏主循环是否运行
bool       g_isRun;

// 游戏是否暂停
bool       g_isPause;

// 游戏是否结束
bool       g_isGameOver;

// 玩家是否胜利
bool       g_isWin;

// 全局玩家对象
Player     g_player;

// 全局子弹列表，管理所有子弹
std::vector<Bullet>  g_bullets;

// 全局怪物列表，管理所有怪物
std::vector<Monster> g_monsters;

// 全局资源对象，管理所有贴图
GameRes    g_res;

// 怪物生成频率（帧间隔）
int        g_spawnRate;

// 怪物生成计时器
int        g_spawnTimer;

// 是否已经生成最终BOSS
bool       g_hasFinalBoss;

// 开始界面-开始游戏按钮
Button     btnStart;

// 开始界面-玩法介绍按钮
Button     btnHelp;

// 开始界面-设置按钮
Button     btnSetting;

// 设置界面-团队介绍按钮
Button     btnTeam;

// 各子界面-返回按钮
Button     btnBack;

// 暂停界面-继续游戏按钮
Button     btnResume;

// 结算/暂停界面-重新开始按钮
Button     btnRestart;

// 通用退出按钮
Button     btnExit;

// 游戏初始化（窗口、资源、变量初始值）
void GameInit();

// 重置游戏数据（重新开始一局）
void GameReset();

// 输入更新（键盘+鼠标消息处理）
void InputUpdate();

// 检测鼠标是否点击指定按钮
bool CheckButtonClick(Button& btn);

// 绘制按钮（文字+背景+悬浮效果）
void DrawButton(Button& btn, const char* text);

// 绘制开始界面
void DrawStartUI();

// 绘制玩法介绍界面
void DrawHelpUI();

// 绘制设置界面
void DrawSettingUI();

// 绘制团队介绍界面
void DrawTeamUI();

// 绘制暂停界面
void DrawPauseUI();

// 绘制结算界面
void DrawSettlementUI();

// 游戏逻辑每帧更新
void GameUpdate();

// 随机生成普通怪物
void SpawnMonster();

// 生成小BOSS
void SpawnMiniBoss();

// 生成大BOSS
void SpawnFinalBoss();

// 更新所有子弹逻辑（移动、越界销毁）
void UpdateBullets();

// 更新所有怪物逻辑（移动、追踪）
void UpdateMonsters();

// 碰撞检测：子弹与怪物
void Collide_BulletMonster();

// 碰撞检测：玩家与怪物
void Collide_PlayerMonster();

// 检测是否满足升级条件
void CheckLevelUp();

// 更新无敌帧状态
void UpdateInvincible();

// 检测游戏结束条件（胜利/失败）
void CheckGameEnd();

// 绘制游戏界面（场景、实体、UI）
void DrawGameUI();

// 绘制玩家UI（血条、经验条、等级、分数）
void DrawPlayerInfo();

// 绘制所有实体（玩家、怪物、子弹）
void DrawEntities();

//对象定义区




int main()
{
    //初始化游戏
    GameInit();
    while (g_isRun)
    {
        //处理鼠标和键盘
        InputUpdate();
        //开始批量绘图，减少闪烁
        BeginBatchDraw();
        cleardevice();

        //绘制当前界面
        switch (g_curUI)
        {
        case START:      DrawStartUI();     break;
        case HELP:       DrawHelpUI();      break;
        case SETTING:    DrawSettingUI();   break;
        case TEAM:       DrawTeamUI();      break;
        case PLAY:
            //非暂停状态下继续游戏
            if (!g_isPause)
                GameUpdate();
            //绘制游戏画面
            DrawGameUI();
            break;
        case PAUSE:      DrawPauseUI();     break;
        case SETTLEMENT: DrawSettlementUI(); break;
        }

        //结束批量绘图，显示画面
        EndBatchDraw();
    }
    g_res.Free();
    closegraph();

    return 0;
}

Player::Player() {

}

void Player::Init() {

}

void Player::Reset() {

}

void Player::Move() {

}

void Player::LimitBorder() {

}

void Player::Attack() {

}

void Player::TakeDamage(int dmg) {

}

void Player::LevelUp() {

}

Bullet::Bullet() {
    this->x = -100;
    this->y = -100;
    this->w = 10;
    this->h = 10;
    this->speed = 8;
    this->atk = 1;
    this->active = false;
}

void Bullet::Init(int px, int py) {
    this->x = px, this->y = py;//更新子弹坐标
    
    this->active = true;
}

void Bullet::Move() { 
    int mx, my, dx, dy;
    double vx, vy, t, s;
    while (true) {
        if (peekmessage(&msg, EX_MOUSE )) {};//获取鼠标消息
        if (msg.message == WM_LBUTTONDOWN) {//左键按下
            mx = msg.x, my = msg.y;
            dx = mx - this->x, dy = my - this->y;
            s = sqrt(dx * dx + dy * dy);
            t = double(s / this->speed);
            vx = (double)(dx / t); vy = (double)(dy / t);//计算子弹x，y速度
            this->x += vx; this->y += vy;//更新子弹坐标
        }
        
    }
}

bool Bullet::CheckBorder() {
    if (this->active == false)return true;//如果初始化的时候子弹就不存在，返回真
    if (this->x + this->w <= 0 || this->x >= getwidth() || this->y + h <= 0 || this->y >= getheight())
     //如果整个图片出界，返回真
    {
        this->active = false;//同时改变子弹的存在状态
        return true;
    }
    return false;
}
//静行开始
Monster::Monster() {
 x=0;
 y=0;
 w=64;//需要到时候再改
 h=64;//同上
 hp=100;
 maxhp=100;
 speed=2;
 expDrop=10;
 score=10;
 active=true;
type=EntityType::MONSTER;
}

void Monster::RandomSpawn() {

}

void Monster::TrackPlayer(Player& player) {

}

void Monster::ShootMonsterBullet() {
    
}
void Monster::TakeDamage(int dmg) {

}

void Monster::OnDead() {

}
//静行结束
void GameRes::Load() {

}

void GameRes::Free() {

}

void GameInit() {

}

void GameReset() {

}

void InputUpdate() {

}

bool CheckButtonClick(Button& btn) {
    return false;
}

void DrawButton(Button& btn, const char* text) {

}

void DrawStartUI() {

}

void DrawHelpUI() {

}

void DrawSettingUI() {

}

void DrawTeamUI() {

}

void DrawPauseUI() {

}

void DrawSettlementUI() {

}

void GameUpdate() {

}

void SpawnMonster() {

}

void SpawnMiniBoss() {

}

void SpawnFinalBoss() {

}

void UpdateBullets() {

}

void UpdateMonsters() {

}

void Collide_BulletMonster() {

}

void Collide_PlayerMonster() {

}

void CheckLevelUp() {

}

void UpdateInvincible() {

}

void CheckGameEnd() {

}

void DrawGameUI() {

}

void DrawPlayerInfo() {

}

void DrawEntities() {

}
//绘制一帧逻辑
// void test() {
//    initgraph(200, 200);
//    setbkcolor(WHITE);
//    int x;
//	DWORD startTime = GetTickCount();
//    while (true)
//    {
//        DWORD currentTime = GetTickCount();
//        if (currentTime - startTime >= 1000 / FPS) // 控制帧率
//        {
//            cleardevice();
//            circle(x, 100, 50);
//            x += 5;
//           
//            startTime = currentTime;
//		}
//		
//    }
//}