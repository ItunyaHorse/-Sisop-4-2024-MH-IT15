# Soal-Shift-Sisop-Modul-3-IT15-2024

Anggota:

1. Michael Kenneth Salim (5027231008)
2. Nicholas Emanuel Fade (5027231070)
3. Veri Rahman (5027231088)

## Soal no 1
Dikerjakan oleh **Veri Rahman (5027231088)**
Pada soal no 1 ini kita diminta mendownload file zip pada website inikaryakita. File tersebut berisi folder gallery dan bahaya yang diminta untuk membuatnya lebih menarik. Pada folder gallery kita diminta menambahkan wm inikaryakita dengan prefix folder wm-, sedangkan pada folder bahaya kita diminta mengubah permission script.sh dan mereverse kalimat pada file dengan prefix test.

```bash
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
```
Soal no 1 ini menggunakan FUSE*reverseBuffer(char buffer, int length)* Fungsi ini digunakan untuk membalikkan isi dari buffer. Ini adalah fungsi bantuan yang digunakan dalam fungsi veri_read untuk membalikkan konten file tertentu.
*veri_getattr(const char path, struct stat status)* Fungsi ini mengembalikan atribut dari file atau direktori yang ditentukan oleh path. Atribut tersebut disimpan dalam struktur stat yang diberikan oleh parameter status. Misalnya, atribut yang dikembalikan termasuk mode, ukuran, dan waktu modifikasi.
*veri_readdir(const char *path, void buff, fuse_fill_dir_t fl, off_t offset, struct fuse_file_info fileInfo)* Fungsi ini digunakan untuk membaca isi direktori yang ditentukan oleh path. Informasi tentang entri direktori disimpan dalam struktur dirent, kemudian diisi ke dalam buffer menggunakan fungsi fl.
*veri_open(const char path, struct fuse_file_info fi)* Fungsi ini membuka file yang ditentukan oleh path dengan opsi yang ditentukan oleh fi->flags. Jika operasi berhasil, file akan ditutup kembali dan fungsi akan mengembalikan nilai 0.
**veri_read(const char *path, char buff, size_t size, off_t offset, struct fuse_file_info fileInfo)* Fungsi ini membaca data dari file yang ditentukan oleh path dan menyimpannya dalam buffer buff. Pembacaan dimulai dari offset tertentu dan jumlah byte yang dibaca sesuai dengan size. Jika path mengandung "/test", data dalam buffer akan dibalik menggunakan fungsi reverseBuffer.
**veri_rename(const char from, const char to)* Fungsi ini mengubah nama file atau memindahkan file dari from ke to. Jika to mengandung "/wm", maka sebuah teks watermark akan ditambahkan ke file yang dituju.
*veri_chmod(const char path, mode_t mode)* Fungsi ini mengubah mode (izin) dari file atau direktori yang ditentukan oleh path menjadi mode yang ditentukan oleh mode. Disini berfungsi mengubah permission script.sh.
*veri_mkdir(const char path, mode_t mode)* Fungsi ini membuat direktori baru dengan nama yang ditentukan oleh path dan dengan mode yang ditentukan oleh mode.
*veri_rmdir(const char path)* Fungsi ini menghapus direktori yang ditentukan oleh path.
*veri_create(const char path, mode_t mode, struct fuse_file_info fi)* Fungsi ini membuat file baru dengan nama yang ditentukan oleh path dan dengan mode yang ditentukan oleh mode.
*veri_unlink(const char path)* Fungsi ini menghapus file yang ditentukan oleh path.
*main(int argc, char argv[])* adalah fungsi utama program. Itu memulai eksekusi program, menginisialisasi FUSE, dan menetapkan fungsi-fungsi yang telah didefinisikan sebelumnya ke operasi FUSE yang sesuai.

## KENDALA
Pada saat merubah permission script.sh tidak secara otomatis dari fungsi di program melainkan secara manual.

## Soal no 2
Dikerjakan oleh **Nicholas Emanuel Fade (5027231070)**


## Soal no 3
Dikerjakan oleh **Michael Kenneth Salim (5027231008)**
Pada soal nomer 3 ini, kita diminta untuk menerapkan sistem FUSE dan Samba untuk menggabungkan banyak split file menjadi satu kesatuan sesuai dengan namanya. Soal ini mengharuskan kita untuk membuat sebuah system file menggunakan FUSE (Filesystem in Userspace) yang menampilkan file-file dari direktori asal relics sebagai file yang telah digabungkan kembali di dalam sebuah direktori virtual bernama [nama_bebas]. Setelah itu, direktori relics akan otomatis menampilkan file-file yang terpecah menjadi bagian-bagian kecil dengan ukuran maksimum 10 KB, masing-masing diberi nomor berurutan seperti .000, .001, dan seterusnya. Ketika kita mengakses atau menyalin file dari direktori [nama_bebas] tersebut, file yang dimasukkan harus muncul sebagai satu kesatuan yang utuh, meskipun sebenarnya tersimpan dalam bentuk terpecah di direktori relics.

Selain itu, soal ini meminta untuk memastikan bahwa setiap file baru yang dibuat di direktori virtual [nama_bebas] dipecah secara otomatis menjadi bagian-bagian kecil di direktori relics. File-file yang telah tergabung ini kemudian perlu disalin ke direktori report, yang akan dibagikan menggunakan Samba File Server. Dengan demikian, kita dapat mengakses dan membuka file-file tersebut melalui jaringan, membuktikan bahwa semua file di direktori [nama_bebas] dapat diakses dengan baik setelah digabungkan kembali. Tugas ini menguji kemampuan kita dalam mengelola file system tingkat lanjut, termasuk pemahaman tentang FUSE, pemecahan dan penggabungan file, serta berbagi file melalui jaringan dengan Samba. 

Secara keseluruhan, dalam code yang saya buat ini, hanya 5 fungsi yang berperan penting untuk menjalankan soal ini. Fungsi itu adalah sebagai berikut: 
    1. FUSE_getattr
    2. FUSE_readdir
    3. FUSE_read
    4. FUSE_write
    5. FUSE_unlink
    6. FUSE_truncate

```bash
#define MAX_SPLIT  10240 //10 KB YGY
static  const  char *dirpath = "/home/mken/SISOPraktikum/SISOP4/soal_3/relics";
```
```bash
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
```

Di awal, dideklarasikan sebuah path yang mana menuju file relics dan juga besar pemecahan dari file yang dimasukkan, yang mana merupakan 10 KB. Selanjutnya, terdapat fungsi FUSE_getattr yang digunakan untuk mendapatkan atribut sebuah file atau direktori dalam file system FUSE. Fungsi ini awalnya menginisialisasi struktur stat dengan nol, kemudian memeriksa apakah path yang diminta adalah root directory ("/"). Jika iya, maka atribut direktori diatur dengan jumlah link 2 dan mode direktori dengan izin 0755. Untuk file selain root, fungsi ini membangun path lengkap dari direktori asal dirpath dan path yang diberikan, lalu menginisialisasi atribut file dengan jumlah link 1, ukuran 0, dan mode file reguler dengan izin 0644. Fungsi ini kemudian mencoba membuka pecahan-pecahan file yang berurutan dengan ekstensi numerik seperti .000, .001, dst menggunakan fopen, dan menambahkan ukuran setiap pecahan ke stbuf->st_size sampai tidak ada lagi pecahan yang ditemukan. Jika tidak ada pecahan yang ditemukan, fungsi mengembalikan kesalahan dengan -errno; jika tidak, fungsi mengembalikan 0, menandakan keberhasilan dalam mengatur atribut file.

```bash
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
```
Selanjutnya, fungsi FUSE_readdir digunakan untuk membaca isi direktori dalam sistem file FUSE. Fungsi ini membangun path direktori berdasarkan path yang diberikan, kemudian membuka direktori tersebut menggunakan opendir. Jika direktori tidak dapat dibuka, fungsi mengembalikan kesalahan dengan -errno. Jika berhasil, fungsi membaca setiap entri dalam direktori menggunakan readdir, dan memeriksa apakah nama file memiliki ekstensi .000. Jika iya, nama file asli (tanpa ekstensi) diambil dan ditambahkan ke buffer dengan filler. Setelah semua entri diproses, direktori ditutup dengan closedir, dan fungsi mengembalikan 0, menandakan keberhasilan dalam membaca isi direktori. Pada dasarnya, fungsi ini bertujuan untuk menunjukkan semua file dalam satu bagian yang utuh dan menyembunyikan yang lainnya.

```bash
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
```

Fungsi FUSE_read digunakan untuk membaca data dari file yang dipecah menjadi beberapa bagian dalam sistem file FUSE. Fungsi ini membangun path lengkap untuk file berdasarkan dirpath dan path yang diberikan, kemudian menginisialisasi variabel untuk melacak jumlah total data yang telah dibaca (total_read) dan ukuran data yang dibaca dalam iterasi saat ini (size_read). Dalam sebuah loop, fungsi mencoba membuka setiap pecahan file secara berurutan (dengan ekstensi numerik seperti .000, .001, dst) menggunakan fopen. Setelah membuka sebuah pecahan, fungsi menghitung ukuran pecahan tersebut dengan ftell dan memeriksa apakah offset lebih besar dari ukuran pecahan; jika iya, offset dikurangi dengan ukuran pecahan dan loop berlanjut ke pecahan berikutnya. Jika tidak, fungsi membaca data dari pecahan file mulai dari offset yang diberikan, menyimpan data yang dibaca ke buffer buf, dan memperbarui variabel size, buf, dan total_read sesuai dengan jumlah data yang berhasil dibaca. Loop berlanjut hingga seluruh data yang diminta telah dibaca atau tidak ada lagi pecahan file yang tersedia. Fungsi mengembalikan jumlah total data yang berhasil dibaca (total_read).

```bash
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
```
Selanjutnya, Fungsi FUSE_write digunakan untuk menulis data ke file dalam sistem file FUSE, di mana file dipecah menjadi bagian-bagian kecil dengan ukuran maksimum tertentu, yang di wakili oleh variabel MAX_SPLIT. Fungsi ini membangun path lengkap untuk file berdasarkan dirpath dan path yang diberikan, dan menginisialisasi variabel yang diperlukan untuk melacak jumlah total data yang telah ditulis (total_written) dan offset dalam bagian file saat ini. Fungsi ini menghitung nomor bagian (part_num) dan offset dalam bagian tersebut berdasarkan offset yang diberikan. Dalam sebuah loop, fungsi mencoba membuka setiap bagian file secara berurutan, jika bagian belum ada, fungsi membuatnya. Data ditulis ke bagian file mulai dari part_offset yang diberikan, dan variabel size, buf, dan total_written diperbarui sesuai dengan jumlah data yang berhasil ditulis. Loop berlanjut hingga seluruh data yang diminta telah ditulis. Fungsi mengembalikan jumlah total data yang berhasil ditulis (total_written).

```bash
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
```

Fungsi FUSE_unlink digunakan untuk menghapus file dalam sistem file FUSE. Fungsi ini membangun path lengkap untuk file berdasarkan dirpath dan path yang diberikan, kemudian membuat variabel untuk melacak nomor bagian file saat ini (pcurrent). Dalam sebuah loop, fungsi mencoba menghapus setiap bagian file secara berurutan menggunakan unlink. Loop berlanjut hingga tidak ada lagi bagian file yang ditemukan. Fungsi mengembalikan 0 untuk menandakan keberhasilan dalam menghapus file.

```bash
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
```

Fungsi FUSE_truncate digunakan untuk mengatur ukuran file dalam sistem file FUSE. Fungsi ini membangun path lengkap untuk file berdasarkan dirpath dan path yang diberikan, lalu menghitung ukuran setiap pecahan file yang baru setelah di-truncate. Fungsi ini memulai dengan menginisialisasi variabel untuk melacak nomor bagian file saat ini dan ukuran yang tersisa untuk di-truncate (size_rmn). Dalam sebuah loop, fungsi memutuskan ukuran setiap pecahan file berdasarkan nilai MAX_SPLIT, mengurangi size_rmn sesuai dengan ukuran yang di-truncate, dan menggunakan truncate untuk mengatur ukuran setiap pecahan file. Setelah mengatur ukuran semua pecahan yang diperlukan, fungsi menghapus sisa pecahan file yang tidak lagi diperlukan menggunakan unlink. Loop ini berlanjut hingga tidak ada lagi pecahan file yang ditemukan. Fungsi mengembalikan 0 untuk menandakan keberhasilan dalam mengatur ulang ukuran file.

```
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
```

Seperti yang sudah disebutkan sebelumnya, dalam pembahasan kode ini, fungsi yang dibahas hanya fungsi yang berpengaruh terhadap soal. Dalam struct di atas, dapat dilihat terdapat banyak fungsi yang dibuat namun fungsi fungsi tersebut hanya digunakan sebagai pelengkap saja. Bagian ini mendefinisikan struktur fuse_operations yang berisi pointer ke berbagai fungsi yang mengimplementasikan operasi sistem file FUSE. Struktur ini kemudian digunakan dalam fungsi main untuk menginisialisasi dan menjalankan sistem file FUSE. Lalu, Dalam fungsi main, umask(0) digunakan untuk mengatur mask izin file sehingga file yang dibuat memiliki izin default yang penuh. Kemudian, fuse_main dipanggil dengan argumen argc, argv, &FUSE_oper, dan NULL, untuk memulai sistem file FUSE dengan operasi yang telah didefinisikan dalam struktur FUSE_oper.

### Kesalahan yang dialami:
#### 1. File berulang yang terlalu banyak 

![Screenshot from 2024-05-19 00-04-26](https://github.com/ItunyaHorse/Sisop-4-2024-MH-IT15/assets/145765887/3bf3a4b5-1b8c-44ab-aca0-d6fa7bf35a91)
#### 2. File tergabung di direktori relics

![Screenshot from 2024-05-18 23-40-37](https://github.com/ItunyaHorse/Sisop-4-2024-MH-IT15/assets/145765887/d1a8c8e6-238e-4632-a83a-3148b9004dc1)
#### 3. Terlalu Penuh

![Screenshot from 2024-05-20 11-53-25](https://github.com/ItunyaHorse/Sisop-4-2024-MH-IT15/assets/145765887/7dd1faa7-8ff2-4ae3-b5db-083091c9ef75)
