#pragma once

const char* base64_encode(char* base64, const char* bindata, int binlength=-1);
const char* base64_decode(char* res, const char* code);