/*--
The ID list contains static constants that refer to IDs. Scripts should use constants from this list, but this is not done in every script 
at the moment. The advantage is, that you can switch certain objects by overloading the constant, without having to overload the object.
@title ID List
@author Marky
@version 0.1.0
--*/

#strict 2

static const ID_CharMenu = ROCK;

// Menu IDs
// Charmenu
static const ID_Menu_Char = LIMC;
static const ID_Menu_CharMenuBar = LISB;
static const ID_Menu_CharButton = LIBU;
static const ID_Menu_Avatar = LIAV;
static const ID_Menu_Equipment = LIES;
static const ID_Menu_Combo = LICM;
static const ID_Menu_ComboSymbol = LICS;

static const ID_Menu_StandardButton = LIGH;

// Menu Decos
static const ID_MenoDeco_Wood = MD69;
static const ID_MenuDeco_Temple = MD70;

// Ringmenu
static const ID_Menu_Ring = LIRM;
static const ID_Menu_RingSymbol = LIRS;

// Funktionalitäten
static const ID_Func_Item = LF_I;
static const ID_Func_Readable = LF_R;
static const ID_Func_Overlay = LF_O;
static const ID_QuestHandler = LF_Q;

// Hilfsobjekte
static const ID_Helper_DialogueBuilder = _DLB;


/* Effektliste mit Beschreibung / Funktionen
 *
 * AddSpawnEffect( pTarget, RGBa1, [ szParticle, RGBa2, iSize = 5, -1/0/1 ]) ein Strudeleffekt, aus Hazard
 *
 */
