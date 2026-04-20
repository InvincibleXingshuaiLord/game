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
    void ShootMonsterBullet();//怪物攻击，发射子弹
    void TakeDamage(int dmg); //怪物受伤处理
    void OnDead();      //怪物死亡处理
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
    void P_Move();                      //玩家子弹移动
    void M_Move(Monster& bigboss);       //大boss子弹移动
    void TrackPlayer(Player& player);      //boss子弹追击玩家
    bool CheckBorder(); //检测子弹是否出界，出界则销毁
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

// 开始界面-开始游戏按钮(开始界面定义了每个功能图形（正矩形）的坐标变量，可根据坐标编写按钮功能，简单很多)
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

//用于开始界面的功能图形绘制
void functionalshape(int rx, int ry, int rw, int rh, std::string s);

int main()
{
    //对象定义区
	GameRes* picture = new GameRes();

    //对象定义结束
	srand((unsigned)time(NULL));
    //初始化游戏
    GameInit(picture);

    //游戏主循环
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

void Bullet::Init(int px, int py) {
    this->x = px, this->y = py;//更新子弹初始坐标
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

void Bullet::M_Move(Monster& bigboss) {
    this->atk = 100;//初始化怪物子弹伤害
    this->TrackPlayer(g_player);//子弹追击玩家 
}

void Bullet::TrackPlayer(Player& player) {//完全照搬怪物追击玩家的函数
    while (abs(player.x - this->x) > 1 && abs(player.y - this->y > 1)) {
        int dx = player.x - this->x;
        int dy = player.y - this->y;
        double distance = sqrt(dx * dx + dy * dy);
        this->x += (dx / distance) * this->speed;
        this->y += (dy / distance) * this->speed;
    }
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
        if (distance < 1.0) {
        return;
    }
        double distance = sqrt(dx * dx + dy * dy);
        x += (dx / distance) * speed;
        y += (dy / distance) * speed;
}

void Monster::ShootMonsterBullet() {

}

void Monster::TakeDamage(int dmg) {
    if (dmg < 100 && dmg>0) {
        hp -= dmg;
    }
    else {
        hp = 0;
    }
    if (hp = 0) {
        OnDead();
    }
}//遇到攻击怪物闪烁 是否需要加
void Monster::OnDead() {
 active = false;
 player.exp += expDrop;
 player.score += score;
}
//静行结束

void GameRes::Load() {
    //
	
	loadimage(&this->imgPlayer, "photo/kun.png", 32, 32);
	loadimage(&this->imgBullet, "photo/bullet.png", 10, 10);
	loadimage(&this->imgMonster, "photo/Xiaoguai.png", 32, 32);
	loadimage(&this->imgMiniBoss, "photo/littleBoss.png", 64, 64);
	loadimage(&this->imgFinalBoss, "photo/BigBoss.png", 128, 128);
	loadimage(&this->bgStart, "photo/kk1.jpg", 1100, 700);
	loadimage(&this->bgHelp, "photo/kk1.jpg", 1100, 700);
	loadimage(&this->bgSetting, "photo/kk1.jpg", 1100, 700);
	loadimage(&this->bgTeam, "photo/kk1.jpg", 1100, 700);
	loadimage(&this->bgGame, "photo/kk1.jpg", 1100, 700);
	loadimage(&this->bgPause, "photo/kk1.jpg", 1100, 700);
	loadimage(&this->bgSettlement, "photo/kk1.jpg", 1100, 700);
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
    // 读取鼠标和键盘消息
    peekmessage(&msg, EX_MOUSE | EX_KEY);

    // 只有在玩游戏、没暂停的时候输入
    if (g_curUI == PLAY && !g_isPause)
    {
        // 玩家移动
        g_player.Move();

        // 鼠标左键点击，玩家发射子弹
        if (msg.message == WM_LBUTTONDOWN)
        {
            g_player.Attack();
        }

        // 按ESC键暂停
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
        {
            g_isPause = true;
            g_curUI = PAUSE;
            Sleep(200);
        }
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
void DrawStartUI() {

    initgraph(1000, 700);

    IMAGE img;
    loadimage(&img, "photo/kk1.jpg", 1100, 700);//开始界面壁纸
    putimage(0, 0, &img);

    int rx, ry[5], rh, rw, i;
    rx = 600;
    ry[0] = 280;
    rw = 130;
    rh = 50;
    for (i = 1; i < 5; i++) {
        ry[i] = ry[i - 1] + rh + 10;
    }
    //按钮图形绘制
    functionalshape(rx, ry[0], rw, rh, "开始游戏");
    functionalshape(rx, ry[1], rw, rh, "玩法介绍");
    functionalshape(rx, ry[2], rw, rh, "团队介绍");
    functionalshape(rx, ry[3], rw, rh, "游戏设置");
    functionalshape(rx, ry[4], rw, rh, "退出游戏");
    setbkmode(TRANSPARENT);
    settextstyle(50, 30, "隶书");
    settextcolor(0X000000);
    char s[50] = "追上我把命都给你！";
    outtextxy((1000 - textwidth(s)) / 2, 30, s);
    getchar();
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

}

void UpdateMonsters() {

}

void Collide_BulletMonster() {

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