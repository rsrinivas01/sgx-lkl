#ifndef _VIC_H
#define _VIC_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <limits.h>

#define VIC_SECTOR_SIZE 512

#define VIC_INLINE static __inline__

#define VIC_PACK_BEGIN _Pragma("pack(push, 1)")
#define VIC_PACK_END _Pragma("pack(pop)")

/*
**==============================================================================
**
** vic_result_t
**
**==============================================================================
*/

typedef enum _vic_result
{
    VIC_OK,
    VIC_FAILED,
    VIC_BAD_VERSION,
    VIC_UNEXPECTED,
    VIC_BAD_PARAMETER,
    VIC_BAD_DEVICE,
    VIC_OUT_OF_MEMORY,
    VIC_NOT_FOUND,
    VIC_OUT_OF_BOUNDS,
    VIC_KEY_TOO_BIG,
    VIC_DEVICE_COUNT_FAILED,
    VIC_DEVICE_GET_FAILED,
    VIC_DEVICE_PUT_FAILED,
    VIC_DEVICE_TOO_SMALL,
    VIC_HEADER_READ_FAILED,
    VIC_KEY_MATERIAL_READ_FAILED,
    VIC_HEADER_WRITE_FAILED,
    VIC_KEY_MATERIAL_WRITE_FAILED,
    VIC_KEY_LOOKUP_FAILED,
    VIC_PBKDF2_FAILED,
    VIC_ENCRYPT_FAILED,
    VIC_DECRYPT_FAILED,
    VIC_AFMERGE_FAILED,
    VIC_AFSPLIT_FAILED,
    VIC_EOF,
    VIC_UNSUPPORTED,
    VIC_BUFFER_TOO_SMALL,
    VIC_UNKNOWN_KEYSLOT_TYPE,
    VIC_UNKNOWN_KDF_TYPE,
    VIC_DIGEST_NOT_FOUND,
    VIC_ARGON2I_FAILED,
    VIC_ARGON2ID_FAILED,
    VIC_UNSUPPORTED_DIGEST_TYPE,
    VIC_NUM_CPUS_FAILED,
    VIC_OUT_OF_KEYSLOTS,
    VIC_BAD_UUID,
    VIC_LAST_KEYSLOT,
    VIC_UNSUPPORTED_INTEGRITY_JOURNALING,
    VIC_DEVICE_OPEN_FAILED,
    VIC_PATH_TOO_LONG,
    VIC_FAILED_TO_GET_LOOP_DEVICE,
    VIC_UNSUPPORTED_CIPHER,
    VIC_READ_FAILED,
    VIC_WRITE_FAILED,
    VIC_STAT_FAILED,
    VIC_NOT_BLOCK_MULTIPLE,
    VIC_FILE_TOO_SMALL,
    VIC_OPEN_FAILED,
    VIC_SEEK_FAILED,
    VIC_IOCTL_FAILED,
    VIC_BAD_SIGNATURE,
    VIC_BAD_CIPHER,
    VIC_BAD_BLOCK_DEVICE,
    VIC_BAD_BLOCK_SIZE,
    VIC_BAD_FLAGS,
    VIC_BAD_PBKDF_TYPE,
}
vic_result_t;

/*
**==============================================================================
**
** vic_blockdev_t
**
**==============================================================================
*/

typedef struct _vic_blockdev vic_blockdev_t;

typedef struct _vic_blockdev
{
    vic_result_t (*bd_get_path)(
        const vic_blockdev_t* dev,
        char path[PATH_MAX]);

    vic_result_t (*bd_get_block_size)(
        const vic_blockdev_t* dev,
        size_t* block_size);

    vic_result_t (*bd_set_block_size)(
        vic_blockdev_t* dev,
        size_t block_size);

    vic_result_t (*bd_get_byte_size)(
        const vic_blockdev_t* dev,
        size_t* byte_size);

    vic_result_t (*bd_get_num_blocks)(
        const vic_blockdev_t* dev,
        size_t* num_blocks);

    vic_result_t (*bd_get)(
        vic_blockdev_t* dev,
        uint64_t blkno,
        void* blocks,
        size_t nblocks);

    vic_result_t (*bd_put)(
        vic_blockdev_t* dev,
        uint64_t blkno,
        const void* blocks,
        size_t nblocks);

    vic_result_t (*bd_close)(vic_blockdev_t* dev);
}
vic_blockdev_t;

#define VIC_RDONLY 1
#define VIC_WRONLY 2
#define VIC_RDWR   4
#define VIC_CREATE 8
#define VIC_TRUNC  16

vic_result_t vic_blockdev_open(
    const char* path,
    uint32_t flags,
    size_t block_size, /* defaults to 512 if zero */
    vic_blockdev_t** dev);

vic_result_t vic_blockdev_get_path(
    const vic_blockdev_t* dev,
    char path[PATH_MAX]);

vic_result_t vic_blockdev_get_block_size(
    const vic_blockdev_t* dev,
    size_t* block_size);

vic_result_t vic_blockdev_set_block_size(
    vic_blockdev_t* dev,
    size_t block_size);

vic_result_t vic_blockdev_get_byte_size(
    const vic_blockdev_t* dev,
    size_t* byte_size);

vic_result_t vic_blockdev_get_num_blocks(
    vic_blockdev_t* dev,
    size_t* num_blocks);

vic_result_t vic_blockdev_get(
    vic_blockdev_t* dev,
    uint64_t blkno,
    void* blocks,
    size_t nblocks);

vic_result_t vic_blockdev_put(
    vic_blockdev_t* dev,
    uint64_t blkno,
    const void* blocks,
    size_t nblocks);

vic_result_t vic_blockdev_close(vic_blockdev_t* dev);

/*
**==============================================================================
**
** vic_key_t
**
**==============================================================================
*/

typedef struct vic_key
{
    /* 1024 bits */
    uint8_t buf[128];
}
vic_key_t;

/*
**==============================================================================
**
** LUKS interface:
**
**==============================================================================
*/

typedef enum vic_luks_version
{
    LUKS_VERSION_1 = 1,
    LUKS_VERSION_2 = 2,
}
vic_luks_version_t;

typedef struct _vic_luks_stat
{
    vic_luks_version_t version;
    size_t payload_offset;
    size_t payload_size;
}
vic_luks_stat_t;

typedef enum vic_integrity
{
    VIC_INTEGRITY_NONE,
    VIC_INTEGRITY_HMAC_AEAD,
    VIC_INTEGRITY_HMAC_SHA256,
    VIC_INTEGRITY_HMAC_SHA512,
    VIC_INTEGRITY_CMAC_AES,
    VIC_INTEGRITY_POLY1305,
}
vic_integrity_t;

const char* vic_result_string(vic_result_t result);

vic_result_t vic_luks_dump(vic_blockdev_t* device);

vic_result_t vic_luks_load_key(
    const char* path,
    vic_key_t* key,
    size_t* key_size);

vic_result_t vic_luks_format(
    vic_blockdev_t* device,
    vic_luks_version_t version,
    const char* cipher,
    const char* uuid,
    const char* hash,
    uint64_t mk_iterations,
    const vic_key_t* master_key,
    size_t master_key_bytes,
    vic_integrity_t integrity);

vic_result_t vic_luks_recover_master_key(
    vic_blockdev_t* device,
    const char* pwd,
    size_t pwd_size,
    vic_key_t* master_key,
    size_t* master_key_bytes);

typedef struct vic_kdf
{
    /* pbkdf2 */
    const char* hash;
    uint64_t iterations;

    /* argon2i */
    uint64_t time;
    uint64_t memory;
    uint64_t cpus;
}
vic_kdf_t;

vic_result_t vic_luks_add_key(
    vic_blockdev_t* device,
    const char* keyslot_cipher,
    const char* kdf_type,
    vic_kdf_t* kdf,
    const char* pwd,
    size_t pwd_size,
    const char* new_pwd,
    size_t new_pwd_size);

vic_result_t vic_luks_add_key_by_master_key(
    vic_blockdev_t* device,
    const char* keyslot_cipher,
    const char* kdf_type,
    vic_kdf_t* kdf,
    const vic_key_t* master_key,
    size_t master_key_bytes,
    const char* pwd,
    size_t pwd_size);

vic_result_t vic_luks_remove_key(
    vic_blockdev_t* device,
    const char* pwd,
    size_t pwd_size);

vic_result_t vic_luks_change_key(
    vic_blockdev_t* device,
    const char* old_pwd,
    size_t old_pwd_size,
    const char* new_pwd,
    size_t new_pwd_size);

vic_result_t vic_luks_stat(vic_blockdev_t* device, vic_luks_stat_t* buf);

vic_result_t vic_luks_open(
    const char* path,
    const char* name,
    const vic_key_t* master_key,
    size_t master_key_bytes);

vic_result_t vic_luks_close(const char* name);

/*
**==============================================================================
**
** verity interface:
**
**==============================================================================
*/

#define VIC_VERITY_MAX_SALT_SIZE 256

VIC_PACK_BEGIN
typedef struct _vic_verity_sb
{
    /* (0) "verity\0\0" */
    uint8_t  signature[8];

    /* (8) superblock version, 1 */
    uint32_t version;

    /* (12) 0 - Chrome OS, 1 - normal */
    uint32_t hash_type;

    /* (16) UUID of hash device */
    uint8_t uuid[16];

    /* (32) Name of the hash algorithm (e.g., sha256) */
    char algorithm[32];

    /* (64) The data block size in bytes */
    uint32_t data_block_size;

    /* (68) The hash block size in bytes */
    uint32_t hash_block_size;

    /* (72) The number of data blocks */
    uint64_t data_blocks;

    /* (80) Size of the salt */
    uint16_t salt_size;

    /* (82) Padding */
    uint8_t  _pad1[6];

    /* (88) The salt */
    uint8_t  salt[VIC_VERITY_MAX_SALT_SIZE];

    /* Padding */
    uint8_t  _pad2[168];
}
vic_verity_sb_t;
VIC_PACK_END

vic_result_t vic_verity_read_superblock(
    vic_blockdev_t* dev,
    vic_verity_sb_t* sb);

vic_result_t vic_verity_dump(vic_blockdev_t* hash_dev);

vic_result_t vic_verity_format(
    vic_blockdev_t* data_dev,
    vic_blockdev_t* hash_dev,
    const char* hash_algorithm,
    const char* uuid,
    const uint8_t* salt,
    size_t salt_size,
    bool need_superblock,
    uint8_t* root_hash,
    size_t* root_hash_size);

vic_result_t vic_verity_open(
    const char* dm_name,
    vic_blockdev_t* data_dev,
    vic_blockdev_t* hash_dev,
    const void* root_hash,
    size_t root_hash_size);

#endif /* _VIC_H */