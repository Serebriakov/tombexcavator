/*
 * Heart Of Darkness engine rewrite
 * Copyright (C) 2009 Gregory Montoir
 */

#ifndef SYSTEMSTUB_H__
#define SYSTEMSTUB_H__

#include "intern.h"

#define SYS_INP_UP    (1 << 0)
#define SYS_INP_RIGHT (1 << 1)
#define SYS_INP_DOWN  (1 << 2)
#define SYS_INP_LEFT  (1 << 3)
#define SYS_INP_RUN   (1 << 4) /* (1 << 0) */
#define SYS_INP_JUMP  (1 << 5) /* (1 << 1) */
#define SYS_INP_SHOOT (1 << 6) /* (1 << 2) */
#define SYS_INP_ESC   (1 << 7)

struct PlayerInput {
	uint8 prevMask, mask;
	bool quit;

	bool keyPressed(int keyMask) const {
		return (prevMask & keyMask) == 0 && (mask & keyMask) == keyMask;
	}
	bool keyReleased(int keyMask) const {
		return (prevMask & keyMask) == keyMask && (mask & keyMask) == 0;
	}
};

struct SystemStub {
	PlayerInput inp;

	virtual ~SystemStub() {}

	virtual void init(const char *title, int w, int h) = 0;
	virtual void destroy() = 0;

	virtual void setPalette(const uint8 *pal, int n, int depth = 8) = 0;
	virtual void copyRect(int x, int y, int w, int h, const uint8 *buf, int pitch) = 0;
	virtual void fillRect(int x, int y, int w, int h, uint8 color) = 0;
	virtual void shakeScreen(int dx, int dy) = 0;
	virtual void updateScreen() = 0;

	virtual void processEvents() = 0;
	virtual void sleep(int duration) = 0;
	virtual uint32 getTimeStamp() = 0;
};

extern SystemStub *SystemStub_SDL_create();

#endif // SYSTEMSTUB_H__
