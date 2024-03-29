#PickLog 드랍 및 제조 기록
#기록종류 (M)onsters Drop, (P)layers Drop/Take, Mobs Drop (L)oot Drop/Take,
# Players (T)rade Give/Take, Players (V)ending Sell/Take, (S)hop Sell/Take, (N)PC Give/Take,
# (C)onsumable Items, (A)dministrators Create/Delete, Sto(R)age, (G)uild Storage,
# (E)mail attachment

#데이터베이스: log
#테이블: picklog
CREATE TABLE `picklog` (
  `id` int(11) NOT NULL auto_increment,
  `time` datetime NOT NULL default '0000-00-00 00:00:00',
  `char_id` int(11) NOT NULL default '0',
  `type` enum('M','P','L','T','V','S','N','C','A','R','G','E') NOT NULL default 'P',
  `nameid` int(11) NOT NULL default '0',
  `amount` int(11) NOT NULL default '1',
  `refine` tinyint(3) unsigned NOT NULL default '0',
  `card0` int(11) NOT NULL default '0',
  `card1` int(11) NOT NULL default '0',
  `card2` int(11) NOT NULL default '0',
  `card3` int(11) NOT NULL default '0',
  `map` varchar(11) NOT NULL default '',
  PRIMARY KEY  (`id`),
  INDEX (`type`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

#ZenyLog 서버내 Zeny 흐름 기록
#기록종류 M=몬스터,T=개인거래,V=노점거래,S=상점거래,N=Npc를통한획득,A=관리자에의한획득,E=E-mail을통한획득
#데이터베이스: log
#테이블: zenylog
CREATE TABLE `zenylog` (
  `id` int(11) NOT NULL auto_increment,
  `time` datetime NOT NULL default '0000-00-00 00:00:00',
  `char_id` int(11) NOT NULL default '0',
  `src_id` int(11) NOT NULL default '0',
  `type` enum('M','T','V','S','N','A','E') NOT NULL default 'S',
  `amount` int(11) NOT NULL default '0',
  `map` varchar(11) NOT NULL default '',
  PRIMARY KEY  (`id`),
  INDEX (`type`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

#Branchlog 고목나뭇가지 사용 기록
#데이터베이스: log
#테이블: branchlog
CREATE TABLE `branchlog` (
  `branch_id` mediumint(9) unsigned NOT NULL auto_increment,
  `branch_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `account_id` int(11) NOT NULL default '0',
  `char_id` int(11) NOT NULL default '0',
  `char_name` varchar(25) NOT NULL default '',
  `map` varchar(11) NOT NULL default '',
  PRIMARY KEY  (`branch_id`),
  INDEX (`account_id`),
  INDEX (`char_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

#Mvplog MVP 기록
#데이터베이스: log
#테이블: mvplog
CREATE TABLE `mvplog` (
  `mvp_id` mediumint(9) unsigned NOT NULL auto_increment,
  `mvp_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `kill_char_id` int(11) NOT NULL default '0',
  `monster_id` smallint(6) NOT NULL default '0',
  `prize` int(11) NOT NULL default '0',
  `mvpexp` mediumint(9) NOT NULL default '0',
  `map` varchar(11) NOT NULL default '',
  PRIMARY KEY  (`mvp_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

#Atcommandlog 운영자 명령어 사용 기록
#데이터베이스: log
#테이블: atcommandlog
CREATE TABLE `atcommandlog` (
  `atcommand_id` mediumint(9) unsigned NOT NULL auto_increment,
  `atcommand_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `account_id` int(11) unsigned NOT NULL default '0',
  `char_id` int(11) unsigned NOT NULL default '0',
  `char_name` varchar(25) NOT NULL default '',
  `map` varchar(11) NOT NULL default '',
  `command` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`atcommand_id`),
  INDEX (`account_id`),
  INDEX (`char_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

#Npclog Npc 대화 기록
#데이터베이스: log
#테이블: npclog
CREATE TABLE `npclog` (
  `npc_id` mediumint(9) unsigned NOT NULL auto_increment,
  `npc_date` datetime NOT NULL default '0000-00-00 00:00:00',
  `account_id` int(11) unsigned NOT NULL default '0',
  `char_id` int(11) unsigned NOT NULL default '0',
  `char_name` varchar(25) NOT NULL default '',
  `map` varchar(11) NOT NULL default '',
  `mes` varchar(255) NOT NULL default '',
  PRIMARY KEY  (`npc_id`),
  INDEX (`account_id`),
  INDEX (`char_id`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

#ChatLog 서버내 대화 기록
#기록종류 O=전체대화,W=귓속말대화,P=파티대화,G=길드대화,M=보통대화
#데이터베이스: log
#테이블: chatlog
CREATE TABLE `chatlog` (
  `id` bigint(20) NOT NULL auto_increment,
  `time` datetime NOT NULL default '0000-00-00 00:00:00',
  `type` enum('O','W','P','G','M') NOT NULL default 'O',
  `type_id` int(11) NOT NULL default '0',
  `src_charid` int(11) NOT NULL default '0',
  `src_accountid` int(11) NOT NULL default '0',
  `src_map` varchar(11) NOT NULL default '',
  `src_map_x` smallint(4) NOT NULL default '0',
  `src_map_y` smallint(4) NOT NULL default '0',
  `dst_charname` varchar(25) NOT NULL default '',
  `message` varchar(150) NOT NULL default '',
  PRIMARY KEY  (`id`),
  INDEX (`src_accountid`),
  INDEX (`src_charid`)
) ENGINE=MyISAM AUTO_INCREMENT=1 ;

#Loginlog 계정 로그인 기록
#데이터베이스: log
#테이블: loginlog
CREATE TABLE `loginlog` (
  `time` datetime NOT NULL default '0000-00-00 00:00:00',
  `ip` varchar(15) NOT NULL default '',
  `user` varchar(23) NOT NULL default '',
  `rcode` tinyint(4) NOT NULL default '0',
  `log` varchar(255) NOT NULL default '',
  INDEX (`ip`)
) ENGINE=MyISAM ;
