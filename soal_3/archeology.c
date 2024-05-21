#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <sys/xattr.h>

#define MAX_SPLIT  10240 //10 KB YGY
static  const  char *dirpath = "/home/mken/SISOPraktikum/SISOP4/soal_3/relics";


static  int  FUSE_getattr(const char *path, struct stat *stbuf)
{
     memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_nlink = 2;
        stbuf->st_mode = S_IFDIR | 0755;  
        return 0;
    }

    char LokasiF[1024];
    sprintf(LokasiF, "%s%s",dirpath,path);

    stbuf->st_nlink = 1;
    stbuf->st_size = 0;
    stbuf->st_mode = S_IFREG | 0644;

    char LokasiP[1030];
    char LokasiQ[1040];
    FILE *fd; 
    int i = 0;

    while(1){
        sprintf(LokasiP, "%s.%03d", LokasiF, i++);
        strcpy(LokasiQ, LokasiP);
        fd = fopen(LokasiQ, "rb");
        if (fd == NULL) {
            break;
        }

        printf("Berhasil dapet ygy \n");
        fseek(fd, 0L, SEEK_END);
        stbuf->st_size += ftell(fd);
        fclose(fd);
    }

    if (i==1) {
        return -errno;
    }
    return 0;
}


static int FUSE_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi)
{
    char LokasiF[1024];
    char LokasiP[1030];

    if(strcmp(path,"/") == 0) {
        path=dirpath;  
        sprintf(LokasiF,"%s",path);
        strcpy(LokasiP, LokasiF);
    } 
    else {
        sprintf(LokasiF, "%s%s",dirpath,path);
        strcpy(LokasiP, LokasiF);
    }

    DIR *dp;
    struct dirent *de;

    dp = opendir(LokasiF);

    if (dp == NULL) {
        return -errno;
    }

    while ((de = readdir(dp)) != NULL) {

        char nama[100];
        char Fix[100];
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if(strstr(de->d_name, ".000") != NULL){
            strncpy(nama, de->d_name, strlen(de->d_name) - 4);
            nama[strlen(de->d_name) - 4] = '\0';
            strcpy(Fix, nama);
            filler(buf, Fix, &st, 0);
            printf("Berhasil baca ygy \n");
        }
    }

    closedir(dp);
    return 0;
}



static int FUSE_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {

    char LokasiF[1024];
    char LokasiQ[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);
    strcpy(LokasiQ, LokasiF);

    size_t total_read = 0;
    size_t size_read;

    for (int i = 0; size > 0; i++) {
        char LokasiP[1030];
        sprintf(LokasiP, "%s.%03d", LokasiF, i);

        FILE *fd = fopen(LokasiP, "rb");
        if (fd == NULL) {
            break;
        }

        fseek(fd, 0L, SEEK_END);
        size_t size_part = ftell(fd);
        fseek(fd, 0L, SEEK_SET);

        if (offset >= size_part) {
            offset -= size_part;
            fclose(fd);
            continue;
        }

        fseek(fd, offset, SEEK_SET);
        size_read = fread(buf, 1, size, fd);
        fclose(fd);

        buf += size_read;
        size -= size_read;
        total_read += size_read;
        offset = 0;
        printf("Berhasil besar ygy \n");
    }

    return total_read;
}

static int FUSE_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    char LokasiF[1024];
    char LokasiP[1030];
    char LokasiQ[1030];
    FILE *fd;

    sprintf(LokasiF, "%s%s", dirpath, path);
    strcpy(LokasiQ, LokasiF);

    int part_num = offset / MAX_SPLIT;
    size_t part_offset = offset % MAX_SPLIT;
    size_t total_written = 0;

    while (size > 0) {
        sprintf(LokasiP, "%s.%03d", LokasiF, part_num);

        fd = fopen(LokasiP, "r+b");
        if (fd == NULL) {
            fd = fopen(LokasiP, "wb");
            if (!fd) return -errno;
        }

        fseek(fd, part_offset, SEEK_SET);

        size_t bytes_to_write;
        if (size > (MAX_SPLIT - part_offset)) {
            bytes_to_write = MAX_SPLIT - part_offset;
            printf("Tambah... \n");
        } else {
            bytes_to_write = size;
            printf("Tambah... \n");
        }

        fwrite(buf, 1, bytes_to_write, fd);
        fclose(fd);

        buf += bytes_to_write;
        size -= bytes_to_write;
        total_written += bytes_to_write;

        part_offset = 0;

        part_num++;
        printf("Tambah besar ygy \n");
    }

    return total_written;
}

static int FUSE_unlink(const char *path) {
    char LokasiF[1024];
    char LokasiP[1028];
    sprintf(LokasiF, "%s%s", dirpath, path);

    int pcurrent = 0;    
    while (1) {
        sprintf(LokasiP, "%s.%03d", LokasiF, pcurrent++);
        int res = unlink(LokasiP);
        if (res == -1 && errno == ENOENT) {
            break;
        }
    }
     printf("Berhasil Unlink \n");
    return 0;
}

static int FUSE_create(const char *path, mode_t mode, struct fuse_file_info *fi) {

    char LokasiF[1024];
    sprintf(LokasiF, "%s%s.000",dirpath,path);
    int res = creat(LokasiF, mode);
    close(res); return 0;
    printf("Berhasil Buat \n");
}

static int FUSE_truncate(const char *path, off_t size) {
    char LokasiF[1024];
    char LokasiP[1028];
    char LokasiQ[1030];
    char LokasiA[1030];
    sprintf(LokasiF, "%s%s", dirpath, path);

    int pcurrent_t = 0;
    size_t size_part;
    off_t size_rmn = size;

    while (size_rmn > 0) {
        sprintf(LokasiP, "%s.%03d", LokasiF, pcurrent_t++);
        strcpy(LokasiQ, LokasiP);
        if (size_rmn > MAX_SPLIT) {
            size_part = MAX_SPLIT;
            printf("Selesai");
        } 
        else {
            size_part = size_rmn;
            printf("Kurang");
        }
        truncate(LokasiQ, size_part);
        size_rmn -= size_part;
        printf("Kurang");
    }

    int pcurrent_u = 0;    
    while (1) {
        sprintf(LokasiP, "%s.%03d", LokasiF, pcurrent_u++);
        strcpy(LokasiA, LokasiP);
        int res = unlink(LokasiA);
            if (res == -1 && errno == ENOENT) {
            break;
            }
    }

    return 0;
}

static int FUSE_mkdir(const char *path, mode_t mode) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);

    int res = mkdir(LokasiF, mode);
    if (res == -1){
        return -errno;
    } 
    printf("Berhasil buat Direct\n");
    return 0;
}

static int FUSE_listxattr(const char *path, char *list, size_t size) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);

    int res = listxattr(LokasiF, list, size);
    if (res == -1) {
        perror("listxattr");
        return -errno;
    }
    printf("Berhasil buat list\n");
    return res;
}

static int FUSE_rmdir(const char *path) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);

    int res = rmdir(LokasiF);
    if (res == -1) return -errno;

    return 0;
}

static int FUSE_chown(const char *path, uid_t uid, gid_t gid) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);

    int res = chown(LokasiF, uid, gid);
    if (res == -1) {
        return -errno;
    } 
    return 0;
}

static int FUSE_mknod(const char *path, mode_t mode, dev_t dev) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);
    int res = mknod(LokasiF, mode, dev);
    if (res == -1) {
        perror("mknod");
        return -errno;
    }
    printf("Successfully created special file \n");
    return 0;
}

static int FUSE_statfs(const char *path, struct statvfs *stbuf) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);
    int res = statvfs(LokasiF, stbuf);
    if (res == -1) {
        perror("statvfs");
        return -errno;
    }
    printf("Successfully retrieved file system statistics \n");
    return 0;
}

static int FUSE_chmod(const char *path, mode_t mode) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);
    int res = chmod(LokasiF, mode);
    if (res == -1) {
        perror("chmod");
        return -errno;
    }
    printf("Successfully changed file mode\n");
    return 0;
}


static int FUSE_setxattr(const char *path, const char *name, const char *value, size_t size, int flags) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);
    int res = lsetxattr(LokasiF, name, value, size, flags);
    if (res == -1) {
        perror("lsetxattr");
        return -errno;
    }
    printf("Successfully set extended attribute: %s\n", name);
    return 0;
}

static int FUSE_removexattr(const char *path, const char *name) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);
    int res = removexattr(LokasiF, name);
    if (res == -1) {
        perror("removexattr");
        return -errno;
    }
    printf("Successfully removed extended attribute: %s\n", name);
    return 0;
}

static int FUSE_symlink(const char *target, const char *linkname) {
    char ftgt[1024], flink[1024];
    sprintf(ftgt, "%s", target);
    sprintf(flink, "%s%s", dirpath, linkname);
    int res = symlink(ftgt, flink);
    if (res == -1) {
        perror("symlink");
        return -errno;
    }
    printf("Successfully created symlink\n");
    return 0;
}

static int FUSE_utimens(const char *path, const struct timespec tv[2]) {
    char LokasiF[1024];
    sprintf(LokasiF, "%s%s", dirpath, path);
    int res = utimensat(0, LokasiF, tv, AT_SYMLINK_NOFOLLOW);
    if (res == -1) {
        perror("utimens");
        return -errno;
    }
    printf("Successfully set access and modification times\n");
    return 0;
}

static int FUSE_rename(const char *from, const char *to) {
    char fromp[1024], top[1024];
    sprintf(fromp, "%s%s", dirpath, from);
    sprintf(top, "%s%s", dirpath, to);
    int res = rename(fromp, top);
    if (res == -1) {
        perror("rename");
        return -errno;
    }
    printf("Successfully renamed file/directory\n");
    return 0;
}

static int FUSE_link(const char *from, const char *to) {
    char fromp[1024], top[1024];
    sprintf(fromp, "%s%s", dirpath, from);
    sprintf(top, "%s%s", dirpath, to);
    int res = link(fromp, top);
    if (res == -1) {
        perror("link");
        return -errno;
    }
    printf("Successfully created hard link\n");
    return 0;
}


static struct fuse_operations FUSE_oper = {
    .getattr = FUSE_getattr,
    .readdir = FUSE_readdir,
    .read = FUSE_read,
    .mkdir = FUSE_mkdir,
    .chown = FUSE_chown,
    .rmdir = FUSE_rmdir,
    .write = FUSE_write,
    .unlink = FUSE_unlink,
    .create = FUSE_create,
    .truncate = FUSE_truncate,
    .setxattr = FUSE_setxattr,
    .mknod = FUSE_mknod,
    .statfs = FUSE_statfs,
    .chmod = FUSE_chmod,
    .removexattr = FUSE_removexattr,
    .listxattr = FUSE_listxattr,
    .symlink = FUSE_symlink,
    .utimens = FUSE_utimens,
    .rename = FUSE_rename,
    .link = FUSE_link,
};


int  main(int  argc, char *argv[]){
    umask(0);
    return fuse_main(argc, argv, &FUSE_oper, NULL);
}
