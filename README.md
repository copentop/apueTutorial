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

