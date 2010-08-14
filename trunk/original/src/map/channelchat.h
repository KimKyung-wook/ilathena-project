// Copyright (c) iLAthena Dev Teams - Licensed under GNU GPL
// 채널 채팅 시스템

#ifndef _CHANNELCHAT_H_
#define _CHANNELCHAT_H_

struct channel_data
{
	char nick[16];
	char passwd[9];
	bool use_pswd;
	bool exist_channel;
	int limit;
	int users;
};

#endif /* _CHANNELCHAT_H_ */