### inode

### 存储设备的分区

分区是指将存储设备划分为几个 逻辑区域 的过程。分区后，就能像管理一个个彼此独立的存储设备一样对这些逻辑区域进行一一单独管理。

```
-----------------------------------------------------------
| RESERVED | PARTITION 1 | PARTITION 2 | ... | PARTITION N|
-----------------------------------------------------------
```


一台存储设备应至少有一个分区，视需求而定，也可进行更多分区。 例如，一个基础的Linux安装就具有三个分区：一个专门用于存储操作系统，一个专门用于存放用户文件，外加一个交换分区；Windows，会对磁盘分区分配以不同的字母编号（A，B，C，D），Windows上 主分区 （Windows所安装的分区）的命名为 C：或驱动器C。


在对存储设备进行分区时，有两种分区方法可供选择：
+ 主引导记录（MBR）方案
+ GUID分区表（GPT）方案 （Globally Unique Identifier Partition Table）


存储设备上的前几个存储块所存储的始终都是有关分区的关键数据。利用这些数据结构，系统的固件便能启动操作系统。固件是嵌入电子设备中以操作该设备或引导另一个程序来操作该设备的低级软件。

计算机固件依据以下两个规范执行：
+ 基本输入/输出（BIOS）
+ 统一可扩展固件接口（UEFI）

当打开计算机电源时，固件是第一个运行的程序。固件的任务（除其它事项外）包括启动计算机，运行操作系统，并将整个系统的控制权传递给操作系统。



#### MBR


MBR分区方案是BIOS规范的一部分，由基于BIOS的固件使用。

在采用MBR分区方案的磁盘上，存储设备上的第一个扇区存储着启动系统所需的基本数据。这一扇区被称为MBR（主引导扇区）。

MBR内存有以下信息：
+ 引导程序，它是（机器代码中的）一个 简单程序，用于启动引导过程的第一阶段
+ 分区表，其中包含有关分区的信息。

```
Offset			Data 				Contents
0x0000~0x0088	Master Boot Record	主引导程序
0x0089~0x01BD	出错信息数据区		数据区
0x01BE~0x01CD	分区1（16字节）
0x01CE~0x01DD	分区2
0x01DE~0x01ED	分区3				分区表
0x01EE~0x01FD	分区4
0x01FE			0x55
0x01FF			0xAA				结束标志
```

以下是其工作过程：

1. 系统启动后，BIOS固件将启动并将MBR的内容加载到内存中，并在其中运行引导加载程序。
2. 通过将引导加载程序和分区表放置在MBR之类的预定义位置中，便可使BIOS来引导系统，而无需处理任何文件。MBR中的引导加载程序代码占用MBR 512字节空间中的434字节至446字节，另有64字节分配给了分区表，分区数量最多为四个。446字节并不足以容纳很多的代码。正因如此，复杂的引导加载程序（例如Linux上的GRUB 2）会将其功能切分为多个部分或多个阶段。其中，最小的部分被称为第一阶段引导加载程序，位于MBR内。

3. 第一阶段引导加载程序将启动引导过程的下一个阶段。MBR之后紧接第一个分区之前，还有一个很小的空间，大约1MB，被称为 MBR间隙。必要时，它也可以用来放置一部分引导加载程序。利用MBR间隙，引导加载程序（例如GRUB 2）存储其功能的另一阶段。GRUB将此称为引导加载程序的 第1.5个阶段 ，其中包含文件系统驱动程序。1.5阶段使GRUB的下一阶段可以处理文件，而不再是再向第一阶段引导加载程序那样从存储设备中加载原始数据。

4. 第二阶段引导加载程序，现在为file-system-aware，可以加载操作系统的引导加载程序文件来引导操作系统。


此时也是操作系统徽标逐渐淡出的时候...



MBR分区总数不超过四个,每个分区的空间最大为2TiB。一旦MBR遭到意外损坏，存储设备就变成废铁了。

#### GPT

GPT是UEFI规范的一部分，该规范正在替代旧的BIOS。

在GPT分区方案下，出于与基于BIOS的系统兼容的考虑，保留了存储设备的第一个扇区。这是因为某些系统可能仍使用基于BIOS的固件，但却具有GPT分区的存储设备。这一扇区被称为保护性MBR。 （这也是第一阶段引导加载程序在MBR分区的磁盘中存储的位置）

第一个扇区之后，将存储GPT数据结构，包括 GPT标头 和 分区表条目。作为备份，GPT条目和GPT标头也会存储在存储设备的最后，这样即便主副本被损坏，也可以将其恢复。此备份被称为 辅助GPT。

在GPT中，所有的引导服务（引导加载程序，引导管理器，预操作系统环境和壳层）都位于名为EFI系统分区（简称ESP）的特殊分区中，UEFI固件可以使用该分区。ESP甚至拥有自己专属的文件系统，该系统是 FAT的一个特定版本。在Linux上，ESP存储在 `/sys/firmware/efi` 路径下。

如果你在自己的系统上找不到此路径，那你的固件可能是基于BIOS的固件。

### 格式化

大多数操作系统都支持基于一组文件系统来格式化分区。

例如，在Windows上格式化分区时，可以选择 FAT32， NTFS 或 exFAT。格式化还会涉及创建各种 数据结构 和用于管理分区内文件的元数据。在将一个分区格式化为NTFS时，格式化过程会将关键NTFS数据结构以及主引导表（MFT）放置在该分区上。

每种操作系统会使用特定的文件系统来管理文件。

Microsoft曾在 MS-DOS 和 Windows 9x 家族系统中使用过 FAT （FAT12，FAT16和FAT32）。自Windows NT 3.1 开始，Microsoft研发了 新技术文件系统（NTFS）。2006年，Microsoft创建 扩展文件分配表（exFAT） 文件系统，exFAT堪称NTFS的精简版。

文件系统中的 扩展文件系统（ext） 家族是专门为Linux内核（即Linux操作系统的核心）创建的。有`ext2`、`ext3`和`ext4`。

### 文件系统的体系结构

一个操作系统中的文件系统有三层结构：
+ 物理文件系统
+ 虚拟文件系统
+ 逻辑文件系统。


物理层是文件系统的具体实现，负责数据存储和检索，以及存储设备上的空间管理（或者更确切地说是分区）。物理文件系统通过 设备驱动程序与实际的存储硬件进行交互。


虚拟文件系统提供了一种支持在操作系统上安装的各类文件系统的 一致视图。也就是说，操作系统需要能够在处理不同程序（文件浏览器和其他处理文件的应用）和不同的挂载文件系统（例如NTFS，APFS，EXT4， FAT32，exFAT和UDF）时提供一种 一致视图。


比如说，当你打开文件资源管理器时，你可以从EXT4文件系统复制一份图像，然后将其直接粘贴到exFAT格式的闪存中，而不必去管文件在后台进行了不同的管理。

VFS它制定了一种 合同 ，要求所有的物理文件系统都必须以操作系统支持的方式工作。

逻辑文件系统是文件系统中面向用户的一层。通过提供API，它能使用户程序无需处理任何存储硬件便能执行各种文件操作，例如 打开， 读， 写。

### 什么叫挂载文件系统

在Unix一类的系统上，VFS为每个分区或可移动存储设备都分配一个 `device ID` （如 `dev/disk1s1`）。

接着，它会创建一个 虚拟目录树 ，并将每个设备的内容按照单独的目录放在该目录树下。而在根目录树下给存储设备分配目录的活动就是 挂载，分配的目录被称为 挂载点。

文件元数据
文件元数据是一种数据结构，存储 有关文件的数据，比如：

+ 文件大小
+ 时间戳，如创建日期，上次访问日期和修改日期
+ 文件所有者
+ 文件权限状态（“谁”可以“如何”处理文件）
+ 分区上的哪些块分配给了文件
+ 等等

元数据不会与文件内容一起存储，而是存储在磁盘上的其它位置并与文件关联。

在Unix一类的系统中，元数据以一种特殊的数据结构形式存储，被称为 索引节点。

在一个ext4索引节点中，所分配区块的地址以索引节点中的 `区段` （一组数据结构）形式存储。每个区段包含分配给文件的第一个数据块的地址，以及文件已占用的下面几个区块的数量。

如果文件是分段存储的，每个分段都会有其专属的范围。

这与ext3的指针系统不同，后者是通过间接块指针指向各个数据块的。使用区段数据结构可使文件系统指向大型文件，但同时又不会占用太多空间。每当请求文件时，其名称都会首先解析为一个索引节点号。有了索引节点号之后，文件系统便会从存储设备中获取相应的索引节点。提取索引节点过后，文件系统便开始根据索引节点中存储的数据块来组成文件。

### 空间管理

存储设备内分为大小固定的区块，称为 `扇区`。扇区是存储设备上的 最小存储单元，大小介于512字节和4096字节之间（高级格式）。

文件系统实际使用高级概念 `区块` 作为存储单元。块是对物理扇区的抽象，由多个扇区组成。根据文件的大小，文件系统为每个文件分配一个或多个块。

ext4格式的分区中最基本的存储单元就是块。为便于管理，连续的块会被集中在一起，组成 块组。

每个块组都有自己的数据结构和数据块。
+ 超级块： 元数据存储库，其中包含有关整个文件系统的元数据，例如文件系统中的总块数，块组中的块总数，索引节点等。但并非所有的块组都包含超级块。一定数量的块组会存储超级副本作为备份。
+ 组描述符： 组描述符同样包含每个块组的簿记信息
+ 索引节点位图： 每个块组都有自己的索引节点配额用于存储文件。块位图是一种数据结构，用于标识块组中 已使用 和 未使用 的索引节点。1 表示已使用的索引节点对象，0 表示未使用的索引节点对象。
+ 块位图: 一种数据结构，用于标识块组中已使用和未使用的数据块。1 表示已使用的数据块，0 表示未使用的数据块。
+ 索引节点表： 一种数据结构，用于定义文件及其索引节点的关系。存储在该区域中的索引节点的数量与文件系统使用的块大小有关。
+ 数据块： 存储文件内容的块组中的区域。

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



