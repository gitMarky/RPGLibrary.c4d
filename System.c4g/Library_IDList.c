/*
The ID list contains static constants that refer to IDs. Scripts should use constants from this list, but this is not done in every script 
at the moment. The advantage is, that you can switch certain objects by overloading the constant, without having to overload the object.
@title ID List
@author Marky
@version 0.1.0
@file indexIDList.html
*/

#strict 2

static const ID_CharMenu = ROCK;

// Menu IDs
// Char menu

static const ID_Menu_Char = LIMC;
static const ID_Menu_CharMenuBar = LISB;
static const ID_Menu_CharButton = LIBU;
static const ID_Menu_Avatar = LIAV;
static const ID_Menu_Equipment = LIES;
static const ID_Menu_Combo = LICM;
static const ID_Menu_ComboSymbol = LICS;

static const ID_MenuIcon_StandardButton = LIGH;
static const ID_MenuIcon_MouseCursor = MCCR;

// Ambience

static const ID_Ambience_Fireflies = FFAC; // the spawner

// Objects

static const ID_Object_CaveElevator = _CEL;
static const ID_Object_CandleHolder = _CHO;
static const ID_Object_WoodenBarricade = _WBR;
static const ID_Object_Greenstuff = _VEG;
static const ID_Object_BigRock1 = RO13;
static const ID_Object_BigRock2 = RO69;
static const ID_Object_Twines =  S_RN;
static const ID_Object_Cobweb = SWEB;
static const ID_Object_StoneIdol2 = IL04;
static const ID_Object_Cart = _CRT;
static const ID_Object_Edge = _EDG;
static const ID_Object_Furframe = _FUR;
static const ID_Object_Skinframe = SKN_;
static const ID_Object_BigCage = _CAG;

static const ID_Passage_Staircase = _SCS;
static const ID_Passage_Cave = CAVE;

// Menu Decos

static const ID_MenuIcon_Default = _CAM;
static const ID_MenuDeco_Default = MD69;
static const ID_MenuDeco_Wood = MD69;
static const ID_MenuDeco_Temple = MD70;

// Ring menu

static const ID_Menu_Ring = LIRM;
static const ID_Menu_RingSymbol = LIRS;

// Functionality

static const ID_Func_Item = LF_I;
static const ID_Func_Readable = LF_R;
static const ID_Func_Overlay = LF_O;
static const ID_QuestHandler = LF_Q;

// Libraries

static const ID_Library_DayNightCycle = LF_C;
static const ID_Library_Decoration = LF_D;
static const ID_Library_Readable = LF_R;

// Helper objects

static const ID_Helper_Story = _STY;
static const ID_Helper_DialogueBuilder = _DLB;
static const ID_Helper_QuestBuilder = __QB;

static const ID_Helper_ScreenFader = _SCF;

static const ID_Helper_DayNightCycleController = DNCC;

static const ID_Helper_DefaultPortrait = CLNK;

