ALTER TABLE `quest` DROP `mob1`;
ALTER TABLE `quest` DROP `mob2`;
ALTER TABLE `quest` DROP `mob3`;

ALTER TABLE `char` ADD `rename` SMALLINT( 3 ) UNSIGNED NOT NULL DEFAULT '0';