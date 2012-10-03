/*
  Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.

  3. This notice may not be removed or altered from any source
  distribution.
*/

/* Based on:
*  Copyright (C) 1998, 2009
*  Paul E. Jones <paulej@packetizer.com>
*  All Rights Reserved
*/

#include "libpal/pal_types.h"
#include "libpal/pal_endian.h"
#include "libpal/pal_sha1.h"

void SHA1PadMessage(palSHA1::SHA1Context* context);
void SHA1ProcessMessageBlock(palSHA1::SHA1Context* context);


/*
 *  Define the circular shift macro
 */
#define SHA1CircularShift(bits,word) \
                ((((word) << (bits)) & 0xFFFFFFFF) | \
                ((word) >> (32-(bits))))

palSHA1::palSHA1() {

}

void palSHA1::Reset() {
  _context.Length_Low             = 0;
  _context.Length_High            = 0;
  _context.Message_Block_Index    = 0;

  _context.Message_Digest[0]      = 0x67452301;
  _context.Message_Digest[1]      = 0xEFCDAB89;
  _context.Message_Digest[2]      = 0x98BADCFE;
  _context.Message_Digest[3]      = 0x10325476;
  _context.Message_Digest[4]      = 0xC3D2E1F0;

  _context.Computed   = 0;
  _context.Corrupted  = 0;
}

void palSHA1::Update(unsigned char* buf, size_t buf_length) {
  if (buf_length == 0) {
    return;
  }

  if (_context.Computed == 1 || _context.Corrupted == 1) {
    _context.Corrupted = 1;
    return;
  }

  while(buf_length-- && _context.Corrupted == 0) {
    _context.Message_Block[_context.Message_Block_Index++] = (*buf & 0xFF);

    _context.Length_Low += 8;
    /* Force it to 32 bits */
    _context.Length_Low &= 0xFFFFFFFF;
    if (_context.Length_Low == 0) {
      _context.Length_High++;
      /* Force it to 32 bits */
      _context.Length_High &= 0xFFFFFFFF;
      if (_context.Length_High == 0) {
        /* Message is too long */
        _context.Corrupted = 1;
      }
    }

    if (_context.Message_Block_Index == 64) {
      SHA1ProcessMessageBlock(&_context);
    }

    buf++;
  }
}

void palSHA1::Finalize() {
  if (_context.Corrupted == 1) {
    return;
  }

  if (_context.Computed == 0) {
    SHA1PadMessage(&_context);
    _context.Computed = 1;
  }
}

void palSHA1::GetSHA1(unsigned char* sha1) {
  if (_context.Corrupted == 1) {
    return;
  }

  if (_context.Computed == 0) {
    SHA1PadMessage(&_context);
    _context.Computed = 1;
  }

#if 0
  unsigned char* sha1_key = (unsigned char*)(&_context.Message_Digest[0]);
  for (int i = 0; i < 20; i++) {
    sha1[i] = sha1_key[i];
  }
#endif
  unsigned* sha1_out = (unsigned*)sha1;
  sha1_out[0] = palEndianByteSwap(_context.Message_Digest[0]);
  sha1_out[1] = palEndianByteSwap(_context.Message_Digest[1]);
  sha1_out[2] = palEndianByteSwap(_context.Message_Digest[2]);
  sha1_out[3] = palEndianByteSwap(_context.Message_Digest[3]);
  sha1_out[4] = palEndianByteSwap(_context.Message_Digest[4]);

  return;
}

/*  
 *  SHA1ProcessMessageBlock
 *
 *  Description:
 *      This function will process the next 512 bits of the message
 *      stored in the Message_Block array.
 *
 *  Parameters:
 *      None.
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *      Many of the variable names in the SHAContext, especially the
 *      single character names, were used because those were the names
 *      used in the publication.
 *         
 *
 */
void SHA1ProcessMessageBlock(palSHA1::SHA1Context *context)
{
    const unsigned K[] =            /* Constants defined in SHA-1   */      
    {
        0x5A827999,
        0x6ED9EBA1,
        0x8F1BBCDC,
        0xCA62C1D6
    };
    int         t;                  /* Loop counter                 */
    unsigned    temp;               /* Temporary word value         */
    unsigned    W[80];              /* Word sequence                */
    unsigned    A, B, C, D, E;      /* Word buffers                 */

    /*
     *  Initialize the first 16 words in the array W
     */
    for(t = 0; t < 16; t++)
    {
        W[t] = ((unsigned) context->Message_Block[t * 4]) << 24;
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 1]) << 16;
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 2]) << 8;
        W[t] |= ((unsigned) context->Message_Block[t * 4 + 3]);
    }

    for(t = 16; t < 80; t++)
    {
       W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = context->Message_Digest[0];
    B = context->Message_Digest[1];
    C = context->Message_Digest[2];
    D = context->Message_Digest[3];
    E = context->Message_Digest[4];

    for(t = 0; t < 20; t++)
    {
        temp =  SHA1CircularShift(5,A) +
                ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5,A) +
               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for(t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        temp &= 0xFFFFFFFF;
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    context->Message_Digest[0] =
                        (context->Message_Digest[0] + A) & 0xFFFFFFFF;
    context->Message_Digest[1] =
                        (context->Message_Digest[1] + B) & 0xFFFFFFFF;
    context->Message_Digest[2] =
                        (context->Message_Digest[2] + C) & 0xFFFFFFFF;
    context->Message_Digest[3] =
                        (context->Message_Digest[3] + D) & 0xFFFFFFFF;
    context->Message_Digest[4] =
                        (context->Message_Digest[4] + E) & 0xFFFFFFFF;

    context->Message_Block_Index = 0;
}

/*  
 *  SHA1PadMessage
 *
 *  Description:
 *      According to the standard, the message must be padded to an even
 *      512 bits.  The first padding bit must be a '1'.  The last 64
 *      bits represent the length of the original message.  All bits in
 *      between should be 0.  This function will pad the message
 *      according to those rules by filling the Message_Block array
 *      accordingly.  It will also call SHA1ProcessMessageBlock()
 *      appropriately.  When it returns, it can be assumed that the
 *      message digest has been computed.
 *
 *  Parameters:
 *      context: [in/out]
 *          The context to pad
 *
 *  Returns:
 *      Nothing.
 *
 *  Comments:
 *
 */
void SHA1PadMessage(palSHA1::SHA1Context *context)
{
    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */
    if (context->Message_Block_Index > 55) {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 64) {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }

        SHA1ProcessMessageBlock(context);

        while(context->Message_Block_Index < 56) {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    } else {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while(context->Message_Block_Index < 56) {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }

    /*
     *  Store the message length as the last 8 octets
     */
    context->Message_Block[56] = (context->Length_High >> 24) & 0xFF;
    context->Message_Block[57] = (context->Length_High >> 16) & 0xFF;
    context->Message_Block[58] = (context->Length_High >> 8) & 0xFF;
    context->Message_Block[59] = (context->Length_High) & 0xFF;
    context->Message_Block[60] = (context->Length_Low >> 24) & 0xFF;
    context->Message_Block[61] = (context->Length_Low >> 16) & 0xFF;
    context->Message_Block[62] = (context->Length_Low >> 8) & 0xFF;
    context->Message_Block[63] = (context->Length_Low) & 0xFF;

    SHA1ProcessMessageBlock(context);
}
