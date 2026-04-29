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
//��ȡȫ����Ϣ
ExMessage msg = { 0 };


//��Ϸ֡��
#define FPS              60

//��ҳ�ʼ����
#define PLAYER_INIT_HP   100
#define PLAYER_INIT_ATK  1
//speed����֡��ʵ���ٶ�
#define PLAYER_SPEED     0.5
#define BULLET_SPEED     2
#define MONSTER_SPEED    0.5

//�������辭��ֵ
#define EXP_PER_LEVEL    20

//ÿ�����ټ�����һ��СBOSS
#define MINI_BOSS_LV     5

//���ټ����ɴ�BOSS
#define FINAL_BOSS_LV    20

//������˺��޵�֡�����룩
#define INVINCIBLE_TIME  800

//��Ϸ����ö�٣���ʶ��ǰ�����ĸ�����
enum GameUI
{
    START,        //��ʼ����
    HELP,         //�淨���ܽ���
    SETTING,      //���ý���
    TEAM,         //�Ŷӽ��ܽ���
    PLAY,         //��Ϸ����������
    PAUSE,        //��Ϸ��ͣ����
    SETTLEMENT    //��Ϸ������棨ʤ��/ʧ�ܣ�
};

//ʵ������ö�٣�������ҡ�����ӵ���BOSS�ȶ���
enum EntityType
{
    PLAYER,      //���
    MONSTER,     //��ͨ����
    MINI_BOSS,   //СBOSS
    FINAL_BOSS,  //��BOSS
    BULLET       //�ӵ�
};


class Button;
class Player;
class Monster;
class Bullet;
class GameRes;
class Bloodbag;

// ��ť��
class Button
{
public:
    int x;              // ��ť���Ͻ�X����
    int y;              // ��ť���Ͻ�Y����
    int w;              // ��ť����
    int h;              // ��ť�߶�
};

//�����
class Player
{
public:
    int x;              //���X����
    int y;              //���Y����
    int w;              //�����ͼ����
    int h;              //�����ͼ�߶�

    int hp;             //��ǰ����ֵ
    int maxHp;          //�������ֵ
    int atk;            //������
    double moveSpeed;      //�ƶ��ٶ�

    int level;          //��ǰ�ȼ�
    int exp;            //��ǰ����ֵ
    int expNeed;        //�������辭��
    int score;          //��ǰ��Ϸ����

    bool isInvincible;  //�Ƿ����޵�״̬
    DWORD invincibleTimer; //�޵�֡��ʱʱ��

public:
    Player();           //���캯��
    void Init();        //���Գ�ʼ��
    void Reset();       //�������״̬
    void Move();        //����ƶ�
    void LimitBorder(); //�����������Ļ�߽���
    void Attack();      //��ҹ����������ӵ�
    void TakeDamage(int dmg); //������˴���
    void LevelUp();     //�����������
};

//������
class Monster
{
public:
    int x;              //����X����
    int y;              //����Y����
    int w;              //������ͼ����
    int h;              //������ͼ�߶�

    int hp;             //��ǰ����ֵ
    int maxHp;          //�������ֵ
    double speed;          //�ƶ��ٶ�
    int expDrop;        //�������侭��ֵ
    int score;          //��ɱ��÷���

    bool active;        //�����Ƿ���
    EntityType type;    //�������ͣ���ͨ/BOSS��

public:
    Monster();          //���캯��
    void RandomSpawn(); //���λ�����ɹ���
    void TrackPlayer(Player& player); //�Զ�׷�����
    void ShootMonsterBullet(Player& player);//���﹥���������ӵ�
    void TakeDamage(int dmg, Player& player); //�������˴���
    void OnDead(Player& player);      //������������
};


//�ӵ���
class Bullet
{
public:
    double x;              //�ӵ�X����
    double y;              //�ӵ�Y����
    int w;              //�ӵ���ͼ����
    int h;              //�ӵ���ͼ�߶�
    double speed;          //�ӵ��ƶ��ٶ�
    int atk;            //�ӵ�������
    int flag;           //�ӵ�����  0�����  1������
    double mx;
    double my;
    bool active;        //�ӵ��Ƿ����

public:
    Bullet();                                 //���캯��
    void Init(double px, double py);                     //��ʼ���ӵ�λ�ú����ͣ����
    void Init(double px, double py, double pmx, double pmy);      //��ʼ���ӵ�λ�ú����ͣ���ң�
    void P_Move();                            //����ӵ��ƶ�
    void M_Move();                            //��boss�ӵ��ƶ�
    void TrackPlayer(Player& player);         //boss�ӵ�׷�����
    bool CheckBorder();                       //����ӵ��Ƿ���磬����������
};


//ͼƬ��
class GameRes
{
public:
    IMAGE imgPlayer;         //���ͼƬ
    IMAGE imgBullet;         //�ӵ�ͼƬ
    IMAGE imgMonster;        //��ͨ����ͼƬ
    IMAGE imgMiniBoss;       //СBOSSͼƬ
    IMAGE imgFinalBoss;      //��BOSSͼƬ

    IMAGE bgStart;           //��ʼ����
    IMAGE bgHelp;            //�淨����
    IMAGE bgSetting;         //���ý���
    IMAGE bgTeam;            //�Ŷӽ���
    IMAGE bgGame;            //��Ϸ����
    IMAGE bgPause;           //��ͣ����
    IMAGE bgWin;            //ʤ������
    IMAGE bgLose;           //ʧ�ܽ���
    IMAGE imgAx;      // ��Ϸ�������Ѫ��ͼ��
    IMAGE imgGj;      // ��Ϸ������ҹ�����ͼ��
public:
    void Load();     //��������ͼƬ
    void Free();     //�ͷ�ͼƬ
};

//Ѫ����
class Bloodbag {
public:
    double x;               //����
    double y;
    double w;
    double h;
    int flag;               //СѪ����0     ��Ѫ����1
    bool active;            //����״̬
public:
    Bloodbag();
    void Init(double bx, double by, int bflag);
    void Recover();
};

// ��ǰ��ʾ����Ϸ����
GameUI     g_curUI;

// ��Ϸ��ѭ���Ƿ�����
bool       g_isRun;

// ��Ϸ�Ƿ���ͣ
bool       g_isPause;

// ��Ϸ�Ƿ����
bool       g_isGameOver;

// ����Ƿ�ʤ��
bool       g_isWin;

// ȫ����Ҷ���
Player     g_player;

// ȫ���ӵ��б������������ӵ�
std::vector<Bullet>  g_bullets;

// ȫ�ֹ����б����������й���
std::vector<Monster> g_monsters;

//ȫ��Ѫ���б�����������Ѫ��
std::vector<Bloodbag>g_bloodbag;

//���Ѫ��װ����
std::vector<Bloodbag>g_pb;


// ȫ����Դ���󣬹���������ͼ
GameRes    g_res;

// ��������Ƶ�ʣ�֡�����
int        g_spawnRate;

// �������ɼ�ʱ��
int        g_spawnTimer;

// �Ƿ��Ѿ���������BOSS
bool       g_hasFinalBoss;

// ��ʼ����-��ʼ��Ϸ��ť
Button     btnStart;

// ��ʼ����-�淨���ܰ�ť
Button     btnHelp;

// ��ʼ����-���ð�ť
Button     btnSetting;

// ���ý���-�Ŷӽ��ܰ�ť
Button     btnTeam;

// ���ӽ���-���ذ�ť
Button     btnBack;

// ��Ϸ�н���-��ͣ��ť��С�����䣩
Button btnPause;

// ��ͣ����-������Ϸ��ť
Button     btnResume;

// ����/��ͣ����-���¿�ʼ��ť
Button     btnRestart;

// ͨ���˳���ť
Button     btnExit;

// ��Ϸ��ʼ�������ڡ���Դ��������ʼֵ��
void GameInit();

// ������Ϸ���ݣ����¿�ʼһ�֣�
void GameReset();

// ������£�����+�����Ϣ������
void InputUpdate();

// �������Ƿ���ָ����ť
bool CheckButtonClick(Button& btn);

// ���ư�ť������+����+����Ч����
void DrawButton(Button& btn, const char* text);

// ���ƿ�ʼ����
void DrawStartUI();

// �����淨���ܽ���
void DrawHelpUI();

// �������ý���
void DrawSettingUI();

// �����Ŷӽ��ܽ���
void DrawTeamUI();

// ������ͣ����
void DrawPauseUI();

// ���ƽ������
void DrawSettlementUI();

// ��Ϸ�߼�ÿ֡����
void GameUpdate();

// ���������ͨ����
void SpawnMonster();

// ����СBOSS
void SpawnMiniBoss();

// ���ɴ�BOSS
void SpawnFinalBoss();

// ���������ӵ��߼����ƶ���Խ�����٣�
void UpdateBullets();

// �������й����߼����ƶ���׷�٣�
void UpdateMonsters();

//��������Ѫ��
void UpdataBloodbags();

// ��ײ��⣺�ӵ������
void Collide_BulletMonster();

// ��ײ��⣺��������
void Collide_PlayerMonster();

// ����Ƿ�������������
void CheckLevelUp();

//���Ѫ��״̬�������Ѫ��
void Collide_Bloodbag();

// �����޵�֡״̬
void UpdateInvincible();

// �����Ϸ����������ʤ��/ʧ�ܣ�
void CheckGameEnd();

// ������Ϸ���棨������ʵ�塢UI��
void DrawGameUI();

// �������UI��Ѫ�������������ȼ���������
void DrawPlayerInfo();
// ��������ʵ�壨��ҡ�����ӵ���
void DrawEntities();

// ���ƹ���Ѫ��
void DrawMonsterHPBar(Monster& monster);

//���ڿ�ʼ����Ĺ���ͼ�λ���
void functionalshape(int rx, int ry, int rw, int rh, std::string s);
//�����淨���ܽ������ֻ���
void drawtext(int x, int y, std::string s);

//Ѫ����������������
void Probability();

int main()
{

    //��������
    //���������

    srand((unsigned)time(NULL));
    //��ʼ����Ϸ
    GameInit();
    //��ʼ������ͼ��������˸
    BeginBatchDraw();
    //��Ϸ��ѭ��
    while (g_isRun)
    {
        //�������ͼ���
        InputUpdate();

        cleardevice();

        //���Ƶ�ǰ����
        switch (g_curUI)
        {
        case START:      DrawStartUI();     break;
        case HELP:       DrawHelpUI();      break;
        case SETTING:    DrawSettingUI();   break;
        case TEAM:       DrawTeamUI();      break;
        case PLAY:
            //����ͣ״̬�¼�����Ϸ
            if (!g_isPause)
                GameUpdate();
            //������Ϸ����
            DrawGameUI();
            break;
        case PAUSE:      DrawPauseUI();     break;
        case SETTLEMENT: DrawSettlementUI(); break;
        }
        FlushBatchDraw();

    }
    //����������ͼ����ʾ����
    EndBatchDraw();
    g_res.Free();
    closegraph();

    return 0;
}

Player::Player() {
    Init();
}

void Player::Init() {
    w = 50;
    h = 50;
    //��Ļ����
    x = WIN_WIDTH / 2 - w / 2;
    y = WIN_HEIGHT / 2 - h / 2;
    //��������
    hp = PLAYER_INIT_HP;
    maxHp = PLAYER_INIT_HP;
    atk = PLAYER_INIT_ATK;
    moveSpeed = PLAYER_SPEED;
    //�ȼ�����
    level = 1;
    exp = 0;
    expNeed = EXP_PER_LEVEL;
    score = 0;
    //�޵�״̬
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
    //����Ļ���ƶ�
    LimitBorder();
}

void Player::LimitBorder() {
    //��߽�
    if (x < 0) x = 0;
    //�ұ߽�
    if (x + w > WIN_WIDTH) x = WIN_WIDTH - w;
    //�ϱ߽�
    if (y < 0) y = 0;
    //�±߽�
    if (y + h > WIN_HEIGHT) y = WIN_HEIGHT - h;
}

void Player::Attack() {
}

void Player::TakeDamage(int dmg) {
    //�޵в����˺�
    if (isInvincible) return;
    //��Ѫ
    hp -= dmg;
    if (hp < 0) hp = 0;
    //�޵�֡
    isInvincible = true;
    invincibleTimer = GetTickCount();
}

void Player::LevelUp() {
    //�۳��������辭��
    exp -= expNeed;
    //�ȼ�����
    level++;
    //��������ǿ��
    maxHp += 20;
    hp = maxHp;   //������������ֵ
    atk += 1;     //����������
    moveSpeed += 0.15;//�ƶ��ٶ�����
    expNeed += EXP_PER_LEVEL * 1.2;//������һ�����辭��
}

Bullet::Bullet() {
    this->x = 0;
    this->y = 0;
    this->w = 10;
    this->h = 10;
    this->mx = 0;
    this->my = 0;
    this->speed = BULLET_SPEED;
    this->atk = 1;

    this->active = false;
}

void Bullet::Init(double px, double py, double pmx, double pmy) {
    this->x = px, this->y = py;//�����ӵ���ʼ����
    this->flag = 0;//��������ӵ�
    this->mx = pmx, this->my = pmy;//��¼��갴�µ�����
    this->active = true;//�����ӵ�����״̬
}

void Bullet::Init(double px, double py) {
    this->x = px, this->y = py;//�����ӵ���ʼ����
    this->flag = 1;//���ù����ӵ�
    this->active = true;//�����ӵ�����״̬
}

void Bullet::P_Move() {
    this->atk = g_player.atk;//��������ӵ��˺�
    //����
    double dx, dy;
    dx = dy = 0;
    double vx, vy, t, s;
    vx = vy = t = s = 0;
    dx = this->mx - this->x, dy = this->my - this->y;
    s = sqrt(dx * dx + dy * dy);
    t = s / this->speed;
    vx = dx / t; vy = dy / t;//�����ӵ�x��y�ٶ�

    this->x += vx; this->y += vy;//�����ӵ�����
    this->mx += vx; this->my += vy;

}

void Bullet::M_Move() {
    this->atk = 100;//��ʼ�������ӵ��˺�
    this->TrackPlayer(g_player);//�ӵ�׷����� 
}

void Bullet::TrackPlayer(Player& player) {//��ȫ�հ����׷����ҵĺ���
    int dx = player.x - this->x;
    int dy = player.y - this->y;
    double distance = sqrt(dx * dx + dy * dy);
    this->x += (dx / distance) * this->speed;
    this->y += (dy / distance) * this->speed;
    if (distance < 1.0) return;
}

bool Bullet::CheckBorder() {
    if (this->active == false)return true;//�����ʼ����ʱ���ӵ��Ͳ����ڣ�������
    if (this->x <= 0 || this->x + this->w >= getwidth() || this->y <= 0 || this->y + this->h >= getheight())
        return true;//����
    else return false;
}
//���п�ʼ
Monster::Monster() {
    x = 0;
    y = 0;
    w = 64;//��Ҫ��ʱ���ٸ�
    h = 64;//ͬ��
    hp = 100;
    maxHp = 100;
    speed = MONSTER_SPEED;
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
    int maxx = 1000;
    int maxy = 700;
    posx = rand() % (maxx - minx + 1) + minx;
    posy = rand() % (maxy - miny + 1) + miny;
    x = posx;
    y = posy;

}

void Monster::TrackPlayer(Player& player)
{
    int dx = player.x + player.w / 2 - (x + w / 2);
    int dy = player.y + player.h / 2 - (y + h / 2);

    double dist = sqrt((double)dx * (double)dx + (double)dy * (double)dy);
    if (dist < 8.0)
        return;

    double moveX = ((double)dx / dist) * speed;
    double moveY = ((double)dy / dist) * speed;

    // jitter for free movement
    double jitterX = ((rand() % 100) / 100.0 - 0.5) * speed * 0.4;
    double jitterY = ((rand() % 100) / 100.0 - 0.5) * speed * 0.4;
    moveX += jitterX;
    moveY += jitterY;

    x += (int)round(moveX);
    y += (int)round(moveY);

    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x + w > WIN_WIDTH)  x = WIN_WIDTH - w;
    if (y + h > WIN_HEIGHT) y = WIN_HEIGHT - h;
}

void Monster::ShootMonsterBullet(Player& player) {
    if (!active) return;
    Bullet bullet;
    bullet.Init(x, y);
    bullet.speed = 3;
    bullet.atk = 1;
    bullet.active = true;
}

void Monster::TakeDamage(int dmg, Player& player) {
    if (dmg < 100 && dmg>0) {
        hp -= dmg;
    }
    else {
        hp = 0;
    }
    if (hp <= 0) {
        OnDead(player);
    }
}//��������������˸ �Ƿ���Ҫ��
void Monster::OnDead(Player& player) {
    active = false;
    player.exp += expDrop;
    player.score += score;
}
//���н���

void Probability(Monster& monster) {
    if (monster.type == MONSTER)//С��
    {
        Bloodbag bloodbag;
        //5%���ʵ�Ѫ���;����
        if (rand() % 20 == 0) {
            //����СѪ��
            bloodbag.Init(monster.x, monster.y, 0);
            g_bloodbag.push_back(bloodbag);
            //���ɾ����


        }
    }
    else if (monster.type == MINI_BOSS) //Сboss
    {
        Bloodbag bloodbag;
        //90%���ʵ�СѪ��,10%���ʵ���Ѫ��
        if (rand() % 10 != 0) {
            bloodbag.Init(monster.x, monster.y, 0);//СѪ��
        }
        else {
            bloodbag.Init(monster.x, monster.y, 1);//��Ѫ��
        }
        g_bloodbag.push_back(bloodbag);
    }
}

//Ѫ�����Ա����
Bloodbag::Bloodbag() {
    this->x = 0;
    this->y = 0;
    this->w = 10;
    this->h = 10;
    this->flag = 0;
    this->active = false;
}

void Bloodbag::Init(double bx, double by, int bflag) {
    this->x = bx, this->y = by;
    this->active = true;
    this->flag = bflag;
}

void Bloodbag::Recover() {
    if (this->flag == 0) {              //СѪ��
        g_player.hp += g_player.maxHp * 0.3;
    }
    else {                              //��Ѫ��
        g_player.hp += g_player.maxHp * 0.6;
        g_player.maxHp += 10;
    }
    this->active = false;
}

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
    loadimage(&this->bgTeam, "photo/td1.png", 1000, 800);
    loadimage(&this->bgGame, "photo/dt1.jpg", 1000, 700);
    loadimage(&this->bgPause, "photo/zt1.jpg", 1000, 700);
    loadimage(&this->bgWin, "photo/sl2.jpg", 1000, 800);
    loadimage(&this->bgLose, "photo/sb1.jpg", 1000, 800);
    loadimage(&this->imgAx, "photo/ax1.png", 25, 25);
    loadimage(&this->imgGj, "photo/gj2.png", 25, 25);

}

void GameRes::Free() {
    delete this;
}

// ��Ϸ��ʼ�������ڡ���Դ��������ʼֵ��
void GameInit()
{
    // ����ͼ�δ���
    initgraph(WIN_WIDTH, WIN_HEIGHT);
    setbkcolor(WHITE);
    cleardevice();

    // ��ʼ�����������
    srand((unsigned)time(NULL));

    // ������Ϸ��Դ
    g_res.Load();

    // ��ʼ����Ϸ״̬
    g_isRun = true;
    g_isPause = false;
    g_isGameOver = false;
    g_isWin = false;
    g_curUI = START;
    g_hasFinalBoss = false;

    // ��ҳ�ʼ��
    g_player.Init();

    // ����ӵ��͹���
    g_bullets.clear();
    g_monsters.clear();

    // ���ƹ����ó�һ��
    g_spawnRate = 1200;
    g_spawnTimer = 0;
}

// ������Ϸ���ݣ����¿�ʼһ�֣�
void GameReset()
{
    // �������״̬
    g_player.Reset();

    // ��������ӵ��͹���
    g_bullets.clear();
    g_monsters.clear();

    // ��Ϸ״̬�ָ���ʼ
    g_isGameOver = false;
    g_isWin = false;
    g_isPause = false;
    g_curUI = PLAY;
    g_hasFinalBoss = false;
    g_spawnTimer = 0;
}

// ������£�����+�����Ϣ������
void InputUpdate()
{
    // ExMessage msg;

    while (peekmessage(&msg, EM_MOUSE | EM_KEY))
    {
        if (msg.message == WM_KEYDOWN)
        {
            if (msg.vkcode == VK_ESCAPE && g_curUI == PLAY)
            {
                g_isPause = true;
                g_curUI = PAUSE;
            }
        }

        if (msg.message == WM_LBUTTONDOWN)
        {
            int mx = msg.x;
            int my = msg.y;

            if (g_curUI == START)
            {
                if (mx >= 600 && mx <= 730 && my >= 280 && my <= 330)
                {
                    GameReset();
                    g_curUI = PLAY;
                }
                else if (mx >= 600 && mx <= 730 && my >= 340 && my <= 390)
                {
                    g_curUI = HELP;
                }
                else if (mx >= 600 && mx <= 730 && my >= 400 && my <= 450)
                {
                    g_curUI = TEAM;
                }
                else if (mx >= 600 && mx <= 730 && my >= 520 && my <= 570)
                {
                    g_isRun = false;
                }
            }
            else if (g_curUI == PAUSE)
            {
                if (mx >= 450 && mx <= 550 && my >= 360 && my <= 410)
                {
                    g_isPause = false;
                    g_curUI = PLAY;
                }
                else if (mx >= 450 && mx <= 550 && my >= 200 && my <= 250)
                {
                    GameReset();
                }
                else if (mx >= 450 && mx <= 550 && my >= 280 && my <= 330)
                {
                    g_curUI = START;
                }
            }
            else if (g_curUI == SETTLEMENT)
            {
                if (mx >= 280 && mx <= 380 && my >= 450 && my <= 500)
                {
                    GameReset();
                }
                else if (mx >= 680 && mx <= 780 && my >= 450 && my <= 500)
                {
                    g_curUI = START;
                }
            }
            else if (g_curUI == HELP || g_curUI == SETTING || g_curUI == TEAM)
            {
                g_curUI = START;
            }
            else if (g_curUI == PLAY && !g_isPause)
            {
                // ��ȫ�����ӵ�
                Bullet b;
                b.Init(g_player.x + g_player.w / 2, g_player.y + g_player.h / 2, mx, my);
                g_bullets.push_back(b);
            }
        }
    }

    if (g_curUI == PLAY && !g_isPause)
    {
        g_player.Move();
    }
}

bool CheckButtonClick(Button& btn) {
    // ֱ�Ӷ�ȡȫ�� msg�����ظ� peekmessage
    if (msg.message == WM_LBUTTONDOWN)
    {
        // �ж���������Ƿ��ڰ�ť������
        if (msg.x >= btn.x && msg.x <= btn.x + btn.w &&
            msg.y >= btn.y && msg.y <= btn.y + btn.h)
        {
            msg.message = 0;  // ���������գ���ֹ��һ֡�ظ�����
            return true;  // ����˰�ť
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
    settextstyle(24, 0, "΢���ź�");
    settextcolor(textColor);


    int textW = textwidth(text);
    int textH = textheight(text);
    int textX = btn.x + (btn.w - textW) / 2;
    int textY = btn.y + (btn.h - textH) / 2;
    outtextxy(textX, textY, text);


}
//���ڿ�ʼ����Ĺ���ͼ�λ���
void functionalshape(int rx, int ry, int rw, int rh, std::string s) {
    setfillcolor(0X000000);
    solidrectangle(rx, ry, rx + rw, ry + rh);
    setbkmode(TRANSPARENT);
    settextstyle(30, 10, "΢���ź�");
    settextcolor(0XFFFFFF);
    outtextxy(rx + (rw - textwidth(s.c_str())) / 2, ry + (rh - textheight(s.c_str())) / 2, s.c_str());
}
void DrawStartUI() {



    putimage(0, 0, &g_res.bgStart);

    int rx, ry[5], rh, rw, i;
    btnStart.x = btnHelp.x = btnTeam.x = btnSetting.x = btnExit.x = 600;
    btnStart.y = 280;
    btnStart.w = btnHelp.w = btnTeam.w = btnSetting.w = btnExit.w = 130;
    btnStart.h = btnHelp.h = btnTeam.h = btnSetting.h = btnExit.h = 50;
    btnHelp.y = btnStart.y + 10 + btnStart.h;
    btnTeam.y = btnHelp.y + 10 + btnHelp.h;
    btnSetting.y = btnTeam.y + btnTeam.h + 10;
    btnExit.y = btnSetting.y + btnSetting.h + 10;
    //��ťͼ�λ���
    functionalshape(btnStart.x, btnStart.y, btnStart.w, btnStart.h, "��ʼ��Ϸ");
    functionalshape(btnHelp.x, btnHelp.y, btnHelp.w, btnHelp.h, "�淨����");
    functionalshape(btnTeam.x, btnTeam.y, btnTeam.w, btnTeam.h, "�Ŷӽ���");
    functionalshape(btnSetting.x, btnSetting.y, btnSetting.w, btnSetting.h, "��Ϸ����");
    functionalshape(btnExit.x, btnExit.y, btnExit.w, btnExit.h, "�˳���Ϸ");
    setbkmode(TRANSPARENT);
    settextstyle(50, 30, "����");
    settextcolor(0X000000);
    char s[50] = "׷���Ұ��������㣡";
    outtextxy((1000 - textwidth(s)) / 2, 30, s);
    setfillcolor(0XFFFFFF);




}
//�����淨���ܽ������ֻ���
void drawtext(int x, int y, std::string s) {
    setbkmode(TRANSPARENT);
    settextstyle(20, 0, "����");
    settextcolor(0XFFFFFF);
    outtextxy(x, y, s.c_str());
}
void DrawHelpUI() {
    //��������ͱ�������ɫ
    putimage(0, 0, &g_res.bgHelp);
    setbkmode(TRANSPARENT);
    settextstyle(25, 0, "΢���ź�");
    settextcolor(0XFFFFFF);
    //���ƽ�������
    drawtext(0, 30, "������");
    drawtext(46, 30, "һ�������˳�Цȴ������Ӧ��ĬĬ��ʵ��֤����������һ��������Ϊ�飬�ⲻ֪��ȴֻ��һ��֤�����ᣬ");
    drawtext(46, 60, "ʧ���ˣ�������������׹����Ԩ��һ�ж��ô�ͷ��ʼ��������Գ���δ֪����ս");
    drawtext(0, 90, "������");
    drawtext(46, 90, "��ʼѪ��100�㣬ÿ��1������������ֵ10�㣬���һظ�10������ֵ��");
    drawtext(46, 120, "ÿ��50�㾭�飬ÿ5���ٻ�Сboss��20���ٻ���boss��");
    drawtext(46, 150, "����������ʽΪ����귽������ӵ����������Ϊ0.1�룬�˺�Ϊ1�㣬ÿ����һ������1���˺���");

    drawtext(0, 180, "С����:");
    drawtext(80, 180, "Ѫ��5�� ,�˺�2��");
    drawtext(0, 210, "СBOSS:");
    drawtext(80, 210, "200��Ѫ�����˺�10�㣬ÿ��һ��ʱ���ٻ�����С������Сboss��������");
    drawtext(0, 240, "��BOSS:");
    drawtext(80, 240, "913��Ѫ���˺�100�㣬������ҷ����ӵ����������ײ���������Ѫ������һ�ö����޵�");
    drawtext(0, 640, "����������ʽ:");
    drawtext(140, 640, "��������������ӵ�������Ϊ���������������ߡ�");
    drawtext(0, 670, "���������ƶ���ʽ:");
    drawtext(180, 670, "���'W'�����ƶ������'D'�����ƶ������'A'�����ƶ������'D'�����ƶ�");
    //���Ʒ��ز˵�
    btnExit.x = 900;
    btnExit.y = 650;
    btnExit.w = 100;
    btnExit.h = 50;
    setfillcolor(0XFFFFFF);
    solidrectangle(btnExit.x, btnExit.y, btnExit.x + btnExit.w, btnExit.y + btnExit.h);
    setbkmode(TRANSPARENT);
    settextstyle(30, 10, "΢���ź�");
    settextcolor(0X000000);
    std::string s = "���ز˵�";
    outtextxy(btnExit.x + (btnExit.w - textwidth(s.c_str())) / 2, btnExit.y + (btnExit.h - textheight(s.c_str())) / 2, s.c_str());
}

void DrawSettingUI() {

}

void DrawTeamUI() {
    putimage(0, 0, &g_res.bgTeam);
    setbkmode(TRANSPARENT);
    settextstyle(50, 30, "����");
    settextcolor(0XFFFFFF);
    char s[50] = "Segmentation Faultless";
    outtextxy((1000 - textwidth(s)) / 2, 30, s);

    drawtext(0, 90, "�鳤-С˧��Segmentation Faultless�쵼�ߣ�����ල��Ա���ȣ���ϷͼƬ�࣬�Լ�������ҡ�������ӵ���");

    drawtext(0, 125, "���鳤-���죺��offer�ո����������Ϸ��ť�ࡣ");
    drawtext(0, 160, "��Ʒ����-���У�����·��Ǯ��������������ࡣ");
    drawtext(0, 195, "��Ʒ����-���ӣ���ʼ���ϵۣ�������Ϸ��ʼ����������Ϸ���̡�");
    drawtext(0, 230, "�ල��-Сǫ�����������ʦ�����������Ϸ�ӵ��ࡣ");
    drawtext(0, 265, "��Ϣ��-Сŵ����ϣ���ƹ��߼����񣬸��������Ϸ�߼���");
    drawtext(0, 300, "������-���磺�ܹ�֮�񣬸�����Ϸ�ܹ���ԭ��ͼ������ࡣ");
    drawtext(0, 335, "������-С��������֮����������Ϸ������ơ�");
    btnExit.x = 920;
    btnExit.y = 650;
    btnExit.w = 80;
    btnExit.h = 50;
    functionalshape(btnExit.x, btnExit.y, btnExit.w, btnExit.h, "���ز˵�");




}

void DrawPauseUI() {

    putimage(0, 0, &g_res.bgPause);
    setbkmode(TRANSPARENT);
    settextstyle(35, 20, "����");
    settextcolor(0X000000);
    char s[50] = "���ϣ�����";
    outtextxy((1000 - textwidth(s)) / 2, 30, s);

    btnBack.x = btnRestart.x = btnResume.x = 450;
    btnBack.h = btnRestart.h = btnResume.h = 50;
    btnBack.w = btnRestart.w = btnResume.w = 100;
    btnRestart.y = 200;
    btnBack.y = 280;
    btnResume.y = 360;
    //	�������¿�ʼ
    functionalshape(btnRestart.x, btnRestart.y, btnRestart.w, btnRestart.h, "���¿�ʼ");
    //���Ʒ��ز˵�
    functionalshape(btnBack.x, btnBack.y, btnBack.w, btnBack.h, "���ز˵�");
    //	//���Ƽ�����Ϸ
    functionalshape(btnResume.x, btnResume.y, btnResume.w, btnResume.h, "������Ϸ");

}


void DrawSettlementUI() {
    //ʧ�ܽ���
    if (g_isGameOver == true) {
        //����ʧ������
        putimage(0, 0, &g_res.bgLose);
        setbkmode(TRANSPARENT);
        settextstyle(65, 50, "����");
        settextcolor(0XFFFFFF);
        char s[50] = "�̾Ͷ���!";
        outtextxy((1000 - textwidth(s)) / 2, 30, s);
        settextstyle(50, 0, "����");
        //���ƽ���ȼ�
        std::string s1 = "�ȼ�:";
        outtextxy(0, 70, s1.c_str());
        char s2[50];
        sprintf_s(s2, 50, "%d", g_player.level);
        outtextxy(textwidth(s1.c_str()) + 4, 70, s2);

        //���ƽ������
        std::string s3 = "����:";
        outtextxy(0, 115, s3.c_str());
        char s4[50];
        sprintf_s(s4, 50, "%d", g_player.score);
        outtextxy(textwidth(s3.c_str()) + 4, 120, s4);

        //�������¿�ʼ
        btnRestart.y = btnBack.y = 450;
        btnRestart.w = btnBack.w = 100;
        btnRestart.h = btnBack.h = 50;
        btnRestart.x = 280;
        btnBack.x = 680;
        functionalshape(btnRestart.x, btnRestart.y, btnRestart.w, btnRestart.h, "���¿�ʼ");
        //���Ʒ��ز˵�
        functionalshape(btnBack.x, btnBack.y, btnBack.w, btnBack.h, "���ز˵�");
    }

    //����ʤ������
    if (g_isWin == true) {
        //����ʤ������
        putimage(0, 0, &g_res.bgWin);
        setbkmode(TRANSPARENT);
        settextstyle(35, 20, "����");
        settextcolor(0XFFFFFF);
        char s[50] = "��ϲ��ϰʱ���������㣬���������кڷۣ�";
        outtextxy((1000 - textwidth(s)) / 2, 30, s);
        char s11[50] = "����Ϊʲô���ʾ���ֻ����̫����";
        outtextxy((1000 - textwidth(s11)) / 2, 60, s11);

        settextstyle(50, 0, "����");
        //���ƽ���ȼ�
        std::string s1 = "�ȼ�:";
        outtextxy(0, 70, s1.c_str());
        char s2[50];
        sprintf_s(s2, 50, "%d", g_player.level);
        outtextxy(textwidth(s1.c_str()) + 4, 70, s2);

        //���ƽ������
        std::string s3 = "����:";
        outtextxy(0, 115, s3.c_str());
        char s4[50];
        sprintf_s(s4, 50, "%d", g_player.score);
        outtextxy(textwidth(s3.c_str()) + 4, 120, s4);

        //�������¿�ʼ
        btnRestart.y = btnBack.y = 450;
        btnRestart.w = btnBack.w = 100;
        btnRestart.h = btnBack.h = 50;
        btnRestart.x = 280;
        btnBack.x = 680;
        functionalshape(btnRestart.x, btnRestart.y, btnRestart.w, btnRestart.h, "���¿�ʼ");
        //���Ʒ��ز˵�
        functionalshape(btnBack.x, btnBack.y, btnBack.w, btnBack.h, "���ز˵�");
    }

}

// ��Ϸ�߼�ÿ֡����
void GameUpdate()
{
    // �������ɼ�ʱ
    g_spawnTimer++;
    if (g_spawnTimer >= g_spawnRate)
    {
        SpawnMonster();
        g_spawnTimer = 0;
    }


    // ���������ӵ�״̬
    UpdateBullets();

    // �������й�����ƶ�����Ϊ
    UpdateMonsters();

    // �ӵ���������ײ���
    Collide_BulletMonster();

    // �����������ײ���
    Collide_PlayerMonster();

    // �������Ƿ�����
    CheckLevelUp();

    // ��������޵�֡״̬
    UpdateInvincible();

    // �����Ϸ�Ƿ������ʤ��/ʧ�ܣ�
    CheckGameEnd();
}

void SpawnMonster() {
    Monster monster;
    monster.RandomSpawn();         // ���λ�úͳߴ�
    monster.type = MONSTER;
    monster.active = true;

    monster.maxHp = 5;             // Ѫ��5��
    monster.hp = monster.maxHp;
    monster.speed = MONSTER_SPEED;
    monster.expDrop = 5;           // ���侭��
    monster.score = 10;            // ��ɱ�÷�

    g_monsters.push_back(monster);
}

void SpawnMiniBoss() {
    Monster miniBoss;
    miniBoss.RandomSpawn();        // ���λ�úͳߴ�
    miniBoss.type = MINI_BOSS;
    miniBoss.active = true;

    miniBoss.maxHp = 200;          // Ѫ��200��
    miniBoss.hp = miniBoss.maxHp;
    miniBoss.speed = MONSTER_SPEED;
    miniBoss.expDrop = 50;         // ���侭��
    miniBoss.score = 200;          // ��ɱ�÷�

    g_monsters.push_back(miniBoss);
}

void SpawnFinalBoss() {
    Monster finalBoss;
    finalBoss.RandomSpawn();       // ���λ�úͳߴ�
    finalBoss.type = FINAL_BOSS;
    finalBoss.active = true;

    finalBoss.maxHp = 913;         // Ѫ��913��
    finalBoss.hp = finalBoss.maxHp;
    finalBoss.speed = MONSTER_SPEED;
    finalBoss.expDrop = 500;       // ���侭��
    finalBoss.score = 1000;        // ��ɱ�÷�

    g_monsters.push_back(finalBoss);
    g_hasFinalBoss = true;
}

void UpdateBullets()
{
    for (int i = 0; i < g_bullets.size(); i++)
    {
        if (!g_bullets[i].active)
            continue;

        if (g_bullets[i].flag == 0)
            g_bullets[i].P_Move();
        else
            g_bullets[i].M_Move();

        if (g_bullets[i].CheckBorder())
            g_bullets[i].active = false;
    }

    // ��ȫ����
    vector<Bullet> temp;
    for (auto& b : g_bullets)
        if (b.active)
            temp.push_back(b);
    g_bullets.swap(temp);
}

void UpdateMonsters() {
    // �������й���
    for (int i = 0; i < g_monsters.size(); i++) {
        Monster& m = g_monsters[i];
        // ֻ���´��Ĺ���
        if (m.active) {
            // ����׷����Һ�����ʵ���ƶ�
            m.TrackPlayer(g_player);
        }
    }
}

void UpdataBloodbags() {
    //��ȫ����
    vector<Bloodbag> temp;
    for (auto& pb : g_bloodbag)
        if (pb.active)
            temp.push_back(pb);
    g_bloodbag.swap(temp);
}

void Collide_BulletMonster()
{
    if (g_bullets.empty() || g_monsters.empty())
        return;

    for (int i = 0; i < g_bullets.size(); i++)
    {
        Bullet& b = g_bullets[i];
        if (!b.active || b.flag != 0)
            continue;

        for (int j = 0; j < g_monsters.size(); j++)
        {
            Monster& m = g_monsters[j];
            if (!m.active)
                continue;

            if (b.x < m.x + m.w &&
                b.x + b.w > m.x &&
                b.y < m.y + m.h &&
                b.y + b.h > m.y)
            {
                m.TakeDamage(b.atk, g_player);
                b.active = false;
                break;
            }
        }
    }
}

void Collide_PlayerMonster() {
    for (auto& m : g_monsters) {
        if (!m.active) continue;

        bool collide = (g_player.x < m.x + m.w) && (g_player.x + g_player.w > m.x) &&
            (g_player.y < m.y + m.h) && (g_player.y + g_player.h > m.y);

        if (collide) {
            g_player.TakeDamage(10);
        }
    }
}

void CheckLevelUp() {
    if (g_player.exp >= g_player.expNeed)
    {
        g_player.LevelUp();
    }
}

void Collide_Bloodbag() {
    if (g_bloodbag.empty())return;
    for (int i = 0; i < g_bloodbag.size(); i++) {
        Bloodbag& bb = g_bloodbag[i];
        if (!bb.active)continue;
        else if (g_player.x < bb.x + bb.w &&
            g_player.x + g_player.w > bb.x &&
            g_player.y < bb.y + bb.h &&
            g_player.y + g_player.h > bb.y)
        {
            //��bb��g_bloodbag�Ƶ�g_pb��
            g_pb.push_back(bb);
            g_bloodbag.erase(g_bloodbag.begin() + i);
        }
    }
}

void UpdateInvincible() {
    if (g_player.isInvincible) {    //��Ҵ����޵�״̬ʱ������Ҫ��ʱ�ж�
        if (GetTickCount() - g_player.invincibleTimer > INVINCIBLE_TIME) {
            g_player.isInvincible = false;
        }
    }
}

// �����Ϸ����������ʤ��/ʧ�ܣ�
void CheckGameEnd()
{
    // ���Ѫ��С�ڵ���0ʱ��Ϸʧ��
    if (g_player.hp <= 0)
    {
        g_isGameOver = true;
        g_isWin = false;
        g_curUI = SETTLEMENT;
    }

    // ��ҵȼ��ﵽ���BOSS�ȼ������Ҵ�BOSSҲ������ʱ��Ϸʤ��
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

    putimage(0, 0, &g_res.bgGame);
    DrawPlayerInfo();
    DrawEntities();
    //��ͣ��ť����
    btnPause.x = 920;
    btnPause.y = 620;
    btnPause.w = 80;
    btnPause.h = 80;
    //functionalshape(btnPause.x, btnPause.y, btnPause.w, btnPause.h, "��ͣ��Ϸ");

}

void DrawPlayerInfo() {

    putimage(0, 10, &g_res.imgAx);
    putimage(0, 40, &g_res.imgGj);
    //���������С�͸�ʽ
    settextstyle(25, 0, "΢���ź�");
    setbkmode(TRANSPARENT);
    settextcolor(0XFFFFFF);
    //������ҵȼ�
    std::string s1 = "�ȼ�:";
    outtextxy(0, 70, s1.c_str());
    char s2[50];
    sprintf_s(s2, 50, "%d", g_player.level);
    outtextxy(textwidth(s1.c_str()) + 4, 70, s2);
    setfillcolor(0XE2961B);
    solidrectangle(70, 78, 70 + g_player.exp, 88);

    //������ҷ���
    std::string s3 = "����:";
    outtextxy(0, 95, s3.c_str());
    char s4[50];
    sprintf_s(s4, 50, "%d", g_player.score);
    outtextxy(textwidth(s3.c_str()) + 4, 95, s4);

    //�������Ѫ��
    setfillcolor(RED);
    solidrectangle(28, 15, g_player.hp + 28, 25);
    //������ҹ�����
    setfillcolor(0XFFFFFF);
    solidrectangle(28, 45, g_player.atk + 28, 60);
}

// ���ƹ���Ѫ��
void DrawMonsterHPBar(Monster& monster)
{
    int barW = monster.w;              // Ѫ������ = �������
    int barH = 6;                      // Ѫ���߶�
    int barX = monster.x;              // Ѫ��X = ����X
    int barY = monster.y - barH - 4;   // Ѫ���ڹ���ͷ���Ϸ�

    // Ѫ����������ң�
    setfillcolor(RGB(80, 80, 80));
    solidrectangle(barX, barY, barX + barW, barY + barH);

    // Ѫ��ǰ��������Ѫ������Ⱦɫ����->��->�죩
    float ratio = (float)monster.hp / (float)monster.maxHp;
    COLORREF hpColor = (ratio > 0.5f) ? RGB(0, 200, 0)
        : (ratio > 0.25f) ? RGB(220, 200, 0)
        : RGB(220, 0, 0);
    setfillcolor(hpColor);
    solidrectangle(barX, barY, barX + (int)(barW * ratio), barY + barH);

    // Ѫ���߿�
    setlinecolor(RGB(255, 255, 255));
    rectangle(barX, barY, barX + barW, barY + barH);
}

// ��������ʵ�壨��ҡ�����ӵ���
void DrawEntities()
{
    //  1. ������� 
    if (g_player.isInvincible)
    {
        // �޵�״̬����͸������
        putimagePNG(&g_res.imgPlayer,
            g_player.x, g_player.y,
            0, 0,
            (int)g_res.imgPlayer.getwidth(),
            (int)g_res.imgPlayer.getheight(),
            0.5);
    }
    else
    {
        putimagePNG(&g_res.imgPlayer, g_player.x, g_player.y);
    }

    //  2. �����ӵ�
    for (int i = 0; i < (int)g_bullets.size(); i++)
    {
        Bullet& bullet = g_bullets[i];
        if (!bullet.active) continue;

        putimagePNG(&g_res.imgBullet, bullet.x, bullet.y,
            0, 0,
            (int)g_res.imgBullet.getwidth(),
            (int)g_res.imgBullet.getheight());
    }

    //  3. ���ƹ��� / BOSS  
    for (int i = 0; i < (int)g_monsters.size(); i++)
    {
        Monster& monster = g_monsters[i];
        if (!monster.active) continue;

        IMAGE* imgToDraw = nullptr;

        switch (monster.type)
        {
        case MONSTER:
            imgToDraw = &g_res.imgMonster;
            break;
        case MINI_BOSS:
            imgToDraw = &g_res.imgMiniBoss;
            break;
        case FINAL_BOSS:
            imgToDraw = &g_res.imgFinalBoss;
            break;
        default:
            imgToDraw = &g_res.imgMonster;
            break;
        }

        // ���ƹ�����ͼ
        putimagePNG(imgToDraw, monster.x, monster.y,
            0, 0,
            (int)imgToDraw->getwidth(),
            (int)imgToDraw->getheight());

        // ����Ѫ��
        DrawMonsterHPBar(monster);
    }
}
//����һ֡�߼�
// void test() {
//    initgraph(200, 200);
//    setbkcolor(WHITE);
//    int x;
//	DWORD startTime = GetTickCount();
//    while (true)
//    {
//        DWORD currentTime = GetTickCount();
//        if (currentTime - startTime >= 1000 / FPS) // ����֡��
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