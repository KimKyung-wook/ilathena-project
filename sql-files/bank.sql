--
-- `bank` 테이블 구조(은행)
--

CREATE TABLE `bank` (
  `acc_id` int(11) NOT NULL default '0',
  `credit` int(11) NOT NULL default '0',
  `last_time` datetime NOT NULL,
  `zeny` int(11) NOT NULL default '0',
  `debt` int(11) NOT NULL default '0',
  `due_time` datetime NOT NULL,
  `state` tinyint(2) NOT NULL default '0',
  PRIMARY KEY  (`acc_id`)
) TYPE=MyISAM;

--
-- `bank_tr` 테이블 구조(은행)
--

CREATE TABLE `bank_tr` (
  `sacc_id` int(11) NOT NULL default '0',
  `hacc_id` int(11) NOT NULL default '0',
  `zeny` int(11) NOT NULL default '0',
  `state` int(11) NOT NULL default '0'
) TYPE=MyISAM;
