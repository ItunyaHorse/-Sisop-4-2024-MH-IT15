#define FUSE_USE_VERSION 28

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fuse.h>
#include <errno.h>
#include <fcntl.h>

static char *rootdir = "/home/rhmnnv/soal1/portofolio";

void reverseBuffer(char *buffer, int length) {
    int i, j;
    char temp;
    for (i = 0, j = length - 1; i < j; i++, j--) {
        temp = buffer[i];
        buffer[i] = buffer[j];
        buffer[j] = temp;
    }
}

static int veri_getattr(const char *path, struct stat *status) {
    int res;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", rootdir, path);
    res = lstat(fpath, status);
    if (res == -1) return -errno;
    return 0;
}

static int veri_readdir(const char *path, void *buff, fuse_fill_dir_t fl, off_t offset, struct fuse_file_info *fileInfo) {
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", rootdir, path);

    DIR *dr;
    struct dirent *de;

    dr = opendir(fpath);
    if (dr == NULL) return -errno;

    while ((de = readdir(dr)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (fl(buff, de->d_name, &st, 0) != 0) break;
    }
    closedir(dr);
    return 0;
}

static int veri_open(const char *path, struct fuse_file_info *fi) {
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", rootdir, path);
    int res = open(fpath, fi->flags);
    if (res == -1) return -errno;
    close(res);
    return 0;
}

static int veri_read(const char *path, char *buff, size_t size, off_t offset, struct fuse_file_info *fileInfo) {
    char fpath[1000];
    int fd, res;
    snprintf(fpath, sizeof(fpath), "%s%s", rootdir, path);
    fd = open(fpath, O_RDONLY);
    res = pread(fd, buff, size, offset);
    if (strstr(fpath, "/test") != NULL) {
        reverseBuffer(buff, res);
    }
    close(fd);
    return res;
}

static int veri_rename(const char *from, const char *to) {
    char fromPath[1000], toPath[1000];
    snprintf(fromPath, sizeof(fromPath), "%s%s", rootdir, from);
    snprintf(toPath, sizeof(toPath), "%s%s", rootdir, to);

    if (strstr(toPath, "/wm") != NULL) {
        int srcfd = open(fromPath, O_RDONLY);
        int destfd = open(toPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        char command[1000], watermark_text[] = "inikaryakita.id";
        snprintf(command, sizeof(command),
                 "convert -gravity south -geometry +0+20 /proc/%d/fd/%d -fill white -pointsize 36 -annotate +0+0 '%s' /proc/%d/fd/%d",
                 getpid(), srcfd, watermark_text, getpid(), destfd);

        system(command);
        unlink(fromPath);

        close(srcfd);
        close(destfd);
    } else {
        rename(fromPath, toPath);
    }
    return 0;
}

static int veri_chmod(const char *path, mode_t mode) {
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", rootdir, path);
    int result = chmod(fpath, mode);
    if (result == -1) return -errno;
    return 0;
}

static int veri_mkdir(const char *path, mode_t mode) {
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", rootdir, path);
    int res = mkdir(fpath, mode);
    if (res == -1) return -errno;
    return 0;
}

static int veri_rmdir(const char *path) {
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", rootdir, path);
    int res = rmdir(fpath);
    if (res == -1) return -errno;
    return 0;
}

static int veri_create(const char *path, mode_t mode, struct fuse_file_info *fi) {
    int fd;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", rootdir, path);
    fd = creat(fpath, mode);
    if (fd == -1) return -errno;
    close(fd);
    return 0;
}

static int veri_unlink(const char *path) {
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", rootdir, path);
    int res = unlink(fpath);
    if (res == -1) return -errno;
    return 0;
}

static struct fuse_operations fuse_oper = {
    .getattr = veri_getattr,
    .readdir = veri_readdir,
    .open = veri_open,
    .read = veri_read,
    .rename = veri_rename,
    .chmod = veri_chmod,
    .mkdir = veri_mkdir,
    .rmdir = veri_rmdir,
    .create = veri_create,
    .unlink = veri_unlink,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &fuse_oper, NULL);
}
