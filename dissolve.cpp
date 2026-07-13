#include "dissolve.h"

void StartDissolveOut(DISSOLVE_STATE* state, float speed, float minY, float maxY)
{
	if (!state) return;

	state->mode = DISSOLVE_OUT;
	state->speed = speed;
	state->threshold = 0.0f; 
	state->minY = minY;
	state->maxY = maxY;
}

void StartDissolveIn(DISSOLVE_STATE* state, float speed, float minY, float maxY)
{
	state->mode = DISSOLVE_IN;
	state->speed = speed;
	state->threshold = 1.0f;
	state->minY = minY;
	state->maxY = maxY;
}

bool bUpdateDdissolve(DISSOLVE_STATE* state, float deltatime)
{
	if (!state || state->mode == DISSOLVE_NONE)
	{
		return false;
	}

	if (state->mode == DISSOLVE_OUT)
	{
		state->threshold += state->speed * deltatime;

		if (state->threshold >= 1.0f)
		{
			state->threshold = 1.0f;
			state->mode = DISSOLVE_NONE;
			return true;
		}
	}
	else if (state->mode == DISSOLVE_IN)
	{
		state->threshold -= state->speed * deltatime;

		if (state->threshold <=0.0f)
		{
			state->threshold = 0.0f;
			state->mode = DISSOLVE_NONE;
			return true;
		}
	}

	return false;
}
