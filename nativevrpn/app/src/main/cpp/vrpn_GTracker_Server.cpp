#include <stdio.h>
#include "vrpn_GTracker_Server.h"

vrpn_GTracker_Server::vrpn_GTracker_Server(const char *name, vrpn_Connection *m_connect)
		:vrpn_Tracker(name, m_connect), vrpn_Button(name, m_connect), vrpn_Analog(name, m_connect)
{
	if(!d_connection)
	{
		fprintf(stderr, "create connection error!\n");
		exit(0);
	}

	m_bodyNum = 0;
	m_flystickNum = 0;
}

vrpn_GTracker_Server::~vrpn_GTracker_Server()
{
	m_body.clear();
	m_flystick.clear();
}

bool vrpn_GTracker_Server::updateGTrackerBodys(uint bodyNum, int *bodyId, float (*status)[7])
{
	if(bodyNum<1 || bodyId==NULL || status==NULL)
	{
		return false;
	}

	if(bodyNum > m_body.size())
	{
		m_bodyNum = bodyNum;
		m_body.reserve(bodyNum);
	}

	m_body.clear();
	for(uint i=0; i<bodyNum; ++i)
	{
		vrpn_gtracker_body_type body;
		body.id = bodyId[i];
		body.loc[0] = status[i][0];
		body.loc[1] = status[i][1];
		body.loc[2] = status[i][2];
		body.qua[0] = status[i][3];
		body.qua[1] = status[i][4];
		body.qua[2] = status[i][5];
		body.qua[3] = status[i][6];

		m_body.push_back(body);
	}

	return true;
}

bool vrpn_GTracker_Server::updateGTrackerFlysticks(uint flystickNum, int *flystickId, float (*posStatus)[7], int *btnNum, char **btnStatus)
{
	if(flystickNum<1 || !flystickId || !posStatus || !btnNum || !btnStatus)
	{
		return false;
	}

	if(flystickNum > m_body.size())
	{
		m_flystickNum = flystickNum;
		m_body.reserve(flystickNum);
	}

	for(uint i=0; i<flystickNum; ++i)
	{
		m_flystick[i].id = flystickId[i];
		m_flystick[i].loc[0] = posStatus[i][0];
		m_flystick[i].loc[1] = posStatus[i][1];
		m_flystick[i].loc[2] = posStatus[i][2];
		m_flystick[i].qua[0] = posStatus[i][3];
		m_flystick[i].qua[1] = posStatus[i][4];
		m_flystick[i].qua[2] = posStatus[i][5];
		m_flystick[i].qua[3] = posStatus[i][6];

		m_flystick[i].num_button = btnNum[i];
		for(int j=0; j<btnNum[i]; ++j)
		{
			m_flystick[i].button[j] = btnStatus[i][j];
		}
	}

	return true;
}

void vrpn_GTracker_Server::mainloop()
{
	struct timeval timestamp;
	char *msgbuf = packetBuffer;

	vrpn_gettimeofday(&timestamp, NULL);

	if(tim_first.tv_sec != 0 || tim_first.tv_usec != 0)
	{
		tim_last = timestamp;
	}
	else
	{
		tim_first = tim_last = timestamp;
	}

	server_mainloop();

	if(!d_connection)
		return;

	for(uint i=0; i<m_bodyNum; ++i)
	{
		d_sensor = m_body[i].id;

		pos[0] = m_body[i].loc[0];
		pos[1] = m_body[i].loc[1];
		pos[2] = m_body[i].loc[2];

		d_quat[0] = m_body[i].qua[0];
		d_quat[1] = m_body[i].qua[1];
		d_quat[2] = m_body[i].qua[2];
		d_quat[3] = m_body[i].qua[3];

		int len = vrpn_Tracker::encode_to(msgbuf);
		if(d_connection->pack_message(len, timestamp, position_m_id, d_sender_id, msgbuf, vrpn_CONNECTION_LOW_LATENCY))
		{
			fprintf(stderr, "vrpn_GTracker_Server: cannot write message: tossing.\n");
		}
	}

	for(uint i=0; i<m_flystickNum; ++i)
	{
		d_sensor = m_flystick[i].id;

		pos[0] = m_flystick[i].loc[0];
		pos[1] = m_flystick[i].loc[1];
		pos[2] = m_flystick[i].loc[2];

		d_quat[0] = m_flystick[i].qua[0];
		d_quat[1] = m_flystick[i].qua[1];
		d_quat[2] = m_flystick[i].qua[2];
		d_quat[3] = m_flystick[i].qua[3];

		int len = vrpn_Tracker::encode_to(msgbuf);
		if(d_connection->pack_message(len, timestamp, position_m_id, d_sender_id, msgbuf, vrpn_CONNECTION_LOW_LATENCY))
		{
			fprintf(stderr, "vrpn_GTracker_Server: cannot write message: tossing.\n");
		}

		int n, j=0, ind;

		n = (m_flystick[i].num_button > VRPN_GTRACKER_FLYSTICK_BUTTON_NUM) ? VRPN_GTRACKER_FLYSTICK_BUTTON_NUM : m_flystick[i].num_button;

		ind = m_flystick[i].id * VRPN_GTRACKER_FLYSTICK_BUTTON_NUM;
		while(j < n)
		{
			buttons[ind++] = m_flystick[i].button[j];
			j++;
		}
		while(j < VRPN_GTRACKER_FLYSTICK_BUTTON_NUM)
		{
			buttons[ind++] = 0;
			j++;
		}
	}

	vrpn_Button::report_changes();

	d_connection->mainloop();
}