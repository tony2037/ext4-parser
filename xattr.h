/* Magic value in attribute blocks */
#define EXT4_XATTR_MAGIC		0xEA020000

/* Maximum number of references to one attribute block */
#define EXT4_XATTR_REFCOUNT_MAX		1024

/* Name indexes */
#define EXT4_XATTR_INDEX_USER			1
#define EXT4_XATTR_INDEX_POSIX_ACL_ACCESS	2
#define EXT4_XATTR_INDEX_POSIX_ACL_DEFAULT	3
#define EXT4_XATTR_INDEX_TRUSTED		4
#define	EXT4_XATTR_INDEX_LUSTRE			5
#define EXT4_XATTR_INDEX_SECURITY	        6
#define EXT4_XATTR_INDEX_SYSTEM			7
#define EXT4_XATTR_INDEX_RICHACL		8
#define EXT4_XATTR_INDEX_ENCRYPTION		9
#define EXT4_XATTR_INDEX_HURD			10 /* Reserved for Hurd */

static inline void INDEX_TO_STRING(uint8_t index, char *prefix)
{
    switch(index) {
        case EXT4_XATTR_INDEX_USER:
            prefix = "user.";
            break;
        case EXT4_XATTR_INDEX_POSIX_ACL_ACCESS:
            prefix = "posix_acl_access.";
            break;
        case EXT4_XATTR_INDEX_POSIX_ACL_DEFAULT:
            prefix = "posix_acl_default";
            break;
        case EXT4_XATTR_INDEX_TRUSTED:
            prefix = "trusted.";
            break;
        case EXT4_XATTR_INDEX_LUSTRE:
            prefix = "lustre.";
            break;
        case EXT4_XATTR_INDEX_SECURITY:
            prefix = "security.";
            break;
        case EXT4_XATTR_INDEX_SYSTEM:
            prefix = "system.";
            break;
        case EXT4_XATTR_INDEX_RICHACL:
            prefix = "richacl.";
            break;
        case EXT4_XATTR_INDEX_ENCRYPTION:
            prefix = "encryption.";
            break;
        case EXT4_XATTR_INDEX_HURD:
            prefix = "hurd.";
            break;
        default:
            prefix = "unknown.";
            break;
    }
}

struct ext4_xattr_header {
	__le32	h_magic;	/* magic number for identification */
	__le32	h_refcount;	/* reference count */
	__le32	h_blocks;	/* number of disk blocks used */
	__le32	h_hash;		/* hash value of all attributes */
	__le32	h_checksum;	/* crc32c(uuid+id+xattrblock) */
				/* id = inum if refcount=1, blknum otherwise */
	__u32	h_reserved[3];	/* zero right now */
};

struct ext4_xattr_ibody_header {
	__le32	h_magic;	/* magic number for identification */
};

struct ext4_xattr_entry {
	__u8	e_name_len;	/* length of name */
	__u8	e_name_index;	/* attribute name index */
	__le16	e_value_offs;	/* offset in disk block of value */
	__le32	e_value_inum;	/* inode in which the value is stored */
	__le32	e_value_size;	/* size of attribute value */
	__le32	e_hash;		/* hash value of name and value */
	char	e_name[];	/* attribute name */
};

#define EXT4_XATTR_PAD_BITS		2
#define EXT4_XATTR_PAD		(1<<EXT4_XATTR_PAD_BITS)
#define EXT4_XATTR_ROUND		(EXT4_XATTR_PAD-1)
#define EXT4_XATTR_LEN(name_len) \
	(((name_len) + EXT4_XATTR_ROUND + \
	sizeof(struct ext4_xattr_entry)) & ~EXT4_XATTR_ROUND)
#define EXT4_XATTR_NEXT(entry) \
	((struct ext4_xattr_entry *)( \
	 (char *)(entry) + EXT4_XATTR_LEN((entry)->e_name_len)))
#define EXT4_XATTR_SIZE(size) \
	(((size) + EXT4_XATTR_ROUND) & ~EXT4_XATTR_ROUND)

// no casting to struct ext4_inode_info *
#define IHDR(inode, raw_inode) \
	((struct ext4_xattr_ibody_header *) \
		((void *)raw_inode + \
		EXT4_GOOD_OLD_INODE_SIZE + \
		inode.i_extra_isize))
#define IFIRST(hdr) ((struct ext4_xattr_entry *)((hdr)+1))

/*
 * XATTR_SIZE_MAX is currently 64k, but for the purposes of checking
 * for file system consistency errors, we use a somewhat bigger value.
 * This allows XATTR_SIZE_MAX to grow in the future, but by using this
 * instead of INT_MAX for certain consistency checks, we don't need to
 * worry about arithmetic overflows.  (Actually XATTR_SIZE_MAX is
 * defined in include/uapi/linux/limits.h, so changing it is going
 * not going to be trivial....)
 */
#define EXT4_XATTR_SIZE_MAX (1 << 24)
