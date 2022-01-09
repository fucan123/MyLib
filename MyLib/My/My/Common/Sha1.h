#pragma once

typedef struct SHA1Context {
	unsigned Message_Digest[5];
	unsigned Length_Low;
	unsigned Length_High;
	unsigned char Message_Block[64];
	int Message_Block_Index;
	int Computed;
	int Corrupted;
} SHA1Context;

static void SHA1Reset(SHA1Context*);
static int  SHA1Result(SHA1Context*);
static void SHA1Input(SHA1Context*, const char*, unsigned);
static void SHA1ProcessMessageBlock(SHA1Context*);
static void SHA1PadMessage(SHA1Context*);

int sha1_hash(const char* source, char* buf, char* buf_str = 0);