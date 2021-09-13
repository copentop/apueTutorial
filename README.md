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