/*
 * Heart Of Darkness engine rewrite
 * Copyright (C) 2009 Gregory Montoir
 */

#include "fileio.h"
#include "game.h"
#include "resource.h"
#include "util.h"

Resource::Resource() {
	memset(_ptr32Pool, 0, sizeof(_ptr32Pool));
	_ptr32PoolSize = 0;
	_isDemoData = detectGameData();
	if (_isDemoData) {
		_lvlFile = new SectorFile;
		_sssFile = new SectorFile;
		_datFile = new SectorFile;
	} else {
		_lvlFile = new File;
		_sssFile = 0;
		_datFile = 0;
	}
}

bool Resource::detectGameData() {
	File f;
	if (!f.open("SETUP.DAT")) {
		error("Unable to open 'SETUP.DAT'");
		return false;
	}
	return f.getSize() == 5804032;
}

void Resource::loadSetupDat() {
	if (!_datFile) {
		return;
	}
	uint8 hdr[512];
	_datFile->open("SETUP.DAT");
	_datFile->read(hdr, sizeof(hdr));
	_datHdr._res_setupDatHeader0x40 = READ_LE_UINT32(hdr + 0x40);
//	printf("Quit Yes/No image index %d\n", _datHdr._res_setupDatHeader0x40);
	for (int i = 0; i < 46; ++i) {
		_datHdr._setupImageOffsetTable[i] = READ_LE_UINT32(hdr + 0x04C + i * 4);
		_datHdr._setupImageSizeTable[i] = READ_LE_UINT32(hdr + 0x104 + i * 4);
	}
}

void Resource::loadLvlScreenMoveData(int num) {
	_lvlFile->seekAlign(0x8 + num * 4);
	_lvlFile->read(&_screensGrid[num * 4], 4);
}

void Resource::loadLvlScreenVectorData(int num) {
	_lvlFile->seekAlign(0xA8 + num * 8);
	LvlScreenVector *dat = &_screensBasePos[num];
	dat->u = _lvlFile->readUint32();
	dat->v = _lvlFile->readUint32();
}

void Resource::loadLvlScreenStateData(int num) {
	_lvlFile->seekAlign(0x1E8 + num * 4);
	LvlScreenState *dat = &_screensState[num];
	dat->s0 = _lvlFile->readByte();
	dat->s1 = _lvlFile->readByte();
	dat->s2 = _lvlFile->readByte();
	dat->s3 = _lvlFile->readByte();
}

void Resource::loadLvlScreenObjectData(int num) {
	_lvlFile->seekAlign(0x288 + num * 96);
	LvlObject *dat = &_resLvlScreenObjectDataTable[num];

	dat->xPos = _lvlFile->readUint32();
	dat->yPos = _lvlFile->readUint32();
	dat->data0x2E08 = _lvlFile->readByte();
	dat->screenState = _lvlFile->readByte();
	dat->flags = _lvlFile->readByte();
	dat->frame = _lvlFile->readByte();
	dat->anim = _lvlFile->readUint16();
	dat->type = _lvlFile->readByte();
	dat->data0x2988 = _lvlFile->readByte();
	dat->flags0 = _lvlFile->readUint16();
	dat->flags1 = _lvlFile->readUint16();
	dat->flags2 = _lvlFile->readUint16();
	dat->stateValue = _lvlFile->readByte();
	dat->stateCounter = _lvlFile->readByte();
	const uint32 objRef = _lvlFile->readUint32();
	if (objRef) {
		dat->linkObjPtr = &_lvlLinkObject;
		printf("loadLvlObj num %d linkObjRef %d\n", num, (int)objRef);
	}
	dat->width = _lvlFile->readUint16();
	dat->height = _lvlFile->readUint16();
	dat->directionKeyMask = _lvlFile->readByte();
	dat->actionKeyMask = _lvlFile->readByte();
	dat->unk22 = _lvlFile->readUint16();
	dat->soundToPlay = _lvlFile->readUint16();
	dat->unk26 = _lvlFile->readByte();
	dat->unk27 = _lvlFile->readByte();
	dat->bitmapBits = 0; _lvlFile->readUint32();
	dat->callbackFuncPtr = 0; _lvlFile->readUint32();
	dat->dataPtr = 0; _lvlFile->readUint32();
	dat->unk34 = _lvlFile->readUint32();
	dat->levelData0x2988 = 0; _lvlFile->readUint32();
	for (int i = 0; i < 8; ++i) {
		dat->posTable[i].x = _lvlFile->readUint16();
		dat->posTable[i].y = _lvlFile->readUint16();
	}
	dat->nextPtr = 0; _lvlFile->readUint32();
}

static void resFixPointersLevelData0x2988(uint8 *src, uint8 *ptr, LvlObjectData *dat) {
	uint8 *base = src;

	dat->unk0 = *src++;
	dat->index = *src++;
	dat->framesCount = READ_LE_UINT16(src); src += 2;
	dat->hotspotsCount = READ_LE_UINT16(src); src += 2;
	dat->movesCount = READ_LE_UINT16(src); src += 2;
	dat->animsCount = READ_LE_UINT16(src); src += 2;
	dat->refCount = *src++;
	dat->frame = *src++;
	dat->anim = READ_LE_UINT16(src); src += 2;
	dat->unkE = *src++;
	dat->unkF = *src++;
	src += 4; // 0x10
	uint32 movesDataOffset = READ_LE_UINT32(src); src += 4; // 0x14
	src += 4; // 0x18
	uint32 framesDataOffset = READ_LE_UINT32(src); src += 4; // 0x1C
	src += 4; // 0x20
	uint32 animsDataOffset = READ_LE_UINT32(src); src += 4; // 0x24
	uint32 hotspotsDataOffset = READ_LE_UINT32(src); src += 4; // 0x28

	if (dat->refCount != 0) {
		return;
	}

	assert(src == base + kLvlAnimHdrOffset);
	dat->animsInfoData = base;
	for (int i = 0; i < dat->framesCount; ++i) {
		LvlAnimHeader *ah = ((LvlAnimHeader *)(base + kLvlAnimHdrOffset)) + i;
		ah->unk0 = FROM_LE16(ah->unk0);
		ah->seqOffset = FROM_LE32(ah->seqOffset);
		if (ah->seqOffset != 0) {
			for (int seq = 0; seq < ah->seqCount; ++seq) {
				LvlAnimSeqHeader *ash = ((LvlAnimSeqHeader *)(base + ah->seqOffset)) + seq;
				ash->firstFrame = FROM_LE16(ash->firstFrame);
				ash->unk2 = FROM_LE16(ash->unk2);
				ash->sound = FROM_LE16(ash->sound);
				ash->flags0 = FROM_LE16(ash->flags0);
				ash->flags1 = FROM_LE16(ash->flags1);
				ash->unkE = FROM_LE16(ash->unkE);
				ash->offset = FROM_LE32(ash->offset);
				if (ash->offset != 0) {
					LvlAnimSeqFrameHeader *asfh = (LvlAnimSeqFrameHeader *)(base + ash->offset);
					asfh->move = FROM_LE16(asfh->move);
					asfh->anim = FROM_LE16(asfh->anim);
				}
			}
		}
	}
	dat->refCount = 0xFF;
	dat->framesData = (framesDataOffset == 0) ? 0 : base + framesDataOffset;
	dat->hotspotsData = (hotspotsDataOffset == 0) ? 0 : base + hotspotsDataOffset;
	dat->movesData = (movesDataOffset == 0) ? 0 : base + movesDataOffset;
	dat->animsData = (animsDataOffset == 0) ? 0 : base + animsDataOffset;
	if (dat->animsData != 0) {
		dat->coordsData = dat->animsData + dat->framesCount * 4;
	}

#if 0 /* ResGetLvlSpriteFramePtr - ResGetLvlSpriteAnimPtr */
	/* original preallocated the structure */
	dat->framesOffsetTable = ptr;
	if (framesDataOffset != 0) {
		assert(dat->framesCount < MAX_SPRITE_FRAMES);
		p = dat->framesData = _ecx + framesDataOffset;
		for (i = 0; i < dat->framesCount; ++i) {
			WRITE_LE_UINT32(dat->framesOffsetsTable + i * 4, p);
			size = READ_LE_UINT16(p);
			p += size;
		}
	}
	if (animsDataOffset != 0) {
		assert(dat->animsData < MAX_SPRITE_ANIMS);
		for (i = 0; i < dat->animsCount; ++i) {
			WRITE_LE_UINT32(dat->coordsData + i * 4, p);
			size = p[0];
			p += size * 4 + 1;
		}
	}
#endif
}

void Resource::loadLvlSpriteData(int num) {
	_lvlFile->seekAlign(0x2988 + num * 16);
	uint32 offs = _lvlFile->readUint32();
	uint32 size = _lvlFile->readUint32();
	uint32 readSize = _lvlFile->readUint32();
	uint8 *ptr = (uint8 *)malloc(size);
	_lvlFile->seek(offs, SEEK_SET);
	_lvlFile->read(ptr, readSize);

	LvlObjectData *dat = &_resLevelData0x2988Table[num];
	resFixPointersLevelData0x2988(ptr, ptr + readSize, dat);
	_resLevelData0x2988PtrTable[dat->index] = dat;
	_resLvlScreenSpriteDataPtrTable[num] = ptr;
	_resLevelData0x2988SizeTable[num] = size;
}

uint8 *Resource::getLevelData0x470CPtr0(int num) {
	assert(num < 160);
	const uint32 offset = READ_LE_UINT32(_resLevelData0x470CTablePtrHdr + num * 8 + 0);
	return (offset != 0) ? _resLevelData0x470CTable + offset : 0;
}

uint8 *Resource::getLevelData0x470CPtr4(int num) {
	assert(num < 160);
	const uint32 offset = READ_LE_UINT32(_resLevelData0x470CTablePtrHdr + num * 8 + 4);
	return (offset != 0) ? _resLevelData0x470CTable + offset : 0;
}

void Resource::loadLevelData0x470C() {
	_lvlFile->seekAlign(0x4708);
	uint32 offs = _lvlFile->readUint32();
	uint32 size = _lvlFile->readUint32();
	_resLevelData0x470CTable = (uint8 *)malloc(size);
	_lvlFile->seek(offs, SEEK_SET);
	_lvlFile->read(_resLevelData0x470CTable, size);
	_resLevelData0x470CTablePtrHdr = _resLevelData0x470CTable;
	_resLevelData0x470CTablePtrData = _resLevelData0x470CTable + 1280;
}

static const uint32 lvlHdrTag = 0x484F4400;

void Resource::loadLvlData(const char *levelName) {
	_lvlFile->close();
	char filename[32];
	snprintf(filename, sizeof(filename), "%s.LVL", levelName);
	if (!_lvlFile->open(filename)) {
		error("Unable to open '%s'", filename);
		return;
	}

	const uint32 tag = _lvlFile->readUint32();
	assert(tag == lvlHdrTag);

	_lvlHdr.screensCount = _lvlFile->readByte();
	_lvlHdr.staticLvlObjectsCount = _lvlFile->readByte();
	_lvlHdr.extraLvlObjectsCount = _lvlFile->readByte();
	_lvlHdr.spritesCount = _lvlFile->readByte();
	debug(DEBUG_RESOURCE, "Resource::loadLvlData() %d %d %d %d", _lvlHdr.screensCount, _lvlHdr.staticLvlObjectsCount, _lvlHdr.extraLvlObjectsCount, _lvlHdr.spritesCount);

	for (int i = 0; i < _lvlHdr.screensCount; ++i) {
		loadLvlScreenMoveData(i);
	}
	for (int i = 0; i < _lvlHdr.screensCount; ++i) {
		loadLvlScreenVectorData(i);
	}
	for (int i = 0; i < _lvlHdr.screensCount; ++i) {
		loadLvlScreenStateData(i);
	}
	for (int i = 0; i < (0x2988 - 0x288) / 96; ++i) {
		loadLvlScreenObjectData(i);
	}

	loadLevelData0x470C();

	memset(_resLevelData0x2B88SizeTable, 0, sizeof(_resLevelData0x2B88SizeTable));
	memset(_resLevelData0x2988SizeTable, 0, sizeof(_resLevelData0x2988SizeTable));
	memset(_resLevelData0x2988PtrTable, 0, sizeof(_resLevelData0x2988PtrTable));

	for (int i = 0; i < _lvlHdr.spritesCount; ++i) {
		loadLvlSpriteData(i);
	}

//	loadLevelDataMst();
}

static void resFixPointersLevelData0x2B88(const uint8 *src, uint8 *ptr, LvlBackgroundData *dat) {
	const uint8 *src_ = src;

	dat->backgroundCount = *src++;
	dat->currentBackgroundId = *src++;
	dat->dataUnk0Count = *src++;
	dat->unk3 = *src++;
	dat->dataUnk1Count = *src++;
	dat->currentDataUnk1Id = *src++;
	dat->dataUnk2Count = *src++;
	dat->unk7 = *src++;
	dat->dataUnk3Count = *src++;
	dat->unk9 = *src++;
	dat->dataUnk45Count = *src++;
	dat->unkB = *src++;
	dat->backgroundPaletteId = READ_LE_UINT16(src); src += 2;
	dat->backgroundBitmapId  = READ_LE_UINT16(src); src += 2;
	for (int i = 0; i < 4; ++i) {
		const uint32 offs = READ_LE_UINT32(src); src += 4;
		dat->backgroundPaletteTable[i] = (offs != 0) ? ptr + offs : 0;
	}
	for (int i = 0; i < 4; ++i) {
		const uint32 offs = READ_LE_UINT32(src); src += 4;
		dat->backgroundBitmapTable[i] = (offs != 0) ? ptr + offs : 0;
	}
	for (int i = 0; i < 4; ++i) {
		const uint32 offs = READ_LE_UINT32(src); src += 4;
		dat->dataUnk0Table[i] = (offs != 0) ? ptr + offs : 0;
	}
	for (int i = 0; i < 4; ++i) {
		const uint32 offs = READ_LE_UINT32(src); src += 4;
		dat->backgroundMaskTable[i] = (offs != 0) ? ptr + offs : 0;
	}
	for (int i = 0; i < 4; ++i) {
		const uint32 offs = READ_LE_UINT32(src); src += 4;
		dat->dataUnk2Table[i] = (offs != 0) ? ptr + offs : 0;
	}
	for (int i = 0; i < 4; ++i) {
		const uint32 offs = READ_LE_UINT32(src); src += 4;
		dat->backgroundAnimationTable[i] = (offs != 0) ? ptr + offs : 0;
	}
	for (int i = 0; i < 4; ++i) {
		const uint32 offs = READ_LE_UINT32(src); src += 4;
		dat->dataUnk4Table[i] = (offs != 0) ? ptr + offs : 0;
	}
	for (int i = 0; i < 4; ++i) {
		const uint32 offs = READ_LE_UINT32(src); src += 4;
		if (offs != 0) {
			dat->dataUnk5Table[i] = (LvlObjectData *)malloc(sizeof(LvlObjectData));
			resFixPointersLevelData0x2988(ptr + offs, 0, dat->dataUnk5Table[i]);
		} else {
			dat->dataUnk5Table[i] = 0;
		}
	}
	for (int i = 0; i < 4; ++i) {
		const uint32 offs = READ_LE_UINT32(src); src += 4;
		dat->dataUnk6Table[i] = (offs != 0) ? ptr + offs : 0;
	}
	assert((src - src_) == 160);
}

void Resource::loadLvlScreenBackgroundData(int num) {
	assert(num >= 0 && num < 40);

	_lvlFile->seekAlign(0x2B88 + num * 16);
	const uint32 offs = _lvlFile->readUint32();
	const int size = _lvlFile->readUint32();
	const int readSize = _lvlFile->readUint32();
	uint8 *ptr = (uint8 *)malloc(size);
	_lvlFile->seek(offs, SEEK_SET);
	_lvlFile->read(ptr, readSize);

	_lvlFile->seekAlign(0x2E08 + num * 160);
	uint8 buf[160];
	_lvlFile->read(buf, 160);
	LvlBackgroundData *dat = &_resLvlScreenBackgroundDataTable[num];
	resFixPointersLevelData0x2B88(buf, ptr, dat);

	_resLvlScreenBackgroundDataPtrTable[num] = ptr;
	_resLevelData0x2B88SizeTable[num] = size;
}

void Resource::unloadLvlScreenBackgroundData(int num) {
	if (_resLevelData0x2B88SizeTable[num] != 0) {
		free(_resLvlScreenBackgroundDataPtrTable[num]);
		_resLvlScreenBackgroundDataPtrTable[num] = 0;
		_resLevelData0x2B88SizeTable[num] = 0;
	}
}

bool Resource::isLevelData0x2988Loaded(int num) {
	return _resLevelData0x2988SizeTable[num] != 0;
}

bool Resource::isLevelData0x2B88Loaded(int num) {
	return _resLevelData0x2B88SizeTable[num] != 0;
}

void Resource::incLevelData0x2988RefCounter(LvlObject *ptr) {
	LvlObjectData *dat = _resLevelData0x2988PtrTable[ptr->data0x2988];
	assert(dat);
	++dat->refCount;
	ptr->levelData0x2988 = dat;
}

void Resource::decLevelData0x2988RefCounter(LvlObject *ptr) {
	LvlObjectData *dat = _resLevelData0x2988PtrTable[ptr->data0x2988];
	if (dat) {
		--dat->refCount;
	}
}

LvlObject *Resource::findLvlObject(uint8 type, uint8 num, int index) {
	LvlObject *ptr = _resLvlData0x288PtrTable[index];
	while (ptr) {
		if (ptr->type == type && ptr->data0x2988 == num) {
			break;
		}
		ptr = ptr->nextPtr;
	}
	return ptr;
}

void Resource::loadSetupImage(int num, uint8 *dst, uint8 *pal) {
	if (!_datFile) {
		return;
	}
	const int offset = _datHdr._setupImageOffsetTable[num];
	const int size = _datHdr._setupImageSizeTable[num];
	assert(size == 256 * 192);
	_datFile->seek(offset, SEEK_SET);
	_datFile->read(dst, size);
	_datFile->flush();
	_datFile->read(pal, 768);
}

uint8 *Resource::getLvlSpriteFramePtr(LvlObjectData *dat, int frame) {
	assert(frame < dat->framesCount);
	uint8 *p = dat->framesData;
	for (int i = 0; i < frame; ++i) {
		const int size = READ_LE_UINT16(p);
		p += size;
	}
	return p;
}

uint8 *Resource::getLvlSpriteCoordPtr(LvlObjectData *dat, int num) {
	assert(num < dat->animsCount);
	uint8 *p = dat->coordsData;
	for (int i = 0; i < num; ++i) {
		const int count = p[0];
		p += count * 4 + 1;
	}
	return p;
}

void Resource::loadSssData(const char *levelName) {
	if (!_sssFile) return;
	char filename[32];
	snprintf(filename, sizeof(filename), "%s.SSS", levelName);
	if (!_sssFile->open(filename)) {
		error("Unable to open '%s'", filename);
		return;
	}
#if 0
	if (_sssBuffer1) {
		int count = _sssHdr.unk30;
		if (count > _sssHdr.unk8) {
			count = _sssHdr.unk8;
		}
		for (int i = 0; i < count; ++i) {
			free(_res_sssUnkTable[i * 20].unk0);
		}
		free(_sssBuffer1);
		_sssBuffer1 = 0;
		_sssHdr.unk10 = 0;
	}
#endif
	_sssHdr.unk0 = _sssFile->readUint32();
	assert(_sssHdr.unk0 == 10);
	_sssHdr.unk4 = _sssFile->readUint32(); // _edi
	_sssHdr.unk8 = _sssFile->readUint32();
	_sssHdr.unkC = _sssFile->readUint32();
	_sssHdr.unk10 = _sssFile->readUint32();
	_sssHdr.unk14 = _sssFile->readUint32(); // _eax
	_sssHdr.unk18 = _sssFile->readUint32(); // _ecx
	_sssHdr.unk1C = _sssFile->readUint32();
	_sssHdr.unk20 = _sssFile->readUint32();
	_sssHdr.unk24 = _sssFile->readUint32() & 255;
	_sssHdr.unk28 = _sssFile->readUint32() & 255;
	_sssHdr.unk2C = _sssFile->readUint32() & 255;
	_sssHdr.unk30 = _sssFile->readUint32(); // _edx
}

void Resource::checkSoundSize(const uint8 *buf, int size) {
	const uint8 *end = buf + size;
	while (buf < end) {
		switch (*buf) {
		case 0:
		case 2:
		case 4:
		case 9:
		case 10:
		case 11:
		case 12:
		case 16:
		case 19:
		case 20:
		case 21:
		case 23:
		case 25:
		case 29:
			buf += 4;
			break;
		case 6:
		case 13:
		case 28:
			buf += 8;
			break;
		case 5:
		case 14:
			buf += 12;
			break;
		case 8:
		case 17:
		case 18:
		case 22:
		case 24:
		case 26:
		case 27:
			buf += 16;
			break;
		}
	}
}

