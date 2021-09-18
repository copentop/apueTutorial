# apueTutorial
apue 3e tutorial


#### docker

build
```
docker build -t gcc:20.04 .
```

run
```
docker run -it --rm --mount type=bind,source=F:/git/docker-c,target=/tmp/workdir --name gcc_ubu gcc:20.04 bash
```

#### 编译

```
cd 01
gcc 04_ls.c ../apue.c
./a.out .
```

#### 文档



[官方文档 man](https://man7.org/linux/man-pages/dir_section_2.html)

man section

```shell
1  Standard commands 标准命令
2  System calls  系统调用
3  Library functions 库函数
4  Special devices  设备说明
5  File formats  文件格式
6  Games and toys  游戏和娱乐
7  Miscellaneous  杂项
8  Administrative Commands 管理员命令
9  其他(Linux特定的) 存放内核例行程序的文档
```

