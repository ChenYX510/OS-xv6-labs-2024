#include "kernel/types.h"  
#include "kernel/stat.h"  
#include "user/user.h"  
#include "kernel/fs.h"  

// 检查给定路径的文件名是否与目标名称匹配  
int match(char *path, char *name)  
{  
    char *p;  

    // 查找路径中最后一个 '/' 位置  
    for (p = path + strlen(path); p >= path && *p != '/'; p--)  
        ;  
    p++; // 指向文件名的第一个字符  

    // 比较文件名  
    if (strcmp(p, name) == 0)  
        return 1; // 匹配  
    else  
        return 0; // 不匹配  
}  

// 查找指定路径下的目标文件  
void find(char *path, char *name)  
{  
    char buf[512], *p;  
    int fd; // 文件描述符  
    struct dirent de; // 目录项  
    struct stat st; // 状态信息  

    // 打开路径  
    if ((fd = open(path, 0)) < 0)  
    {  
        fprintf(2, "ls: cannot open %s\n", path); // 打开失败  
        return;  
    }  

    // 获取路径的状态信息  
    if (fstat(fd, &st) < 0)  
    {  
        fprintf(2, "ls: cannot stat %s\n", path); // 获取状态失败  
        close(fd);  
        return;  
    }  

    switch (st.type) // 根据路径类型进行处理  
    {  
    case T_FILE: // 文件  
        if (match(path, name)) // 检查文件名  
        {  
            printf("%s\n", path); // 打印匹配的文件路径  
        }  
        break;  

    case T_DIR: // 目录  
        // 检查路径长度是否超出限制  
        if (strlen(path) + 1 + DIRSIZ + 1 > sizeof buf)  
        {  
            printf("ls: path too long\n"); // 路径过长  
            break;  
        }  
        strcpy(buf, path); // 拷贝路径到缓冲区  
        p = buf + strlen(buf); // 指向路径末尾  
        *p++ = '/'; // 添加 '/' 到路径末尾  

        // 读取目录的每个条目  
        while (read(fd, &de, sizeof(de)) == sizeof(de))  
        {  
            if (de.inum == 0) // 跳过空条目  
                continue;  
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) // 跳过当前和父目录  
                continue;  

            // 将目录项名称拷贝到路径中  
            memmove(p, de.name, DIRSIZ);  
            p[DIRSIZ] = 0; // 结束字符串  

            // 递归调用查找函数  
            find(buf, name);  
        }  
        break;  
    }  
    close(fd); // 关闭文件描述符  
}  

// 主函数  
int main(int argc, char *argv[])  
{  
    if (argc < 3) // 检查参数数量  
    {  
        printf("argc is %d and it is less than 3\n", argc); // 参数不足  
        exit(1);  
    }  
    find(argv[1], argv[2]); // 调用查找函数  
    exit(0);  
}