PSB
===

[P]oisson[S]olver[Benchmarks].

# To build:
```bash
cd build && cmake ../src && make
../bin/main
```


# Images
To get the images: 
```bash
mkdir "./media" && wget http://pascal.inrialpes.fr/data/holidays/jpg{1,2}.tar.gz && tar -xvf jpg{1,2}.tar.gz --strip-components=1 -C media
```
