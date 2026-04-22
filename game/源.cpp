#include <graphics.h>
#include <iostream>
#include <vector>
#include <ctime>
#include <cstdio>
#include <windows.h>
#include<cmath>
#include "EasyXpng.h"
#define WIN_WIDTH        1000
#define WIN_HEIGHT       700
using namespace std;
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


class Button;
class Player;
class Monster;
class Bullet;
class GameRes;

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
    void ShootMonsterBullet(Player& player);//怪物攻击，发射子弹
    void TakeDamage(int dmg,Player& player); //怪物受伤处理
    void OnDead(Player& player);      //怪物死亡处理
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
    int flag;           //子弹类型  0：玩家  1：怪物
    bool active;        //子弹是否存在

public:
    Bullet();                                 //构造函数
    void Init(int px, int py,int pflag);      //初始化子弹位置和类型
    void P_Move();                            //玩家子弹移动
    void M_Move();                            //大boss子弹移动
    void TrackPlayer(Player& player);         //boss子弹追击玩家
    bool CheckBorder();                       //检测子弹是否出界，出界则销毁
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

// 游戏中界面-暂停按钮（小康补充）
Button btnPause;

// 暂停界面-继续游戏按钮
Button     btnResume;

// 结算/暂停界面-重新开始按钮
Button     btnRestart;

// 通用退出按钮
Button     btnExit;

// 游戏初始化（窗口、资源、变量初始值）
void GameInit(GameRes* picture);

// 重置游戏数据（重新开始一局）
void GameReset();

// 输入更新（键盘+鼠标消息处理）
void InputUpdate();

// 检测鼠标是否点击指定按钮
bool CheckButtonClick(Button& btn);

// 绘制按钮（文字+背景+悬浮效果）
void DrawButton(Button& btn, const char* text);

// 绘制开始界面
void DrawStartUI(GameRes* picture);

// 绘制玩法介绍界面
void DrawHelpUI(GameRes* picture);

// 绘制设置界面
void DrawSettingUI();

// 绘制团队介绍界面
void DrawTeamUI();

// 绘制暂停界面
void DrawPauseUI(GameRes* picture);

// 绘制结算界面
void DrawSettlementUI(GameRes* picture, Player* player);

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
void DrawGameUI(GameRes* picture, Player* p);

// 绘制玩家UI（血条、经验条、等级、分数）
void DrawPlayerInfo(GameRes* picture, Player* p);
// 绘制所有实体（玩家、怪物、子弹）
void DrawEntities(Player* player, GameRes* picture);

// 绘制怪物血条
void DrawMonsterHPBar(Monster& monster);

//用于开始界面的功能图形绘制
void functionalshape(int rx, int ry, int rw, int rh, std::string s);
//用于玩法介绍界面文字绘制
void drawtext(int x, int y, std::string s);

int main()
{
   
    //对象定义区
	GameRes* picture = new GameRes();
    Player* player = new Player();
    //对象定义结束

	srand((unsigned)time(NULL));
    //初始化游戏
    GameInit(picture);
    //开始批量绘图，减少闪烁
    BeginBatchDraw();
    //游戏主循环
    while (g_isRun)
    {
        //处理鼠标和键盘
        InputUpdate();
        
        cleardevice();

        //绘制当前界面
        switch (g_curUI)
        {
        case START:      DrawStartUI(picture);     break;
        case HELP:       DrawHelpUI( picture);      break;
        case SETTING:    DrawSettingUI();   break;
        case TEAM:       DrawTeamUI();      break;
        case PLAY:
            //非暂停状态下继续游戏
            if (!g_isPause)
                GameUpdate();
            //绘制游戏画面
             DrawGameUI( picture, player);
            break;
        case PAUSE:      DrawPauseUI( picture);     break;
        case SETTLEMENT: DrawSettlementUI( picture, player); break;
        }
		FlushBatchDraw();
        
    }
    //结束批量绘图，显示画面
    EndBatchDraw();
    g_res.Free();
    closegraph();
	picture->Free();

    return 0;
}

Player::Player() {
    Init();
}

void Player::Init() {
    w = 50;
    h = 50;
    //屏幕中心
    x = WIN_WIDTH / 2 - w / 2;
    y = WIN_HEIGHT / 2 - h / 2;
    //基础属性
    hp = PLAYER_INIT_HP;
    maxHp = PLAYER_INIT_HP;
    atk = PLAYER_INIT_ATK;
    moveSpeed = PLAYER_SPEED;
    //等级经验
    level = 1;
    exp = 0;
    expNeed = EXP_PER_LEVEL;
    score = 0;
    //无敌状态
    isInvincible = false;
    invincibleTimer = 0;
}

void Player::Reset() {
    Init();
}

void Player::Move() {
    if (GetAsyncKeyState('W') & 0x8000) {
        y -= moveSpeed;
    }
    if (GetAsyncKeyState('S') & 0x8000) {
        y += moveSpeed;
    }
    if (GetAsyncKeyState('A') & 0x8000) {
        x -= moveSpeed;
    }
    if (GetAsyncKeyState('D') & 0x8000) {
        x += moveSpeed;
    }
    //在屏幕内移动
    LimitBorder();
}

void Player::LimitBorder() {
    //左边界
    if (x < 0) x = 0;
    //右边界
    if (x + w > WIN_WIDTH) x = WIN_WIDTH - w;
    //上边界
    if (y < 0) y = 0;
    //下边界
    if (y + h > WIN_HEIGHT) y = WIN_HEIGHT - h;
}

void Player::Attack() {
    //鼠标左键攻击
    if (msg.message == WM_LBUTTONDOWN)
    {
        Bullet bullet;
        //从玩家中心发射
        bullet.Init(x + w / 2, y + h / 2);
        g_bullets.push_back(bullet);
    }
}

void Player::TakeDamage(int dmg) {
    //无敌不受伤害
    if (isInvincible) return;
    //扣血
    hp -= dmg;
    if (hp < 0) hp = 0;
    //无敌帧
    isInvincible = true;
    invincibleTimer = GetTickCount();
}

void Player::LevelUp() {
    //扣除升级所需经验
    exp -= expNeed;
    //等级提升
    level++;
    //升级属性强化
    maxHp += 20;
    hp = maxHp;   //升级回满生命值
    atk += 1;     //攻击力提升
    moveSpeed += 1;//移动速度提升
    expNeed += EXP_PER_LEVEL * 1.2;//提升下一级所需经验
}

Bullet::Bullet() {
    this->x = 0;
    this->y = 0;
    this->w = 10;
    this->h = 10;
    this->speed = 8;
    this->atk = 1;
    this->active = false;
}

void Bullet::Init(int px, int py,int pflag) {
    this->x = px, this->y = py;//更新子弹初始坐标
    this->flag = flag;
    this->active = true;//更新子弹存在状态
}

void Bullet::P_Move() {
    this->atk = g_player.atk;//更新玩家子弹伤害
    //计算
    int mx, my, dx, dy;
    mx = my = dx = dy = 0;
    double vx, vy, t, s;
    vx = vy = t = s = 0;

    mx = msg.x, my = msg.y;
    dx = mx - this->x, dy = my - this->y;
    s = sqrt(dx * dx + dy * dy);
    t = double(s / this->speed);
    vx = (double)(dx / t); vy = (double)(dy / t);//计算子弹x，y速度

    this->x += vx; this->y += vy;//更新子弹坐标
       
}

void Bullet::M_Move() {
    this->atk = 100;//初始化怪物子弹伤害
    this->TrackPlayer(g_player);//子弹追击玩家 
}

void Bullet::TrackPlayer(Player& player) {//完全照搬怪物追击玩家的函数
    int dx = player.x - this->x;
    int dy = player.y - this->y;
    double distance = sqrt(dx * dx + dy * dy);
    this->x += (dx / distance) * this->speed;
    this->y += (dy / distance) * this->speed;
    if (distance < 1.0) return;
}

bool Bullet::CheckBorder() {
    if (this->active == false)return true;//如果初始化的时候子弹就不存在，返回真
    if (this->x  <= 0 || this->x + this->w >= getwidth() || this->y  <= 0 || this->y + this->h >= getheight())
        //如果整个图片出界，返回真
    {
        this->active = false;//同时改变子弹的存在状态
        return true;
    }
    return false;
}
//静行开始
Monster::Monster() {
    x = 0;
    y = 0;
    w = 64;//需要到时候再改
    h = 64;//同上
    hp = 100;
    maxHp = 100;
    speed = 2;
    expDrop = 10;
    score = 10;
    active = true;
    type = EntityType::MONSTER;
}

void Monster::RandomSpawn() {
    int posx;
    int posy;
    int minx = 0;
    int miny = 0;
    int maxx = 100;
    int maxy = 100;
    posx = rand() % (maxx - minx + 1) + minx;
    posy = rand() % (maxy - miny + 1) + miny;
    x = posx;
    y = posy;

}

void Monster::TrackPlayer(Player& player) {
  
        int dx = player.x - x;
        int dy = player.y - y;
        double distance = sqrt(dx * dx + dy * dy);
        x += (dx / distance) * speed;
        y += (dy / distance) * speed;
         if (distance < 1.0) {
        return;
    }
}

void Monster::ShootMonsterBullet(Player& player) {
 if (!active) return;
 Bullet bullet;
 bullet.Init(x, y); 
 bullet.speed = 3;   
 bullet.atk = 1;     
 bullet.active = true;
}

void Monster::TakeDamage(int dmg,Player& player) {
    if (dmg < 100 && dmg>0) {
        hp -= dmg;
    }
    else {
        hp =0;
    }
    if (hp == 0) {
        OnDead(player);
    }
}//遇到攻击怪物闪烁 是否需要加
void Monster::OnDead(Player& player) {
 active = false;
 player.exp += expDrop;
 player.score += score;
}
//静行结束

void GameRes::Load() {
    //
	
	loadimage(&this->imgPlayer, "photo/kun.png", 32, 32);
	loadimage(&this->imgBullet, "photo/bullet.png", 10, 10);
	loadimage(&this->imgMonster, "photo/monster.png", 32, 32);
	loadimage(&this->imgMiniBoss, "photo/littleBoss.png", 64, 64);
	loadimage(&this->imgFinalBoss, "photo/BigBoss.png", 128, 128);
	loadimage(&this->bgStart, "photo/kk1.jpg", 1100, 700);
	loadimage(&this->bgHelp, "photo/js1.jpg", 1000, 700);
	loadimage(&this->bgSetting, "photo/kk1.jpg", 1100, 700);
	loadimage(&this->bgTeam, "photo/kk1.jpg", 1000, 700);
	loadimage(&this->bgGame, "photo/kk4.jpg", 1000, 700);
	loadimage(&this->bgPause, "photo/zt1.jpg", 1000, 700);
	loadimage(&this->bgSettlement, "photo/sb1.jpg", 1100, 700);
    loadimage(&this->bgSettlement, "photo/sl2.jpg", 1000, 800);
    loadimage(&this->bgGame, "photo/ax1.png", 25, 25);
    loadimage(&this->bgGame, "photo/gj1.png", 25, 25);
   
}

void GameRes::Free() {
    delete this;
}

// 游戏初始化（窗口、资源、变量初始值）
void GameInit(GameRes* picture)
{
    // 创建图形窗口
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    setbkcolor(WHITE);
    cleardevice();

    // 初始化随机数种子
    srand((unsigned)time(NULL));

	// 加载游戏资源
	picture->Load();

    // 初始化游戏状态
    g_isRun = true;
    g_isPause = false;
    g_isGameOver = false;
    g_isWin = false;
    g_curUI = START;
    g_hasFinalBoss = false;

    // 玩家初始化
    g_player.Init();

    // 清空子弹和怪物
    g_bullets.clear();
    g_monsters.clear();

    // 控制怪物多久出一个
    g_spawnRate = 60;
    g_spawnTimer = 0;
}

// 重置游戏数据（重新开始一局）
void GameReset()
{
    // 重置玩家状态
    g_player.Reset();

    // 清空所有子弹和怪物
    g_bullets.clear();
    g_monsters.clear();

    // 游戏状态恢复初始
    g_isGameOver = false;
    g_isWin = false;
    g_isPause = false;
    g_curUI = PLAY;
    g_hasFinalBoss = false;
    g_spawnTimer = 0;
}

// 输入更新（键盘+鼠标消息处理）
void InputUpdate()
{
    ExMessage msg;
    while (peekmessage(&msg, EX_MOUSE | EX_KEY))
    {
        // 鼠标点击
        if (msg.message == WM_LBUTTONDOWN)
        {
            int mx = msg.x;
            int my = msg.y;

            if (g_curUI == START)
            {
                // 开始游戏 (600,280, 130,50)
                if (mx >= 600 && mx <= 730 && my >= 280 && my <= 330) {
                    GameReset();
                    g_curUI = PLAY;
                }
                // 玩法介绍
                else if (mx >= 600 && mx <= 730 && my >= 340 && my <= 390) {
                    g_curUI = HELP;
                }
                // 退出游戏
                else if (mx >= 600 && mx <= 730 && my >= 520 && my <= 570) {
                    g_isRun = false;
                }
            }
            else if (g_curUI == PAUSE)
            {
                // 继续游戏
                if (mx >= 450 && mx <= 550 && my >= 360 && my <= 410) {
                    g_isPause = false;
                    g_curUI = PLAY;
                }
                // 重新开始
                else if (mx >= 450 && mx <= 550 && my >= 200 && my <= 250) {
                    GameReset();
                }
                // 返回菜单
                else if (mx >= 450 && mx <= 550 && my >= 280 && my <= 330) {
                    g_curUI = START;
                }
            }
            else if (g_curUI == SETTLEMENT)
            {
                // 重新开始
                if (mx >= 280 && mx <= 380 && my >= 450 && my <= 500) {
                    GameReset();
                }
                // 返回菜单
                else if (mx >= 680 && mx <= 780 && my >= 450 && my <= 500) {
                    g_curUI = START;
                }
            }
            // 帮助/设置/团队 点一下返回
            else if (g_curUI == HELP || g_curUI == SETTING || g_curUI == TEAM)
            {
                g_curUI = START;
            }
        }
        // 键盘按键
        if (msg.message == WM_KEYDOWN)
        {
            if (msg.vkcode == VK_ESCAPE && g_curUI == PLAY) //ESC 暂停
            {
                g_isPause = true;
                g_curUI = PAUSE;
            }
        }
    }

    // ----------------------
    // 持续按键：WASD 移动
    // ----------------------
    if (g_curUI == PLAY && !g_isPause)
    {
        g_player.Move(); // 内部用 GetAsyncKeyState
    }
}

bool CheckButtonClick(Button& btn) {
    ExMessage msg;
    if (peekmessage(&msg, EX_MOUSE))
    {
        // 检测鼠标左键按下
        if (msg.message == WM_LBUTTONDOWN)
        {
            // 判断鼠标坐标是否在按钮区域内
            if (msg.x >= btn.x && msg.x <= btn.x + btn.w &&
                msg.y >= btn.y && msg.y <= btn.y + btn.h)
            {
                return true;  // 点击了按钮
            }
        }
    }
    return false;
}

void DrawButton(Button& btn, const char* text)
{

    POINT cursor;
    GetCursorPos(&cursor);
    HWND hwnd = GetHWnd();
    ScreenToClient(hwnd, &cursor);
    bool isHover = (cursor.x >= btn.x && cursor.x <= btn.x + btn.w &&
        cursor.y >= btn.y && cursor.y <= btn.y + btn.h);


    COLORREF bgColor, textColor;
    if (isHover)
    {
        bgColor = RGB(80, 80, 80);
        textColor = RGB(255, 255, 0);
    }
    else
    {
        bgColor = RGB(50, 50, 50);
        textColor = RGB(255, 255, 255);
    }


    setfillcolor(bgColor);
    setlinecolor(RGB(200, 200, 200));
    solidrectangle(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h);
    rectangle(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h);


    setbkmode(TRANSPARENT);
    settextstyle(24, 0, "微软雅黑");
    settextcolor(textColor);


    int textW = textwidth(text);
    int textH = textheight(text);
    int textX = btn.x + (btn.w - textW) / 2;
    int textY = btn.y + (btn.h - textH) / 2;
    outtextxy(textX, textY, text);
    
    
}
//用于开始界面的功能图形绘制
void functionalshape(int rx, int ry, int rw, int rh, std::string s) {
    setfillcolor(0X000000);
    solidrectangle(rx, ry, rx + rw, ry + rh);
    setbkmode(TRANSPARENT);
    settextstyle(30, 10, "微软雅黑");
    settextcolor(0XFFFFFF);
    outtextxy(rx + (rw - textwidth(s.c_str())) / 2, ry + (rh - textheight(s.c_str())) / 2, s.c_str());
}
void DrawStartUI(GameRes* picture) {


    
    putimage(0, 0, &picture->bgStart);

    int rx, ry[5], rh, rw, i;
    btnStart.x = btnHelp.x= btnTeam.x= btnSetting.x= btnExit.x=600;
    btnStart.y = 280;
    btnStart.w = btnHelp.w= btnTeam.w= btnSetting.w= btnExit.w=130;
    btnStart.h = btnHelp.h= btnTeam.h= btnSetting.h= btnExit.h=50;
    btnHelp.y = btnStart.y + 10 + btnStart.h;
    btnTeam.y = btnHelp.y + 10 + btnHelp.h;
    btnSetting.y = btnTeam.y + btnTeam.h + 10;
    btnExit.y = btnSetting.y + btnSetting.h + 10;
    //按钮图形绘制
    functionalshape(btnStart.x, btnStart.y, btnStart.w, btnStart.h, "开始游戏");
    functionalshape(btnHelp.x, btnHelp.y, btnHelp.w, btnHelp.h, "玩法介绍");
    functionalshape(btnTeam.x, btnTeam.y, btnTeam.w, btnTeam.h, "团队介绍");
    functionalshape(btnSetting.x, btnSetting.y, btnSetting.w, btnSetting.h, "游戏设置");
    functionalshape(btnExit.x, btnExit.y, btnExit.w, btnExit.h, "退出游戏");
    setbkmode(TRANSPARENT);
    settextstyle(50, 30, "隶书");
    settextcolor(0X000000);
    char s[50] = "追上我把命都给你！";
    outtextxy((1000 - textwidth(s)) / 2, 30, s);
    setfillcolor(0XFFFFFF);


   
   
}
//用于玩法介绍界面文字绘制
void drawtext(int x, int y, std::string s) {
    setbkmode(TRANSPARENT);
    settextstyle(20, 0, "楷书");
    settextcolor(0XFFFFFF);
    outtextxy(x, y, s.c_str());
}
void DrawHelpUI(GameRes* picture) {
    //绘制字体和背景、颜色
    putimage(0, 0, &picture->bgHelp);
    setbkmode(TRANSPARENT);
    settextstyle(25, 0, "微软雅黑");
    settextcolor(0XFFFFFF);
    //绘制介绍内容
    drawtext(0, 30, "背景：");
    drawtext(46, 30, "一名被世人嘲笑却不做回应，默默用实力证明的坤坤，一生与篮球为伴，殊不知他却只有一次证明机会，");
    drawtext(46, 60, "失败了（死亡）将彻底坠入深渊，一切都得从头开始，重新面对充满未知的挑战");
    drawtext(0, 90, "坤坤：");
    drawtext(46, 90, "初始血量100点，每升1级提高最大生命值10点，并且回复10点生命值。");
    drawtext(46, 120, "每级50点经验，每5级召唤小boss，20级召唤大boss。");
    drawtext(46, 150, "坤坤攻击方式为向鼠标方向射出子弹，攻击间隔为0.1秒，伤害为1点，每提升一级增加1点伤害。");

    drawtext(0, 180, "小怪物:");
    drawtext(80, 180, "血量5点 ,伤害2点");
    drawtext(0, 210, "小BOSS:");
    drawtext(80, 210, "200点血量，伤害10点，每隔一定时间召唤两个小怪物在小boss左右两边");
    drawtext(0, 240, "大BOSS:");
    drawtext(80, 240, "913点血量伤害100点，会向玩家发射子弹，与玩家碰撞，减少玩家血量，玩家获得短暂无敌");

    //绘制返回菜单
    btnExit.x = 900;
    btnExit.y= 650;
    btnExit.w = 100;
    btnExit.h = 50;  
     setfillcolor(0XFFFFFF);
    solidrectangle(btnExit.x, btnExit.y, btnExit.x+btnExit.w, btnExit.y + btnExit.h);
    setbkmode(TRANSPARENT);
    settextstyle(30, 10, "微软雅黑");
    settextcolor(0X000000);
    std::string s = "返回菜单";
    outtextxy(btnExit.x + (btnExit.w - textwidth(s.c_str())) / 2, btnExit.y + (btnExit.h - textheight(s.c_str())) / 2, s.c_str());
}

void DrawSettingUI() {

}

void DrawTeamUI() {

}

void DrawPauseUI(GameRes* picture) {
  
    putimage(0, 0, &picture->bgPause);
    setbkmode(TRANSPARENT);
    settextstyle(35, 20, "隶书");
    settextcolor(0X000000);
    char s[50] = "哎呦！你干嘛！";
    outtextxy((1000 - textwidth(s)) / 2, 30, s);

    btnBack.x = btnRestart.x = btnResume.x = 450;
    btnBack.h = btnRestart.h = btnResume.h = 50;
    btnBack.w = btnRestart.w = btnResume.w = 100;
    btnRestart.y = 200;
    btnBack.y = 280;
    btnResume.y = 360;
    //	绘制重新开始
    functionalshape(btnRestart.x, btnRestart.y, btnRestart.w, btnRestart.h, "重新开始");
    //绘制返回菜单
    functionalshape(btnBack.x, btnBack.y, btnBack.w, btnBack.h, "返回菜单");
    //	//绘制继续游戏
    functionalshape(btnResume.x, btnResume.y, btnResume.w, btnResume.h,  "继续游戏");
    
}


void DrawSettlementUI(GameRes* picture, Player* player) {
   //失败界面
    if (g_isGameOver == true) {
      //绘制失败文字
        putimage(0, 0, &picture->bgSettlement);
        setbkmode(TRANSPARENT);
        settextstyle(65, 50, "隶书");
        settextcolor(0XFFFFFF);
        char s[50] = "蔡就多练!";
        outtextxy((1000 - textwidth(s)) / 2, 30, s);
        settextstyle(50, 0, "隶书");
        //绘制结算等级
        std::string s1 = "等级:";
        outtextxy(0, 70, s1.c_str());
        char s2[50];
        sprintf_s(s2, 50, "%d", player->level);
        outtextxy(textwidth(s1.c_str()) + 4, 70, s2);

        //绘制结算分数
        std::string s3 = "分数:";
        outtextxy(0, 115, s3.c_str());
        char s4[50];
        sprintf_s(s4, 50, "%d",player->score);
        outtextxy(textwidth(s3.c_str()) + 4, 120, s4);

        //绘制重新开始
        btnRestart.y = btnBack.y=450;
        btnRestart.w = btnBack.w=100;
        btnRestart.h = btnBack.h= 50;
        btnRestart.x = 280;
        btnBack.x = 680;
        functionalshape(btnRestart.x, btnRestart.y, btnRestart.w, btnRestart.h, "重新开始");
        //绘制返回菜单
        functionalshape(btnBack.x, btnBack.y, btnBack.w, btnBack.h, "返回菜单");
    }

    //绘制胜利界面
    if (g_isWin==true) {
        //绘制胜利文字
        putimage(0, 0, &picture->bgSettlement);
        setbkmode(TRANSPARENT);
        settextstyle(35, 20, "隶书");
        settextcolor(0XFFFFFF);
        char s[50] = "恭喜练习时长连年半的你，征服了所有黑粉！";
        outtextxy((1000 - textwidth(s)) / 2, 30, s);
        char s11[50] = "别问为什么，问就是只因你太美！";
        outtextxy((1000 - textwidth(s11)) / 2, 60, s11);

        settextstyle(50, 0, "隶书");
        //绘制结算等级
        std::string s1 = "等级:";
        outtextxy(0, 70, s1.c_str());
        char s2[50];
        sprintf_s(s2, 50, "%d", player->level);
        outtextxy(textwidth(s1.c_str()) + 4, 70, s2);

        //绘制结算分数
        std::string s3 = "分数:";
        outtextxy(0, 115, s3.c_str());
        char s4[50];
        sprintf_s(s4, 50, "%d", player->score);
        outtextxy(textwidth(s3.c_str()) + 4, 120, s4);

        //绘制重新开始
        btnRestart.y = btnBack.y = 450;
        btnRestart.w = btnBack.w = 100;
        btnRestart.h = btnBack.h = 50;
        btnRestart.x = 280;
        btnBack.x = 680;
        functionalshape(btnRestart.x, btnRestart.y, btnRestart.w, btnRestart.h, "重新开始");
        //绘制返回菜单
        functionalshape(btnBack.x, btnBack.y, btnBack.w, btnBack.h, "返回菜单");
    }
   
}

// 游戏逻辑每帧更新
void GameUpdate()
{
    // 怪物生成计时
    g_spawnTimer++;
    if (g_spawnTimer >= g_spawnRate)
    {
        SpawnMonster();
        g_spawnTimer = 0;
    }


    // 更新所有子弹状态
    UpdateBullets();

    // 更新所有怪物的移动与行为
    UpdateMonsters();

    // 子弹与怪物的碰撞检测
    Collide_BulletMonster();

    // 玩家与怪物的碰撞检测
    Collide_PlayerMonster();

    // 检测玩家是否升级
    CheckLevelUp();

    // 更新玩家无敌帧状态
    UpdateInvincible();

    // 检测游戏是否结束（胜利/失败）
    CheckGameEnd();
}

void SpawnMonster() {
    Monster monster;
    monster.RandomSpawn();         // 随机位置和尺寸
    monster.type = MONSTER;
    monster.active = true;

    monster.maxHp = 5;             // 血量5点
    monster.hp = monster.maxHp;
    monster.speed = MONSTER_SPEED;
    monster.expDrop = 5;           // 掉落经验
    monster.score = 10;            // 击杀得分

    g_monsters.push_back(monster);
}

void SpawnMiniBoss() {
    Monster miniBoss;
    miniBoss.RandomSpawn();        // 随机位置和尺寸
    miniBoss.type = MINI_BOSS;
    miniBoss.active = true;

    miniBoss.maxHp = 200;          // 血量200点
    miniBoss.hp = miniBoss.maxHp;
    miniBoss.speed = MONSTER_SPEED;
    miniBoss.expDrop = 50;         // 掉落经验
    miniBoss.score = 200;          // 击杀得分

    g_monsters.push_back(miniBoss);
}

void SpawnFinalBoss() {
    Monster finalBoss;
    finalBoss.RandomSpawn();       // 随机位置和尺寸
    finalBoss.type = FINAL_BOSS;
    finalBoss.active = true;

    finalBoss.maxHp = 913;         // 血量913点
    finalBoss.hp = finalBoss.maxHp;
    finalBoss.speed = MONSTER_SPEED;
    finalBoss.expDrop = 500;       // 掉落经验
    finalBoss.score = 1000;        // 击杀得分

    g_monsters.push_back(finalBoss);
    g_hasFinalBoss = true;
}

void UpdateBullets() {
    for (auto i = g_bullets.begin(); i != g_bullets.end(); i++) {
        while (!i->CheckBorder()&&i->active) {
            //只要子弹存在并且不出界就更新坐标
            if (i->flag == 0)i->P_Move();//判断子弹类型
            else i->M_Move();
        }
        //出界后销毁
        i->active = false;
        g_bullets.erase(i);//在vector中删除子弹对象i
    }
}

void UpdateMonsters() {

}

void Collide_BulletMonster() {
    for (auto B = g_bullets.begin(); B != g_bullets.end(); B++) {
        //玩家子弹
        if (B->flag == 0) {
            for (auto M = g_monsters.begin(); M != g_monsters.end(); M++) {
                //判断子弹和怪物的贴图是否有重合
                if ((M->x-B->w<=B->x||M->x+M->w<=B->x)&&(M->y-B->h<=B->x||M->y+M->h<=B->y))
                {
                    M->TakeDamage(B->atk, g_player);//怪物受到子弹攻击
                    B->active = false;//子弹销毁
                    g_bullets.erase(B);
                }
            }
        }
        //怪物子弹 
        //判断子弹和怪物的贴图是否有重合
        else if((g_player.x - B->w <= B->x || g_player.x + g_player.w <= B->x) 
            && (g_player.y - B->h <= B->x || g_player.y + g_player.h <= B->y))
        {
            g_player.TakeDamage(B->atk);//玩家受到子弹伤害
            B->active = false;//子弹销毁
            g_bullets.erase(B);
        }
    }
}

void Collide_PlayerMonster() {

}

void CheckLevelUp() {
    if(g_player.exp >= g_player.expNeed)
    {
        g_player.LevelUp();
    }
}

void UpdateInvincible() {

}

// 检测游戏结束条件（胜利/失败）
void CheckGameEnd()
{
    // 玩家血量小于等于0时游戏失败
    if (g_player.hp <= 0)
    {
        g_isGameOver = true;
        g_isWin = false;
        g_curUI = SETTLEMENT;
    }

    // 玩家等级达到最大BOSS等级，并且大BOSS也被打死时游戏胜利
    if (g_player.level >= FINAL_BOSS_LV && g_hasFinalBoss)
    {
        bool bossAlive = false;

        for (int i = 0; i < g_monsters.size(); i++)
        {
            if (g_monsters[i].type == FINAL_BOSS && g_monsters[i].active)
            {
                bossAlive = true;
                break;
            }
        }

        if (bossAlive == false)
        {
            g_isGameOver = true;
            g_isWin = true;
            g_curUI = SETTLEMENT;
        }
    }
}

void DrawGameUI(GameRes* picture, Player* player) {

    putimage(0, 0, &picture->bgGame);
    DrawPlayerInfo(picture, player);
	DrawEntities(player, picture);
    //暂停按钮绘制
    btnPause.x = 920;
    btnPause.y = 620;
    btnPause.w = 80;
    btnPause.h = 80;
    functionalshape(btnPause.x, btnPause.y, btnPause.w, btnPause.h, "暂停游戏");

}

void DrawPlayerInfo(GameRes* picture, Player* player) {
    //设置字体大小和格式
    settextstyle(25, 0, "微软雅黑");
    setbkmode(TRANSPARENT);
    settextcolor(0X000000);
    //绘制玩家等级
    std::string s1 = "等级:";
    outtextxy(0, 70, s1.c_str());

    char s2[50];
    sprintf_s(s2, 50, "%d", player->level);
    outtextxy(textwidth(s1.c_str()) + 4, 70, s2);

    //绘制玩家分数
    std::string s3 = "分数:";
    outtextxy(0, 95, s3.c_str());
    char s4[50];
    sprintf_s(s4, 50, "%d", player->score);
    outtextxy(textwidth(s3.c_str()) + 4, 95, s4);
    setfillcolor(0XE2961B);
    solidrectangle(70, 78, 100, 88);
    //绘制玩家血量
    setfillcolor(RED);
    solidrectangle(28, 15, 1 + player->hp, 25);
    //绘制玩家攻击力
    setfillcolor(0X46CEFF);
    solidrectangle(28, 45, player->atk + 28, 60);
}

// 绘制怪物血条
void DrawMonsterHPBar(Monster& monster)
{
    int barW = monster.w;              // 血条宽度 = 怪物宽度
    int barH = 6;                      // 血条高度
    int barX = monster.x;              // 血条X = 怪物X
    int barY = monster.y - barH - 4;   // 血条在怪物头顶上方

    // 血条背景（深灰）
    setfillcolor(RGB(80, 80, 80));
    solidrectangle(barX, barY, barX + barW, barY + barH);

    // 血条前景（根据血量比例染色：绿->黄->红）
    float ratio = (float)monster.hp / (float)monster.maxHp;
    COLORREF hpColor = (ratio > 0.5f) ? RGB(0, 200, 0)
                     : (ratio > 0.25f) ? RGB(220, 200, 0)
                     : RGB(220, 0, 0);
    setfillcolor(hpColor);
    solidrectangle(barX, barY, barX + (int)(barW * ratio), barY + barH);

    // 血条边框
    setlinecolor(RGB(255, 255, 255));
    rectangle(barX, barY, barX + barW, barY + barH);
}

// 绘制所有实体（玩家、怪物、子弹）
void DrawEntities(Player* player, GameRes* picture)
{
   //  1. 绘制玩家 
    if (player->isInvincible)
    {
        // 无敌状态：半透明绘制
        putimagePNG(&picture->imgPlayer,
            player->x, player->y,
            0, 0,
            (int)picture->imgPlayer.getwidth(),
            (int)picture->imgPlayer.getheight(),
            0.5);
    }
    else
    {
        putimagePNG(&picture->imgPlayer, player->x, player->y);
    }

    //  2. 绘制子弹
    for (int i = 0; i < (int)g_bullets.size(); i++)
    {
        Bullet& bullet = g_bullets[i];
        if (!bullet.active) continue;

        putimagePNG(&picture->imgBullet, bullet.x, bullet.y,
            0, 0,
            (int)picture->imgBullet.getwidth(),
            (int)picture->imgBullet.getheight());
    }

    //  3. 绘制怪物 / BOSS  
    for (int i = 0; i < (int)g_monsters.size(); i++)
    {
        Monster& monster = g_monsters[i];
        if (!monster.active) continue;

        IMAGE* imgToDraw = nullptr;

        switch (monster.type)
        {
        case MONSTER:
            imgToDraw = &picture->imgMonster;
            break;
        case MINI_BOSS:
            imgToDraw = &picture->imgMiniBoss;
            break;
        case FINAL_BOSS:
            imgToDraw = &picture->imgFinalBoss;
            break;
        default:
            imgToDraw = &picture->imgMonster;
            break;
        }

        // 绘制怪物贴图
        putimagePNG(imgToDraw, monster.x, monster.y,
            0, 0,
            (int)imgToDraw->getwidth(),
            (int)imgToDraw->getheight());

        // 绘制血条
        DrawMonsterHPBar(monster);
    }
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