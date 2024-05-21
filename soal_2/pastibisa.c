#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <sys/types.h>
#include <dirent.h>

static const char *dirpath = "./sensitif";
static const char *logpath = "./logs-fuse.log";

// Fungsi ngelog
void log_event(const char *status, const char *tag, const char *info) {
    FILE *logfile = fopen(logpath, "a");
    if (logfile == NULL) return;
    
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    
    fprintf(logfile, "[%s]::%02d/%02d/%04d-%02d:%02d:%02d::[%s]::[%s]\n",
            status,
            local->tm_mday, local->tm_mon + 1, local->tm_year + 1900,
            local->tm_hour, local->tm_min, local->tm_sec,
            tag, info);
    
    fclose(logfile);
}

// decode base64
char* decode_base64(const char *input) {
    BIO *bio, *b64;
    int decodeLen = strlen(input) * 3 / 4;
    char *buffer = (char *)malloc(decodeLen + 1);
    bio = BIO_new_mem_buf(input, -1);
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    decodeLen = BIO_read(bio, buffer, strlen(input));
    buffer[decodeLen] = '\0';
    BIO_free_all(bio);
    return buffer;
}

// decode rot13
char* decode_rot13(const char *input) {
    char *output = strdup(input);
    for (int i = 0; output[i] != '\0'; i++) {
        if ((output[i] >= 'A' && output[i] <= 'Z')) {
            output[i] = (output[i] - 'A' + 13) % 26 + 'A';
        } else if ((output[i] >= 'a' && output[i] <= 'z')) {
            output[i] = (output[i] - 'a' + 13) % 26 + 'a';
        }
    }
    return output;
}

// decode heksadesimal
char* decode_hex(const char *input) {
    int len = strlen(input);
    char *output = (char *)malloc(len / 2 + 1);
    for (int i = 0; i < len; i += 2) {
        sscanf(input + i, "%2hhx", &output[i / 2]);
    }
    output[len / 2] = '\0';
    return output;
}

// rev teks
char* reverse_text(const char *input) {
    int len = strlen(input);
    char *output = (char *)malloc(len + 1);
    for (int i = 0; i < len; i++) {
        output[i] = input[len - 1 - i];
    }
    output[len] = '\0';
    return output;
}

static int xmp_getattr(const char *path, struct stat *stbuf) {
    int res;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", dirpath, path);
    res = lstat(fpath, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    DIR *dp;
    struct dirent *de;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", dirpath, path);

    dp = opendir(fpath);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL) {
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi) {
    int res;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", dirpath, path);

    if (strncmp(path, "/rahasia-berkas", 15) == 0) {
        char password[100];
        printf("Enter password to access %s: ", path);
        scanf("%99s", password);

        
        if (strcmp(password, "password") != 0) {
            log_event("FAILED", "open", "Incorrect password");
            return -EACCES;
        }
    }

    res = open(fpath, fi->flags);
    if (res == -1) {
        log_event("FAILED", "open", fpath);
        return -errno;
    }

    close(res);
    log_event("SUCCESS", "open", fpath);
    return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    int fd;
    int res;
    char fpath[1000];
    snprintf(fpath, sizeof(fpath), "%s%s", dirpath, path);

    fd = open(fpath, O_RDONLY);
    if (fd == -1) {
        log_event("FAILED", "read", fpath);
        return -errno;
    }

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);

    // see prefix
    if (strncmp(path, "/pesan/base64", 14) == 0) {
        char *decoded = decode_base64(buf);
        strncpy(buf, decoded, size);
        free(decoded);
    } else if (strncmp(path, "/pesan/rot13", 13) == 0) {
        char *decoded = decode_rot13(buf);
        strncpy(buf, decoded, size);
        free(decoded);
    } else if (strncmp(path, "/pesan/hex", 11) == 0) {
        char *decoded = decode_hex(buf);
        strncpy(buf, decoded, size);
        free(decoded);
    } else if (strncmp(path, "/pesan/rev", 11) == 0) {
        char *decoded = reverse_text(buf);
        strncpy(buf, decoded, size);
        free(decoded);
    }

    log_event("SUCCESS", "read", fpath);
    return res;
}

static struct fuse_operations xmp_oper = {
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .open = xmp_open,
    .read = xmp_read,
};

int main(int argc, char *argv[]) {
    return fuse_main(argc, argv, &xmp_oper, NULL);
}
