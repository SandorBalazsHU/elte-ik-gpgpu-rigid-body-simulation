#include "gTimer.h"


gTimer::gTimer(void)
{
	glGenQueries(2, m_queries);

	m_act = 0;
}


gTimer::~gTimer(void)
{
	glDeleteQueries(2, m_queries);
}

void gTimer::Start()
{
	glGetQueryObjectui64v(m_queries[m_act], GL_QUERY_RESULT, &m_timer_last_delta);

	m_act = 1 - m_act;

	glBeginQuery(GL_TIME_ELAPSED, m_queries[m_act]);
}

void gTimer::Stop()
{
	glEndQuery(GL_TIME_ELAPSED);
}

double gTimer::StopMillis()
{
	glEndQuery(GL_TIME_ELAPSED);
	return m_timer_last_delta/1000000.0f;
}

GLuint64 gTimer::GetLastDeltaNano()
{
	return m_timer_last_delta;
}

double gTimer::GetLastDeltaMicro()
{
	return m_timer_last_delta/1000.0f;
}

double gTimer::GetLastDeltaMilli()
{
	return m_timer_last_delta/1000000.0f;
}