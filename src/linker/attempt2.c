#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <ftw.h>

#define __USE_XOPEN_EXTENDED 500

/* These are external references to the symbols created by OBJCOPY */
extern char _binary_archive_tar_start[];
extern char _binary_archive_tar_end[];

int err;

// int delete_file(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
//     int rv = remove(fpath);
//     if (rv) {
//         perror(fpath);
//     }
//     return rv;
// }

// int delete_directory(char *path) {
//     return nftw(path, delete_file, 64, FTW_DEPTH | FTW_PHYS);
// }

int main(int argc, char* argv[])
{
    char* command = calloc(200, sizeof(char));
    char *data_start  = _binary_archive_tar_start;
    char *data_end    = _binary_archive_tar_end;
    size_t data_size  = _binary_archive_tar_end - _binary_archive_tar_start;
    
    char* proto_name = "_raku_XXXXXX";
    char temp_name[strlen(proto_name)+1];
    strcpy(temp_name, proto_name);
    int tar_ball = mkstemp(temp_name);
    FILE* sfd = fdopen(tar_ball, "wb");
    fwrite(data_start, data_size, 1, sfd);
    fclose(sfd);

    // rtrim first char (&temp_name[1]) for directory name
    sprintf(command, "mkdir %s", &temp_name[1]);
    system(command);
    
    sprintf(command, "tar -xf ./%s -C %s", &temp_name, &temp_name[1]);
    system(command);

    sprintf(command, "sed -i -n -u '/^MOARVM*/,$p' %s/*", &temp_name[1]);
    system(command);

    sprintf(command, "x=$(ls %s | grep -v '\\.bc'); if [ \"$x\" ]; then raku --bm=./%s/$x -b ./%s/%s.bc; else raku -b ./%s/%s.bc; fi", &temp_name[1], &temp_name[1], &temp_name[1], argv[0], &temp_name[1], argv[0]);
    err = system(command);
    
    // remove(tar_ball);

    // sprintf(command, "rm -r %s", &temp_name[0]);
    // system(command);

    free(command);
    return 0;
}