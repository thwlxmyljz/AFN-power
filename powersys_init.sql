﻿drop database powersys;
create database powersys;
use powersys;

CREATE TABLE IF NOT EXISTS `SYS_VAR` (
  `VAR` char(8) NOT NULL,
  `VARNAME` char(45) default NULL,
  `VALTYPE` char(1) default NULL COMMENT '指定值VARVALUE或CHARVALUE有效',
  `VARVALUE` int(11) NOT NULL,
  `CHARVALUE` varchar(128) default NULL,
  `REMARK` char(30) default NULL,
  PRIMARY KEY  (`VAR`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

CREATE TABLE IF NOT EXISTS `SYS_USER` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `USER` char(64) NOT NULL DEFAULT '' COMMENT '帐号',
  `PWD` char(64) NOT NULL DEFAULT '',
  `UNAME` char(64) NOT NULL DEFAULT '' COMMENT '名称',
  `SEX` smallint DEFAULT 1,
  `BIRTHDAY` date default NULL,
  `CARDID` char(20) DEFAULT NULL COMMENT '身份证号',
  `ADDR` char(128) DEFAULT NULL COMMENT '创建时间',
  `EMAIL` char(128) DEFAULT NULL,
  `PHONE` char(15) DEFAULT NULL,
  `MOBILE` char(15) DEFAULT NULL,
  `WDATE` datetime default NULL COMMENT '创建时间',
  `UTYPE` smallint DEFAULT 0 COMMENT '角色权限',
  `STATUS` smallint DEFAULT 1 COMMENT '帐号状态',
  `LOGINTIME` datetime default NULL COMMENT '最后一次登录时间',
  `REMARK` char(128) DEFAULT NULL,
  UNIQUE INDEX INDEX1(`USER`),
  PRIMARY KEY  (`ID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

insert into SYS_USER(USER,PWD,UNAME,CARDID,UTYPE) VALUES('admin','E10ADC3949BA59ABBE56E057F20F883E','管理员','111111111111111111',1);

CREATE TABLE IF NOT EXISTS `PW_CORP` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `CORPID` int(11) NOT NULL COMMENT '企业指定ID',
  `NAME` varchar(100) default NULL COMMENT '企业名称',
  `UNAME` varchar(32) default NULL COMMENT '企业管理人',
  `PHONE` varchar(32) default NULL COMMENT '电话',
  `WWW` varchar(32) default NULL COMMENT '网站',
  `EMAIL` varchar(32) default NULL COMMENT '邮箱',
  `ADDR` varchar(128) DEFAULT NULL COMMENT '企业地址',
  `LO` varchar(128) DEFAULT NULL COMMENT '地理位置经度',
  `LA` varchar(128) DEFAULT NULL COMMENT '地理位置纬度',
  `X` int(11) DEFAULT -1 COMMENT '页面位置x',
  `Y` int(11) DEFAULT -1 COMMENT '页面位置y',
  `MAP` varchar(256) DEFAULT NULL COMMENT '企业地图文件地址',
  `REMARK` varchar(1024) DEFAULT NULL COMMENT '企业简介',
  PRIMARY KEY  (`ID`),
  UNIQUE INDEX INDEX1(`CORPID`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

insert into PW_CORP(CORPID,NAME,UNAME) values(851000001,'贵州财经大学','夏换');

CREATE TABLE IF NOT EXISTS `PW_BLOCK` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `NUM` varchar(128) NOT NULL DEFAULT '楼编号',
  `CORPID` int(11) NOT NULL COMMENT '所属企业ID',
  `NAME` varchar(128) NOT NULL DEFAULT '楼名称',
  `X` int(11) DEFAULT -1 COMMENT '所在企业地图位置X',
  `Y` int(11) DEFAULT -1 COMMENT '所在企业地图位置Y',  
  `REMARK` char(128) default NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE INDEX INDEX1(`CORPID`,`NUM`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

insert into PW_BLOCK(NUM,CORPID,NAME) values('1',851000001,'正德楼');
insert into PW_BLOCK(NUM,CORPID,NAME) values('2',851000001,'进德楼');
insert into PW_BLOCK(NUM,CORPID,NAME) values('3',851000001,'崇德楼');
insert into PW_BLOCK(NUM,CORPID,NAME) values('4',851000001,'明德楼');
insert into PW_BLOCK(NUM,CORPID,NAME) values('5',851000001,'笃行楼');
insert into PW_BLOCK(NUM,CORPID,NAME) values('6',851000001,'文德楼');
insert into PW_BLOCK(NUM,CORPID,NAME) values('7',851000001,'修德楼');
insert into PW_BLOCK(NUM,CORPID,NAME) values('8',851000001,'博学楼');

CREATE TABLE IF NOT EXISTS `PW_JZQ` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `NAME` char(128) NOT NULL DEFAULT '' COMMENT '名称',
  `AREACODE` int(11) NOT NULL COMMENT '行政区划码',
  `ADDRESS` int(11) NOT NULL COMMENT '终端地址',
  `STIME` datetime default NULL COMMENT '上离线时间',
  `STATUS` int(11) default 0 COMMENT '状态1在线0离线',  
  `TOCID` int(11) NOT NULL COMMENT '所属企业ID',
  `TONUM` varchar(128) NOT NULL DEFAULT '所属楼编号',
  `REMARK` char(128) default NULL,
  PRIMARY KEY  (`ID`),
  UNIQUE INDEX INDEX1(`AREACODE`,`ADDRESS`),
  INDEX INDEX2(`TOCID`,`TONUM`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

insert into PW_JZQ(NAME,AREACODE,ADDRESS,TOCID,TONUM) values('test',4096,1101,851000001,'1');
insert into PW_JZQ(NAME,AREACODE,ADDRESS,TOCID,TONUM) values('test01',65535,65535,851000001,'2');

CREATE TABLE IF NOT EXISTS `DATA_0C33` (
  `ID` int(11) NOT NULL AUTO_INCREMENT,
  `AREACODE` int(11) NOT NULL COMMENT '行政区划码',
  `ADDRESS` int(11) NOT NULL COMMENT '终端地址',
  `STIME` datetime NOT NULL COMMENT '采集时间',
  `USERKWH` float NOT NULL COMMENT '正向有功总电能示值',
  `DEVKVAR` float NOT NULL COMMENT '正向无功总电能示值',
  `FIRSTKVAR` float NOT NULL COMMENT '一象限无功总电能示值',
  `FOURKVAR` float NOT NULL COMMENT '四象限无功总电能示值',
  `M_USERKWH` varchar(50) DEFAULT NULL COMMENT '正向有功费率1-M示值,#分隔',
  `M_DEVKVAR` varchar(50) DEFAULT NULL COMMENT '正向无功费率1-M示值,#分隔',
  `M_FIRSTKVAR` varchar(50) DEFAULT NULL COMMENT '一象限无功费率1-M示值,#分隔',
  `M_FOURKVAR` varchar(50) DEFAULT NULL COMMENT '四象限无功费率1-M示值,#分隔',
  PRIMARY KEY  (`ID`),
  INDEX INDEX1(`AREACODE`,`ADDRESS`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;


