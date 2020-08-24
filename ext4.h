#include<stdint.h>
#include <linux/types.h>

/*
 * Flags used in mballoc's allocation_context flags field.
 *
 * Also used to show what's going on for debugging purposes when the
 * flag field is exported via the traceport interface
 */

/* prefer goal again. length */
#define EXT4_MB_HINT_MERGE		0x0001
/* blocks already reserved */
#define EXT4_MB_HINT_RESERVED		0x0002
/* metadata is being allocated */
#define EXT4_MB_HINT_METADATA		0x0004
/* first blocks in the file */
#define EXT4_MB_HINT_FIRST		0x0008
/* search for the best chunk */
#define EXT4_MB_HINT_BEST		0x0010
/* data is being allocated */
#define EXT4_MB_HINT_DATA		0x0020
/* don't preallocate (for tails) */
#define EXT4_MB_HINT_NOPREALLOC		0x0040
/* allocate for locality group */
#define EXT4_MB_HINT_GROUP_ALLOC	0x0080
/* allocate goal blocks or none */
#define EXT4_MB_HINT_GOAL_ONLY		0x0100
/* goal is meaningful */
#define EXT4_MB_HINT_TRY_GOAL		0x0200
/* blocks already pre-reserved by delayed allocation */
#define EXT4_MB_DELALLOC_RESERVED	0x0400
/* We are doing stream allocation */
#define EXT4_MB_STREAM_ALLOC		0x0800
/* Use reserved root blocks if needed */
#define EXT4_MB_USE_ROOT_BLOCKS		0x1000
/* Use blocks from reserved pool */
#define EXT4_MB_USE_RESERVED		0x2000

/*
 * Special inodes numbers
 */
#define	EXT4_BAD_INO		 1	/* Bad blocks inode */
#define EXT4_ROOT_INO		 2	/* Root inode */
#define EXT4_USR_QUOTA_INO	 3	/* User quota inode */
#define EXT4_GRP_QUOTA_INO	 4	/* Group quota inode */
#define EXT4_BOOT_LOADER_INO	 5	/* Boot loader inode */
#define EXT4_UNDEL_DIR_INO	 6	/* Undelete directory inode */
#define EXT4_RESIZE_INO		 7	/* Reserved group descriptors inode */
#define EXT4_JOURNAL_INO	 8	/* Journal inode */

/* First non-reserved inode for old ext4 filesystems */
#define EXT4_GOOD_OLD_FIRST_INO	11

/*
 * Maximal count of links to a file
 */
#define EXT4_LINK_MAX		65000

/*
 * Macro-instructions used to manage several block sizes
 */
#define EXT4_MIN_BLOCK_SIZE		1024
#define	EXT4_MAX_BLOCK_SIZE		65536
#define EXT4_MIN_BLOCK_LOG_SIZE		10
#define EXT4_MAX_BLOCK_LOG_SIZE		16
#define EXT4_MAX_CLUSTER_LOG_SIZE	30

/*
 * Structure of a blocks group descriptor
 */
struct ext4_group_desc
{
	__le32	bg_block_bitmap_lo;	/* Blocks bitmap block */
	__le32	bg_inode_bitmap_lo;	/* Inodes bitmap block */
	__le32	bg_inode_table_lo;	/* Inodes table block */
	__le16	bg_free_blocks_count_lo;/* Free blocks count */
	__le16	bg_free_inodes_count_lo;/* Free inodes count */
	__le16	bg_used_dirs_count_lo;	/* Directories count */
	__le16	bg_flags;		/* EXT4_BG_flags (INODE_UNINIT, etc) */
	__le32  bg_exclude_bitmap_lo;   /* Exclude bitmap for snapshots */
	__le16  bg_block_bitmap_csum_lo;/* crc32c(s_uuid+grp_num+bbitmap) LE */
	__le16  bg_inode_bitmap_csum_lo;/* crc32c(s_uuid+grp_num+ibitmap) LE */
	__le16  bg_itable_unused_lo;	/* Unused inodes count */
	__le16  bg_checksum;		/* crc16(sb_uuid+group+desc) */
	__le32	bg_block_bitmap_hi;	/* Blocks bitmap block MSB */
	__le32	bg_inode_bitmap_hi;	/* Inodes bitmap block MSB */
	__le32	bg_inode_table_hi;	/* Inodes table block MSB */
	__le16	bg_free_blocks_count_hi;/* Free blocks count MSB */
	__le16	bg_free_inodes_count_hi;/* Free inodes count MSB */
	__le16	bg_used_dirs_count_hi;	/* Directories count MSB */
	__le16  bg_itable_unused_hi;    /* Unused inodes count MSB */
	__le32  bg_exclude_bitmap_hi;   /* Exclude bitmap block MSB */
	__le16  bg_block_bitmap_csum_hi;/* crc32c(s_uuid+grp_num+bbitmap) BE */
	__le16  bg_inode_bitmap_csum_hi;/* crc32c(s_uuid+grp_num+ibitmap) BE */
	uint32_t   bg_reserved;
};

#define EXT4_BG_INODE_UNINIT	0x0001 /* Inode table/bitmap not in use */
#define EXT4_BG_BLOCK_UNINIT	0x0002 /* Block bitmap not in use */
#define EXT4_BG_INODE_ZEROED	0x0004 /* On-disk itable initialized to zero */

/*
 * Macro-instructions used to manage group descriptors
 */
#define EXT4_MIN_DESC_SIZE		32
#define EXT4_MIN_DESC_SIZE_64BIT	64
#define	EXT4_MAX_DESC_SIZE		EXT4_MIN_BLOCK_SIZE

/*
 * Constants relative to the data blocks
 */
#define	EXT4_NDIR_BLOCKS		12
#define	EXT4_IND_BLOCK			EXT4_NDIR_BLOCKS
#define	EXT4_DIND_BLOCK			(EXT4_IND_BLOCK + 1)
#define	EXT4_TIND_BLOCK			(EXT4_DIND_BLOCK + 1)
#define	EXT4_N_BLOCKS			(EXT4_TIND_BLOCK + 1)

/*
 * Inode flags
 */
#define	EXT4_SECRM_FL			0x00000001 /* Secure deletion */
#define	EXT4_UNRM_FL			0x00000002 /* Undelete */
#define	EXT4_COMPR_FL			0x00000004 /* Compress file */
#define EXT4_SYNC_FL			0x00000008 /* Synchronous updates */
#define EXT4_IMMUTABLE_FL		0x00000010 /* Immutable file */
#define EXT4_APPEND_FL			0x00000020 /* writes to file may only append */
#define EXT4_NODUMP_FL			0x00000040 /* do not dump file */
#define EXT4_NOATIME_FL			0x00000080 /* do not update atime */
/* Reserved for compression usage... */
#define EXT4_DIRTY_FL			0x00000100
#define EXT4_COMPRBLK_FL		0x00000200 /* One or more compressed clusters */
#define EXT4_NOCOMPR_FL			0x00000400 /* Don't compress */
	/* nb: was previously EXT2_ECOMPR_FL */
#define EXT4_ENCRYPT_FL			0x00000800 /* encrypted file */
/* End compression flags --- maybe not all used */
#define EXT4_INDEX_FL			0x00001000 /* hash-indexed directory */
#define EXT4_IMAGIC_FL			0x00002000 /* AFS directory */
#define EXT4_JOURNAL_DATA_FL		0x00004000 /* file data should be journaled */
#define EXT4_NOTAIL_FL			0x00008000 /* file tail should not be merged */
#define EXT4_DIRSYNC_FL			0x00010000 /* dirsync behaviour (directories only) */
#define EXT4_TOPDIR_FL			0x00020000 /* Top of directory hierarchies*/
#define EXT4_HUGE_FILE_FL               0x00040000 /* Set to each huge file */
#define EXT4_EXTENTS_FL			0x00080000 /* Inode uses extents */
#define EXT4_EA_INODE_FL	        0x00200000 /* Inode used for large EA */
#define EXT4_EOFBLOCKS_FL		0x00400000 /* Blocks allocated beyond EOF */
#define EXT4_INLINE_DATA_FL		0x10000000 /* Inode has inline data. */
#define EXT4_PROJINHERIT_FL		0x20000000 /* Create with parents projid */
#define EXT4_RESERVED_FL		0x80000000 /* reserved for ext4 lib */

#define EXT4_FL_USER_VISIBLE		0x004BDFFF /* User visible flags */
#define EXT4_FL_USER_MODIFIABLE		0x004380FF /* User modifiable flags */

/* Flags that should be inherited by new inodes from their parent. */
#define EXT4_FL_INHERITED (EXT4_SECRM_FL | EXT4_UNRM_FL | EXT4_COMPR_FL |\
			   EXT4_SYNC_FL | EXT4_NODUMP_FL | EXT4_NOATIME_FL |\
			   EXT4_NOCOMPR_FL | EXT4_JOURNAL_DATA_FL |\
			   EXT4_NOTAIL_FL | EXT4_DIRSYNC_FL)

/* Flags that are appropriate for regular files (all but dir-specific ones). */
#define EXT4_REG_FLMASK (~(EXT4_DIRSYNC_FL | EXT4_TOPDIR_FL))

/* Flags that are appropriate for non-directories/regular files. */
#define EXT4_OTHER_FLMASK (EXT4_NODUMP_FL | EXT4_NOATIME_FL)

/*
 * Inode flags used for atomic set/get
 */
enum {
	EXT4_INODE_SECRM	= 0,	/* Secure deletion */
	EXT4_INODE_UNRM		= 1,	/* Undelete */
	EXT4_INODE_COMPR	= 2,	/* Compress file */
	EXT4_INODE_SYNC		= 3,	/* Synchronous updates */
	EXT4_INODE_IMMUTABLE	= 4,	/* Immutable file */
	EXT4_INODE_APPEND	= 5,	/* writes to file may only append */
	EXT4_INODE_NODUMP	= 6,	/* do not dump file */
	EXT4_INODE_NOATIME	= 7,	/* do not update atime */
/* Reserved for compression usage... */
	EXT4_INODE_DIRTY	= 8,
	EXT4_INODE_COMPRBLK	= 9,	/* One or more compressed clusters */
	EXT4_INODE_NOCOMPR	= 10,	/* Don't compress */
	EXT4_INODE_ENCRYPT	= 11,	/* Encrypted file */
/* End compression flags --- maybe not all used */
	EXT4_INODE_INDEX	= 12,	/* hash-indexed directory */
	EXT4_INODE_IMAGIC	= 13,	/* AFS directory */
	EXT4_INODE_JOURNAL_DATA	= 14,	/* file data should be journaled */
	EXT4_INODE_NOTAIL	= 15,	/* file tail should not be merged */
	EXT4_INODE_DIRSYNC	= 16,	/* dirsync behaviour (directories only) */
	EXT4_INODE_TOPDIR	= 17,	/* Top of directory hierarchies*/
	EXT4_INODE_HUGE_FILE	= 18,	/* Set to each huge file */
	EXT4_INODE_EXTENTS	= 19,	/* Inode uses extents */
	EXT4_INODE_EA_INODE	= 21,	/* Inode used for large EA */
	EXT4_INODE_EOFBLOCKS	= 22,	/* Blocks allocated beyond EOF */
	EXT4_INODE_INLINE_DATA	= 28,	/* Data in inode. */
	EXT4_INODE_PROJINHERIT	= 29,	/* Create with parents projid */
	EXT4_INODE_RESERVED	= 31,	/* reserved for ext4 lib */
};

/* Used to pass group descriptor data when online resize is done */
struct ext4_new_group_input {
	uint32_t group;		/* Group number for this data */
	uint64_t block_bitmap;	/* Absolute block number of block bitmap */
	uint64_t inode_bitmap;	/* Absolute block number of inode bitmap */
	uint64_t inode_table;	/* Absolute block number of inode table start */
	uint32_t blocks_count;	/* Total number of blocks in this group */
	uint16_t reserved_blocks;	/* Number of reserved blocks in this group */
	uint16_t unused;
};

struct compat_ext4_new_group_input {
	uint32_t group;
	uint64_t block_bitmap;
	uint64_t inode_bitmap;
	uint64_t inode_table;
	uint32_t blocks_count;
	uint16_t reserved_blocks;
	uint16_t unused;
};

/* The struct ext4_new_group_input in kernel space, with free_blocks_count */
struct ext4_new_group_data {
	uint32_t group;
	uint64_t block_bitmap;
	uint64_t inode_bitmap;
	uint64_t inode_table;
	uint32_t blocks_count;
	uint16_t reserved_blocks;
	uint16_t unused;
	uint32_t free_blocks_count;
};

/* Indexes used to index group tables in ext4_new_group_data */
enum {
	BLOCK_BITMAP = 0,	/* block bitmap */
	INODE_BITMAP,		/* inode bitmap */
	INODE_TABLE,		/* inode tables */
	GROUP_TABLE_COUNT,
};

/*
 * Flags used by ext4_map_blocks()
 */
	/* Allocate any needed blocks and/or convert an unwritten
	   extent to be an initialized ext4 */
#define EXT4_GET_BLOCKS_CREATE			0x0001
	/* Request the creation of an unwritten extent */
#define EXT4_GET_BLOCKS_UNWRIT_EXT		0x0002
#define EXT4_GET_BLOCKS_CREATE_UNWRIT_EXT	(EXT4_GET_BLOCKS_UNWRIT_EXT|\
						 EXT4_GET_BLOCKS_CREATE)
	/* Caller is from the delayed allocation writeout path
	 * finally doing the actual allocation of delayed blocks */
#define EXT4_GET_BLOCKS_DELALLOC_RESERVE	0x0004
	/* caller is from the direct IO path, request to creation of an
	unwritten extents if not allocated, split the unwritten
	extent if blocks has been preallocated already*/
#define EXT4_GET_BLOCKS_PRE_IO			0x0008
#define EXT4_GET_BLOCKS_CONVERT			0x0010
#define EXT4_GET_BLOCKS_IO_CREATE_EXT		(EXT4_GET_BLOCKS_PRE_IO|\
					 EXT4_GET_BLOCKS_CREATE_UNWRIT_EXT)
	/* Convert extent to initialized after IO complete */
#define EXT4_GET_BLOCKS_IO_CONVERT_EXT		(EXT4_GET_BLOCKS_CONVERT|\
					 EXT4_GET_BLOCKS_CREATE_UNWRIT_EXT)
	/* Eventual metadata allocation (due to growing extent tree)
	 * should not fail, so try to use reserved blocks for that.*/
#define EXT4_GET_BLOCKS_METADATA_NOFAIL		0x0020
	/* Don't normalize allocation size (used for fallocate) */
#define EXT4_GET_BLOCKS_NO_NORMALIZE		0x0040
	/* Request will not result in inode size update (user for fallocate) */
#define EXT4_GET_BLOCKS_KEEP_SIZE		0x0080
	/* Do not take i_data_sem locking in ext4_map_blocks */
#define EXT4_GET_BLOCKS_NO_LOCK			0x0100
	/* Convert written extents to unwritten */
#define EXT4_GET_BLOCKS_CONVERT_UNWRITTEN	0x0200

/*
 * The bit position of these flags must not overlap with any of the
 * EXT4_GET_BLOCKS_*.  They are used by ext4_find_extent(),
 * read_extent_tree_block(), ext4_split_extent_at(),
 * ext4_ext_insert_extent(), and ext4_ext_create_new_leaf().
 * EXT4_EX_NOCACHE is used to indicate that the we shouldn't be
 * caching the extents when reading from the extent tree while a
 * truncate or punch hole operation is in progress.
 */
#define EXT4_EX_NOCACHE				0x40000000
#define EXT4_EX_FORCE_CACHE			0x20000000

/*
 * Flags used by ext4_free_blocks
 */
#define EXT4_FREE_BLOCKS_METADATA	0x0001
#define EXT4_FREE_BLOCKS_FORGET		0x0002
#define EXT4_FREE_BLOCKS_VALIDATED	0x0004
#define EXT4_FREE_BLOCKS_NO_QUOT_UPDATE	0x0008
#define EXT4_FREE_BLOCKS_NOFREE_FIRST_CLUSTER	0x0010
#define EXT4_FREE_BLOCKS_NOFREE_LAST_CLUSTER	0x0020

/* Encryption algorithms */
#define EXT4_ENCRYPTION_MODE_INVALID		0
#define EXT4_ENCRYPTION_MODE_AES_256_XTS	1
#define EXT4_ENCRYPTION_MODE_AES_256_GCM	2
#define EXT4_ENCRYPTION_MODE_AES_256_CBC	3
#define EXT4_ENCRYPTION_MODE_AES_256_CTS	4

/* Max physical block we can address w/o extents */
#define EXT4_MAX_BLOCK_FILE_PHYS	0xFFFFFFFF

/*
 * Structure of an inode on the disk
 */
struct ext4_inode {
	__le16	i_mode;		/* File mode */
	__le16	i_uid;		/* Low 16 bits of Owner Uid */
	__le32	i_size_lo;	/* Size in bytes */
	__le32	i_atime;	/* Access time */
	__le32	i_ctime;	/* Inode Change time */
	__le32	i_mtime;	/* Modification time */
	__le32	i_dtime;	/* Deletion Time */
	__le16	i_gid;		/* Low 16 bits of Group Id */
	__le16	i_links_count;	/* Links count */
	__le32	i_blocks_lo;	/* Blocks count */
	__le32	i_flags;	/* File flags */
	union {
		struct {
			__le32  l_i_version;
		} linux1;
		struct {
			uint32_t  h_i_translator;
		} hurd1;
		struct {
			uint32_t  m_i_reserved1;
		} masix1;
	} osd1;				/* OS dependent 1 */
	__le32	i_block[EXT4_N_BLOCKS];/* Pointers to blocks */
	__le32	i_generation;	/* File version (for NFS) */
	__le32	i_file_acl_lo;	/* File ACL */
	__le32	i_size_high;
	__le32	i_obso_faddr;	/* Obsoleted fragment address */
	union {
		struct {
			__le16	l_i_blocks_high; /* were l_i_reserved1 */
			__le16	l_i_file_acl_high;
			__le16	l_i_uid_high;	/* these 2 fields */
			__le16	l_i_gid_high;	/* were reserved2[0] */
			__le16	l_i_checksum_lo;/* crc32c(uuid+inum+inode) LE */
			__le16	l_i_reserved;
		} linux2;
		struct {
			__le16	h_i_reserved1;	/* Obsoleted fragment number/size which are removed in ext4 */
			uint16_t	h_i_mode_high;
			uint16_t	h_i_uid_high;
			uint16_t	h_i_gid_high;
			uint32_t	h_i_author;
		} hurd2;
		struct {
			__le16	h_i_reserved1;	/* Obsoleted fragment number/size which are removed in ext4 */
			__le16	m_i_file_acl_high;
			uint32_t	m_i_reserved2[2];
		} masix2;
	} osd2;				/* OS dependent 2 */
	__le16	i_extra_isize;
	__le16	i_checksum_hi;	/* crc32c(uuid+inum+inode) BE */
	__le32  i_ctime_extra;  /* extra Change time      (nsec << 2 | epoch) */
	__le32  i_mtime_extra;  /* extra Modification time(nsec << 2 | epoch) */
	__le32  i_atime_extra;  /* extra Access time      (nsec << 2 | epoch) */
	__le32  i_crtime;       /* File Creation time */
	__le32  i_crtime_extra; /* extra FileCreationtime (nsec << 2 | epoch) */
	__le32  i_version_hi;	/* high 32 bits for 64-bit version */
	__le32	i_projid;	/* Project ID */
#ifdef CONFIG_SYNO_EXT4_ARCHIVE_BIT
	__le32  i_syno_archive_bit; /* archive bits and attribute bits for SMB and Syno backup */
#endif /* CONFIG_SYNO_EXT4_ARCHIVE_BIT */
};

struct move_extent {
	uint32_t reserved;		/* should be zero */
	uint32_t donor_fd;		/* donor file descriptor */
	uint64_t orig_start;	/* logical start offset in block for orig */
	uint64_t donor_start;	/* logical start offset in block for donor */
	uint64_t len;		/* block length to be moved */
	uint64_t moved_len;	/* moved block length */
};

#define EXT4_EPOCH_BITS 2
#define EXT4_EPOCH_MASK ((1 << EXT4_EPOCH_BITS) - 1)
#define EXT4_NSEC_MASK  (~0UL << EXT4_EPOCH_BITS)

/*
 * Structure of the super block
 */
struct ext4_super_block {
/*00*/	__le32	s_inodes_count;		/* Inodes count */
	__le32	s_blocks_count_lo;	/* Blocks count */
	__le32	s_r_blocks_count_lo;	/* Reserved blocks count */
	__le32	s_free_blocks_count_lo;	/* Free blocks count */
/*10*/	__le32	s_free_inodes_count;	/* Free inodes count */
	__le32	s_first_data_block;	/* First Data Block */
	__le32	s_log_block_size;	/* Block size */
	__le32	s_log_cluster_size;	/* Allocation cluster size */
/*20*/	__le32	s_blocks_per_group;	/* # Blocks per group */
	__le32	s_clusters_per_group;	/* # Clusters per group */
	__le32	s_inodes_per_group;	/* # Inodes per group */
	__le32	s_mtime;		/* Mount time */
/*30*/	__le32	s_wtime;		/* Write time */
	__le16	s_mnt_count;		/* Mount count */
	__le16	s_max_mnt_count;	/* Maximal mount count */
	__le16	s_magic;		/* Magic signature */
	__le16	s_state;		/* File system state */
	__le16	s_errors;		/* Behaviour when detecting errors */
	__le16	s_minor_rev_level;	/* minor revision level */
/*40*/	__le32	s_lastcheck;		/* time of last check */
	__le32	s_checkinterval;	/* max. time between checks */
	__le32	s_creator_os;		/* OS */
	__le32	s_rev_level;		/* Revision level */
/*50*/	__le16	s_def_resuid;		/* Default uid for reserved blocks */
	__le16	s_def_resgid;		/* Default gid for reserved blocks */
	/*
	 * These fields are for EXT4_DYNAMIC_REV superblocks only.
	 *
	 * Note: the difference between the compatible feature set and
	 * the incompatible feature set is that if there is a bit set
	 * in the incompatible feature set that the kernel doesn't
	 * know about, it should refuse to mount the filesystem.
	 *
	 * e2fsck's requirements are more strict; if it doesn't know
	 * about a feature in either the compatible or incompatible
	 * feature set, it must abort and not try to meddle with
	 * things it doesn't understand...
	 */
	__le32	s_first_ino;		/* First non-reserved inode */
	__le16  s_inode_size;		/* size of inode structure */
	__le16	s_block_group_nr;	/* block group # of this superblock */
	__le32	s_feature_compat;	/* compatible feature set */
/*60*/	__le32	s_feature_incompat;	/* incompatible feature set */
	__le32	s_feature_ro_compat;	/* readonly-compatible feature set */
/*68*/	uint8_t	s_uuid[16];		/* 128-bit uuid for volume */
/*78*/	char	s_volume_name[16];	/* volume name */
/*88*/	char	s_last_mounted[64];	/* directory where last mounted */
/*C8*/	__le32	s_algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT4_FEATURE_COMPAT_DIR_PREALLOC flag is on.
	 */
	uint8_t	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	uint8_t	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	__le16	s_reserved_gdt_blocks;	/* Per group desc for online growth */
	/*
	 * Journaling support valid if EXT4_FEATURE_COMPAT_HAS_JOURNAL set.
	 */
/*D0*/	uint8_t	s_journal_uuid[16];	/* uuid of journal superblock */
/*E0*/	__le32	s_journal_inum;		/* inode number of journal file */
	__le32	s_journal_dev;		/* device number of journal file */
	__le32	s_last_orphan;		/* start of list of inodes to delete */
	__le32	s_hash_seed[4];		/* HTREE hash seed */
	uint8_t	s_def_hash_version;	/* Default hash version to use */
	uint8_t	s_jnl_backup_type;
	__le16  s_desc_size;		/* size of group descriptor */
/*100*/	__le32	s_default_mount_opts;
	__le32	s_first_meta_bg;	/* First metablock block group */
	__le32	s_mkfs_time;		/* When the filesystem was created */
	__le32	s_jnl_blocks[17];	/* Backup of the journal inode */
	/* 64bit support valid if EXT4_FEATURE_COMPAT_64BIT */
/*150*/	__le32	s_blocks_count_hi;	/* Blocks count */
	__le32	s_r_blocks_count_hi;	/* Reserved blocks count */
	__le32	s_free_blocks_count_hi;	/* Free blocks count */
	__le16	s_min_extra_isize;	/* All inodes have at least # bytes */
	__le16	s_want_extra_isize; 	/* New inodes should reserve # bytes */
	__le32	s_flags;		/* Miscellaneous flags */
	__le16  s_raid_stride;		/* RAID stride */
	__le16  s_mmp_update_interval;  /* # seconds to wait in MMP checking */
	__le64  s_mmp_block;            /* Block for multi-mount protection */
	__le32  s_raid_stripe_width;    /* blocks on all data disks (N*stride)*/
	uint8_t	s_log_groups_per_flex;  /* FLEX_BG group size */
	uint8_t	s_checksum_type;	/* metadata checksum algorithm used */
	uint8_t	s_encryption_level;	/* versioning level for encryption */
	uint8_t	s_reserved_pad;		/* Padding to next 32bits */
	__le64	s_kbytes_written;	/* nr of lifetime kilobytes written */
	__le32	s_snapshot_inum;	/* Inode number of active snapshot */
	__le32	s_snapshot_id;		/* sequential ID of active snapshot */
	__le64	s_snapshot_r_blocks_count; /* reserved blocks for active
					      snapshot's future use */
	__le32	s_snapshot_list;	/* inode number of the head of the
					   on-disk snapshot list */
#define EXT4_S_ERR_START offsetof(struct ext4_super_block, s_error_count)
	__le32	s_error_count;		/* number of fs errors */
	__le32	s_first_error_time;	/* first time an error happened */
	__le32	s_first_error_ino;	/* inode involved in first error */
	__le64	s_first_error_block;	/* block involved of first error */
	uint8_t	s_first_error_func[32];	/* function where the error happened */
	__le32	s_first_error_line;	/* line number where error happened */
	__le32	s_last_error_time;	/* most recent time of an error */
	__le32	s_last_error_ino;	/* inode involved in last error */
	__le32	s_last_error_line;	/* line number where error happened */
	__le64	s_last_error_block;	/* block involved of last error */
	uint8_t	s_last_error_func[32];	/* function where the error happened */
#define EXT4_S_ERR_END offsetof(struct ext4_super_block, s_mount_opts)
	uint8_t	s_mount_opts[64];
	__le32	s_usr_quota_inum;	/* inode for tracking user quota */
	__le32	s_grp_quota_inum;	/* inode for tracking group quota */
	__le32	s_overhead_clusters;	/* overhead blocks/clusters in fs */
	__le32	s_backup_bgs[2];	/* groups with sparse_super2 SBs */
	uint8_t	s_encrypt_algos[4];	/* Encryption algorithms in use  */
	uint8_t	s_encrypt_pw_salt[16];	/* Salt used for string2key algorithm */
	__le32	s_lpf_ino;		/* Location of the lost+found inode */
	__le32	s_prj_quota_inum;	/* inode for tracking project quota */
	__le32	s_checksum_seed;	/* crc32c(uuid) if csum_seed set */
#if defined(CONFIG_SYNO_EXT4_ARCHIVE_VERSION) || defined(CONFIG_SYNO_EXT4_CASELESS_STAT)
	__le32	s_reserved[95];	/* Paddint to the end of the block */
	__le32  s_syno_hash_magic;  /* magic number for syno caseless dir_index */
	__le32	s_archive_version;	/* Last archived version */
	__le32  s_archive_version_obsoleted;
#else
	__le32	s_reserved[98];		/* Padding to the end of the block */
#endif /* CONFIG_SYNO_EXT4_ARCHIVE_VERSION || CONFIG_SYNO_EXT4_CASELESS_STAT */
	__le32	s_checksum;		/* crc32c(superblock) */
};

#define EXT4_S_ERR_LEN (EXT4_S_ERR_END - EXT4_S_ERR_START)

/* Number of quota types we support */
#define EXT4_MAXQUOTAS 2

/*
 * Inode dynamic state flags
 */
enum {
	EXT4_STATE_JDATA,		/* journaled data exists */
	EXT4_STATE_NEW,			/* inode is newly created */
	EXT4_STATE_XATTR,		/* has in-inode xattrs */
	EXT4_STATE_NO_EXPAND,		/* No space for expansion */
	EXT4_STATE_DA_ALLOC_CLOSE,	/* Alloc DA blks on close */
	EXT4_STATE_EXT_MIGRATE,		/* Inode is migrating */
	EXT4_STATE_DIO_UNWRITTEN,	/* need convert on dio done*/
	EXT4_STATE_NEWENTRY,		/* File just added to dir */
	EXT4_STATE_DIOREAD_LOCK,	/* Disable support for dio read
					   nolocking */
	EXT4_STATE_MAY_INLINE_DATA,	/* may have in-inode data */
	EXT4_STATE_ORDERED_MODE,	/* data=ordered mode */
	EXT4_STATE_EXT_PRECACHED,	/* extents have been precached */
};

/*
 * Codes for operating systems
 */
#define EXT4_OS_LINUX		0
#define EXT4_OS_HURD		1
#define EXT4_OS_MASIX		2
#define EXT4_OS_FREEBSD		3
#define EXT4_OS_LITES		4

/*
 * Revision levels
 */
#define EXT4_GOOD_OLD_REV	0	/* The good old (original) format */
#define EXT4_DYNAMIC_REV	1	/* V2 format w/ dynamic inode sizes */

#define EXT4_CURRENT_REV	EXT4_GOOD_OLD_REV
#define EXT4_MAX_SUPP_REV	EXT4_DYNAMIC_REV

#define EXT4_GOOD_OLD_INODE_SIZE 128

#define HAS_COMPAT_FEATURE(sb,mask)\
        ( sb.s_feature_compat & (mask) )
#define HAS_RO_COMPAT_FEATURE(sb,mask)\
        ( sb.s_feature_ro_compat & (mask) )
#define HAS_INCOMPAT_FEATURE(sb,mask)\
        ( sb.s_feature_incompat & (mask) )

#define EXT4_FEATURE_COMPAT_DIR_PREALLOC	0x0001
#define EXT4_FEATURE_COMPAT_IMAGIC_INODES	0x0002
#define EXT4_FEATURE_COMPAT_HAS_JOURNAL		0x0004
#define EXT4_FEATURE_COMPAT_EXT_ATTR		0x0008
#define EXT4_FEATURE_COMPAT_RESIZE_INODE	0x0010
#define EXT4_FEATURE_COMPAT_DIR_INDEX		0x0020
#define EXT4_FEATURE_COMPAT_SPARSE_SUPER2	0x0200

#define EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER	0x0001
#define EXT4_FEATURE_RO_COMPAT_LARGE_FILE	0x0002
#define EXT4_FEATURE_RO_COMPAT_BTREE_DIR	0x0004
#define EXT4_FEATURE_RO_COMPAT_HUGE_FILE        0x0008
#define EXT4_FEATURE_RO_COMPAT_GDT_CSUM		0x0010
#define EXT4_FEATURE_RO_COMPAT_DIR_NLINK	0x0020
#define EXT4_FEATURE_RO_COMPAT_EXTRA_ISIZE	0x0040
#define EXT4_FEATURE_RO_COMPAT_QUOTA		0x0100
#define EXT4_FEATURE_RO_COMPAT_BIGALLOC		0x0200
/*
 * METADATA_CSUM also enables group descriptor checksums (GDT_CSUM).  When
 * METADATA_CSUM is set, group descriptor checksums use the same algorithm as
 * all other data structures' checksums.  However, the METADATA_CSUM and
 * GDT_CSUM bits are mutually exclusive.
 */
#define EXT4_FEATURE_RO_COMPAT_METADATA_CSUM	0x0400
#define EXT4_FEATURE_RO_COMPAT_READONLY		0x1000
#define EXT4_FEATURE_RO_COMPAT_PROJECT		0x2000

#define EXT4_FEATURE_INCOMPAT_COMPRESSION	0x0001
#define EXT4_FEATURE_INCOMPAT_FILETYPE		0x0002
#define EXT4_FEATURE_INCOMPAT_RECOVER		0x0004 /* Needs recovery */
#define EXT4_FEATURE_INCOMPAT_JOURNAL_DEV	0x0008 /* Journal device */
#define EXT4_FEATURE_INCOMPAT_META_BG		0x0010
#define EXT4_FEATURE_INCOMPAT_EXTENTS		0x0040 /* extents support */
#define EXT4_FEATURE_INCOMPAT_64BIT		0x0080
#define EXT4_FEATURE_INCOMPAT_MMP               0x0100
#define EXT4_FEATURE_INCOMPAT_FLEX_BG		0x0200
#define EXT4_FEATURE_INCOMPAT_EA_INODE		0x0400 /* EA in inode */
#define EXT4_FEATURE_INCOMPAT_DIRDATA		0x1000 /* data in dirent */
#define EXT4_FEATURE_INCOMPAT_CSUM_SEED		0x2000
#define EXT4_FEATURE_INCOMPAT_LARGEDIR		0x4000 /* >2GB or 3-lvl htree */
#define EXT4_FEATURE_INCOMPAT_INLINE_DATA	0x8000 /* data in inode */
#define EXT4_FEATURE_INCOMPAT_ENCRYPT		0x10000

#define EXT2_FEATURE_COMPAT_SUPP	EXT4_FEATURE_COMPAT_EXT_ATTR
#define EXT2_FEATURE_INCOMPAT_SUPP	(EXT4_FEATURE_INCOMPAT_FILETYPE| \
					 EXT4_FEATURE_INCOMPAT_META_BG)
#define EXT2_FEATURE_RO_COMPAT_SUPP	(EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER| \
					 EXT4_FEATURE_RO_COMPAT_LARGE_FILE| \
					 EXT4_FEATURE_RO_COMPAT_BTREE_DIR)

#define EXT3_FEATURE_COMPAT_SUPP	EXT4_FEATURE_COMPAT_EXT_ATTR
#define EXT3_FEATURE_INCOMPAT_SUPP	(EXT4_FEATURE_INCOMPAT_FILETYPE| \
					 EXT4_FEATURE_INCOMPAT_RECOVER| \
					 EXT4_FEATURE_INCOMPAT_META_BG)
#define EXT3_FEATURE_RO_COMPAT_SUPP	(EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER| \
					 EXT4_FEATURE_RO_COMPAT_LARGE_FILE| \
					 EXT4_FEATURE_RO_COMPAT_BTREE_DIR)

#define EXT4_FEATURE_COMPAT_SUPP	EXT4_FEATURE_COMPAT_EXT_ATTR
#define EXT4_FEATURE_INCOMPAT_SUPP	(EXT4_FEATURE_INCOMPAT_FILETYPE| \
					 EXT4_FEATURE_INCOMPAT_RECOVER| \
					 EXT4_FEATURE_INCOMPAT_META_BG| \
					 EXT4_FEATURE_INCOMPAT_EXTENTS| \
					 EXT4_FEATURE_INCOMPAT_64BIT| \
					 EXT4_FEATURE_INCOMPAT_FLEX_BG| \
					 EXT4_FEATURE_INCOMPAT_MMP | \
					 EXT4_FEATURE_INCOMPAT_INLINE_DATA | \
					 EXT4_FEATURE_INCOMPAT_ENCRYPT | \
					 EXT4_FEATURE_INCOMPAT_CSUM_SEED)
#define EXT4_FEATURE_RO_COMPAT_SUPP	(EXT4_FEATURE_RO_COMPAT_SPARSE_SUPER| \
					 EXT4_FEATURE_RO_COMPAT_LARGE_FILE| \
					 EXT4_FEATURE_RO_COMPAT_GDT_CSUM| \
					 EXT4_FEATURE_RO_COMPAT_DIR_NLINK | \
					 EXT4_FEATURE_RO_COMPAT_EXTRA_ISIZE | \
					 EXT4_FEATURE_RO_COMPAT_BTREE_DIR |\
					 EXT4_FEATURE_RO_COMPAT_HUGE_FILE |\
					 EXT4_FEATURE_RO_COMPAT_BIGALLOC |\
					 EXT4_FEATURE_RO_COMPAT_METADATA_CSUM|\
					 EXT4_FEATURE_RO_COMPAT_QUOTA)

/*
 * Default values for user and/or group using reserved blocks
 */
#define	EXT4_DEF_RESUID		0
#define	EXT4_DEF_RESGID		0

#define EXT4_DEF_INODE_READAHEAD_BLKS	32

/*
 * Default mount options
 */
#define EXT4_DEFM_DEBUG		0x0001
#define EXT4_DEFM_BSDGROUPS	0x0002
#define EXT4_DEFM_XATTR_USER	0x0004
#define EXT4_DEFM_ACL		0x0008
#define EXT4_DEFM_UID16		0x0010
#define EXT4_DEFM_JMODE		0x0060
#define EXT4_DEFM_JMODE_DATA	0x0020
#define EXT4_DEFM_JMODE_ORDERED	0x0040
#define EXT4_DEFM_JMODE_WBACK	0x0060
#define EXT4_DEFM_NOBARRIER	0x0100
#define EXT4_DEFM_BLOCK_VALIDITY 0x0200
#define EXT4_DEFM_DISCARD	0x0400
#define EXT4_DEFM_NODELALLOC	0x0800


/*
 * Minimum number of groups in a flexgroup before we separate out
 * directories into the first block group of a flexgroup
 */
#define EXT4_FLEX_SIZE_DIR_ALLOC_SCHEME	4

/*
 * Structure of a directory entry
 */
#define EXT4_NAME_LEN 255

struct ext4_dir_entry {
	__le32	inode;			/* Inode number */
	__le16	rec_len;		/* Directory entry length */
	__le16	name_len;		/* Name length */
	char	name[EXT4_NAME_LEN];	/* File name */
};

/*
 * The new version of the directory entry.  Since EXT4 structures are
 * stored in intel byte order, and the name_len field could never be
 * bigger than 255 chars, it's safe to reclaim the extra byte for the
 * file_type field.
 */
struct ext4_dir_entry_2 {
	__le32	inode;			/* Inode number */
	__le16	rec_len;		/* Directory entry length */
	uint8_t	name_len;		/* Name length */
	uint8_t	file_type;
	char	name[EXT4_NAME_LEN];	/* File name */
};

/*
 * This is a bogus directory entry at the end of each leaf block that
 * records checksums.
 */
struct ext4_dir_entry_tail {
	__le32	det_reserved_zero1;	/* Pretend to be unused */
	__le16	det_rec_len;		/* 12 */
	uint8_t	det_reserved_zero2;	/* Zero name length */
	uint8_t	det_reserved_ft;	/* 0xDE, fake file type */
	__le32	det_checksum;		/* crc32c(uuid+inum+dirblock) */
};

/*
 * Ext4 directory file types.  Only the low 3 bits are used.  The
 * other bits are reserved for now.
 */
#define EXT4_FT_UNKNOWN		0
#define EXT4_FT_REG_FILE	1
#define EXT4_FT_DIR		2
#define EXT4_FT_CHRDEV		3
#define EXT4_FT_BLKDEV		4
#define EXT4_FT_FIFO		5
#define EXT4_FT_SOCK		6
#define EXT4_FT_SYMLINK		7

#define EXT4_FT_MAX		8

#define EXT4_FT_DIR_CSUM	0xDE

/*
 * EXT4_DIR_PAD defines the directory entries boundaries
 *
 * NOTE: It must be a multiple of 4
 */
#define EXT4_DIR_PAD			4
#define EXT4_DIR_ROUND			(EXT4_DIR_PAD - 1)
#define EXT4_DIR_REC_LEN(name_len)	(((name_len) + 8 + EXT4_DIR_ROUND) & \
					 ~EXT4_DIR_ROUND)
#define EXT4_MAX_REC_LEN		((1<<16)-1)

#define EXT4_DIR_LINK_MAX(dir) (!is_dx(dir) && (dir)->i_nlink >= EXT4_LINK_MAX)
#define EXT4_DIR_LINK_EMPTY(dir) ((dir)->i_nlink == 2 || (dir)->i_nlink == 1)

/* Legal values for the dx_root hash_version field: */

#define DX_HASH_LEGACY		0
#define DX_HASH_HALF_MD4	1
#define DX_HASH_TEA		2
#define DX_HASH_LEGACY_UNSIGNED	3
#define DX_HASH_HALF_MD4_UNSIGNED	4
#define DX_HASH_TEA_UNSIGNED		5

/* 32 and 64 bit signed EOF for dx directories */
#define EXT4_HTREE_EOF_32BIT   ((1UL  << (32 - 1)) - 1)
#define EXT4_HTREE_EOF_64BIT   ((1ULL << (64 - 1)) - 1)
#define EXT4_GROUP_INFO_NEED_INIT_BIT		0
#define EXT4_GROUP_INFO_WAS_TRIMMED_BIT		1
#define EXT4_GROUP_INFO_BBITMAP_CORRUPT_BIT	2
#define EXT4_GROUP_INFO_IBITMAP_CORRUPT_BIT	3
