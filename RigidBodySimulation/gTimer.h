#pragma once

/*

	Nanoszekundum pontosságú (*) GPU idõzítõ.

	Fontos: nem lehet egymásba ágyazni a különbözõ timer-ek kéréseit sem, amíg
	az OpenGL specifikáció nem engedélyezi a nested glBegin/EndQuery-ket!

	(*): driver és GPU függõ

*/

#include <GL\glew.h>

class gTimer
{
public:
	gTimer(void);
	~gTimer(void);

	void		Start();
	void		Stop();

	double		StopMillis();

	GLuint64	GetLastDeltaNano();
	double		GetLastDeltaMicro();
	double		GetLastDeltaMilli();
private:
	GLuint		m_queries[2];
	int			m_act;
	GLuint64	m_timer_last_delta;
};

