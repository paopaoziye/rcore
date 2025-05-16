#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>  

#define TARGET_PATH "./user/bin/"
/* 比较字符串 */
int compare_strings(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

void insert_app_data() {
    //以写入模式打开link_app.S
    FILE* f = fopen("src/link_app.S", "w");
    if (f == NULL) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    //
    char* apps[100]; 
    int app_count = 0;

    //打开app所在文件夹
    DIR* dir = opendir("./user/bin");
    if (dir == NULL) {
        perror("Failed to open directory");
        exit(EXIT_FAILURE);
    }
    //遍历app所在文件夹所有目录项，读取所有app名称存入apps中
    struct dirent* dir_entry;
    while ((dir_entry = readdir(dir)) != NULL) {
        //存储目录项的名字
        char* name_with_ext = dir_entry->d_name;
        //排除掉 . 和 .. 条目
        if (name_with_ext[0] == '.' && (name_with_ext[1] == '\0' || (name_with_ext[1] == '.' && name_with_ext[2] == '\0'))) {
            continue;
        }
        //将目录项名字中的.替换为\0，从而移除其拓展名
        int len = strlen(name_with_ext);
        for (int i = 0; i < len; i++) {
            if (name_with_ext[i] == '.') {
                name_with_ext[i] = '\0';
                break;
            }
        }
        //将name_with_ext复制一份并存入apps数组
        apps[app_count] = strdup(name_with_ext);
        app_count++;
        printf("File name: %s, app_count: %d\n", name_with_ext, app_count);
    }
    //关闭文件夹
    closedir(dir);
    //对app名称进行排序
    qsort(apps, app_count, sizeof(char*), compare_strings);
    //向link_app.S写入一段riscv汇编代码，定义应用程序数量
    fprintf(f, "\n.align 3\n.section .data\n.global _num_app\n_num_app:\n.quad %d", app_count);
    //向link_app.S写入一段riscv汇编代码，定义各个应用程序的起始地址
    for (int i = 0; i < app_count; i++) {
        fprintf(f, "\n.quad app_%d_start", i);
    }
    //向link_app.S写入一段riscv汇编代码，定义最后一个应用程序的终止地址
    fprintf(f, "\n.quad app_%d_end", app_count - 1);
    //向link_app.S写入一段riscv汇编代码，将app二进制文件嵌入内核
    for (int i = 0; i < app_count; i++) {
        printf("app_%d: %s\n", i, apps[i]);
        fprintf(f, "\n.section .data\n.global app_%d_start\n.global app_%d_end\n.align 3\napp_%d_start:\n.incbin \"%s%s\"\napp_%d_end:", i, i, i, TARGET_PATH, apps[i], i);
        free(apps[i]);
    }
    //关闭link_app.S
    fclose(f);
}

int main() {
    insert_app_data();
    return 0;
}
