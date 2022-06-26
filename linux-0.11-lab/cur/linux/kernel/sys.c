/*
 *	linux/kernel/sys.c
 *
 *	(C) 1991	Linus Torvalds
 */
#include <errno.h>
#include <linux/sched.h>
#include <linux/tty.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <sys/times.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
struct linux_dirent {
	long           d_ino;
	off_t          d_off;
	unsigned short d_reclen;
	char           d_name[14];
};
int sys_ftime()
{
	return -ENOSYS;
}

int sys_break()
{
	return -ENOSYS;
}

int sys_ptrace()
{
	return -ENOSYS;
}

int sys_stty()
{
	return -ENOSYS;
}

int sys_gtty()
{
	return -ENOSYS;
}

int sys_rename()
{
	return -ENOSYS;
}

int sys_prof()
{
	return -ENOSYS;
}

int sys_setregid(int rgid, int egid)
{
	if (rgid>0) {
		if ((current->gid == rgid) || 
				suser())
			current->gid = rgid;
		else
			return(-EPERM);
	}
	if (egid>0) {
		if ((current->gid == egid) ||
				(current->egid == egid) ||
				suser()) {
			current->egid = egid;
			current->sgid = egid;
		} else
			return(-EPERM);
	}
	return 0;
}

int sys_setgid(int gid)
{
/*	return(sys_setregid(gid, gid)); */
	if (suser())
		current->gid = current->egid = current->sgid = gid;
	else if ((gid == current->gid) || (gid == current->sgid))
		current->egid = gid;
	else
		return -EPERM;
	return 0;
}

int sys_acct()
{
	return -ENOSYS;
}

int sys_phys()
{
	return -ENOSYS;
}

int sys_lock()
{
	return -ENOSYS;
}

int sys_mpx()
{
	return -ENOSYS;
}

int sys_ulimit()
{
	return -ENOSYS;
}

int sys_time(long * tloc)
{
	int i;

	i = CURRENT_TIME;
	if (tloc) {
		verify_area(tloc,4);
		put_fs_long(i,(unsigned long *)tloc);
	}
	return i;
}

/*
 * Unprivileged users may change the real user id to the effective uid
 * or vice versa.
 */
int sys_setreuid(int ruid, int euid)
{
	int old_ruid = current->uid;
	
	if (ruid>0) {
		if ((current->euid==ruid) ||
										(old_ruid == ruid) ||
				suser())
			current->uid = ruid;
		else
			return(-EPERM);
	}
	if (euid>0) {
		if ((old_ruid == euid) ||
										(current->euid == euid) ||
				suser()) {
			current->euid = euid;
			current->suid = euid;
		} else {
			current->uid = old_ruid;
			return(-EPERM);
		}
	}
	return 0;
}

int sys_setuid(int uid)
{
/*	return(sys_setreuid(uid, uid)); */
	if (suser())
		current->uid = current->euid = current->suid = uid;
	else if ((uid == current->uid) || (uid == current->suid))
		current->euid = uid;
	else
		return -EPERM;
	return(0);
}

int sys_stime(long * tptr)
{
	if (!suser())
		return -EPERM;
	startup_time = get_fs_long((unsigned long *)tptr) - jiffies/HZ;
	return 0;
}

int sys_times(struct tms * tbuf)
{
	if (tbuf) {
		verify_area(tbuf,sizeof *tbuf);
		put_fs_long(current->utime,(unsigned long *)&tbuf->tms_utime);
		put_fs_long(current->stime,(unsigned long *)&tbuf->tms_stime);
		put_fs_long(current->cutime,(unsigned long *)&tbuf->tms_cutime);
		put_fs_long(current->cstime,(unsigned long *)&tbuf->tms_cstime);
	}
	return jiffies;
}

int sys_brk(unsigned long end_data_seg)
{
	if (end_data_seg >= current->end_code &&
			end_data_seg < current->start_stack - 16384)
		current->brk = end_data_seg;
	return current->brk;
}

/*
 * This needs some heave checking ...
 * I just haven't get the stomach for it. I also don't fully
 * understand sessions/pgrp etc. Let somebody who does explain it.
 */
int sys_setpgid(int pid, int pgid)
{
	int i;

	if (!pid)
		pid = current->pid;
	if (!pgid)
		pgid = current->pid;
	for (i=0 ; i<NR_TASKS ; i++)
		if (task[i] && task[i]->pid==pid) {
			if (task[i]->leader)
				return -EPERM;
			if (task[i]->session != current->session)
				return -EPERM;
			task[i]->pgrp = pgid;
			return 0;
		}
	return -ESRCH;
}

int sys_getpgrp(void)
{
	return current->pgrp;
}

int sys_setsid(void)
{
	if (current->leader && !suser())
		return -EPERM;
	current->leader = 1;
	current->session = current->pgrp = current->pid;
	current->tty = -1;
	return current->pgrp;
}

int sys_getgroups()
{
	return -ENOSYS;
}

int sys_setgroups()
{
	return -ENOSYS;
}

int sys_uname(struct utsname * name)
{
	static struct utsname thisname = {
		"linux .0","nodename","release ","version ","machine "
	};
	int i;

	if (!name) return -ERROR;
	verify_area(name,sizeof *name);
	for(i=0;i<sizeof *name;i++)
		put_fs_byte(((char *) &thisname)[i],i+(char *) name);
	return 0;
}

int sys_sethostname()
{
	return -ENOSYS;
}

int sys_getrlimit()
{
	return -ENOSYS;
}

int sys_setrlimit()
{
	return -ENOSYS;
}

int sys_getrusage()
{
	return -ENOSYS;
}

int sys_gettimeofday()
{
	return -ENOSYS;
}

int sys_settimeofday()
{
	return -ENOSYS;
}


int sys_umask(int mask)
{
	int old = current->umask;

	current->umask = mask & 0777;
	return (old);
}

int sys_getdents(unsigned int fd, struct linux_dirent *dirp, unsigned int count)
{
	char* buf;
	struct linux_dirent tdir;
	struct m_inode *d_inode;
	struct buffer_head *data;
	struct dir_entry *dir;
	int num = 0;
	int k;
	int l1 = sizeof(struct dir_entry);
	int l2 = sizeof(struct linux_dirent);
	d_inode = current->filp[fd]->f_inode;
	data = bread(d_inode->i_dev, d_inode->i_zone[0]);
	for (k = 0; k < d_inode->i_size; k += l1)
	{
		if (num + l2 >= count)
			return 0;
		dir = (struct dir_entry *)(data->b_data + k);
		if (dir->inode)
		{	tdir.d_ino = dir->inode;
			int i;
			for (i = 0; i < NAME_LEN; i++)
				tdir.d_name[i] = dir->name[i];
			tdir.d_off = 0;
			tdir.d_reclen = sizeof(tdir);
			buf = &tdir;
			for (i = 0; i < tdir.d_reclen; i++)
			{			put_fs_byte(*(buf + i), ((char *)dirp) + i + num);
			}
			num += tdir.d_reclen;
		}
		else continue;
	}
	brelse(data);
	return num;
}
int sys_sleep(unsigned int seconds)
{
	sys_signal(SIGALRM, SIG_IGN);
	sys_alarm(seconds);
	sys_pause();
	return 0;
}

char*sys_getcwd(char* buf, size_t size)
{
	unsigned int i_start;
	struct dir_entry* dir, * dir2;
	struct m_inode* m1, * m2;
	struct buffer_head* block;
	char* tmp, * t_buf;
	t_buf = (char*)malloc(256);
	strcpy(t_buf, "");
	int len = sizeof(struct dir_entry);
	m1 = current->pwd;
	if (m1 == current->root) strcpy(t_buf, "/");
	while (m1 != current->root) {
		block = bread(m1->i_dev, m1->i_zone[0]);
		dir = (struct dir_entry*)(block->b_data + len);
		m2 = iget(m1->i_dev, dir->inode);
		brelse(block);
		block = bread(m2->i_dev, m2->i_zone[0]);
		int k = 0;
		dir2 = (struct dir_entry*)(block->b_data + k);
		while (dir2->inode) {
			if (dir2->inode == m1->i_num) {
			strcpy(tmp, "/");
			strcat(tmp, dir2->name);
			strcat(tmp, t_buf);
			strcpy(t_buf, tmp);
			break;}
			k += len;
			dir2 = (struct dir_entry*)(block->b_data + k);
		}
		m1 = m2;
		brelse(block);
	}
	if (strlen(t_buf) > size) return NULL;
	int i;
	for (i = 0; t_buf[i]; i++)
	{	put_fs_byte(t_buf[i], buf + i);
	}
	return buf;

}


