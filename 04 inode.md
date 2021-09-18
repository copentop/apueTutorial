### inode



**超级块（super_block）**

超级块代表了整个文件系统，超级块是文件系统的控制块，有整个文件系统信息，一个文件系统所有的inode都要连接到超级块上，可以说，一个超级块就代表了一个文件系统。



**索引节点（index node）**

索引节点，也就是 `inode`，用来记录文件的元信息，比如 `inode` 编号、文件大小、访问权限、创建时间、修改时间、数据在磁盘的位置等等。

索引节点是文件的唯一标识，它们之间一一对应，也同样都会被存储在硬盘中，所以索引节点同样占用磁盘空间。



**目录项（directory entry）**

目录项，也就是 `dentry`，用来记录文件的名字、索引节点指针以及与其他目录项的层级关联关系。多个目录项关联起来，就会形成目录结构。

目录项是由内核维护的一个数据结构，不存放于磁盘，而是缓存在内存。



### 目录文件和普通文件

目录也是文件，也是用索引节点唯一标识，和普通文件不同的是，普通文件在磁盘里面保存的是文件数据，而目录文件在磁盘里面保存子目录或文件。



### 数据结构



**`struct dentry`**

```c
struct dentry {
	/* RCU lookup touched fields */
	unsigned int d_flags;        	// 状态位 protected by d_lock  
	seqcount_t d_seq;        		// per dentry seqlock 
	struct hlist_bl_node d_hash;    // 哈希链表节点，方便寻找 lookup hash list 
	struct dentry *d_parent;    	// 父目录指针 parent directory 
	struct qstr d_name;				// 文件或者是目录的名 
	struct inode *d_inode;        	// 目录的inode Where the name belongs to - NULL is  negative 
	unsigned char d_iname[DNAME_INLINE_LEN];    // 短的文件名 small names 

	/* Ref lookup also touches following */
	unsigned int d_count;        			// dentry的引用计数，为零时会释放 protected by d_lock  
	spinlock_t d_lock;        				// 自旋锁 per dentry lock  
	const struct dentry_operations *d_op;
	struct super_block *d_sb;    			// 目录的超级块指针 The root of the dentry tree
	unsigned long d_time;        			// 最近使用时间 used by d_revalidate 
	void *d_fsdata;            				// 私有数据 fs-specific data 

	struct list_head d_lru;        			/* LRU list */
	/*
	 * d_child and d_rcu can share memory
	 */
	union {
		struct list_head d_child;    		/* child of parent list */
		struct rcu_head d_rcu;
	} d_u;
	struct list_head d_subdirs;    			// 目录中子项的指针 our children 
	struct list_head d_alias;    			// 相关索引节点的指针 inode alias list 
};
```

一个有效的`dentry`必然与一个`inode`关联，但是一个`inode`可以对应多个`dentry`，因为一个文件可以被链接到其他文件，所以，这个`dentry`就是通过这个字段链接到属于自己的`inode`结构中的`i_dentry`链表中的。



**`struct path`**

```c
struct path {
	struct vfsmount *mnt;
	struct dentry *dentry;
};
```

**`struct vfsmount`**

```c
struct vfsmount {
	struct dentry 		*mnt_root;	/*挂载点根目录的dentry */
	struct super_block 	*mnt_sb;	/* 挂载点批向superblock的指针*/
	int 				mnt_flags;	//挂载标识
};
```



**`struct file`**

```c
struct file {
    /*
     * fu_list becomes invalid after file_free is called and queued via
     * fu_rcuhead for RCU freeing
     */
    union {
        struct list_head	fu_list;		// 所有的打开的文件形成的链表！	
        struct rcu_head		fu_rcuhead;
    } f_u;
    struct path				f_path;
#define f_dentry			f_path.dentry 	// 文件路径归属的目录项dentry
#define f_vfsmnt        	f_path.mnt

    const struct file_operations    *f_op;
    atomic_t				f_count;		// 引用计数
    unsigned int			f_flags;		// 打开文件时候指定的标识
    mode_t					f_mode;			// 文件的访问模式
    loff_t                  f_pos;			// 相对开头的偏移
    struct fown_struct      f_owner;		// 记录一个进程ID，以及当某些事发送的时候发送给该ID进程的信号
    unsigned int            f_uid, f_gid;	// 用户ID, 组ID
    struct file_ra_state    f_ra;

    u64                     f_version;		// 版本号，当f_pos改变时候，version递增
#ifdef CONFIG_SECURITY
    void                    *f_security;
#endif
    /* needed for tty driver, and maybe others */
    void                    *private_data;	// 私有数据( 文件系统和驱动程序使用 )

#ifdef CONFIG_EPOLL
    /* Used by fs/eventpoll.c to link all the hooks to this file */
    struct list_head        f_ep_links;
    spinlock_t              f_ep_lock;

#endif /* #ifdef CONFIG_EPOLL */

    struct address_space    *f_mapping;
};
```

**`struct inode`**

```c
struct inode {         
	struct list_head        i_hash;					// 加快inode的查找效率
    struct list_head        i_list;
    struct list_head        i_dentry;				// 一个inode可以对应多个dentry
    struct list_head        i_dirty_buffers;		// 脏数据缓冲区
    struct list_head        i_dirty_data_buffers;

    unsigned long           i_ino;					// 索引节点号
    atomic_t                i_count;				// 引用计数
    kdev_t                  i_dev;					// inode代表设备，那么就是设备号
    umode_t                 i_mode;					// 文件的类型和访问权限
    unsigned int            i_nlink;				// 与该节点建立链接的文件数(硬链接数)
    uid_t                   i_uid;
    gid_t                   i_gid;
    kdev_t                  i_rdev;					//实际的设备标识
    loff_t                  i_size;					// 代表的的文件的大小，以字节为单位
    time_t                  i_atime;
    time_t                  i_mtime;
    time_t                  i_ctime;
    unsigned int            i_blkbits;				// 块大小，字节单位
    unsigned long           i_blksize;				// 块大小，bit单位
    unsigned long           i_blocks;				// 文件所占块数
    unsigned long           i_version;				// 版本号
    unsigned short          i_bytes;				// 文件中最后一个块的字节数
    struct semaphore        i_sem;					// 同步操作的信号量结构
    struct rw_semaphore     i_alloc_sem;			// 保护inode上的IO操作不被另一个打断
    struct semaphore        i_zombie;				// 僵尸inode信号量
    struct inode_operations *i_op;
    struct file_operations  *i_fop; 				// 文件操作 former ->i_op->default_file_ops         
    struct super_block      *i_sb;					// inode所属文件系统的超级块指针
    wait_queue_head_t       i_wait;					// 指向索引节点等待队列指针
    struct file_lock        *i_flock;				// 文件锁链表
    struct address_space    *i_mapping;				// 表示向谁请求页面
    struct address_space    i_data;					// 表示被inode读写的页面，真实的数据块
    struct dquot            *i_dquot[MAXQUOTAS];	// inode的磁盘限额
    /* These three should probably be a union */         
    struct list_head        i_devices;				// 设备链表。共用同一个驱动程序的设备形成的链表。
    struct pipe_inode_info  *i_pipe;				// 指向管道文件（如果文件是管道文件时使用）
    struct block_device     *i_bdev;				// 指向块设备文件指针（如果文件是块设备文件时使用）
    struct char_device      *i_cdev;				// 指向字符设备文件指针（如果文件是字符设备时使用）
    unsigned long           i_dnotify_mask; 		// 目录通知事件掩码  Directory notify events 
	struct dnotify_struct   *i_dnotify; 			// 用于目录通知  for directory notifications 
	unsigned long           i_state;				// 索引节点的状态标识：I_NEW，I_LOCK，I_FREEING
    unsigned int            i_flags;				// 索引节点的安装标识
    unsigned char           i_sock;					// 如果是套接字文件则为True
    atomic_t                i_writecount;			// 记录多少进程以刻写模式打开此文件
    unsigned int            i_attr_flags;			// 文件创建标识
    __u32                   i_generation;
    union {                 						// 具体的inode信息
    	struct minix_inode_info         minix_i;
        struct ext2_inode_info          ext2_i;
        struct ext3_inode_info          ext3_i;
        struct hpfs_inode_info          hpfs_i;
        struct ntfs_inode_info          ntfs_i;
        struct msdos_inode_info         msdos_i;
        struct umsdos_inode_info        umsdos_i;
        struct iso_inode_info           isofs_i;
        struct nfs_inode_info           nfs_i;
        struct sysv_inode_info          sysv_i;
        struct affs_inode_info          affs_i;
        struct ufs_inode_info           ufs_i;
        struct efs_inode_info           efs_i;
        struct romfs_inode_info         romfs_i;
        struct shmem_inode_info         shmem_i;
        struct coda_inode_info          coda_i;
        struct smb_inode_info           smbfs_i;
        struct hfs_inode_info           hfs_i;
        struct adfs_inode_info          adfs_i;
        struct qnx4_inode_info          qnx4_i;
        struct reiserfs_inode_info      reiserfs_i;
        struct bfs_inode_info           bfs_i;
        struct udf_inode_info           udf_i;
        struct ncp_inode_info           ncpfs_i;
        struct proc_inode_info          proc_i;
        struct socket                   socket_i;
        struct usbdev_inode_info        usbdev_i;
        struct jffs2_inode_info         jffs2_i;
        void                            *generic_ip;
    } u;
 };

```

**`struct super_block`**

```c
struct super_block {
    struct list_head    		s_list;			// 双向循环链表 Keep this first 
    dev_t            			s_dev;			// 设备标识符 search index; _not_ kdev_t 
    unsigned char        		s_dirt;			// 脏位，标识是否超级块被修改
    unsigned char        		s_blocksize_bits;	// 文件系统中数据块大小，以字节单位
    unsigned long        		s_blocksize;		// 上面的size大小占用位数
    loff_t            			s_maxbytes;			// 允许的最大的文件大小(字节数) Max file size 
    struct file_system_type		*s_type;			// 文件系统类型，ext2还是fat32？
    const struct super_operations		*s_op;		
    const struct dquot_operations		*dq_op;
    const struct quotactl_ops			*s_qcop;	// 配置磁盘限额的的方法
    const struct export_operations		*s_export_op;
    unsigned long				s_flags;		// 安装标识
    unsigned long        		s_magic;		// 区别于其他文件系统的标识
    struct dentry        		*s_root;		// 该具体文件系统安装目录的目录项
    struct rw_semaphore    		s_umount;		// 对超级块读写时进行同步
    struct mutex				s_lock;			// 锁标志位，若置该位，则其它进程不能对该超级块操作
    int							s_count;		// 对超级块的使用计数
    atomic_t					s_active;		// 引用计数

 #ifdef CONFIG_SECURITY
     void						*s_security;
 #endif
    const struct xattr_handler	**s_xattr;

    struct list_head			s_inodes;    	/* all inodes */
    struct hlist_bl_head		s_anon;        	/* anonymous dentries for (nfs) exporting */
 #ifdef CONFIG_SMP
     struct list_head __percpu	*s_files;
 #else
     struct list_head			s_files;
 #endif
    struct list_head			s_mounts;			/* list of mounts; _not_ for fs use */

    /* s_dentry_lru, s_nr_dentry_unused protected by dcache.c lru locks */
    struct list_head			s_dentry_lru;		/* unused dentry lru */
    int							s_nr_dentry_unused;	/* # of dentry on lru */
 
    /* s_inode_lru_lock protects s_inode_lru and s_nr_inodes_unused */
    spinlock_t  s_inode_lru_lock ____cacheline_aligned_in_smp;
    struct list_head    		s_inode_lru;		/* unused inode lru */
    int							s_nr_inodes_unused;	/* # of inodes on lru */
 
    struct block_device			*s_bdev;		// 文件系统被安装的块设备
    struct backing_dev_info		*s_bdi;
    struct mtd_info				*s_mtd;
    struct hlist_node			s_instances;	// 同一类型的文件系统通过这个实例将所有的super_block连接起来
    struct quota_info			s_dquot;		// 磁盘限额相关选项 Diskquota specific options
 
    int							s_frozen;
    wait_queue_head_t			s_wait_unfrozen;
 
    char 						s_id[32];		/* Informational name */
    u8 							s_uuid[16];		/* UUID */
 
    void						*s_fs_info;		/* Filesystem private info */
    unsigned int				s_max_links;
    fmode_t						s_mode;
 
    /* Granularity of c/m/atime in ns.
        Cannot be worse than a second */
    u32           				s_time_gran;
 
    /*
     * The next field is for VFS *only*. No filesystems have any business
     * even looking at it. You had been warned.
     */
    struct mutex				s_vfs_rename_mutex;		/* Kludge */
 
    /*
     * Filesystem subtype.  If non-empty the filesystem type field
     * in /proc/mounts will be "type.subtype"
     */
    char						*s_subtype;
 
    /*
     * Saved mount options for lazy filesystems using
     * generic_show_options()
     */
    char __rcu					*s_options;
    const struct dentry_operations *s_d_op;		/* default d_op for dentries */
 
    /*
     * Saved pool identifier for cleancache (-1 means none)
     */
    int 						cleancache_poolid;
 
    struct shrinker				s_shrink;		/* per-sb shrinker handle */
 
    /* Number of inodes with nlink == 0 but still referenced */
    atomic_long_t 				s_remove_count;
 
    /* Being remounted read-only */
    int 						s_readonly_remount;
};
```



