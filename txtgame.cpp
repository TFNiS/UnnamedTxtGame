/**
Game Made by: Tomasz Bronisław Rahn (aka Drejzer)
as a project for TFNiS 2017
**/
#include<cctype>
#include<iostream>
#include<fstream>
#include<thread>
#include<chrono>
#include<cmath>
#include<random>
#include<string>
#include<vector>
#include<cstdlib>

using namespace std;

bool explored;
int rpt;
int inputint()
{
bool neg=false;
cin.clear();
string a;
getline(cin,a);
int liczba=0;
if(a[0]=='-'){neg=true;a[0]='0';}
else if(!isdigit(a[0]))   //if(a[0]!='1' and a[0]!='2' and a[0]!='3' and a[0]!='4' and a[0]!='5' and a[0]!='6' and a[0]!='7' and a[0]!='8' and a[0]!='9' and a[0]!='0' and a[0]!='-')
    {
    cout<<"Something went wrong, try again:"<<endl;
    a="";
    cin.clear();
    cin.ignore(1000,'\n');
    getline(cin,a);
    }
for (int i=0;i<a.length();i++)
    {
   // cout<<a[a.length()-(i+1)]<<endl<<endl;
    if(!isdigit(a[a.length()-(i+1)])) //a[a.length()-(i+1)]!=1 and a[a.length()-(i+1)]!=2 and a[a.length()-(i+1)]!=3 and a[a.length()-(i+1)]!=4 and a[a.length()-(i+1)]!=5 and a[a.length()-(i+1)]!=6 and a[a.length()-(i+1)]!=7 and a[a.length()-(i+1)]!=8 and a[a.length()-(i+1)]!=9 and a[a.length()-(i+1)]!=0)
        {
        cout<<"Invalid input, try again!"<<endl;
        neg=false;
        cin.clear();
        cin.ignore(1000,'\n');
        a="";
        inputint();
        break;
        }
    liczba+=((int)a[a.length()-(1+i)]-'0')*pow(10,i);
    }
if(neg){liczba*=-1;}
//cout<<liczba<<endl<<endl;
return liczba;
}

int rollD(int sides)
{
if(sides<1){return 0;}
return rand()%sides+1;
}

int lockcount;
int killcount;
bool NG;
int LabyrinthFloor;

class Weapon
{
public:
string Name;
int reqBody=0;
int reqSoul=0;
int reqMind=0;
int bazDMG=0;
int SoulDMGmod=0;
int BodyDMGmod=0;
int MindDMGmod=0;
int bazProgress=0;
int BodyProgress=0;
int SoulProgress=0;
int MindProgress=0;
int SDiff=10;
int BDiff=10;
int MDiff=10;
int lvl=-1;
bool bound=false;
void Devour(Weapon food)
    {
    bazProgress+=(int)(food.bazDMG*0.95)+1;
    BodyProgress+=(food.BodyDMGmod+food.BodyProgress)/(max((BDiff*0.75),0.85));
    SoulProgress+=(food.SoulDMGmod+food.SoulProgress)/(max((SDiff*0.75),0.85));
    MindProgress+=(food.MindDMGmod+food.MindProgress)/(max((MDiff*0.8),0.9));
    }
void Morph(Weapon target)
    {
    Name=target.Name;
    BodyProgress=BodyDMGmod;
    MindProgress=MindDMGmod;
    SoulProgress=SoulDMGmod;
    bazProgress=bazDMG;
    lvl=target.lvl;
    BDiff=target.BDiff;
    SDiff=target.SDiff;
    MDiff=target.MDiff;
    bazDMG=target.bazDMG;
    BodyDMGmod=target.BodyDMGmod;
    SoulDMGmod=target.SoulDMGmod;
    MindDMGmod=target.MindDMGmod;
    }
};

class Room
{
public:
bool isOccupied;
int Special;
bool hasSecret;
bool isLocked;
bool isSealed;
int LockLevel;
int SealLevel;
int SecretreqBody;
int SecretreqSoul;
int SecretreqMind;
bool SecretKnown=false;
void DiscoverSecret(){SecretKnown=true;}
void MakeSecret()
    {
    int secretlvl=1+rollD(LabyrinthFloor);
    SecretreqMind=rollD(secretlvl);
    SecretreqBody=rollD(secretlvl);
    secretlvl-=SecretreqBody;
    SecretreqSoul=rollD(secretlvl);
    }
};
vector<bool> mapa;
vector <Room> Level;

class Creature
{
public:
string Name;
Weapon uzbrojenie;
int traces=0;
int Energy=0;
int MaxHP=0;
int HP=0;
int MaxMp=0;
int MP=0;
int Body=0;
int Soul=0;
int Mind=0;
int HPmods=0;
int MPmods=0;
int Attack=0;
int atkmods=0;
int Defence=0;
int defmods=0;
int DMG=0;
int DMGmods=0;
int Armour=0;
int isinRoom=0;
void binding()
    {
    MP--;
    HP--;
    uzbrojenie.bound=true;
    cout<<"You feel a mysterious connection to your weapon... moreover it almost seems as if the weapon was somewhat sentient..."<<endl<<"and hapy for some reason"<<endl;
    CalculateStats();
    }
bool isAlive()
    {
    if (HP<=0){return false;}
    else {return true;}
    }
void CalculateStats()
    {
    if (HP>MaxHP){HP=MaxHP;}
    if(MP>MaxMp){MP=MaxMp;}
    int delta=(int)((double)Body*1.05+(double)Soul*0.20)+HPmods-MaxHP;
    MaxHP+=delta;
    HP+=delta;
    delta=(int)((double)Soul*1.05+(double)Body*0.15+(double)Mind*0.05)+MPmods-MaxMp;
    MaxMp+=delta;
    MP+=delta;
    if (HP>MaxHP){HP=MaxHP;}
    if(MP>MaxMp){MP=MaxMp;}
    Attack=(int)((double)Body*0.95+(double)Mind*0.125+(double)Soul*0.075)+atkmods;
    Defence=(int)((double)Body*1.05+(double)Mind*0.05+(double)Soul*0.05)+defmods;
    DMGmods=uzbrojenie.bazDMG+Body*uzbrojenie.BodyDMGmod+Soul*uzbrojenie.SoulDMGmod+Mind*uzbrojenie.MindDMGmod;
    DMG=(int)((double)Body*0.05+(double)Mind*0.025+(double)Soul*0.025)+DMGmods;
    }
};

Creature *Hero;
Creature *Enemy;

void LootWpn()
{
Weapon drop;
ifstream Armoury;
Armoury.open("Armoury.txt");
int j;
Armoury>>j;
int k=rollD(j)-1;
//cout<<k<<endl;
for (int i=0;i<j;i++)
    {
    string temp;
    getline(Armoury,temp);
  //  cout<<temp<<endl;
    if(i==k)
        {
        Armoury>>drop.Name>>drop.bazDMG>>drop.BodyDMGmod>>drop.SoulDMGmod>>drop.MindDMGmod>>drop.reqBody>>drop.reqSoul>>drop.reqMind>>drop.BDiff>>drop.SDiff>>drop.MDiff;
        }
    }
Armoury.close();
this_thread::sleep_for(chrono::milliseconds(100));
if (Hero->uzbrojenie.lvl==-1)
    {
    if(drop.reqBody>Hero->Body or drop.reqMind>Hero->Mind or drop.reqSoul>Hero->Soul)
        {
        LootWpn();
        return;
        }
    }
cout<<"You found a weapon... it seems to be a";
if (drop.Name[0]=='a' || drop.Name[0]=='A' || drop.Name[0]=='e' || drop.Name[0]=='E' || drop.Name[0]=='o' || drop.Name[0]=='O' || drop.Name[0]=='i' || drop.Name[0]=='I'){cout<<"n";}
cout<<" "<<drop.Name<<endl;
drop.lvl=rollD(LabyrinthFloor+1)-2+rollD(2);
drop.bazDMG+=(int)(drop.lvl*1.375);
if (Hero->Body>=drop.reqBody and Hero->Soul>=drop.reqSoul && Hero->Mind>=drop.reqMind)
    {
    if (Hero->uzbrojenie.lvl==-1)
        {cout<<"Since any weapon is better than no weapon, you pick up your find"<<endl;
        Hero->uzbrojenie=drop;
        }
    else if(Hero->uzbrojenie.bound)
        {
        cout<<"As you look at the weapon you found you feel as if your "<<Hero->uzbrojenie.Name<<"was trying to tell you taht it could be just like this one..."<<endl;
        cout<<"Should it Keep it's current appearance or should it Switch?";
        if(rollD(100)>=66){cout<<" you wonder how do those forms Compare against each other..."<<endl<<"You feel as if your weapon wants to warn you about something...";}
        cout<<endl;
        bool chosen=false;
        string choice;
        do{
        cin.clear();
        cin.ignore(666,'\n');
        cout<<"So…?"<<endl;
        getline(cin,choice);
        if (choice.compare("Keep")==0 or choice.compare("keep")==0 or choice[0]=='k' or choice[0]=='K')
            {
            cout<<"You decide that you like your weapon as it is"<<endl;
            chosen=true;
            Hero->uzbrojenie.Devour(drop);
            cout<<"Your weapon strikes the "<<drop.Name<<"on it's own, destroying it."<<endl<<"You could swear that your weapon was proud of itself..."<<endl;
            }
        else if(choice.compare("Switch")==0 or choice.compare("switch")==0 or choice[0]=='S' or choice[0]=='s')
            {
            cout<<"You think that "<<drop.Name<<" would be more comfortable to use"<<endl;
            Hero->uzbrojenie.Morph(drop);
            cout<<"Your weapon turns into a transparent liquid and drops onto the "<<drop.Name<<endl<<"After a short while there is no visible trace left after the previous shape of your weapon as now it turned into "<<Hero->uzbrojenie.Name<<endl;
            Hero->CalculateStats();
            chosen=true;
            }
        else if(choice.compare("Compare")==0 or choice.compare("compare")==0 or choice.compare("Inspect")==0 or choice.compare("inspect")==0 or choice[0]=='C' or choice[0]=='c')
            {
            cout<<"You feel a stream of information flowing from your weapon."<<endl;
            cout<<Hero->uzbrojenie.Name<<"/"<<drop.Name<<endl<<"Foundation:"<<Hero->uzbrojenie.bazDMG<<"/"<<drop.bazDMG<<endl;
            cout<<"Sharpness:"<<Hero->uzbrojenie.BodyDMGmod<<"/";
            if(drop.BodyDMGmod>Hero->uzbrojenie.BodyDMGmod){cout<<"Higher";if(Hero->uzbrojenie.BDiff<drop.BDiff){cout<<",worse"<<endl;}}
            else if(drop.BodyDMGmod<Hero->uzbrojenie.BodyDMGmod){cout<<"Lower";if(Hero->uzbrojenie.BDiff>drop.BDiff){cout<<",better"<<endl;}}
            else if(drop.BodyDMGmod==Hero->uzbrojenie.BodyDMGmod){cout<<"Equal";if(Hero->uzbrojenie.BDiff<drop.BDiff){cout<<",worse"<<endl;}else if(Hero->uzbrojenie.BDiff>drop.BDiff){cout<<",better"<<endl;}}
            cout<<"Aura:"<<Hero->uzbrojenie.SoulDMGmod<<"/";
            if(drop.SoulDMGmod>Hero->uzbrojenie.SoulDMGmod){cout<<"Higher";if(Hero->uzbrojenie.SDiff<drop.SDiff){cout<<",worse"<<endl;}}
            else if(drop.SoulDMGmod<Hero->uzbrojenie.SoulDMGmod){cout<<"Lower";if(Hero->uzbrojenie.SDiff>drop.SDiff){cout<<",better"<<endl;}}
            else if(drop.SoulDMGmod==Hero->uzbrojenie.SoulDMGmod){cout<<"Equal";if(Hero->uzbrojenie.SDiff<drop.SDiff){cout<<",worse"<<endl;}else if(Hero->uzbrojenie.SDiff>drop.SDiff){cout<<",better"<<endl;}}
            cout<<"Precision:"<<Hero->uzbrojenie.MindDMGmod<<"/";
            if(drop.MindDMGmod>Hero->uzbrojenie.MindDMGmod){cout<<"Higher";if(Hero->uzbrojenie.MDiff<drop.MDiff){cout<<",worse"<<endl;}}
            else if(drop.MindDMGmod<Hero->uzbrojenie.MindDMGmod){cout<<"Lower";if(Hero->uzbrojenie.MDiff>drop.MDiff){cout<<",better"<<endl;}}
            else if(drop.MindDMGmod==Hero->uzbrojenie.MindDMGmod){cout<<"Equal";if(Hero->uzbrojenie.MDiff<drop.MDiff){cout<<",worse"<<endl;}else if(Hero->uzbrojenie.MDiff>drop.MDiff){cout<<",better"<<endl;}}
            }
        else{cout<<"This is somewhat important, you know?"<<endl;}
        }while(!chosen);
        }
    else {cout<<"You already have a weapon ("<<Hero->uzbrojenie.Name<<") but you could always switch to using this one..."<<endl<<"will you Keep the current one or Switch for the new one?"<<endl;
        if(rollD(100)>=75){cout<<"You could also Compare them..."<<endl;}
        bool chosen=false;
        string choice;
        do{
        cin.clear();
        cin.ignore(666,'\n');
        cout<<"So…?"<<endl;
        getline(cin,choice);
        if (choice.compare("Keep")==0 or choice.compare("keep")==0 or choice[0]=='k' or choice[0]=='K')
            {
            cout<<"You decide to keep your current weapon"<<endl<<"You see as the "<<drop.Name<<" dissipates..."<<endl;
            chosen=true;
            }
        else if(choice.compare("Switch")==0 or choice.compare("switch")==0 or choice[0]=='S' or choice[0]=='s')
            {
            cout<<"You swap your current weapon for the "<<drop.Name<<endl<<"Now that you look, you can't find your previous weapon..."<<endl;
            Hero->uzbrojenie=drop;
            Hero->CalculateStats();
            chosen=true;
            }
        else if(choice.compare("Compare")==0 or choice.compare("compare")==0 or choice.compare("Inspect")==0 or choice.compare("inspect")==0 or choice[0]=='C' or choice[0]=='c')
            {
            cout<<Hero->uzbrojenie.Name<<"/"<<drop.Name<<endl<<"Basic DMG:"<<Hero->uzbrojenie.bazDMG<<"/"<<drop.bazDMG<<endl;
            }
        else{cout<<"This is somewhat important, you know?"<<endl;}
        }while(!chosen);
        }
    }
else if (Hero->uzbrojenie.bound)
    {
    cout<<"You feel it would be a pity to leave this "<<drop.Name<<" here, but, unfortunately you can't find any use for it..."<<endl;
    cout<<"Your "<<Hero->uzbrojenie.Name<<" suddenly jumps out of your hand and strikes the "<<drop.Name<<"!"<<endl;
    Hero->uzbrojenie.Devour(drop);
    cout<<"After "<<drop.Name<<" ceased to exist, your weapon returned to your hand on it's own"<<endl<<"There seem to be some barely noitceable changes... and it seems to feel happy for some reasun..."<<endl;
    }
else {cout<<"Regretfully, you are incapable of making use of this weapon so you leve it, knowing that it will disappear as soon as you take you eyes away from it"<<endl;}
}

void Attack(Creature *attacker, Creature *defender)
{
cout<<attacker->Name<<" attacks "<<defender->Name<<" and..."<<endl;
this_thread::sleep_for(chrono::milliseconds(125));
if(attacker->Attack+rollD(40)>defender->Defence+rollD(40))
    {
    this_thread::sleep_for(chrono::milliseconds(455));
    cout<<"... Hits,";
    if (defender->Armour>=attacker->DMG)
        {
        cout<<" but is unable to pierce through "<<defender->Name<<"'s Armour."<<endl;
        defender->Armour--;
        return;
        }
    else{
        if (defender->Armour<0){defender->Armour=0;}
        cout<<" dealing "<<(attacker->DMG-defender->Armour)<<" points of damage."<<endl;
        defender->HP-=(attacker->DMG-defender->Armour);
        if (defender->Armour>0){defender->Armour--;}
        return;
        }
    }
else {
    this_thread::sleep_for(chrono::milliseconds(375));
    cout<<"fails."<<endl;
    return;
    }
}

bool WALKA()
{
//cout<<Enemy->Body<<" "<<Enemy->Soul<<" "<<Enemy->HP<<" "<<Enemy->MaxHP<<endl;
cout<<"You eccountered  an enemy!"<<endl;
bool v=true;
while(Enemy->isAlive() and Hero->isAlive())
    {
    int delta;
    bool d=0;
    string a;
    char c;
    cout<<"Will you atack or defend?"<<endl;
    getline(cin,a);
    c=a[0];
    switch(c)
        {
        case 'c':
        case 'C':{cout<<"---------------------------------------------------------------------------------------"<<endl<<"STATUS:"<<endl;
            cout<<"NAME: "<<Hero->Name<<endl;
            cout<<"Body: "<<Hero->Body<<endl<<"MIND: "<<Hero->Mind<<endl<<"SOUL: "<<Hero->Soul<<endl;
            cout<<Hero->HP<<"/"<<Hero->MaxHP<<"HP  "<<Hero->MP<<"/"<<Hero->MaxMp<<"MP"<<endl;
            if (Hero->Armour>0){cout<<"Armour: "<<Hero->Armour<<endl;}
            cout<<"Weapon: "<<Hero->uzbrojenie.Name<<endl;
            if(Hero->uzbrojenie.bound){cout<<"Damage output:"<<Hero->DMG<<endl;;}else{cout<<"Base weapon damage:"<<Hero->uzbrojenie.bazDMG<<endl;}
            cout<<"---------------------------------------------------------------------------------------"<<endl;
            if(Enemy->HP>=Enemy->MaxHP*(3/4)){cout<<"Your oponent seems to be in a good shape"<<endl;}
            else if(Enemy->HP>=Enemy->MaxHP/2){cout<<"Your oponent seems to be a bit roughed-up"<<endl;}
            else if(Enemy->HP>=Enemy->MaxHP/4){cout<<"Your oponent seems to be in a rather bad shape..."<<endl;}
            else {cout<<"Your oponent seems to be barely holding up"<<endl;}
            continue;}
        case 'a':
        case 'A':{
                Attack(Hero,Enemy);
                break;
                }
        case 'd':
        case 'D':{
                d=true;
                delta=(int)((double)(Hero->Defence)*0.375)+5;
                Hero->Defence+=delta;
                cout<<"You ready yourself for the upcoming attack."<<endl;
                this_thread::sleep_for(chrono::milliseconds(500));
                break;
                }
        case 'F':
        case 'f':{
                cout<<"You run for your petty life..."<<endl;
                v=false;
                Enemy->HP=0;
                Enemy->isAlive();
                break;
                }
        default:{
                cout<<"You stand dazed, unshure what to do."<<endl;
                this_thread::sleep_for(chrono::milliseconds(666));
                break;
                }
        }
        Attack(Enemy,Hero);
        if(d){Hero->Defence-=delta;}
        if(!Hero->isAlive()){v=false;}
        }
    if(v){
        cout<<"You see as "<<Enemy->Name<<" slowly dissipates"<<endl;
        Hero->Energy+=1+(floor(LabyrinthFloor/10));
        if (Hero->Body+Hero->Mind+Hero->Soul>2*(Enemy->Body+Enemy->Mind+Enemy->Soul))
            {
            Hero->Energy+=floor(LabyrinthFloor/12);
            }
        else {
        string a;
            cout<<"You see a floating blob of... something."<<endl;
            this_thread::sleep_for(chrono::milliseconds(500));
            cout<<"What will you do?"<<endl;
            if(rollD(10)>=8){cout<<"...Will you try to TAKE it?"<<endl;}
            getline(cin,a);
            char c=a[0];
            switch(c)
                {
                case 'A':
                case 'a':
                        {
                        cout<<"You attack ";
                        if(Hero->uzbrojenie.lvl>=0)
                            {
                            switch(rollD(9))
                                {
                                case 1:Hero->uzbrojenie.bazProgress+=1;break;
                                case 2:Hero->uzbrojenie.SoulProgress+=1;break;
                                case 3:
                                case 4:Hero->uzbrojenie.bazProgress+=1;break;
                                case 5:Hero->uzbrojenie.SoulProgress+=1;break;
                                case 6:Hero->uzbrojenie.bazProgress+=2;break;
                                case 7:Hero->uzbrojenie.BodyProgress+=1;break;
                                case 9:Hero->Energy++;break;
                                default: Hero->uzbrojenie.bazProgress++;break;
                             }
                            cout<<"and see as the blob gets absorbed into your weapon"<<endl;
                            break;
                            }
                        }
                case 't':
                case 'T':{cout<<"You reach your hand and as you touch it..."<<endl;
                        this_thread::sleep_for(chrono::milliseconds(250));
                        switch(rollD(12))
                            {
                            case 1:
                            case 3:
                            case 5:
                            case 7:Hero->Energy+=3+2*rollD(LabyrinthFloor/1.5);break;
                            case 6:Hero->HPmods++;break;
                            case 4:Hero->MPmods++;break;
                            case 2:Hero->Body++;break;
                            case 11:Hero->Mind++;break;
                            case 12:Hero->Soul++;break;
                            default:Hero->Energy+=3+rollD(LabyrinthFloor/1.49);break;

                            }
                        }
                default: {cout<<"You observe as it dissipates..."<<endl; this_thread::sleep_for(chrono::milliseconds(875));break;}
                }
            }
        if(rollD(10+rollD(9+rollD(8))+rollD(11))==11){LootWpn();}
        killcount++;
        return true;
        }
    else {return false;}
}

bool Spawn()
{
Enemy->Name="Spectre";
if(LabyrinthFloor==0){Enemy->Name=="Training Dummy";}
int stat=1+LabyrinthFloor+rollD(floor(LabyrinthFloor/3));
Enemy->Body=LabyrinthFloor+rollD(stat);
stat=stat+LabyrinthFloor-Enemy->Body;
Enemy->Soul=LabyrinthFloor+rollD(stat);
stat=stat+LabyrinthFloor-Enemy->Soul;
Enemy->Mind=LabyrinthFloor+rollD(stat);
Enemy->HPmods+=1+rollD(LabyrinthFloor*2)+rollD(3);
Enemy->MPmods+=rollD(LabyrinthFloor+15);
Enemy->uzbrojenie.bazDMG+=LabyrinthFloor + rollD(floor(LabyrinthFloor/33));
if(Enemy->Body>Enemy->Soul)
    {
    if (Enemy->Soul>Enemy->Mind)
        {
        Enemy->uzbrojenie.BodyDMGmod=floor(1+LabyrinthFloor/12);
        Enemy->uzbrojenie.SoulDMGmod=floor(LabyrinthFloor/15);
        }
    else if(Enemy->Mind>Enemy->Body) {Enemy->uzbrojenie.MindDMGmod=floor(1+LabyrinthFloor/10);
        Enemy->uzbrojenie.BodyDMGmod=floor(LabyrinthFloor/15);}
    }
else if (Enemy->Mind>Enemy->Body)
    {
    if(Enemy->Soul>Enemy->Mind)
        {
        Enemy->uzbrojenie.SoulDMGmod=floor(1+LabyrinthFloor/12);
        Enemy->uzbrojenie.MindDMGmod=LabyrinthFloor/15;
        }
    else if(Enemy->Body>Enemy->Soul)
        {
        Enemy->uzbrojenie.MindDMGmod=1+LabyrinthFloor/10;
        Enemy->uzbrojenie.BodyDMGmod=LabyrinthFloor/15;
        }
    }
else {Enemy->uzbrojenie.BodyDMGmod=LabyrinthFloor/13;Enemy->uzbrojenie.SoulDMGmod=LabyrinthFloor/23;Enemy->uzbrojenie.MindDMGmod=LabyrinthFloor/33;}
Enemy->CalculateStats();
Enemy->HP=Enemy->MaxHP;
return WALKA();
}

void NewFloor()
{
explored=false;
mapa.clear();
Level.clear();
Hero->isinRoom=-1;
int rooms=0;
if(NG)
    {
    LabyrinthFloor=0;
    killcount=0;
    lockcount=0;
    NG=false;
    }
else{LabyrinthFloor++;}
Level.push_back(Room());
mapa.push_back(false);
Level[rooms].Special=0;
Level[rooms].hasSecret=min(1,max(0,(-4+rollD(6))));
Level[rooms].isOccupied=1;
Level[rooms].isLocked=min(1,max(0,(-5+rollD(6))));
Level[rooms].isSealed=1;
Level[rooms].SealLevel=rollD(6)+LabyrinthFloor+1.25*(rollD(LabyrinthFloor)+max(0,-100+LabyrinthFloor)*rollD(LabyrinthFloor))+max(-1,66+LabyrinthFloor);
Level[rooms].LockLevel=rollD(6)+LabyrinthFloor+1.25*(rollD(LabyrinthFloor)+max(0,-100+LabyrinthFloor));
if (Level[rooms].hasSecret==true)
    {
    Level[rooms].MakeSecret();
    }
    rooms++;
for (int i=((int)(5+LabyrinthFloor*1.025+rollD(LabyrinthFloor)+max(0,-66+LabyrinthFloor)*rollD(LabyrinthFloor*LabyrinthFloor*0.0125-LabyrinthFloor-44)))%777;rooms<i;rooms++)
    {
    Level.push_back(Room());
    mapa.push_back(false);
    Level[rooms].hasSecret=min(1,max(0,(-4+rollD(6))));
    Level[rooms].MakeSecret();
    Level[rooms].isSealed=min(1,max(0,(-7+rollD(8))));
    Level[rooms].isLocked=min(1,max(0,(-9+rollD(10))));
    Level[rooms].SealLevel=rollD(8)+LabyrinthFloor+1.25*(rollD(LabyrinthFloor)+max(0,-100+LabyrinthFloor)*LabyrinthFloor*rollD(LabyrinthFloor));
    Level[rooms].LockLevel=rollD(8)+LabyrinthFloor+1.25*(rollD(LabyrinthFloor)+max(0,-100+LabyrinthFloor));
    Level[rooms].Special=-1;
    Level[rooms].MakeSecret();
    }
int specs[5];
specs[0]=0;
int startspecrooms=rollD(rooms-4);
int k;
for(k=0;k<4;k++)
    {
    Level[startspecrooms+k].Special=k+1;
    }
}

void Strenghten()
{
cout<<"Which streak of light caught your fancy?"<<endl;
string choice;
getline(cin,choice);
if(choice.compare("Red")==0 or choice.compare("red")==0)
    {
    if(Hero->Energy>=Hero->traces+5)
        {
        Hero->Energy-=(5+Hero->traces);
        Hero->HPmods++;
        Hero->traces++;
        Hero->CalculateStats();
        cout<<"You feel... healthlier"<<endl;
        }
    else{cout<<"There was some reaction, but it seemed as if there was something lackig on your side..."<<endl;}
    }
else if(choice.compare("Blue")==0 or choice.compare("blue")==0)
    {
    if(Hero->Energy>=5+Hero->traces)
        {
        Hero->Energy-=(5+Hero->traces);
        Hero->MPmods++;
        Hero->traces++;
        Hero->CalculateStats();
        cout<<"You feel more energetic"<<endl;
        }
    else{cout<<"There was some reaction, but it seemed as if there was something lackig on your side..."<<endl;}
    }
else if(choice.compare("White")==0 or choice.compare("white")==0)
    {
    if(Hero->Energy>=10+Hero->traces)
        {
        Hero->Energy-=(10+Hero->traces);
        Hero->atkmods++;
        Hero->traces+=10;
        Hero->CalculateStats();
        cout<<"You have a feeling that you will hit your targets more often"<<endl;
        }
    else{cout<<"There was some reaction, but it seemed as if there was something lackig on your side..."<<endl;}
    }
else if(choice.compare("Black")==0 or choice.compare("black")==0)
    {
    if(Hero->Energy>=10+Hero->traces)
        {
        Hero->Energy-=(10+Hero->traces);
        Hero->defmods++;
        Hero->traces+=10;
        Hero->CalculateStats();
        cout<<"You have a feeling that you won't get hit as much from now on"<<endl;
        }
    else{cout<<"There was some reaction, but it seemed as if there was something lackig on your side..."<<endl;}
    }
else if(choice.compare("Silver")==0 or choice.compare("silver")==0)
    {
    if(Hero->Energy>=15+Hero->traces)
        {
        Hero->Energy-=(15+Hero->traces);
        Hero->DMGmods++;
        Hero->traces+=15;
        Hero->CalculateStats();
        cout<<"You have a feeling that you will hit your targets harder"<<endl;
        }
    else{cout<<"There was some reaction, but it seemed as if there was something lackig on your side..."<<endl;}
    }
else if(choice.compare("Green")==0 or choice.compare("green")==0)
    {
    if(Hero->Energy>=25+Hero->traces)
        {
        Hero->Energy-=(25+Hero->traces);
        Hero->Mind++;
        Hero->traces+=30+LabyrinthFloor/2;
        Hero->CalculateStats();
        cout<<"You feel as if you mind cleared up..."<<endl;
        }
    else{cout<<"There was some reaction, but it seemed as if there was something lackig on your side..."<<endl;}
    }
else if(choice.compare("Magenta")==0 or choice.compare("magenta")==0)
    {
    if(Hero->Energy>=25+Hero->traces)
        {
        Hero->Energy-=(25+Hero->traces);
        Hero->Body++;
        Hero->traces+=30+LabyrinthFloor/2;
        Hero->CalculateStats();
        cout<<"You feel as if your body became better overall"<<endl;
        }
    else{cout<<"There was some reaction, but it seemed as if there was something lackig on your side..."<<endl;}
    }
else if(choice.compare("Cyan")==0 or choice.compare("cyan")==0)
    {
    if(Hero->Energy>=25+Hero->traces)
        {
        Hero->Energy-=(25+Hero->traces);
        Hero->Mind++;
        Hero->traces+=25;
        Hero->CalculateStats();
        cout<<"You feel a mysterious force inside you..."<<endl;
        }
    else{cout<<"There was some reaction, but it seemed as if there was something lackig on your side..."<<endl;}
    }
else{cout<<"Hmm... nothing happens, so you step away."<<endl;}
}

void Reforge()
{
string choice;
cout<<"Weapon or Armour?"<<endl;
getline(cin,choice);
if (choice.compare("Armour")==0 or choice.compare("armour")==0 or choice[0]=='a' or choice[0]=='A')
    {
    int quota;
    cout<<"How much armour do you want?"<<endl<<"(FYI: armour requires about 15 Energy, and you have "<<Hero->Energy<<")"<<endl;
    quota=inputint();
    if(quota<=0){cout<<"You changed your mind and stepped away from the anvil"<<endl; return;}
    if (quota*15<Hero->Energy)
        {
        this_thread::sleep_for(chrono::milliseconds(666));
        cout<<"After a while you step away collecting the armour you... manifested into existence?"<<endl;
        Hero->Energy-=quota*15-rollD(floor(quota/3-2));
        return;
        }
    else if(quota*15-Hero->Energy<=Hero->MP)
        {
        this_thread::sleep_for(chrono::milliseconds(666));
        cout<<"After a while you step away collecting the armour you... manifested into existence?"<<endl;
        Hero->MP-=quota*15-Hero->Energy-rollD(floor(quota/3-2));
        Hero->Energy=0;
        return;
        }
    }
else if(choice.compare("Weapon")==0 or choice.compare("weapon")==0 or choice.at(0)=='w' or choice.at(0)=='W')
   {
    if(Hero->uzbrojenie.lvl==-1)
        {
        cout<<"You are unarmed! Find yourself a weapon before trying to upgrade it!"<<endl;
        return;
        }
    else
        {
        if (Hero->uzbrojenie.bound==0)
            {
            string q;
            cout<<"Do you want to bind your weapon?(only binded weapons can be upgraded)"<<endl;
            getline(cin,q);
            if(q.compare("Yes")==0 or q.compare("yes")==0 or q.compare("y")==0 or q.compare("Y")==0)
                {Hero->binding();}
            }
        if(Hero->uzbrojenie.BodyProgress!=0 and Hero->uzbrojenie.SoulProgress!=0 and Hero->uzbrojenie.MindProgress!=0 and Hero->uzbrojenie.bazProgress!=0 and Hero->uzbrojenie.lvl!=-1)
            {
            cout<<"Which aspect should be enchanced: Sharpness, Aura, Precision, or Foundation?"<<endl;
            string q;
            char c;
            getline(cin,q);
            c=(char)q[0];
            switch((int)c)
                {
                case 'S':
                case 's':{
                        if(Hero->uzbrojenie.BodyProgress>=Hero->uzbrojenie.BodyDMGmod+(Hero->uzbrojenie.lvl*Hero->uzbrojenie.BDiff))
                            {
                            Hero->uzbrojenie.BodyProgress-=(Hero->uzbrojenie.BodyDMGmod+Hero->uzbrojenie.lvl);
                            Hero->uzbrojenie.BodyDMGmod++;
                            Hero->uzbrojenie.lvl++;
                            this_thread::sleep_for(chrono::milliseconds(375));
                            cout<<"Reforging succes!"<<endl;
                            }
                        if ((Hero->uzbrojenie.BodyProgress+Hero->Energy/4)>=(Hero->uzbrojenie.BodyDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.BDiff) and Hero->uzbrojenie.BodyProgress<Hero->uzbrojenie.BodyDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.BDiff)
                            {
                            cout<<"Do you wish to spend Energy?"<<endl;
                            getline(cin,q);
                            c=(char)q[0];
                            if(c=='y' or c=='Y')
                                {
                                int calc=Hero->uzbrojenie.BodyDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.BDiff;
                                calc-=Hero->uzbrojenie.BodyProgress;
                                Hero->Energy-=4*calc;
                                Hero->uzbrojenie.BodyProgress=0;
                                Hero->uzbrojenie.lvl++;
                                Hero->uzbrojenie.BodyDMGmod++;
                                }
                            }
                        break;
                        }
                case 'A':
                case 'a':{
                        if(Hero->uzbrojenie.SoulProgress>=Hero->uzbrojenie.SoulDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.SDiff)
                            {
                            Hero->uzbrojenie.SoulProgress-=(Hero->uzbrojenie.SoulDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.SDiff);
                            Hero->uzbrojenie.SoulDMGmod++;
                            Hero->uzbrojenie.lvl++;
                            this_thread::sleep_for(chrono::milliseconds(375));
                            cout<<"Reforging succes!"<<endl;
                            }
                        if ((Hero->uzbrojenie.SoulProgress+Hero->Energy/4)>=Hero->uzbrojenie.SoulDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.SDiff and Hero->uzbrojenie.SoulProgress<Hero->uzbrojenie.SoulDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.SDiff)
                            {
                            cout<<"Do you wish to spend Energy?"<<endl;
                            getline(cin,q);
                            c=(char)q[0];
                            if(c=='y' or c=='Y')
                                {
                                int calc=Hero->uzbrojenie.SoulDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.SDiff;
                                calc-=Hero->uzbrojenie.SoulProgress;
                                Hero->Energy-=4*calc;
                                Hero->uzbrojenie.SoulProgress=0;
                                Hero->uzbrojenie.lvl++;
                                Hero->uzbrojenie.SoulDMGmod++;
                                }
                            }
                        break;
                        }
                case 'P':
                case 'p':{
                        if(Hero->uzbrojenie.MindProgress>=Hero->uzbrojenie.MindDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.MDiff)
                            {
                            Hero->uzbrojenie.MindProgress-=(Hero->uzbrojenie.MindDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.MDiff);
                            Hero->uzbrojenie.MindDMGmod++;
                            Hero->uzbrojenie.lvl++;
                            this_thread::sleep_for(chrono::milliseconds(375));
                            cout<<"Reforging succes!"<<endl;
                            }
                        if ((Hero->uzbrojenie.MindProgress+Hero->Energy/4)>=Hero->uzbrojenie.MindDMGmod+Hero->uzbrojenie.lvl and Hero->uzbrojenie.MindProgress<Hero->uzbrojenie.MindDMGmod+Hero->uzbrojenie.lvl)
                            {
                            cout<<"Do you wish to spend Energy?"<<endl;
                            getline(cin,q);
                            c=(char)q[0];
                            if(c=='y' or c=='Y')
                                {
                                int calc=Hero->uzbrojenie.MindDMGmod+Hero->uzbrojenie.lvl*Hero->uzbrojenie.MDiff;
                                calc-=Hero->uzbrojenie.MindProgress;
                                Hero->Energy-=4*calc;
                                Hero->uzbrojenie.MindProgress=0;
                                Hero->uzbrojenie.lvl++;
                                Hero->uzbrojenie.MindDMGmod++;
                                }
                            }
                        break;
                        }
                case 'F':
                case 'f':{
                        while(Hero->uzbrojenie.bazProgress>=Hero->uzbrojenie.bazDMG*4+Hero->uzbrojenie.lvl)
                            {
                            Hero->uzbrojenie.bazProgress-=Hero->uzbrojenie.bazDMG*4+Hero->uzbrojenie.lvl;
                            Hero->uzbrojenie.bazDMG++;
                            this_thread::sleep_for(chrono::milliseconds(250));
                            }
                        cout<<"Done."<<endl;
                        break;
                        }
                default: {break;}
                }
            }
        cout<<"You are incapable of improving this weapon any further"<<endl;
        return;
        }
    }
else {cout<<"You changed your mind and step away"<<endl;return;}
}

void Interact()
{
if (Hero->isinRoom==-1)
    {
    cout<<" There is nothing interesting in the place you are curretly in..."<<endl;
    }
else{
    if(Level[Hero->isinRoom].Special==0)
        {
        int price= 1+(int)(LabyrinthFloor*1.125);
        if (Hero->Energy+Hero->MP>=price)
                {
                cout<<"You are capable of entering the deeper level, proceed?"<<endl;
                string q;
                char a;
                getline(cin,q);
                a=q[0];
                if (a=='Y' or a=='y' or a=='1' or a=='T' or a=='t')
                    {
                     int k=Hero->MP/2;
                     Hero->MP-=k;
                     price-=k;
                     Hero->Energy-=price;
                     NewFloor();
                    }
                }
        else{
            cout<<"You need more energy to power the portal.";
            }
        }


    else if(Level[Hero->isinRoom].Special==1)
        {
        cout<<"You approach a strange, altar-like... anvil? Or maybe anvil-like altar..."<<endl;
        Reforge();
        }
    else if(Level[Hero->isinRoom].Special==2)
        {
        cout<<"You stand in front of a big floating blob of transluscent liquid with multiple streaks of light inside it, and touch it."<<endl;
        Strenghten();
        }
    else if(Level[Hero->isinRoom].Special==4)
        {
        cout<< "You approach the odd egg and try to touch it..."<<endl;
        if(Hero->MP>1 and Hero->Energy>LabyrinthFloor)
            {
            Hero->MP--;
            Hero->Energy-=LabyrinthFloor;
            cout<<"But it shatters and an enemy appears from the inside!"<<endl;
            if(Spawn())
                {
                cout<<"After defeating the enemy, the egg reforms itself."<<endl;
                }
            else{cout<<"You left the room, but the enemy disapeared when you looked back..."<<endl; Level[Hero->isinRoom].isOccupied=true;}
            }
        else {cout<<"and  its smooth surface is cold to the touch... it feels as if it lacked something but you are unshure what exsactly that 'Something' is."<<endl;}
        }
    else if (Level[Hero->isinRoom].Special==3)
        {
        cout<<"You approach the TREASURE CHEST..."<<endl;
            switch(rollD(20))
                {
                case 1:{cout<<"But it's a trap!"<<endl<<"The enemy destroys the chest in which it was hiding and attacks you!"<<endl;
                        Level[Hero->isinRoom].Special=-1;
                        Level[Hero->isinRoom].isOccupied=true;
                        if(Spawn()){Level[Hero->isinRoom].isOccupied=false; cout<< "You dipatched th enemy"<<endl;}
                        else{
                            cout<<"You left the room!"<<endl;
                            Hero->isinRoom=-1;
                            }
                        break;
                        }
                case 2:{
                        Level[Hero->isinRoom].Special=-1;
                        int loot=rollD(5+rollD(2*LabyrinthFloor));
                        cout<<"and find "<<loot<<"Energy inside"<<endl;
                        Hero->Energy+=loot;
                        break;
                        }
                case 13:{Level[Hero->isinRoom].Special=-1;cout<<"..."<<endl<<"It's EMPTY!"<<endl;break;}
                default:{
                        cout<<"It's locked!"<<endl;
                        Level[Hero->isinRoom].LockLevel=1+rollD(6)+LabyrinthFloor+0.1*(rollD(LabyrinthFloor)+max(0,-100+LabyrinthFloor));
                        if((Hero->Mind+1+Hero->Soul/5+Hero->Body/15+Hero->Attack/25)<Level[Hero->isinRoom].LockLevel)
                        {
                        lockcount++;
                        switch(rollD(13))
                            {
                            case 1:{
                                    Level[Hero->isinRoom].Special=-1;
                                    cout<<"The enemy jumps out of the chest... you just opened..."<<endl;
                                    if(Spawn()){Level[Hero->isinRoom].isOccupied=false; cout<< "You dipatched the enemy"<<endl;}
                                        else{
                                            cout<<"You left the room!"<<endl;
                                            Hero->isinRoom=-1;
                                            }
                                    break;
                                    }
                            case 12:{Level[Hero->isinRoom].Special=-1;LootWpn();break;}
                            case 2:{
                                    int loot=2*rollD(5+rollD(2*LabyrinthFloor));
                                    cout<<"and find "<<loot<<"Energy inside"<<endl;
                                    Hero->Energy+=loot;
                                    Level[Hero->isinRoom].Special=-1;
                                    break;
                                        }
                            case 3:{
                                    cout<<"As you open the chest, everything turns Red for a moment...";
                                    Hero->HPmods+=rollD(3);
                                    cout<<" and you feel slighty different afterwards."<<endl;
                                    Level[Hero->isinRoom].Special=-1;
                                    break;
                                    }
                            case 4:{
                                    cout<<"As you open the chest, everything turns Blue for a moment...";
                                    Hero->MPmods+=rollD(3);
                                    cout<<" and you feel slighty different afterwards."<<endl;
                                    Level[Hero->isinRoom].Special=-1;
                                    break;
                                    }
                            case 5:{
                                    cout<<"As you open the chest, everything turns White for a moment...";
                                    Hero->atkmods+=rollD(2);
                                    Level[Hero->isinRoom].Special=-1;
                                    cout<<" and you feel slighty different afterwards."<<endl;
                                    break;
                                    }
                            case 6:{
                                    cout<<"As you open the chest, everything turns Black for a moment...";
                                    Hero->defmods+=rollD(2);
                                    Level[Hero->isinRoom].Special=-1;
                                    cout<<" and you feel slighty different afterwards."<<endl;
                                    break;
                                    }
                            case 7:{
                                    cout<<"As you open the chest, everything turns Silver for a moment...";
                                    Hero->DMGmods+=rollD(2);
                                    Level[Hero->isinRoom].Special=-1;
                                    cout<<" and you feel slighty different afterwards."<<endl;
                                    break;
                                    }
                            case 8:{
                                    cout<<"You find some armour!"<<endl;
                                    Hero->Armour+=rollD(3+LabyrinthFloor);
                                    Level[Hero->isinRoom].Special=-1;
                                    break;
                                    }
                            case 9:{
                                    cout<<"As you open the chest, everything turns Green for a moment...";
                                    Hero->Mind++;
                                    Level[Hero->isinRoom].Special=-1;
                                    cout<<" and you feel slighty different afterwards."<<endl;
                                    break;
                                    }
                            case 10:{
                                    cout<<"As you open the chest, everything turns Cyan for a moment...";
                                    Hero->Soul++;
                                    cout<<" and you feel slighty different afterwards."<<endl;
                                    Level[Hero->isinRoom].Special=-1;
                                    break;
                                    }
                            case 11:{
                                    cout<<"As you open the chest, everything turns Magenta for a moment...";
                                    Hero->Body++;
                                    Level[Hero->isinRoom].Special=-1;
                                    cout<<" and you feel slighty different afterwards."<<endl;
                                    break;
                                    }
                            case 13:{
                                    cout<<"Guess what,"<<endl<<" IT'S EMPTY!"<<endl;
                                    Level[Hero->isinRoom].Special=-1;
                                    break;
                                    }
                            default:{
                                    Hero->Energy++;
                                    cout<<"Nothing interesting... π-π"<<endl;
                                    Level[Hero->isinRoom].Special=-1;
                                    break;
                                    }
                            }
                        }
                        else{cout<<"You were unable(too stupid) to open the Chest without destroying it's contents"<<endl;
                            Level[Hero->isinRoom].Special=-1;
                            Hero->Energy++;
                            break;
                            }
                        }
                }
            Hero->CalculateStats();
        cout<<"The chest abruptly turns into smoke."<<endl;
                }
    }
}

void Search()
{
if(Hero->uzbrojenie.lvl==-1){LootWpn();return;}
if(Hero->isinRoom==-1)
{
this_thread::sleep_for(chrono::milliseconds(404));
cout<<"The corridor you're in is rather unremarkable. Maybe you should explore a bit?"<<endl;
return;
}
else if (Level[Hero->isinRoom].hasSecret)
    {
    if(Hero->Mind>=Level[Hero->isinRoom].SecretreqMind or Level[Hero->isinRoom].SecretKnown)
        {
        if(!Level[Hero->isinRoom].SecretKnown)
            {
            cout<<"You found something!"<<endl;
            Level[Hero->isinRoom].DiscoverSecret();
            }
        if(Hero->Body>=Level[Hero->isinRoom].SecretreqBody and Hero->Soul>=Level[Hero->isinRoom].SecretreqSoul)
            {switch(rollD(8))
                {
                case 1:
                        {
                        Hero->Energy+=rollD(LabyrinthFloor/3+rollD(killcount/10)+rollD(lockcount)+3);
                        cout<<"There was some Energy to collect..."<<endl;
                        Level[Hero->isinRoom].hasSecret=false;
                        break;
                        }
                case 3:{
                        cout<<"You found some Armour!"<<endl;
                        Hero->Armour+=1+LabyrinthFloor/5+killcount/25+Hero->Mind/73+Hero->Soul/13;
                        Level[Hero->isinRoom].hasSecret=false;
                        break;
                        }
                case 4:{
                        cout<<"You found a regeneration potion!"<<endl;
                        Hero->HP+=Hero->MaxHP/3;
                        Hero->MP+=Hero->MaxMp/3;
                        Hero->CalculateStats();
                        Level[Hero->isinRoom].hasSecret=false;
                        break;
                        }
                case 7:{if(rollD(10+rollD(8))>16 and rollD(15-rollD(9))==7){cout<<"You found a map of this floor!";}}
                case 6:{
                string a;
            cout<<"You see a floating blob of... something."<<endl;
            this_thread::sleep_for(chrono::milliseconds(500));
            cout<<"What will you do?"<<endl;
            if(rollD(10)>=9){cout<<"...Will you try to take it?"<<endl;}
            getline(cin,a);
            char c=a[0];
            switch(c)
                {
                case 'A':
                case 'a':
                        {
                        if(Hero->uzbrojenie.lvl>=0)
                            {
                            cout<<"You attack ";
                            switch(rollD(9))
                                {
                                case 1:Hero->uzbrojenie.bazProgress+=1;break;
                                case 2:Hero->uzbrojenie.SoulProgress+=1;break;
                                case 3:
                                case 4:Hero->uzbrojenie.bazProgress+=1;break;
                                case 5:Hero->uzbrojenie.SoulProgress+=1;break;
                                case 6:Hero->uzbrojenie.bazProgress+=2;break;
                                case 7:Hero->uzbrojenie.BodyProgress+=1;break;
                                case 9:Hero->Energy++;break;
                                default: Hero->uzbrojenie.bazProgress++;break;
                                }
                            Level[Hero->isinRoom].hasSecret=false;
                            cout<<"and see as the blob gets absorbed into your weapon"<<endl;
                            Level[Hero->isinRoom].hasSecret=false;
                            break;
                            }
                        }
                case 't':
                case 'T':{cout<<"You reach your hand and touch it..."<<endl;
                        this_thread::sleep_for(chrono::milliseconds(250));
                        switch(rollD(20))
                            {
                            case 1:
                            case 3:
                            case 5:
                            case 7:{Hero->Energy+=3+2*rollD(LabyrinthFloor/1.5);break;}
                            case 6:{Hero->HPmods++;break;}
                            case 4:{Hero->MPmods++;break;}
                            case 2:{Hero->Body++;break;}
                            case 20:{Hero->traces--;break;}
                            case 18:{Hero->Soul++;break;}
                            case 17:{Hero->Mind++;break;}
                            default: {Hero->Energy+=2*rollD(LabyrinthFloor/1.375);break;}
                            }
                        Level[Hero->isinRoom].hasSecret=false;
                        cout<<"It sinks into you... but you don't feel anythig wrong."<<endl;
                        break;
                        }
                default: {cout<<"You observe as it dissipates..."<<endl; this_thread::sleep_for(chrono::milliseconds(875));
                        Level[Hero->isinRoom].hasSecret=false;
                        break;
                        }
                }}
                default:Hero->Energy++;
                Level[Hero->isinRoom].hasSecret=false;
                break;
                }
        }
        else{cout<<"You saw something interesting, but You are incapable of aquiring whatever it is..."<<endl;}
    }
}
else {cout<<"No interestings finds..."<<endl;}
cout<<"Also, you see ";
if(Level[Hero->isinRoom].Special==0){cout<<"a drawing on the floor... it looks like some kind of a portal, but it lacks power..."<<endl;}
else if (Level[Hero->isinRoom].Special==1){cout<<"an anvil-shaped altar, with images of severral weapons engraved onto it's surface... and it seems to emit a faint glow..."<<endl;}
else if (Level[Hero->isinRoom].Special==2){cout<<"a rather big, floating transluscent blob of something, insisde you can se streaks of light? circling around"<<endl<<"you see black, white, red, blue, silver, green, cyan and magenta ones."<<endl;}
else if (Level[Hero->isinRoom].Special==3){cout<<"a chest. A Treasure Chest."<<endl<<"THERE. IS. A. TREASURE. CHEST. IN. THE. ROOM!!!"<<endl;}
else if (Level[Hero->isinRoom].Special==4){cout<<"a...n egg? perhaps... made of stone... and it looks as it would open up any moment now..."<<endl;}
else {cout << "a rather mundane room, albeit the floor seems to be covered in a thin layer of fog"<<endl;}
}

void Relocate()
{
bool knows=false;
if(Hero->isinRoom<=-1){cout<<"Currently you aren't in any room, but you know how to get to these rooms: "<<endl;}
else{cout<<"You are in the room "<<Hero->isinRoom<<" and you know how to get to rooms:"<<endl;}
for(int i=0,disp=0;i<mapa.size();i++)
    {
    if(mapa[i])
        {cout<<i<<" ";disp++;knows=true;}
    if ((disp+1)%10==0)
        {cout<<endl;}
    }
if(!knows){cout<<"Let me corect that statement":<<endl<<"YOU HAVE NO IDEA WHERE (if any) ARE ROOMS";}
cout<<endl;
int go;
go=inputint();
if (go==Hero->isinRoom){cout<<"You changed your mind..."<<endl;}
else if (go<0){Hero->isinRoom=-1;cout<<"You left the room."<<endl;}
else if( go>mapa.size() or mapa[go]==false){cout<<"You can't be sure if such room even exists... so you stay where you are."<<endl;return;}
else{
cout<<"You set off..."<<endl;
cout<<Level[go].isOccupied;
switch(rollD(20))
    {
    case 13:
    case 1:
    case 2:
    case 6:
    case 0:{Spawn();break;}
    case 18:{cout<<"You found some refreshments along the way..."<<endl;
        Hero->HP++;
        Hero->MP++;
        break;}
    case 20:{
            cout<<"You found some Energy along the way"<<endl;
            Hero->Energy+=rollD(rollD(LabyrinthFloor/10)+1);
            }
    default: break;
    }
cout<<endl<<endl<<go<<endl<<endl;
cout<<"And you arrive at your destination!"<<endl;
if (Level[go].isLocked)
    {
    cout<<"but it's locked."<<endl;
    if (1+Hero->Mind+(int)(Hero->Soul*0.75)>=Level[go].LockLevel){this_thread::sleep_for(chrono::milliseconds(600));cout<<"After a while, you break open the lock."<<endl;lockcount++;Level[go].isLocked=false;}
    else{Level[go].LockLevel-=2*rollD((Hero->Mind+(int)(Hero->Soul*0.1))/2);cout<<"You failed to open the lock, but you almost figured it out!"<<endl;Hero->isinRoom=-1;return;}
    }
if(Level[go].isSealed)
    {
    cout<<"You eccountered a magical boundary!";
    int m;
    cout<<"How much MP you want to use to break this barrier?"<<endl;
    m=inputint();
    if ((m*(Hero->Soul+(int)(Hero->Mind*0.1)+(int)(Hero->Body*0.05)))>=Level[go].SealLevel)
        {
        lockcount++;
        Level[go].isSealed=false;
        cout<<"You broke through."<<endl;
        }
    else {Level[go].SealLevel-=(m*(Hero->Soul+(int)(Hero->Mind*0.095)+(int)(Hero->Body*0.045))); cout<<"The barrier seems weakened..."<<endl;Hero->isinRoom=-1;return;}
    }
Hero->isinRoom=go;
}
if(Level[Hero->isinRoom].isOccupied)
    {
    if(Spawn()==true)
        {
        Level[go].isOccupied=false;
        }
    else if(!Hero->isAlive()){cout<<"you got killed..."<<endl;return;}
    else {Hero->isinRoom=-1; cout<<"You left the room, when you looked it was as if the enemy just dissapeared, but it surely will be there once you enter again..."<<endl;}
    }
}

void Explore()
{
if(rpt>0 and !explored){cout<<"...and you wander in search of new places..."<<endl;}
else if(explored){cout<<"You see no point in doing that, since you know this floor like the back of you hand..."<<endl;return;}
else{cout<<"You start to wander around..."<<endl;}
Hero->isinRoom==-1;
switch(rollD(20))
    {
    case 13:this_thread::sleep_for(chrono::milliseconds(130)); break;
    case 1:this_thread::sleep_for(chrono::milliseconds(10));break;
    case 2:this_thread::sleep_for(chrono::milliseconds(20));break;
    case 6:this_thread::sleep_for(chrono::milliseconds(60));break;
    case 0:{(WALKA());break;}
    case 18:{cout<<"You found some refreshments along the way..."<<endl;
        this_thread::sleep_for(chrono::milliseconds(180));
        Hero->HP++;
        Hero->MP++;
        Hero->CalculateStats();
        break;}
    case 20:{
            cout<<"You found some Energy along the way!"<<endl;
            Hero->Energy+=rollD(rollD(LabyrinthFloor/10)+2);
            }
    default: this_thread::sleep_for(chrono::milliseconds(250)); break;
    }

int now=rollD(Level.size())-1;
if(mapa[now]==true and rpt<5 and !explored)
    {
    rpt++;
    Explore();
    return;
    }
else
    {
    explored=true;
    for(int i=0;i<Level.size() and explored;i++)
        {
        if(mapa[i]==false)
            {
            explored=false;
            }
        }
    if(explored)
        {
        cout<<"You have fully explored this floor, maybe it's time to delve deeper?"<<endl;
        }
    }
cout<<"You came to the room "<<now<<" ";
mapa[now]=true;
if (Level[now].isLocked)
    {
    cout<<"but it's locked."<<endl;
    if (Hero->Mind+(int)(Hero->Soul*0.15)>=Level[now].LockLevel){this_thread::sleep_for(chrono::milliseconds(600));cout<<"After a while, you break open the lock."<<endl;lockcount++;Level[now].isLocked=false;}
    else{Level[now].LockLevel-=2*rollD((Hero->Mind+(int)(Hero->Soul*0.1))/2);cout<<"You failed to open the lock, you'll have better luck next time, especially so that you are begining to understand how that one works..."<<endl;
    Hero->isinRoom=-1;
    return;}
    }
if(Level[now].isSealed)
    {
    cout<<"but you eccountered a magical boundary!";
    int m;
    cout<<"How much MP you want to use to break this barrier?"<<endl;
    m=inputint();
    while (m<0 or m>Hero->MP){cout<<"Stop joking..."<<endl; m=0;m=inputint(); }
    if ((m+m*(Hero->Soul+(int)(Hero->Mind*0.1)+(int)(Hero->Body*0.05)))>=Level[now].SealLevel)
        {
        lockcount++;
        Level[now].isSealed=false;
        cout<<"You broke through."<<endl;
        Hero->MP-=m;
        }
    else {Level[now].SealLevel-=(m+m*(Hero->Soul+(int)(Hero->Mind*0.095)+(int)(Hero->Body*0.045))); cout<<"The barrier seems weakened..."<<endl;Hero->MP-=m;Hero->isinRoom=-1;return;}
    }
Hero->isinRoom=now;
if(Level[Hero->isinRoom].isOccupied)
    {
    if(Spawn())
        {
        Level[now].isOccupied=false;
        }
    else {Hero->isinRoom=-1; cout<<"You left the room, when you looked it was sa if the enemy just dissapeared, but it surely will be there one you enter again..."<<endl;}
    }
rpt=0;
}

void WhatToDo()
{
string action="";
char a;
if(!Hero->isAlive())
    {
    return;
    }
cout<<Hero->Name<<", what will you do?"<<endl;
getline(cin,action);
a=(char)action[0];
switch ((int)a)
    {
    case 'c':
    case 'C':{cout<<"---------------------------------------------------------------------------------------"<<endl<<"STATUS:"<<endl;
            cout<<"NAME: "<<Hero->Name<<endl;
            cout<<"Body: "<<Hero->Body<<endl<<"MIND: "<<Hero->Mind<<endl<<"SOUL: "<<Hero->Soul<<endl;
            cout<<Hero->HP<<"/"<<Hero->MaxHP<<"HP  "<<Hero->MP<<"/"<<Hero->MaxMp<<"MP"<<endl;
            if (Hero->Armour>0){cout<<"Armour: "<<Hero->Armour<<endl;}
            cout<<"Energy:"<<Hero->Energy<<endl;
            cout<<"Weapon: "<<Hero->uzbrojenie.Name<<endl;
            if(Hero->uzbrojenie.bound)
                {
                cout<<"§Foundation: "<<Hero->uzbrojenie.bazDMG<<endl<<"§Sharpness: "<<Hero->uzbrojenie.BodyDMGmod<<endl<<"§Aura: "<<Hero->uzbrojenie.SoulDMGmod<<endl<<"§Precision: "<<Hero->uzbrojenie.MindDMGmod<<endl;
                cout<<"Damage Output: "<<Hero->DMG<<endl;
                }
            else{cout<<"DMG:"<<Hero->uzbrojenie.bazDMG<<endl;}
            break;
            }
    case 'A':
    case 'a':{if(rollD(100)<=2){Hero->HP-=(Hero->DMG-Hero->Armour);if(Hero->Armour>0){Hero->Armour--;} cout<<"You managed to hurt yourself..."<<endl;if(Hero->HP<=0){killcount++;}}else {cout<<"The surrounding air seems to be unperturbed by your attack."<<endl;}break;}
    case 'D':
    case 'd':{  cout<<"ARE YOU COMPLETELY SURE OF WHAT YOU ARE DOING?"<<endl;
                string areyousure;
                getline(cin,areyousure);
                if(areyousure.compare("YES")==0 or areyousure.compare("Yes")==0)
                {Hero->HP-=(Hero->HP)*10;
                break;}
                else{cout<<"That was close, almost lost you there..."<<endl;break;}
            }
    case 'g':
    case 'G':{Relocate();break;}
    case 'E':
    case 'e':{
            Explore();
            break;
            }
    case 'r':
    case 'R':{cout<<"you rest for a while..."<<endl;
           this_thread::sleep_for(chrono::milliseconds(600));
           if(rollD(100)>10){this_thread::sleep_for(chrono::milliseconds(440));Hero->HP+=Hero->MaxHP/2;Hero->MP=Hero->MaxMp; cout<<"You recovered"<<Hero->MaxHP/2<<"HP & all the mising MP"<<endl; Hero->CalculateStats();}
           else{cout<<"You have been attacked while you were resting!"<<endl;
           Spawn();
           }
           break;
           }
    case 'S':
    case 's':{Search();break;}
    case 'I':
    case 'i':{Interact();break;}
    case '?':{cout<<endl<<"---------------------------------------------------------------"<<endl<<"? -> shows this"<<endl<<"a -> attacks"<<endl<<"s -> searches around"<<endl<<"i -> iteracts with whatever there is to interact with (usually nothing)"<<endl<<"e -> explores the area"<<endl<<"g -> allows to move to a previously visited room"<<endl<<"There are some more but you'll have to fid out on your own..."<<endl<<"---------------------------------------------------------------"<<endl;break;}
    default:{cout<<"Sorry, it's impossible."<<endl;}
    }
}

int main()
{
rpt=false;
NG=true;
cout<<"---------------------------------------------------------------------------------"<<endl;
cout<<endl<<"---------------------------------------------------------------"<<endl<<"? -> shows this"<<endl<<"a -> attacks"<<endl<<"s -> searches around"<<endl<<"i -> iteracts with whatever there is to interact with (usually nothing)"<<endl<<"e -> explores the area"<<endl<<"There are some more but you'll have to fid out on your own..."<<endl<<" oh, yes, 'd' is commiting sucide, just so you know"<<endl<<"---------------------------------------------------------------"<<endl;
cout<<"**********************"<<endl<<"Pro Tips:"<<endl<<"most of the time answer beggining with letter 'D' (or 'd') can be lethal for you"<<endl<<"SEARCH EVERYTHING!"<<endl<<"Starting floor is a Tutorial of sorts..."<<"Enery is a currency used for several things, so if something doesn't work, but it looks as if it should, it probaby means that you are too poor"<<endl<<"**********************"<<endl;
Hero=new Creature();
Enemy=new Creature();
srand(time(NULL));
Hero->isinRoom=-1;
Hero->HPmods=3+rollD(5);
Hero->MPmods=3+rollD(5);
Hero->Body=1;
Hero->Soul=1;
Hero->Mind=1;
Hero->uzbrojenie.Name="fist";
Hero->uzbrojenie.bazDMG=1;
Hero->uzbrojenie.BodyDMGmod=0;
Hero->uzbrojenie.MindDMGmod=0;
Hero->uzbrojenie.SoulDMGmod=0;
Hero->CalculateStats();
killcount=0;
cout<<"How, should I call you?"<<endl;
getline(cin,Hero->Name);
cout<<"Welcome... "<<endl;
this_thread::sleep_for(chrono::milliseconds(875));
NewFloor();
cout<<"---------------------------------------------------------------------------------------"<<endl<<"STATUS:"<<endl;
            cout<<"NAME: "<<Hero->Name<<endl;
            cout<<"Body: "<<Hero->Body<<endl<<"MIND: "<<Hero->Mind<<endl<<"SOUL: "<<Hero->Soul<<endl;
            cout<<Hero->HP<<"/"<<Hero->MaxHP<<"HP  "<<Hero->MP<<"/"<<Hero->MaxMp<<"MP"<<endl;
            if (Hero->Armour>0){cout<<"Armour: "<<Hero->Armour<<endl;}
            cout<<"Weapon: "<<Hero->uzbrojenie.Name<<endl<<"Base Damage: "<<Hero->uzbrojenie.bazDMG<<endl;
while(Hero->isAlive())
    {
    WhatToDo();
    }
this_thread::sleep_for(chrono::milliseconds(666));
cout<<"And so... ";
this_thread::sleep_for(chrono::milliseconds(100));
cout<<Hero->Name<< " has fallen..."<<endl;
delete Enemy;
ofstream scorelog;
scorelog.open("Scorelog.txt", ios_base::app);
scorelog<<Hero->Name<<": Enemies Defeated: "<<killcount<<" | Floor: "<<LabyrinthFloor<<endl<<" | Obstacles Overcame: "<<lockcount<<endl;
cout<<"---------------------------------------------------------------------------------"<<endl;
cout<<Hero->Name<<":"<<endl<<" Enemies Defeated: "<<killcount<<endl<<"Floor: "<<LabyrinthFloor<<endl<<"Obstacles Overcame: "<<lockcount<<endl;
cout<<"---------------------------------------------------------------------------------"<<endl;
cout<<"retry?[y/N]"<<endl;
delete Hero;
string q;
getline(cin,q);
if (q[0]=='y' or q[0]=='Y'){main();}
return 0;
}
//Armoury explanation: Name(bez Spacji) DMG_baz Body_Mult Soul_mult Mind_mult Body_req Soul_req Mind_req Body_Mult_Upgrade_Difficulty Soul_Mult_Upgrade_Difficulty Mind_Mult_Upgrade_Difficulty
