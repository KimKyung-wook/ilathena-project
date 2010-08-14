// Copyright (c) Athena Dev Teams - Licensed under GNU GPL
// For more information, see LICENCE in the main folder

#include "../common/nullpo.h"
#include "../common/malloc.h"
#include "../common/showmsg.h"
#include "../common/strlib.h"
#include "itemdb.h"
#include "map.h"
#include "battle.h" // struct battle_config
#include "script.h" // item script processing
#include "pc.h"     // W_MUSICAL, W_WHIP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 32k array entries (the rest goes to the db)
//#define MAX_ITEMDB 0x8000 // itemdb.h 로 이동



static struct item_data* itemdb_array[MAX_ITEMDB];
static DBMap*            itemdb_other;// int nameid -> struct item_data*

static struct item_group itemgroup_db[MAX_ITEMGROUP];

struct item_data dummy_item; //This is the default dummy item used for non-existant items. [Skotlex]

// iLAthena 추가시작 [사용자정의아이템드롭시스템]
struct item_global_drop *item_global_drop;
extern int max_global_drop=0;
// iLAthena 추가종료 [사용자정의아이템드롭시스템]

/*==========================================
 * 뼹멟궳뙚랊뾭
 *------------------------------------------*/
// name = item alias, so we should find items aliases first. if not found then look for "jname" (full name)
static int itemdb_searchname_sub(DBKey key,void *data,va_list ap)
{
	struct item_data *item=(struct item_data *)data,**dst,**dst2;
	char *str;
	str=va_arg(ap,char *);
	dst=va_arg(ap,struct item_data **);
	dst2=va_arg(ap,struct item_data **);
	if(item == &dummy_item) return 0;

	//Absolute priority to Aegis code name.
	if (*dst != NULL) return 0;
	if( strcmpi(item->name,str)==0 )
		*dst=item;

	//Second priority to Client displayed name.
	if (*dst2 != NULL) return 0;
	if( strcmpi(item->jname,str)==0 )
		*dst2=item;
	return 0;
}

/*==========================================
 * 뼹멟궳뙚랊
 *------------------------------------------*/
struct item_data* itemdb_searchname(const char *str)
{
	struct item_data* item;
	struct item_data* item2=NULL;
	int i;

	for( i = 0; i < ARRAYLENGTH(itemdb_array); ++i )
	{
		item = itemdb_array[i];
		if( item == NULL )
			continue;

		// Absolute priority to Aegis code name.
		if( strcasecmp(item->name,str) == 0 )
			return item;

		//Second priority to Client displayed name.
		if( strcasecmp(item->jname,str) == 0 )
			item2 = item;
	}

	item = NULL;
	itemdb_other->foreach(itemdb_other,itemdb_searchname_sub,str,&item,&item2);
	return item?item:item2;
}

static int itemdb_searchname_array_sub(DBKey key,void * data,va_list ap)
{
	struct item_data *item=(struct item_data *)data;
	char *str;
	str=va_arg(ap,char *);
	if (item == &dummy_item)
		return 1; //Invalid item.
	if(stristr(item->jname,str))
		return 0;
	if(stristr(item->name,str))
		return 0;
	return strcmpi(item->jname,str);
}

/*==========================================
 * Founds up to N matches. Returns number of matches [Skotlex]
 *------------------------------------------*/
int itemdb_searchname_array(struct item_data** data, int size, const char *str)
{
	struct item_data* item;
	int i;
	int count=0;

	// Search in the array
	for( i = 0; i < ARRAYLENGTH(itemdb_array); ++i )
	{
		item = itemdb_array[i];
		if( item == NULL )
			continue;

		if( stristr(item->jname,str) || stristr(item->name,str) )
		{
			if( count < size )
				data[count] = item;
			++count;
		}
	}

	// search in the db
	if( count >= size )
	{
		data = NULL;
		size = 0;
	}
	else
	{
		data -= count;
		size -= count;
	}
	return count + itemdb_other->getall(itemdb_other,(void**)data,size,itemdb_searchname_array_sub,str);
}


/*==========================================
 * 뵠똭귺귽긡?뙚랊
 *------------------------------------------*/
int itemdb_searchrandomid(int group)
{
	if(group<1 || group>=MAX_ITEMGROUP) {
		ShowError("아이템 데이터베이스 임의코드검색: 그룹ID오류 %d\n", group);
		return UNKNOWN_ITEM_ID;
	}
	if (itemgroup_db[group].qty)
		return itemgroup_db[group].nameid[rand()%itemgroup_db[group].qty];
	
	ShowError("아이템 데이터베이스 임의코드검색: 그룹ID %d 아이템 오류\n", group);
	return UNKNOWN_ITEM_ID;
}

/*==========================================
 * Calculates total item-group related bonuses for the given item
 *------------------------------------------*/
int itemdb_group_bonus(struct map_session_data* sd, int itemid)
{
	int bonus = 0, i, j;
	for (i=0; i < MAX_ITEMGROUP; i++) {
		if (!sd->itemgrouphealrate[i])
			continue;
		ARR_FIND( 0, itemgroup_db[i].qty, j, itemgroup_db[i].nameid[j] == itemid );
		if( j < itemgroup_db[i].qty )
			bonus += sd->itemgrouphealrate[i];
	}
	return bonus;
}

/// Searches for the item_data.
/// Returns the item_data or NULL if it does not exist.
struct item_data* itemdb_exists(int nameid)
{
	struct item_data* item;

	if( nameid >= 0 && nameid < ARRAYLENGTH(itemdb_array) )
		return itemdb_array[nameid];
	item = (struct item_data*)idb_get(itemdb_other,nameid);
	if( item == &dummy_item )
		return NULL;// dummy data, doesn't exist
	return item;
}

/*==========================================
 * Converts the jobid from the format in itemdb 
 * to the format used by the map server. [Skotlex]
 *------------------------------------------*/
static void itemdb_jobid2mapid(unsigned int *bclass, unsigned int jobmask)
{
	int i;
	bclass[0]= bclass[1]= bclass[2]= 0;
	//Base classes
	if (jobmask & 1<<JOB_NOVICE)
	{	//Both Novice/Super-Novice are counted with the same ID
		bclass[0] |= 1<<MAPID_NOVICE;
		bclass[1] |= 1<<MAPID_NOVICE;
	}
	for (i = JOB_NOVICE+1; i <= JOB_THIEF; i++)
	{
		if (jobmask & 1<<i)
			bclass[0] |= 1<<(MAPID_NOVICE+i);
	}
	//2-1 classes
	if (jobmask & 1<<JOB_KNIGHT)
		bclass[1] |= 1<<MAPID_SWORDMAN;
	if (jobmask & 1<<JOB_PRIEST)
		bclass[1] |= 1<<MAPID_ACOLYTE;
	if (jobmask & 1<<JOB_WIZARD)
		bclass[1] |= 1<<MAPID_MAGE;
	if (jobmask & 1<<JOB_BLACKSMITH)
		bclass[1] |= 1<<MAPID_MERCHANT;
	if (jobmask & 1<<JOB_HUNTER)
		bclass[1] |= 1<<MAPID_ARCHER;
	if (jobmask & 1<<JOB_ASSASSIN)
		bclass[1] |= 1<<MAPID_THIEF;
	//2-2 classes
	if (jobmask & 1<<JOB_CRUSADER)
		bclass[2] |= 1<<MAPID_SWORDMAN;
	if (jobmask & 1<<JOB_MONK)
		bclass[2] |= 1<<MAPID_ACOLYTE;
	if (jobmask & 1<<JOB_SAGE)
		bclass[2] |= 1<<MAPID_MAGE;
	if (jobmask & 1<<JOB_ALCHEMIST)
		bclass[2] |= 1<<MAPID_MERCHANT;
	if (jobmask & 1<<JOB_BARD)
		bclass[2] |= 1<<MAPID_ARCHER;
//	Bard/Dancer share the same slot now.
//	if (jobmask & 1<<JOB_DANCER)
//		bclass[2] |= 1<<MAPID_ARCHER;
	if (jobmask & 1<<JOB_ROGUE)
		bclass[2] |= 1<<MAPID_THIEF;
	//Special classes that don't fit above.
	if (jobmask & 1<<21) //Taekwon boy
		bclass[0] |= 1<<MAPID_TAEKWON;
	if (jobmask & 1<<22) //Star Gladiator
		bclass[1] |= 1<<MAPID_TAEKWON;
	if (jobmask & 1<<23) //Soul Linker
		bclass[2] |= 1<<MAPID_TAEKWON;
	if (jobmask & 1<<JOB_GUNSLINGER)
		bclass[0] |= 1<<MAPID_GUNSLINGER;
	if (jobmask & 1<<JOB_NINJA)
		bclass[0] |= 1<<MAPID_NINJA;
}

static void create_dummy_data(void)
{
	memset(&dummy_item, 0, sizeof(struct item_data));
	dummy_item.nameid=500;
	dummy_item.weight=1;
	dummy_item.value_sell=1;
	dummy_item.type=IT_ETC; //Etc item
	safestrncpy(dummy_item.name,"UNKNOWN_ITEM",sizeof(dummy_item.name));
	safestrncpy(dummy_item.jname,"UNKNOWN_ITEM",sizeof(dummy_item.jname));
	dummy_item.view_id=UNKNOWN_ITEM_ID;
}

static struct item_data* create_item_data(int nameid)
{
	struct item_data *id;
	CREATE(id, struct item_data, 1);
	id->nameid = nameid;
	id->weight = 1;
	id->type = IT_ETC;
	return id;
}

/*==========================================
 * Loads (and creates if not found) an item from the db.
 *------------------------------------------*/
struct item_data* itemdb_load(int nameid)
{
	struct item_data *id;

	if( nameid >= 0 && nameid < ARRAYLENGTH(itemdb_array) )
	{
		id = itemdb_array[nameid];
		if( id == NULL || id == &dummy_item )
			id = itemdb_array[nameid] = create_item_data(nameid);
		return id;
	}

	id = (struct item_data*)idb_get(itemdb_other, nameid);
	if( id == NULL || id == &dummy_item )
	{
		id = create_item_data(nameid);
		idb_put(itemdb_other, nameid, id);
	}
	return id;
}

/*==========================================
 * Loads an item from the db. If not found, it will return the dummy item.
 *------------------------------------------*/
struct item_data* itemdb_search(int nameid)
{
	struct item_data* id;
	if( nameid >= 0 && nameid < ARRAYLENGTH(itemdb_array) )
		id = itemdb_array[nameid];
	else
		id = (struct item_data*)idb_get(itemdb_other, nameid);

	if( id == NULL )
	{
		ShowWarning("itemdb_search: Item ID %d does not exists in the item_db. Using dummy data.\n", nameid);
		id = &dummy_item;
		dummy_item.nameid = nameid;
	}
	return id;
}

/*==========================================
 * Returns if given item is a player-equippable piece.
 *------------------------------------------*/
int itemdb_isequip(int nameid)
{
	int type=itemdb_type(nameid);
	switch (type) {
		case IT_WEAPON:
		case IT_ARMOR:
		case IT_AMMO:
			return 1;
		default:
			return 0;
	}
}

/*==========================================
 * Alternate version of itemdb_isequip
 *------------------------------------------*/
int itemdb_isequip2(struct item_data *data)
{ 
	nullpo_retr(0, data);
	switch(data->type) {
		case IT_WEAPON:
		case IT_ARMOR:
		case IT_AMMO:
			return 1;
		default:
			return 0;
	}
}

/*==========================================
 * Returns if given item's type is stackable.
 *------------------------------------------*/
int itemdb_isstackable(int nameid)
{
  int type=itemdb_type(nameid);
  switch(type) {
	  case IT_WEAPON:
	  case IT_ARMOR:
	  case IT_PETEGG:
	  case IT_PETARMOR:
		  return 0;
	  default:
		  return 1;
  }
}

/*==========================================
 * Alternate version of itemdb_isstackable
 *------------------------------------------*/
int itemdb_isstackable2(struct item_data *data)
{
  nullpo_retr(0, data);
  switch(data->type) {
	  case IT_WEAPON:
	  case IT_ARMOR:
	  case IT_PETEGG:
	  case IT_PETARMOR:
		  return 0;
	  default:
		  return 1;
  }
}


/*==========================================
 * Trade Restriction functions [Skotlex]
 *------------------------------------------*/
int itemdb_isdropable_sub(struct item_data *item, int gmlv, int unused)
{
	return (item && (!(item->flag.trade_restriction&1) || gmlv >= item->gm_lv_trade_override));
}

int itemdb_cantrade_sub(struct item_data* item, int gmlv, int gmlv2)
{
	return (item && (!(item->flag.trade_restriction&2) || gmlv >= item->gm_lv_trade_override || gmlv2 >= item->gm_lv_trade_override));
}

int itemdb_canpartnertrade_sub(struct item_data* item, int gmlv, int gmlv2)
{
	return (item && (item->flag.trade_restriction&4 || gmlv >= item->gm_lv_trade_override || gmlv2 >= item->gm_lv_trade_override));
}

int itemdb_cansell_sub(struct item_data* item, int gmlv, int unused)
{
	return (item && (!(item->flag.trade_restriction&8) || gmlv >= item->gm_lv_trade_override));
}

int itemdb_cancartstore_sub(struct item_data* item, int gmlv, int unused)
{	
	return (item && (!(item->flag.trade_restriction&16) || gmlv >= item->gm_lv_trade_override));
}

int itemdb_canstore_sub(struct item_data* item, int gmlv, int unused)
{	
	return (item && (!(item->flag.trade_restriction&32) || gmlv >= item->gm_lv_trade_override));
}

int itemdb_canguildstore_sub(struct item_data* item, int gmlv, int unused)
{	
	return (item && (!(item->flag.trade_restriction&64) || gmlv >= item->gm_lv_trade_override));
}

int itemdb_isrestricted(struct item* item, int gmlv, int gmlv2, int (*func)(struct item_data*, int, int))
{
	struct item_data* item_data = itemdb_search(item->nameid);
	int i;

	if (!func(item_data, gmlv, gmlv2))
		return 0;
	
	if(item_data->slot == 0 || itemdb_isspecial(item->card[0]))
		return 1;
	
	for(i = 0; i < item_data->slot; i++) {
		if (!item->card[i]) continue;
		if (!func(itemdb_search(item->card[i]), gmlv, gmlv2))
			return 0;
	}
	return 1;
}

/*==========================================
 *	Specifies if item-type should drop unidentified.
 *------------------------------------------*/
int itemdb_isidentified(int nameid)
{
	int type=itemdb_type(nameid);
	switch (type) {
		case IT_WEAPON:
		case IT_ARMOR:
		case IT_PETARMOR:
		// iLAthena 수정 [아이템감정]
			return (battle_config.drop_item_identify == 1 ? 1:0);
		default:
			return 1;
	}
}

/*==========================================
 * 귺귽긡?럊뾭됀?긲깋긐궻긆?긫?깋귽긤
 *------------------------------------------*/
static int itemdb_read_itemavail (void)
{
	FILE *fp;
	int nameid, j, k, ln = 0;
	char line[1024], *str[10], *p;
	struct item_data *id;

	sprintf(line, "%s/item_avail.txt", db_path);
	if ((fp = fopen(line,"r")) == NULL) {
		ShowError("can't read %s\n", line);
		return -1;
	}

	while(fgets(line, sizeof(line), fp))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;
		memset(str, 0, sizeof(str));
		for (j = 0, p = line; j < 2 && p; j++) {
			str[j] = p;
			p = strchr(p, ',');
			if(p) *p++ = 0;
		}

		if (j < 2 || str[0] == NULL ||
			(nameid = atoi(str[0])) < 0 || !(id = itemdb_exists(nameid)))
			continue;

		k = atoi(str[1]);
		if (k > 0) {
			id->flag.available = 1;
			id->view_id = k;
		} else
			id->flag.available = 0;
		ln++;
	}
	fclose(fp);
	ShowStatus("'"CL_WHITE"item_avail.txt"CL_RESET"' 에서 '"CL_WHITE"%d"CL_RESET"' 개의 자료를 읽었습니다.\n", ln);

	return 0;
}

/*==========================================
 * read item group data
 *------------------------------------------*/
static void itemdb_read_itemgroup_sub(const char* filename)
{
	FILE *fp;
	char line[1024];
	int ln=0;
	int groupid,j,k,nameid;
	char *str[3],*p;
	char w1[1024], w2[1024];
	
	if( (fp=fopen(filename,"r"))==NULL ){
		ShowError("can't read %s\n", filename);
		return;
	}

	while(fgets(line, sizeof(line), fp))
	{
		ln++;
		if(line[0]=='/' && line[1]=='/')
			continue;
		if(strstr(line,"import")) {
			if (sscanf(line, "%[^:]: %[^\r\n]", w1, w2) == 2 &&
				strcmpi(w1, "import") == 0) {
				itemdb_read_itemgroup_sub(w2);
				continue;
			}
		}
		memset(str,0,sizeof(str));
		for(j=0,p=line;j<3 && p;j++){
			str[j]=p;
			p=strchr(p,',');
			if(p) *p++=0;
		}
		if(str[0]==NULL)
			continue;
		if (j<3) {
			if (j>1) //Or else it barks on blank lines...
				ShowWarning("itemdb_read_itemgroup: Insufficient fields for entry at %s:%d\n", filename, ln);
			continue;
		}
		groupid = atoi(str[0]);
		if (groupid < 0 || groupid >= MAX_ITEMGROUP) {
			ShowWarning("itemdb_read_itemgroup: Invalid group %d in %s:%d\n", groupid, filename, ln);
			continue;
		}
		nameid = atoi(str[1]);
		if (!itemdb_exists(nameid)) {
			ShowWarning("itemdb_read_itemgroup: Non-existant item %d in %s:%d\n", nameid, filename, ln);
			continue;
		}
		k = atoi(str[2]);
		if (itemgroup_db[groupid].qty+k >= MAX_RANDITEM) {
			ShowWarning("itemdb_read_itemgroup: Group %d is full (%d entries) in %s:%d\n", groupid, MAX_RANDITEM, filename, ln);
			continue;
		}
		for(j=0;j<k;j++)
			itemgroup_db[groupid].nameid[itemgroup_db[groupid].qty++] = nameid;
	}
	fclose(fp);
	return;
}

static void itemdb_read_itemgroup(void)
{
	char path[256];
	snprintf(path, 255, "%s/item_group_db.txt", db_path);

	memset(&itemgroup_db, 0, sizeof(itemgroup_db));
	itemdb_read_itemgroup_sub(path);
	ShowStatus("'"CL_WHITE"item_group_db.txt"CL_RESET"' 자료를 읽었습니다.\n");
	return;
}

/*==========================================
 * 몧뷈맕뙽긲?귽깑벶귒뢯궢
 *------------------------------------------*/
static int itemdb_read_noequip(void)
{
	FILE *fp;
	char line[1024];
	int ln=0;
	int nameid,j;
	char *str[32],*p;
	struct item_data *id;

	sprintf(line, "%s/item_noequip.txt", db_path);
	if( (fp=fopen(line,"r"))==NULL ){
		ShowError("can't read %s\n", line);
		return -1;
	}
	while(fgets(line, sizeof(line), fp))
	{
		if(line[0]=='/' && line[1]=='/')
			continue;
		memset(str,0,sizeof(str));
		for(j=0,p=line;j<2 && p;j++){
			str[j]=p;
			p=strchr(p,',');
			if(p) *p++=0;
		}
		if(str[0]==NULL)
			continue;

		nameid=atoi(str[0]);
		if(nameid<=0 || !(id=itemdb_exists(nameid)))
			continue;

		id->flag.no_equip |= atoi(str[1]);

		ln++;

	}
	fclose(fp);
	if (ln > 0) {
		ShowStatus("'"CL_WHITE"item_noequip.txt"CL_RESET"' 에서 '"CL_WHITE"%d"CL_RESET"' 개의 자료를 읽었습니다.\n", ln);
	}	
	return 0;
}

/*==========================================
 * Reads item trade restrictions [Skotlex]
 *------------------------------------------*/
static int itemdb_read_itemtrade(void)
{
	FILE *fp;
	int nameid, j, flag, gmlv, ln = 0;
	char line[1024], *str[10], *p;
	struct item_data *id;

	sprintf(line, "%s/item_trade.txt", db_path);
	if ((fp = fopen(line,"r")) == NULL) {
		ShowError("can't read %s\n", line);
		return -1;
	}

	while(fgets(line, sizeof(line), fp))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;
		memset(str, 0, sizeof(str));
		for (j = 0, p = line; j < 3 && p; j++) {
			str[j] = p;
			p = strchr(p, ',');
			if(p) *p++ = 0;
		}

		if (j < 3 || str[0] == NULL ||
			(nameid = atoi(str[0])) < 0 || !(id = itemdb_exists(nameid)))
			continue;

		flag = atoi(str[1]);
		gmlv = atoi(str[2]);
		
		if (flag > 0 && flag < 128 && gmlv > 0) { //Check range
			id->flag.trade_restriction = flag;
			id->gm_lv_trade_override = gmlv;
			ln++;
		}
	}
	fclose(fp);
	ShowStatus("'"CL_WHITE"item_trade.txt"CL_RESET"' 에서 '"CL_WHITE"%d"CL_RESET"' 개의 자료를 읽었습니다.\n", ln);

	return 0;
}

/*======================================
 * 카드확률조합시스템[★orz。]
 *======================================*/
int itemdb_read_probcard(void)
{
	FILE *fp;
	int nameid, j, percent, del, ln = 0;
	char line[1024], *str[10], *p;
	struct item_data *id;

	sprintf(line, "%s/item_probcard.txt", db_path);
	if ((fp = fopen(line,"r")) == NULL) {
		ShowError("%s 를 읽지 못하였습니다.\n", db_path);
		return -1;
	}

	while(fgets(line, sizeof(line), fp))
	{
		if (line[0] == '/' && line[1] == '/')
			continue;
		memset(str, 0, sizeof(str));
		for (j = 0, p = line; j < 3 && p; j++) {
			str[j] = p;
			p = strchr(p, ',');
			if(p) *p++ = 0;
		}

		if (j < 3 || str[0] == NULL || (nameid = atoi(str[0])) < 0 || !(id = itemdb_exists(nameid)))
			continue;
		if (id->type != 6)	// 설정아이템이카드인지필터링
			continue;

		percent = atoi(str[1]);
		del = atoi(str[2]);
		
		if (percent > 0 && percent <= 10000 && del >= 0) { //Check range
			id->probcard_percentage = percent;
			id->probcard_del = del;
			ln++;
		}
	}
	fclose(fp);
	ShowStatus("'"CL_WHITE"item_probcard.txt"CL_RESET"' 에서 '"CL_WHITE"%d"CL_RESET"' 개의 자료를 읽었습니다.\n", ln);

	return 0;
}

/*==========================================
 * 차암 시스템 [페퍼민트]
 *------------------------------------------*/
static int itemdb_read_keepitem(void)
{

	uint32 lines = 0, count = 0, nameid;
	char line[1024];
	char path[256];
	FILE* fp;
	struct item_data *id;

	sprintf(path, "%s/item_keepitem.txt", db_path);
	fp = fopen(path, "r");
	if( fp == NULL )
	{
		ShowError("%s 를 읽지 못하였습니다.\n", path);
		return -1;
	}

	// process rows one by one
	while(fgets(line, sizeof(line), fp))
	{
		char *str[7], *p;
		int i;

		lines++;
		if(line[0] == '/' && line[1] == '/')
			continue;
		memset(str, 0, sizeof(str));

		p = line;
		while( ISSPACE(*p) )
			++p;
		if( *p == '\0' )
			continue;// empty line
		for( i = 0; i < 6; i++ )
		{
			str[i] = p;
			p = strchr(p,',');
			if( p == NULL )
				break;// comma not found
			*p = '\0';
			++p;
		}

		if(str[0]==NULL)
			continue;

		nameid=atoi(str[0]);

		if(nameid<=0 || !(id=itemdb_exists(nameid)))
			continue;

		if( p == NULL )
		{
			ShowError("itemdb_read_keepitem: \"%s\"파일의 %d번째 줄에서 잘못된 형식을 취하고있으므로 무시합니다. (item id %d)\n", path, lines, nameid);
			continue;
		}

		// Script
		if( *p != '{' )
		{
			ShowError("itemdb_read_keepitem: \"%s\"파일의 %d번째 줄의 스크립트부분에서 잘못된 형식을 취하고있으므로 무시합니다. (item id %d)\n", path, lines, nameid);
			continue;
		}
		str[6]=p;

		id->ki.exist = true;
		id->ki.del = (atoi(str[1]) > 0)?1:0;
		id->ki.ref = (atoi(str[2]) > 0)?1:0;
		id->ki.pvp = (atoi(str[3]) > 0)?1:0;
		id->ki.gvg = (atoi(str[4]) > 0)?1:0;
		id->ki.addeff = (atoi(str[5]) > 0)?1:0;

		if (id->ki.script)
		{
			script_free_code(id->ki.script);
			id->ki.script = NULL;
		}

		if (str[6])
			id->ki.script = parse_script(str[6], line, lines, 0);

		count++;
	}

	fclose(fp);

	ShowStatus("'"CL_WHITE"item_keepitem.txt"CL_RESET"' 에서 '"CL_WHITE"%d"CL_RESET"' 개의 자료를 읽었습니다.\n", count);

	return 0;
}

/*==========================================
 * 즉시효과아이템 [★orz。]
 *------------------------------------------*/
static int itemdb_read_immediate(void)
{
	uint32 lines = 0, count = 0, nameid;
	char line[1024];
	char path[256];
	FILE* fp;
	struct item_data *id;

	sprintf(path, "%s/item_immediate.txt", db_path);
	fp = fopen(path, "r");
	if( fp == NULL )
	{
		ShowError("%s 를 읽지 못하였습니다.\n", path);
		return -1;
	}

	// process rows one by one
	while(fgets(line, sizeof(line), fp))
	{
		char *str[4], *p;
		int i;

		lines++;
		if(line[0] == '/' && line[1] == '/')
			continue;
		memset(str, 0, sizeof(str));

		p = line;
		while( ISSPACE(*p) )
			++p;
		if( *p == '\0' )
			continue;// empty line
		for( i = 0; i < 3; i++ )
		{
			str[i] = p;
			p = strchr(p,',');
			if( p == NULL )
				break;// comma not found
			*p = '\0';
			++p;
		}

		if(str[0]==NULL)
			continue;

		nameid=atoi(str[0]);

		if(nameid<=0 || !(id=itemdb_exists(nameid)))
			continue;

		if( p == NULL )
		{
			ShowError("itemdb_read_immediate: \"%s\"파일의 %d번째 줄에서 잘못된 형식을 취하고있으므로 무시합니다. (item id %d)\n", path, lines, nameid);
			continue;
		}

		// Script
		if( *p != '{' )
		{
			ShowError("itemdb_read_immediate: \"%s\"파일의 %d번째 줄의 스크립트부분에서 잘못된 형식을 취하고있으므로 무시합니다. (item id %d)\n", path, lines, nameid);
			continue;
		}
		str[3]=p;

		id->imd.flag = atoi(str[1]);	// 0- 사용안함, 1- 취득시, 2- 드랍시, 3- 둘다
		id->imd.forcedrop = atoi(str[2]);	// 0- 강제드랍안함, 1- 강제드랍

		if (id->imd.script)
		{
			script_free_code(id->imd.script);
			id->imd.script = NULL;
		}

		if (str[3])
			id->imd.script = parse_script(str[3], line, lines, 0);

		count++;
	}

	fclose(fp);

	ShowStatus("'"CL_WHITE"item_immediate.txt"CL_RESET"' 에서 '"CL_WHITE"%d"CL_RESET"' 개의 자료를 읽었습니다.\n", count);

	return 0;
}

/*======================================
 * Applies gender restrictions according to settings. [Skotlex]
 *======================================*/
static int itemdb_gendercheck(struct item_data *id)
{
	if (id->nameid == WEDDING_RING_M) //Grom Ring
		return 1;
	if (id->nameid == WEDDING_RING_F) //Bride Ring
		return 0;
	if (id->look == W_MUSICAL && id->type == IT_WEAPON) //Musical instruments are always male-only
		return 1;
	if (id->look == W_WHIP && id->type == IT_WEAPON) //Whips are always female-only
		return 0;

	return (battle_config.ignore_items_gender) ? 2 : id->sex;
}

/*==========================================
 * processes one itemdb entry
 *------------------------------------------*/
static bool itemdb_parse_dbrow(char** str, const char* source, int line, int scriptopt)
{
	/*
		+----+--------------+---------------+------+-----------+------------+--------+--------+---------+-------+-------+------------+-------------+---------------+-----------------+--------------+-------------+------------+------+--------+--------------+----------------+
		| 00 |      01      |       02      |  03  |     04    |     05     |   06   |   07   |    08   |   09  |   10  |     11     |      12     |       13      |        14       |      15      |      16     |     17     |  18  |   19   |      20      |        21      |
		+----+--------------+---------------+------+-----------+------------+--------+--------+---------+-------+-------+------------+-------------+---------------+-----------------+--------------+-------------+------------+------+--------+--------------+----------------+
		| id | name_english | name_japanese | type | price_buy | price_sell | weight | attack | defence | range | slots | equip_jobs | equip_upper | equip_genders | equip_locations | weapon_level | equip_level | refineable | view | script | equip_script | unequip_script |
		+----+--------------+---------------+------+-----------+------------+--------+--------+---------+-------+-------+------------+-------------+---------------+-----------------+--------------+-------------+------------+------+--------+--------------+----------------+
	*/
	int nameid;
	struct item_data* id;
	
	nameid = atoi(str[0]);
	if( nameid <= 0 )
	{
		ShowWarning("itemdb_parse_dbrow: Invalid id %d in line %d of \"%s\", skipping.\n", nameid, line, source);
		return false;
	}

	//ID,Name,Jname,Type,Price,Sell,Weight,ATK,DEF,Range,Slot,Job,Job Upper,Gender,Loc,wLV,eLV,refineable,View
	id = itemdb_load(nameid);
	safestrncpy(id->name, str[1], sizeof(id->name));
	safestrncpy(id->jname, str[2], sizeof(id->jname));

	id->type = atoi(str[3]);
	if (id->type == IT_DELAYCONSUME)
	{	//Items that are consumed only after target confirmation
		id->type = IT_USABLE;
		id->flag.delay_consume = 1;
	} else //In case of an itemdb reload and the item type changed.
		id->flag.delay_consume = 0;

	//When a particular price is not given, we should base it off the other one
	//(it is important to make a distinction between 'no price' and 0z)
	if ( str[4][0] )
		id->value_buy = atoi(str[4]);
	else
		id->value_buy = atoi(str[5]) * 2;

	if ( str[5][0] )
		id->value_sell = atoi(str[5]);
	else
		id->value_sell = id->value_buy / 2;
	/* 
	if ( !str[4][0] && !str[5][0])
	{  
		ShowWarning("itemdb_parse_dbrow: No buying/selling price defined for item %d (%s), using 20/10z\n",       nameid, id->jname);
		id->value_buy = 20;
		id->value_sell = 10;
	} else
	*/
	if (id->value_buy/124. < id->value_sell/75.)
		ShowWarning("itemdb_parse_dbrow: Buying/Selling [%d/%d] price of item %d (%s) allows Zeny making exploit  through buying/selling at discounted/overcharged prices!\n",
			id->value_buy, id->value_sell, nameid, id->jname);

	id->weight = atoi(str[6]);
	id->atk = atoi(str[7]);
	id->def = atoi(str[8]);
	id->range = atoi(str[9]);
	id->slot = atoi(str[10]);

	if (id->slot > MAX_SLOTS)
	{
		ShowWarning("itemdb_parse_dbrow: Item %d (%s) specifies %d slots, but the server only supports up to %d. Using %d slots.\n", nameid, id->jname, id->slot, MAX_SLOTS, MAX_SLOTS);
		id->slot = MAX_SLOTS;
	}

	itemdb_jobid2mapid(id->class_base, (unsigned int)strtoul(str[11],NULL,0));
	id->class_upper = atoi(str[12]);
	id->sex	= atoi(str[13]);
	id->equip = atoi(str[14]);

	if (!id->equip && itemdb_isequip2(id))
	{
		ShowWarning("Item %d (%s) is an equipment with no equip-field! Making it an etc item.\n", nameid, id->jname);
		id->type = IT_ETC;
	}

	id->wlv = atoi(str[15]);
	id->elv = atoi(str[16]);
	id->flag.no_refine = atoi(str[17]) ? 0 : 1; //FIXME: verify this
	id->look = atoi(str[18]);

	id->flag.available = 1;
	id->flag.value_notdc = 0;
	id->flag.value_notoc = 0;
	id->view_id = 0;
	id->sex = itemdb_gendercheck(id); //Apply gender filtering.

	if (id->script)
	{
		script_free_code(id->script);
		id->script = NULL;
	}
	if (id->equip_script)
	{
		script_free_code(id->equip_script);
		id->equip_script = NULL;
	}
	if (id->unequip_script)
	{
		script_free_code(id->unequip_script);
		id->unequip_script = NULL;
	}

	if (*str[19])
		id->script = parse_script(str[19], source, line, scriptopt);
	if (*str[20])
		id->equip_script = parse_script(str[20], source, line, scriptopt);
	if (*str[21])
		id->unequip_script = parse_script(str[21], source, line, scriptopt);

	return true;
}

/*==========================================
 * 귺귽긡?긢??긹?긚궻벶귒뜛귒
 *------------------------------------------*/
static int itemdb_readdb(void)
{
	const char* filename[] = { "item_db.txt", "item_db2.txt" };
	int fi;

	for( fi = 0; fi < ARRAYLENGTH(filename); ++fi )
	{
		uint32 lines = 0, count = 0;
		char line[1024];

		char path[256];
		FILE* fp;

		sprintf(path, "%s/%s", db_path, filename[fi]);
		fp = fopen(path, "r");
		if( fp == NULL )
		{
			ShowWarning("itemdb_readdb: File not found \"%s\", skipping.\n", path);
			continue;
		}

		// process rows one by one
		while(fgets(line, sizeof(line), fp))
		{
			char *str[32], *p;
			int i;

			lines++;
			if(line[0] == '/' && line[1] == '/')
				continue;
			memset(str, 0, sizeof(str));

			p = line;
			while( ISSPACE(*p) )
				++p;
			if( *p == '\0' )
				continue;// empty line
			for( i = 0; i < 19; ++i )
			{
				str[i] = p;
				p = strchr(p,',');
				if( p == NULL )
					break;// comma not found
				*p = '\0';
				++p;
			}

			if( p == NULL )
			{
				ShowError("itemdb_readdb: Insufficient columns in line %d of \"%s\" (item with id %d), skipping.\n", lines, path, atoi(str[0]));
				continue;
			}

			// Script
			if( *p != '{' )
			{
				ShowError("itemdb_readdb: Invalid format (Script column) in line %d of \"%s\" (item with id %d), skipping.\n", lines, path, atoi(str[0]));
				continue;
			}
			str[19] = p;
			p = strstr(p+1,"},");
			if( p == NULL )
			{
				ShowError("itemdb_readdb: Invalid format (Script column) in line %d of \"%s\" (item with id %d), skipping.\n", lines, path, atoi(str[0]));
				continue;
			}
			p[1] = '\0';
			p += 2;

			// OnEquip_Script
			if( *p != '{' )
			{
				ShowError("itemdb_readdb: Invalid format (OnEquip_Script column) in line %d of \"%s\" (item with id %d), skipping.\n", lines, path, atoi(str[0]));
				continue;
			}
			str[20] = p;
			p = strstr(p+1,"},");
			if( p == NULL )
			{
				ShowError("itemdb_readdb: Invalid format (OnEquip_Script column) in line %d of \"%s\" (item with id %d), skipping.\n", lines, path, atoi(str[0]));
				continue;
			}
			p[1] = '\0';
			p += 2;

			// OnUnequip_Script (last column)
			if( *p != '{' )
			{
				ShowError("itemdb_readdb: Invalid format (OnUnequip_Script column) in line %d of \"%s\" (item with id %d), skipping.\n", lines, path, atoi(str[0]));
				continue;
			}
			str[21] = p;


			if (!itemdb_parse_dbrow(str, path, lines, 0))
				continue;

			count++;
		}

		fclose(fp);

		ShowStatus("'"CL_WHITE"%s"CL_RESET"' 에서 '"CL_WHITE"%lu"CL_RESET"' 개의 자료를 읽었습니다.\n", filename[fi], count);
	}

	return 0;
}

#ifndef TXT_ONLY
/*======================================
 * item_db table reading
 *======================================*/
static int itemdb_read_sqldb(void)
{
	const char* item_db_name[] = { item_db_db, item_db2_db };
	int fi;
	
	for( fi = 0; fi < ARRAYLENGTH(item_db_name); ++fi )
	{
		uint32 lines = 0, count = 0;

		// retrieve all rows from the item database
		if( SQL_ERROR == Sql_Query(mmysql_handle, "SELECT * FROM `%s`", item_db_name[fi]) )
		{
			Sql_ShowDebug(mmysql_handle);
			continue;
		}

		// process rows one by one
		while( SQL_SUCCESS == Sql_NextRow(mmysql_handle) )
		{// wrap the result into a TXT-compatible format
			char* str[22];
			char* dummy = "";
			int i;
			++lines;
			for( i = 0; i < 22; ++i )
			{
				Sql_GetData(mmysql_handle, i, &str[i], NULL);
				if( str[i] == NULL ) str[i] = dummy; // get rid of NULL columns
			}

			if (!itemdb_parse_dbrow(str, item_db_name[fi], lines, SCRIPT_IGNORE_EXTERNAL_BRACKETS))
				continue;
			++count;
		}

		// free the query result
		Sql_FreeResult(mmysql_handle);

		ShowStatus("'"CL_WHITE"%s"CL_RESET"' 에서 '"CL_WHITE"%lu"CL_RESET"' 개의 자료를 읽었습니다.\n", item_db_name[fi], count);
	}

	return 0;
}
#endif /* not TXT_ONLY */

// iLAthena 추가시작 [사용자정의아이템드롭시스템]
static bool itemdb_parse_row_globaldrop(struct item_global_drop *gd, char** str, const char* source, int line)
{
	static const struct {
		char str[32];
		enum GlobalDropState id;
	} cond[] = {
		{ "always",	GDC_ALWAYS	},
		{ "id",		GDC_MOBID	},
		{ "race",	GDC_RACE	},
		{ "size",	GDC_SIZE	},
		{ "ele",	GDC_ELE		},
		{ "minlv",	GDC_MINLV	},
		{ "maxlv",	GDC_MAXLV	},
	};

	int nameid, p;
	int i =0, j;

	nameid = atoi(str[0]);
	p = atoi(str[1]);
	if(nameid > 0 && itemdb_exists(nameid) == NULL)
	{
		ShowError("item_readglobaldrop: Non existant Item id %d at %d, line %d\n", nameid, source, line);
		return false;
	}

	gd->nameid = nameid;
	gd->p = p;

	for(i=0; i<7; i=i+2){
		ARR_FIND(0, ARRAYLENGTH(cond), j, strcmp(str[2+i], cond[j].str) == 0);
		if( j < ARRAYLENGTH(cond) )
			gd->condition[i].cond = cond[j].id;
		else
			gd->condition[i].cond = -1;
		gd->condition[i].val =atoi(str[3+i]);
	}

	return true;
}



/*==========================================
 * mob_global_drop.txt reading
 *------------------------------------------*/
static int itemdb_read_globaldrop(void)
{
	uint32 lines = 0, num = 0;
	char line[1024];
	int i;

	char path[256];
	FILE *fp;

	max_global_drop = 10;
	CREATE(item_global_drop, struct item_global_drop, max_global_drop);

	sprintf(path, "%s/%s", db_path, "item_global_drop.txt");
	fp = fopen(path, "r");
	if( fp == NULL )
	{
		ShowWarning("item_readglobaldrop: \"%s\" 파일을 찾을 수 없습니다.\n", path);
		return 0;
	}

	while(fgets(line, sizeof(line), fp))
	{
		char *str[16], *p, *np;
		int j=0;

		lines++;
		if(line[0] == '/' && line[1] == '/')
			continue;
		memset(str, 0, sizeof(str));

		p = line;
		while( ISSPACE(*p) )
			++p;
		if( *p == '\0' )
			continue;// empty line
		for(i = 0; i < 16; i++)
		{
			str[i] = p;
			if((np = strchr(p, ',')) != NULL) {
				*np = '\0';
				p = np + 1;
			}
		}
		
		if( i < 16 )
		{
			ShowError("item_readglobaldrop: 잘못된 값이 설정되어있습니다. 행: %d\n", lines);
			continue;
		}

		if( !itemdb_parse_row_globaldrop( &item_global_drop[num] ,str, path, lines) )
			continue;

		if(++num >= max_global_drop){
			max_global_drop += 5;
			RECREATE(item_global_drop, struct item_global_drop, max_global_drop);
		}
	}
	
	fclose(fp);

	ShowStatus("'"CL_WHITE"item_global_drop.txt"CL_RESET"'에서 '"CL_WHITE"%d"CL_RESET"'개의 자료를 읽었습니다.\n", num);

	return 0;
}

struct item_global_drop *itemdb_getglobaldrop(int id){
	return &item_global_drop[id];
}

int itemdb_globaldrop_max(void){
	return max_global_drop;
}
// iLAthena 추가종료 [사용자정의아이템드롭시스템]

/*====================================
 * read item_limit_db.txt
 *------------------------------------*/
static bool itemdb_parse_row_limitdb(char** str, const char* source, int line){
	int nameid;
	struct item_data *item;

	nameid = atoi(str[0]);

	item = itemdb_search(nameid);

	if(!item || item == &dummy_item)
		return false;

	item->limit_lv_min = atoi(str[1]);
	item->limit_lv_max = atoi(str[2]);

	return true;
}

static int itemdb_read_itemlimit(void){
	uint32 lines = 0, num = 0;
	char line[1024], path[256];
	int i;
	FILE *fp;

	sprintf(path, "%s/%s", db_path, "item_limit_db.txt");
	fp = fopen(path, "r");
	if( fp == NULL ){
		ShowWarning("item_limit : \"%s\" 파일을 찾을 수 없습니다", path);
		return 0;
	}

	while(fgets(line, sizeof(line), fp)){
		char *str[3], *p, *np;
		int j=0;

		lines++;
		if(line[0] == '/' && line[1] == '/')
			continue;
		memset(str, 0, sizeof(str));

		p = line;
		while( ISSPACE(*p) )
			++p;
		if( *p == '\0' )
			continue;// empty line
		for(i = 0; i < 3; i++)
		{
			str[i] = p;
			if((np = strchr(p, ',')) != NULL) {
				*np = '\0';
				p = np + 1;
			}
		}
		
		if( i < 3 )
		{
			ShowError("item_limit: 잘못된 값이 설정되어있습니다. 행: %d\n", lines);
			continue;
		}

		if( !itemdb_parse_row_limitdb(str, path, lines) )
			continue;

		num++;
	}
	
	fclose(fp);

	ShowStatus("'"CL_WHITE"item_limit_db.txt"CL_RESET"'에서 '"CL_WHITE"%d"CL_RESET"'개의 자료를 읽었습니다.\n", num);

	return 0;
}

/*====================================
 * read all item-related databases
 *------------------------------------*/
static void itemdb_read(void)
{
#ifndef TXT_ONLY
	if (db_use_sqldbs)
		itemdb_read_sqldb();
	else
#endif
		itemdb_readdb();

	if (battle_config.keepitem_use) 
		itemdb_read_keepitem();	// 차암 시스템 [페퍼민트]
	if (battle_config.immediateitem_use)
		itemdb_read_immediate();	// 즉시효과아이템 [★orz。]
	if (battle_config.probcard_use)
		itemdb_read_probcard();	// 카드확률조합시스템[★orz。]
	itemdb_read_itemgroup();
	itemdb_read_itemavail();
	itemdb_read_noequip();
	itemdb_read_itemtrade();
	itemdb_read_globaldrop();
	itemdb_read_itemlimit();
}

/*==========================================
 * Initialize / Finalize
 *------------------------------------------*/

/// Destroys the item_data.
static void destroy_item_data(struct item_data* self, int free_self)
{
	if( self == NULL )
		return;
	// free scripts
	if( self->script )
		script_free_code(self->script);
	if( self->equip_script )
		script_free_code(self->equip_script);
	if( self->unequip_script )
		script_free_code(self->unequip_script);
	// 차암 시스템 [페퍼민트]
	if( self->ki.script )
		script_free_code(self->ki.script);
	// 즉시효과아이템 [★orz。]
	if( self->imd.script )
		script_free_code(self->imd.script);
#if defined(DEBUG)
	// trash item
	memset(self, 0xDD, sizeof(struct item_data));
#endif
	// free self
	if( free_self )
		aFree(self);
}

static void destroy_globaldrop_data()
{
	if(!item_global_drop)
		return;

	aFree(item_global_drop);
}

static int itemdb_final_sub(DBKey key,void *data,va_list ap)
{
	struct item_data *id = (struct item_data *)data;

	if( id != &dummy_item )
		destroy_item_data(id, 1);

	return 0;
}

void itemdb_reload(void)
{
	struct s_mapiterator* iter;
	struct map_session_data* sd;

	int i;

	// clear the previous itemdb data
	for( i = 0; i < ARRAYLENGTH(itemdb_array); ++i )
		if( itemdb_array[i] )
			destroy_item_data(itemdb_array[i], 1);

	itemdb_other->clear(itemdb_other, itemdb_final_sub);

	memset(itemdb_array, 0, sizeof(itemdb_array));

	// read new data
	itemdb_read();

	// readjust itemdb pointer cache for each player
	iter = mapit_geteachpc();
	for( sd = (struct map_session_data*)mapit_first(iter); mapit_exists(iter); sd = (struct map_session_data*)mapit_next(iter) )
		pc_setinventorydata(sd);
	mapit_free(iter);
}

void do_final_itemdb(void)
{
	int i;

	for( i = 0; i < ARRAYLENGTH(itemdb_array); ++i )
		if( itemdb_array[i] )
			destroy_item_data(itemdb_array[i], 1);

	itemdb_other->destroy(itemdb_other, itemdb_final_sub);
	destroy_item_data(&dummy_item, 0);
	destroy_globaldrop_data();
}

int do_init_itemdb(void)
{
	memset(itemdb_array, 0, sizeof(itemdb_array));
	itemdb_other = idb_alloc(DB_OPT_BASE); 
	create_dummy_data(); //Dummy data item.
	itemdb_read();

	return 0;
}
