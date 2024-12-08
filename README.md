# dbintrospect


## How to build

### Ubuntu

Prerequisites:
```
sudo apt install fuse libfuse3-dev
```

Run:
```
make dbintro
```


### Inside Docker

Prepare image:
```
docker build docker -t fuse3
```

Start:
```
docker run -it --device /dev/fuse  --cap-add SYS_ADMIN  --privileged  -v.:/bld fuse3:latest   /bin/bash
```

And then `cd /bld` inside container and you are in this directory but inside Docker container

