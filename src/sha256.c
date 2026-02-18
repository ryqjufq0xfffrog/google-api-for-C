#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

uint32_t ror(uint32_t x, int n) {
  return (x << (32 - n)) | (x >> n);
}

void writeInt32BigEndian(char* p, uint32_t number) {
  p[0] = (number >> 24) & 0xff;
  p[1] = (number >> 16) & 0xff;
  p[2] = (number >> 8) & 0xff;
  p[3] = number & 0xff;
  return;
}

/* Calculates SHA256 hash value for provided data
 *
 * Params:
 *   result : at least 32 bytes of memory to write the hash value to
 *   input : data to hash
 *   length : the length of the data
 */
char* sha256gen(char* result, const char* const input, unsigned int length) {
  const unsigned int bufLength = (((length + 9) >> 6) + !!((length + 9) & 63)) * 64;
  unsigned char* buffer = (char* ) malloc(bufLength);

  const uint32_t k[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
  };

  // init
  uint32_t h0 = 0x6a09e667;
  uint32_t h1 = 0xbb67ae85;
  uint32_t h2 = 0x3c6ef372;
  uint32_t h3 = 0xa54ff53a;
  uint32_t h4 = 0x510e527f;
  uint32_t h5 = 0x9b05688c;
  uint32_t h6 = 0x1f83d9ab;
  uint32_t h7 = 0x5be0cd19;

  memcpy(buffer, input, length);
  // Padding
  buffer[length] = 0x80;
  for(unsigned int i = length + 1; i < bufLength - 4; i++) {
    buffer[i] = 0;
  }
  // length is in bytes. Shifting left by 3 to convert to size in bits.
  buffer[bufLength - 4] = (length >> 21) & 0xff;
  buffer[bufLength - 3] = (length >> 13) & 0xff;
  buffer[bufLength - 2] = (length >> 5) & 0xff;
  buffer[bufLength - 1] = (length << 3) & 0xff;

  // For each chunk
  for(unsigned int chunkN = 0; chunkN < bufLength; chunkN += 64) {
    uint32_t w[64];
    // Copy chunk to w
    for(int i = 0; i < 16; i++) {
      const int wordN = chunkN + i * 4;
      w[i] = (buffer[wordN] << 24) | (buffer[wordN + 1] << 16) | (buffer[wordN + 2] << 8) | buffer[wordN + 3];
    }

    // Extend w[0:15] to w[16:63]
    for(int i = 16; i < 64; i++) {
      uint32_t s0 = ror(w[i - 15], 7) ^ ror(w[i - 15], 18) ^ (w[i - 15] >> 3);
      uint32_t s1 = ror(w[i - 2], 17) ^ ror(w[i - 2], 19) ^ (w[i - 2] >> 10);
      w[i] = w[i - 16] + s0 + w[i - 7] + s1;
    }
    // a to h are working variables
    uint32_t a = h0;
    uint32_t b = h1;
    uint32_t c = h2;
    uint32_t d = h3;
    uint32_t e = h4;
    uint32_t f = h5;
    uint32_t g = h6;
    uint32_t h = h7;
    // Compression
    for(int i = 0; i < 64; i++) {
      uint32_t S1 = ror(e, 6) ^ ror(e, 11) ^ ror(e, 25);
      uint32_t ch = (e & f) ^ (~e & g);
      uint32_t tmp1 = h + S1 + ch + k[i] + w[i];
      uint32_t S0 = ror(a, 2) ^ ror(a, 13) ^ ror(a, 22);
      uint32_t maj = (a & b) ^ (b & c) ^ (c & a);
      uint32_t tmp2 = S0 + maj;

      h = g;
      g = f;
      f = e;
      e = d + tmp1;
      d = c;
      c = b;
      b = a;
      a = tmp1 + tmp2;
    }
    // Add to hash
    h0 += a;
    h1 += b;
    h2 += c;
    h3 += d;
    h4 += e;
    h5 += f;
    h6 += g;
    h7 += h;
  }
  // Write result
  writeInt32BigEndian(result, h0);
  writeInt32BigEndian(result + 4, h1);
  writeInt32BigEndian(result + 8, h2);
  writeInt32BigEndian(result + 12, h3);
  writeInt32BigEndian(result + 16, h4);
  writeInt32BigEndian(result + 20, h5);
  writeInt32BigEndian(result + 24, h6);
  writeInt32BigEndian(result + 28, h7);
  // Clean up
  free(buffer);

  return result;
}

/* Binary to hex string
 *
 * Params:
 *   result : enough memory to write the result to
 *   data : binary data
 *   length : the length of the data
 */
char* bin2hexStr(char* hex, unsigned char* data, int length) {
  for(int i = 0; i < length; i++)
    sprintf(hex + 2 * i, "%.2x", data[i]);

  return hex;
}
