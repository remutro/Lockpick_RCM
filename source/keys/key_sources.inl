/*
 * Copyright (c) 2019-2020 shchmue
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Sha256 hash of the null string.
static u8 null_hash[0x20] = {
    0xE3, 0xB0, 0xC4, 0x42, 0x98, 0xFC, 0x1C, 0x14, 0x9A, 0xFB, 0xF4, 0xC8, 0x99, 0x6F, 0xB9, 0x24,
    0x27, 0xAE, 0x41, 0xE4, 0x64, 0x9B, 0x93, 0x4C, 0xA4, 0x95, 0x99, 0x1B, 0x78, 0x52, 0xB8, 0x55};

static const u8 keyblob_key_source[][0x10] = {
    {0xDF, 0x20, 0x6F, 0x59, 0x44, 0x54, 0xEF, 0xDC, 0x70, 0x74, 0x48, 0x3B, 0x0D, 0xED, 0x9F, 0xD3}, //1.0.0
    {0x0C, 0x25, 0x61, 0x5D, 0x68, 0x4C, 0xEB, 0x42, 0x1C, 0x23, 0x79, 0xEA, 0x82, 0x25, 0x12, 0xAC}, //3.0.0
    {0x33, 0x76, 0x85, 0xEE, 0x88, 0x4A, 0xAE, 0x0A, 0xC2, 0x8A, 0xFD, 0x7D, 0x63, 0xC0, 0x43, 0x3B}, //3.0.1
    {0x2D, 0x1F, 0x48, 0x80, 0xED, 0xEC, 0xED, 0x3E, 0x3C, 0xF2, 0x48, 0xB5, 0x65, 0x7D, 0xF7, 0xBE}, //4.0.0
    {0xBB, 0x5A, 0x01, 0xF9, 0x88, 0xAF, 0xF5, 0xFC, 0x6C, 0xFF, 0x07, 0x9E, 0x13, 0x3C, 0x39, 0x80}, //5.0.0
    {0xD8, 0xCC, 0xE1, 0x26, 0x6A, 0x35, 0x3F, 0xCC, 0x20, 0xF3, 0x2D, 0x3B, 0x51, 0x7D, 0xE9, 0xC0}  //6.0.0
};

static const u8 master_kek_sources[KB_FIRMWARE_VERSION_MAX - KB_FIRMWARE_VERSION_620 + 1][0x10] = {
    {0x37, 0x4B, 0x77, 0x29, 0x59, 0xB4, 0x04, 0x30, 0x81, 0xF6, 0xE5, 0x8C, 0x6D, 0x36, 0x17, 0x9A}, //6.2.0
    {0x9A, 0x3E, 0xA9, 0xAB, 0xFD, 0x56, 0x46, 0x1C, 0x9B, 0xF6, 0x48, 0x7F, 0x5C, 0xFA, 0x09, 0x5C}, //7.0.0
    {0xDE, 0xDC, 0xE3, 0x39, 0x30, 0x88, 0x16, 0xF8, 0xAE, 0x97, 0xAD, 0xEC, 0x64, 0x2D, 0x41, 0x41}, //8.1.0
    {0x1A, 0xEC, 0x11, 0x82, 0x2B, 0x32, 0x38, 0x7A, 0x2B, 0xED, 0xBA, 0x01, 0x47, 0x7E, 0x3B, 0x67}, //9.0.0
    {0x30, 0x3F, 0x02, 0x7E, 0xD8, 0x38, 0xEC, 0xD7, 0x93, 0x25, 0x34, 0xB5, 0x30, 0xEB, 0xCA, 0x7A}, //9.1.0
};

static const u8 master_key_vectors[KB_FIRMWARE_VERSION_MAX + 1][0x10] = {
    {0x0C, 0xF0, 0x59, 0xAC, 0x85, 0xF6, 0x26, 0x65, 0xE1, 0xE9, 0x19, 0x55, 0xE6, 0xF2, 0x67, 0x3D}, /* Zeroes encrypted with Master Key 00. */
    {0x29, 0x4C, 0x04, 0xC8, 0xEB, 0x10, 0xED, 0x9D, 0x51, 0x64, 0x97, 0xFB, 0xF3, 0x4D, 0x50, 0xDD}, /* Master key 00 encrypted with Master key 01. */
    {0xDE, 0xCF, 0xEB, 0xEB, 0x10, 0xAE, 0x74, 0xD8, 0xAD, 0x7C, 0xF4, 0x9E, 0x62, 0xE0, 0xE8, 0x72}, /* Master key 01 encrypted with Master key 02. */
    {0x0A, 0x0D, 0xDF, 0x34, 0x22, 0x06, 0x6C, 0xA4, 0xE6, 0xB1, 0xEC, 0x71, 0x85, 0xCA, 0x4E, 0x07}, /* Master key 02 encrypted with Master key 03. */
    {0x6E, 0x7D, 0x2D, 0xC3, 0x0F, 0x59, 0xC8, 0xFA, 0x87, 0xA8, 0x2E, 0xD5, 0x89, 0x5E, 0xF3, 0xE9}, /* Master key 03 encrypted with Master key 04. */
    {0xEB, 0xF5, 0x6F, 0x83, 0x61, 0x9E, 0xF8, 0xFA, 0xE0, 0x87, 0xD7, 0xA1, 0x4E, 0x25, 0x36, 0xEE}, /* Master key 04 encrypted with Master key 05. */
    {0x1E, 0x1E, 0x22, 0xC0, 0x5A, 0x33, 0x3C, 0xB9, 0x0B, 0xA9, 0x03, 0x04, 0xBA, 0xDB, 0x07, 0x57}, /* Master key 05 encrypted with Master key 06. */
    {0xA4, 0xD4, 0x52, 0x6F, 0xD1, 0xE4, 0x36, 0xAA, 0x9F, 0xCB, 0x61, 0x27, 0x1C, 0x67, 0x65, 0x1F}, /* Master key 06 encrypted with Master key 07. */
    {0xEA, 0x60, 0xB3, 0xEA, 0xCE, 0x8F, 0x24, 0x46, 0x7D, 0x33, 0x9C, 0xD1, 0xBC, 0x24, 0x98, 0x29}, /* Master key 07 encrypted with Master key 08. */
    {0x4D, 0xD9, 0x98, 0x42, 0x45, 0x0D, 0xB1, 0x3C, 0x52, 0x0C, 0x9A, 0x44, 0xBB, 0xAD, 0xAF, 0x80}, /* Master key 08 encrypted with Master key 09. */
    {0xB8, 0x96, 0x9E, 0x4A, 0x00, 0x0D, 0xD6, 0x28, 0xB3, 0xD1, 0xDB, 0x68, 0x5F, 0xFB, 0xE1, 0x2A}, /* Master key 09 encrypted with Master key 0A. */
};

//======================================Keys======================================//
// from Package1 -> Secure_Monitor
static const u8 aes_kek_generation_source[0x10] = {
    0x4D, 0x87, 0x09, 0x86, 0xC4, 0x5D, 0x20, 0x72, 0x2F, 0xBA, 0x10, 0x53, 0xDA, 0x92, 0xE8, 0xA9};
static const u8 aes_kek_seed_01[0x10] = {
    0xA2, 0xAB, 0xBF, 0x9C, 0x92, 0x2F, 0xBB, 0xE3, 0x78, 0x79, 0x9B, 0xC0, 0xCC, 0xEA, 0xA5, 0x74};
static const u8 aes_kek_seed_03[0x10] = {
    0xE5, 0x4D, 0x9A, 0x02, 0xF0, 0x4F, 0x5F, 0xA8, 0xAD, 0x76, 0x0A, 0xF6, 0x32, 0x95, 0x59, 0xBB};
static const u8 package2_key_source[0x10] = {
    0xFB, 0x8B, 0x6A, 0x9C, 0x79, 0x00, 0xC8, 0x49, 0xEF, 0xD2, 0x4D, 0x85, 0x4D, 0x30, 0xA0, 0xC7};
static const u8 titlekek_source[0x10] = {
    0x1E, 0xDC, 0x7B, 0x3B, 0x60, 0xE6, 0xB4, 0xD8, 0x78, 0xB8, 0x17, 0x15, 0x98, 0x5E, 0x62, 0x9B};
static const u8 retail_specific_aes_key_source[0x10] = {
    0xE2, 0xD6, 0xB8, 0x7A, 0x11, 0x9C, 0xB8, 0x80, 0xE8, 0x22, 0x88, 0x8A, 0x46, 0xFB, 0xA1, 0x95};

// from Package1ldr (or Secure_Monitor on 6.2.0+)
static const u8 keyblob_mac_key_source[0x10] = {
    0x59, 0xC7, 0xFB, 0x6F, 0xBE, 0x9B, 0xBE, 0x87, 0x65, 0x6B, 0x15, 0xC0, 0x53, 0x73, 0x36, 0xA5};
static const u8 master_key_source[0x10] = {
    0xD8, 0xA2, 0x41, 0x0A, 0xC6, 0xC5, 0x90, 0x01, 0xC6, 0x1D, 0x6A, 0x26, 0x7C, 0x51, 0x3F, 0x3C};
static const u8 per_console_key_source[0x10] = {
    0x4F, 0x02, 0x5F, 0x0E, 0xB6, 0x6D, 0x11, 0x0E, 0xDC, 0x32, 0x7D, 0x41, 0x86, 0xC2, 0xF4, 0x78};
static const u8 device_master_key_source_kek_source[0x10] = {
    0x0C, 0x91, 0x09, 0xDB, 0x93, 0x93, 0x07, 0x81, 0x07, 0x3C, 0xC4, 0x16, 0x22, 0x7C, 0x6C, 0x28};
static const u8 mariko_master_kek_sources[KB_FIRMWARE_VERSION_MAX - KB_FIRMWARE_VERSION_600 + 1][0x10] = {
    {0x77, 0x60, 0x5A, 0xD2, 0xEE, 0x6E, 0xF8, 0x3C, 0x3F, 0x72, 0xE2, 0x59, 0x9D, 0xAC, 0x5E, 0x56}, // 6.0.0.
    {0x1E, 0x80, 0xB8, 0x17, 0x3E, 0xC0, 0x60, 0xAA, 0x11, 0xBE, 0x1A, 0x4A, 0xA6, 0x6F, 0xE4, 0xAE}, // 6.2.0.
    {0x94, 0x08, 0x67, 0xBD, 0x0A, 0x00, 0x38, 0x84, 0x11, 0xD3, 0x1A, 0xDB, 0xDD, 0x8D, 0xF1, 0x8A}, // 7.0.0.
    {0x5C, 0x24, 0xE3, 0xB8, 0xB4, 0xF7, 0x00, 0xC2, 0x3C, 0xFD, 0x0A, 0xCE, 0x13, 0xC3, 0xDC, 0x23}, // 8.1.0.
    {0x86, 0x69, 0xF0, 0x09, 0x87, 0xC8, 0x05, 0xAE, 0xB5, 0x7B, 0x48, 0x74, 0xDE, 0x62, 0xA6, 0x13}, // 9.0.0.
    {0x0E, 0x44, 0x0C, 0xED, 0xB4, 0x36, 0xC0, 0x3F, 0xAA, 0x1D, 0xAE, 0xBF, 0x62, 0xB1, 0x09, 0x82}, // 9.1.0.
};

static const u8 device_master_key_source_sources[KB_FIRMWARE_VERSION_MAX - KB_FIRMWARE_VERSION_400 + 1][0x10] = {
    {0x8B, 0x4E, 0x1C, 0x22, 0x42, 0x07, 0xC8, 0x73, 0x56, 0x94, 0x08, 0x8B, 0xCC, 0x47, 0x0F, 0x5D}, /* 4.0.0 Device Master Key Source Source. */
    {0x6C, 0xEF, 0xC6, 0x27, 0x8B, 0xEC, 0x8A, 0x91, 0x99, 0xAB, 0x24, 0xAC, 0x4F, 0x1C, 0x8F, 0x1C}, /* 5.0.0 Device Master Key Source Source. */
    {0x70, 0x08, 0x1B, 0x97, 0x44, 0x64, 0xF8, 0x91, 0x54, 0x9D, 0xC6, 0x84, 0x8F, 0x1A, 0xB2, 0xE4}, /* 6.0.0 Device Master Key Source Source. */
    {0x8E, 0x09, 0x1F, 0x7A, 0xBB, 0xCA, 0x6A, 0xFB, 0xB8, 0x9B, 0xD5, 0xC1, 0x25, 0x9C, 0xA9, 0x17}, /* 6.2.0 Device Master Key Source Source. */
    {0x8F, 0x77, 0x5A, 0x96, 0xB0, 0x94, 0xFD, 0x8D, 0x28, 0xE4, 0x19, 0xC8, 0x16, 0x1C, 0xDB, 0x3D}, /* 7.0.0 Device Master Key Source Source. */
    {0x67, 0x62, 0xD4, 0x8E, 0x55, 0xCF, 0xFF, 0x41, 0x31, 0x15, 0x3B, 0x24, 0x0C, 0x7C, 0x07, 0xAE}, /* 8.1.0 Device Master Key Source Source. */
    {0x4A, 0xC3, 0x4E, 0x14, 0x8B, 0x96, 0x4A, 0xD5, 0xD4, 0x99, 0x73, 0xC4, 0x45, 0xAB, 0x8B, 0x49}, /* 9.0.0 Device Master Key Source Source. */
    {0x14, 0xB8, 0x74, 0x12, 0xCB, 0xBD, 0x0B, 0x8F, 0x20, 0xFB, 0x30, 0xDA, 0x27, 0xE4, 0x58, 0x94}, /* 9.1.0 Device Master Key Source Source. */
};

// from ES
static const u8 eticket_rsa_kek_source[0x10] = {
    0XDB, 0XA4, 0X51, 0X12, 0X4C, 0XA0, 0XA9, 0X83, 0X68, 0X14, 0XF5, 0XED, 0X95, 0XE3, 0X12, 0X5B};
static const u8 eticket_rsa_kekek_source[0x10] = {
    0X46, 0X6E, 0X57, 0XB7, 0X4A, 0X44, 0X7F, 0X02, 0XF3, 0X21, 0XCD, 0XE5, 0X8F, 0X2F, 0X55, 0X35};

// from SSL
static const u8 ssl_rsa_kek_source_x[0x10] = {
    0X7F, 0X5B, 0XB0, 0X84, 0X7B, 0X25, 0XAA, 0X67, 0XFA, 0XC8, 0X4B, 0XE2, 0X3D, 0X7B, 0X69, 0X03};
static const u8 ssl_rsa_kek_source_y[0x10] = {
    0X9A, 0X38, 0X3B, 0XF4, 0X31, 0XD0, 0XBD, 0X81, 0X32, 0X53, 0X4B, 0XA9, 0X64, 0X39, 0X7D, 0XE3};

static const u8 device_master_kek_sources[KB_FIRMWARE_VERSION_MAX - KB_FIRMWARE_VERSION_400 + 1][0x10] = {
    {0x88, 0x62, 0x34, 0x6E, 0xFA, 0xF7, 0xD8, 0x3F, 0xE1, 0x30, 0x39, 0x50, 0xF0, 0xB7, 0x5D, 0x5D}, /* 4.0.0 Device Master Kek Source. */
    {0x06, 0x1E, 0x7B, 0xE9, 0x6D, 0x47, 0x8C, 0x77, 0xC5, 0xC8, 0xE7, 0x94, 0x9A, 0xA8, 0x5F, 0x2E}, /* 5.0.0 Device Master Kek Source. */
    {0x99, 0xFA, 0x98, 0xBD, 0x15, 0x1C, 0x72, 0xFD, 0x7D, 0x9A, 0xD5, 0x41, 0x00, 0xFD, 0xB2, 0xEF}, /* 6.0.0 Device Master Kek Source. */
    {0x81, 0x3C, 0x6C, 0xBF, 0x5D, 0x21, 0xDE, 0x77, 0x20, 0xD9, 0x6C, 0xE3, 0x22, 0x06, 0xAE, 0xBB}, /* 6.2.0 Device Master Kek Source. */
    {0x86, 0x61, 0xB0, 0x16, 0xFA, 0x7A, 0x9A, 0xEA, 0xF6, 0xF5, 0xBE, 0x1A, 0x13, 0x5B, 0x6D, 0x9E}, /* 7.0.0 Device Master Kek Source. */
    {0xA6, 0x81, 0x71, 0xE7, 0xB5, 0x23, 0x74, 0xB0, 0x39, 0x8C, 0xB7, 0xFF, 0xA0, 0x62, 0x9F, 0x8D}, /* 8.1.0 Device Master Kek Source. */
    {0x03, 0xE7, 0xEB, 0x43, 0x1B, 0xCF, 0x5F, 0xB5, 0xED, 0xDC, 0x97, 0xAE, 0x21, 0x8D, 0x19, 0xED}, /* 9.0.0 Device Master Kek Source. */
    {0xCE, 0xFE, 0x41, 0x0F, 0x46, 0x9A, 0x30, 0xD6, 0xF2, 0xE9, 0x0C, 0x6B, 0xB7, 0x15, 0x91, 0x36}, /* 9.1.0 Device Master Kek Source. */
};

// from SPL
static const u8 aes_key_generation_source[0x10] = {
    0x89, 0x61, 0x5E, 0xE0, 0x5C, 0x31, 0xB6, 0x80, 0x5F, 0xE5, 0x8F, 0x3D, 0xA2, 0x4F, 0x7A, 0xA8};

// from FS
static const u8 bis_kek_source[0x10] = {
    0x34, 0xC1, 0xA0, 0xC4, 0x82, 0x58, 0xF8, 0xB4, 0xFA, 0x9E, 0x5E, 0x6A, 0xDA, 0xFC, 0x7E, 0x4F};
static const u8 bis_key_source[3][0x20] = {
    {0xF8, 0x3F, 0x38, 0x6E, 0x2C, 0xD2, 0xCA, 0x32, 0xA8, 0x9A, 0xB9, 0xAA, 0x29, 0xBF, 0xC7, 0x48,
     0x7D, 0x92, 0xB0, 0x3A, 0xA8, 0xBF, 0xDE, 0xE1, 0xA7, 0x4C, 0x3B, 0x6E, 0x35, 0xCB, 0x71, 0x06},
    {0x41, 0x00, 0x30, 0x49, 0xDD, 0xCC, 0xC0, 0x65, 0x64, 0x7A, 0x7E, 0xB4, 0x1E, 0xED, 0x9C, 0x5F,
     0x44, 0x42, 0x4E, 0xDA, 0xB4, 0x9D, 0xFC, 0xD9, 0x87, 0x77, 0x24, 0x9A, 0xDC, 0x9F, 0x7C, 0xA4},
    {0x52, 0xC2, 0xE9, 0xEB, 0x09, 0xE3, 0xEE, 0x29, 0x32, 0xA1, 0x0C, 0x1F, 0xB6, 0xA0, 0x92, 0x6C,
     0x4D, 0x12, 0xE1, 0x4B, 0x2A, 0x47, 0x4C, 0x1C, 0x09, 0xCB, 0x03, 0x59, 0xF0, 0x15, 0xF4, 0xE4}
};
static const u8 header_kek_source[0x10] = {
    0x1F, 0x12, 0x91, 0x3A, 0x4A, 0xCB, 0xF0, 0x0D, 0x4C, 0xDE, 0x3A, 0xF6, 0xD5, 0x23, 0x88, 0x2A};
static const u8 header_key_source[0x20] = {
    0x5A, 0x3E, 0xD8, 0x4F, 0xDE, 0xC0, 0xD8, 0x26, 0x31, 0xF7, 0xE2, 0x5D, 0x19, 0x7B, 0xF5, 0xD0,
    0x1C, 0x9B, 0x7B, 0xFA, 0xF6, 0x28, 0x18, 0x3D, 0x71, 0xF6, 0x4D, 0x73, 0xF1, 0x50, 0xB9, 0xD2};
static const u8 key_area_key_sources[3][0x10] = {
    {0x7F, 0x59, 0x97, 0x1E, 0x62, 0x9F, 0x36, 0xA1, 0x30, 0x98, 0x06, 0x6F, 0x21, 0x44, 0xC3, 0x0D}, // application
    {0x32, 0x7D, 0x36, 0x08, 0x5A, 0xD1, 0x75, 0x8D, 0xAB, 0x4E, 0x6F, 0xBA, 0xA5, 0x55, 0xD8, 0x82}, // ocean
    {0x87, 0x45, 0xF1, 0xBB, 0xA6, 0xBE, 0x79, 0x64, 0x7D, 0x04, 0x8B, 0xA6, 0x7B, 0x5F, 0xDA, 0x4A}, // system
};
static const u8 save_mac_kek_source[0x10] = {
    0XD8, 0X9C, 0X23, 0X6E, 0XC9, 0X12, 0X4E, 0X43, 0XC8, 0X2B, 0X03, 0X87, 0X43, 0XF9, 0XCF, 0X1B};
static const u8 save_mac_key_source[0x10] = {
    0XE4, 0XCD, 0X3D, 0X4A, 0XD5, 0X0F, 0X74, 0X28, 0X45, 0XA4, 0X87, 0XE5, 0XA0, 0X63, 0XEA, 0X1F};
static const u8 save_mac_sd_card_kek_source[0x10] = {
    0X04, 0X89, 0XEF, 0X5D, 0X32, 0X6E, 0X1A, 0X59, 0XC4, 0XB7, 0XAB, 0X8C, 0X36, 0X7A, 0XAB, 0X17};
static const u8 save_mac_sd_card_key_source[0x10] = {
    0X6F, 0X64, 0X59, 0X47, 0XC5, 0X61, 0X46, 0XF9, 0XFF, 0XA0, 0X45, 0XD5, 0X95, 0X33, 0X29, 0X18};
static const u8 sd_card_custom_storage_key_source[0x20] = {
    0X37, 0X0C, 0X34, 0X5E, 0X12, 0XE4, 0XCE, 0XFE, 0X21, 0XB5, 0X8E, 0X64, 0XDB, 0X52, 0XAF, 0X35,
    0X4F, 0X2C, 0XA5, 0XA3, 0XFC, 0X99, 0X9A, 0X47, 0XC0, 0X3E, 0XE0, 0X04, 0X48, 0X5B, 0X2F, 0XD0};
static const u8 sd_card_kek_source[0x10] = {
    0X88, 0X35, 0X8D, 0X9C, 0X62, 0X9B, 0XA1, 0XA0, 0X01, 0X47, 0XDB, 0XE0, 0X62, 0X1B, 0X54, 0X32};
static const u8 sd_card_nca_key_source[0x20] = {
    0X58, 0X41, 0XA2, 0X84, 0X93, 0X5B, 0X56, 0X27, 0X8B, 0X8E, 0X1F, 0XC5, 0X18, 0XE9, 0X9F, 0X2B,
    0X67, 0XC7, 0X93, 0XF0, 0XF2, 0X4F, 0XDE, 0XD0, 0X75, 0X49, 0X5D, 0XCA, 0X00, 0X6D, 0X99, 0XC2};
static const u8 sd_card_save_key_source[0x20] = {
    0X24, 0X49, 0XB7, 0X22, 0X72, 0X67, 0X03, 0XA8, 0X19, 0X65, 0XE6, 0XE3, 0XEA, 0X58, 0X2F, 0XDD,
    0X9A, 0X95, 0X15, 0X17, 0XB1, 0X6E, 0X8F, 0X7F, 0X1F, 0X68, 0X26, 0X31, 0X52, 0XEA, 0X29, 0X6A};
