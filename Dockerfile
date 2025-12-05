FROM emscripten/emsdk:latest

RUN npm install -g http-server 

RUN wget https://github.com/Kitware/CMake/releases/download/v3.27.9/cmake-3.27.9-linux-x86_64.sh \
 && chmod +x cmake-3.27.9-linux-x86_64.sh \
 && ./cmake-3.27.9-linux-x86_64.sh --skip-license --prefix=/usr/local

WORKDIR /project

COPY . .

RUN rm -rf build

RUN mkdir build
WORKDIR /project/build

RUN emcmake cmake .. \
    && cmake --build .

EXPOSE 8080

WORKDIR /project/build/editor

CMD ["http-server", ".", "-p", "8080", "-a", "0.0.0.0"]