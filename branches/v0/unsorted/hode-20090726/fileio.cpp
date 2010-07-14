/*
 * Heart Of Darkness engine rewrite
 * Copyright (C) 2009 Gregory Montoir
 */

#include "fileio.h"

const char *File::_dataPath = ".";

File::File()
	: _fp(0) {
}

File::~File() {
	close();
}

void File::setDataPath(const char *path) {
	_dataPath = path;
}

static void fioToLowerString(char *p) {
	for (; *p; ++p) {
		if (*p >= 'A' && *p <= 'Z') {
			*p += 'a' - 'A';
		}
	}
}

static void fioToUpperString(char *p) {
	for (; *p; ++p) {
		if (*p >= 'a' && *p <= 'z') {
			*p += 'A' - 'a';
		}
	}
}

bool File::open(const char *filename) {
	char filePath[kMaxFilePathLen];
	sprintf(filePath, "%s/%s", _dataPath, filename);
	char *p = strrchr(filePath, '/');
	fioToUpperString(p + 1);
	_fp = fopen(filePath, "rb");
	if (!_fp) {
		fioToLowerString(p + 1);
		_fp = fopen(filePath, "rb");
	}
	return _fp != 0;
}

void File::close() {
	if (_fp) {
		fclose(_fp);
		_fp = 0;
	}
}

void File::seekAlign(int pos) {
	fseek(_fp, pos, SEEK_SET);
}

void File::seek(int pos, int whence) {
	fseek(_fp, pos, whence);
}

int File::read(uint8 *ptr, int size) {
	return fread(ptr, 1, size, _fp);
}

uint8 File::readByte() {
	uint8 buf;
	read(&buf, 1);
	return buf;
}

uint16 File::readUint16() {
	uint8 buf[2];
	read(buf, 2);
	return buf[0] | (buf[1] << 8);
}

uint32 File::readUint32() {
	uint8 buf[4];
	read(buf, 4);
	return buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24);
}

int File::getSize() {
	const int pos = ftell(_fp);
	fseek(_fp, 0, SEEK_END);
	const int size = ftell(_fp);
	fseek(_fp, pos, SEEK_SET);
	return size;
}

void File::flush() {
}

SectorFile::SectorFile() {
	memset(_buf, 0, sizeof(_buf));
	_bufPos = _bufLen = 0;
}

static int fioAlignSizeTo2048(int size) {
	return ((size + 2043) / 2044) * 2048;
}

static uint32 fioUpdateCRC(uint32 sum, const uint8 *buf, uint32 size) {
	assert((size & 3) == 0);
	size >>= 2;
	while (size--) {
		sum ^= READ_LE_UINT32(buf); buf += 4;
	}
	return sum;
}

void SectorFile::refillBuffer() {
	int size = fread(_buf, 1, 2048, _fp);
	if (size == 2048) {
		uint32 crc = fioUpdateCRC(0, _buf, 2048);
		assert(crc == 0);
		size -= 4;
	}
	_bufPos = 0;
	_bufLen = size;
}

void SectorFile::seekAlign(int pos) {
	pos += (pos / 2048) * 4;
	const int alignPos = (pos / 2048) * 2048;
	fseek(_fp, alignPos, SEEK_SET);
	refillBuffer();
	const int skipCount = pos - alignPos;
	_bufPos += skipCount;
	_bufLen -= skipCount;
}

void SectorFile::seek(int pos, int whence) {
	_bufLen = _bufPos = 0;
	assert(whence == SEEK_SET);
	File::seek(pos, whence);
}

int SectorFile::read(uint8 *ptr, int size) {
	if (size >= _bufLen) {
		const int count = fioAlignSizeTo2048(size) / 2048;
		for (int i = 0; i < count; ++i) {
			memcpy(ptr, _buf + _bufPos, _bufLen);
			ptr += _bufLen;
			size -= _bufLen;
			refillBuffer();
			if (_bufLen == 0 || size < _bufLen) {
				break;
			}
		}
	}
	if (_bufLen != 0 && size != 0) {
		memcpy(ptr, _buf + _bufPos, size);
		_bufLen -= size;
		_bufPos += size;
	}
	return 0;
}

void SectorFile::flush() {
	const int currentPos = ftell(_fp);
	assert((currentPos & 2047) == 0);
	_bufLen = _bufPos = 0;
}

