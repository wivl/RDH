# RDH

# 简体中文

此仓库是一篇[论文](https://link.springer.com/article/10.1007/s11042-022-12364-7)的 C 语言简单实现。

论文讨论了一种在 8bit 位深的灰度图片中隐藏两个水印的方法，并且此过程是可逆的。其中使用了直方图位移和翻转特定比特的方法。

本仓库在此论文的基础上对流程进行了修改，同时由于选择了最简单的直方图位移法，在提取信息步骤需要额外的参数传递步骤 (隐藏信息的容量、P 值和 Z 值)。并且只在严格的正方形图像测试过，并且仅支持 png 文件格式。测试环境为 macOS。

使用了 [lodepng 库](https://github.com/lvandeve/lodepng)

## 使用方法

### 编译

使用 CMake:

```
cd src
mkdir build
cd build
cmake ..
make
```

生成名为 <code>rdh</code> 的可执行文件

### 生成 key

以 sample 文件夹中的 <code>lena</code> 图为例，将 <code>lena.png</code> 移动到 build 文件夹

```
./rdh -g -i lena.png
```

生成适用于 指定图片的 key 图片

### 隐藏信息和加密图像

需要创建两个分别写有水印 <code>We</code> 和 <code>Ws</code> 的文件 <code>message.txt</code> <code>watermark.txt</code>

```
./rdh -e -i lena.png -k key.png
```
使用 key 进行加密，默认从两个水印文件 <code>message.txt</code> <code>watermark.txt</code> 中读取水印

或者你也可以指定两个水印的文件名

```
./rdh -e -i lena.png -k key.png -w watermark.txt -m message.txt
```

执行后在命令行中会有隐藏的 watermark 的字节数和在提取步骤中需要用到的**四组** <code>cap</code>，<code>p</code> 和 <code>z</code> 值

处理后的图像将保存为 <code>processed.png</code>

### 提取信息和恢复图像

使用和加密相同的 key 文件

```
./rdh -d -i processed.png -k key.png
```

默认的保存两个水印的文件为 <code>message.txt</code> <code>watermark.txt</code>

同样的，可以指定两个水印保存的文件

```
./rdh -d -i processed.png -k key.png -w watermark.txt -m message.txt
```

# English

This repository is a simple implementation in C of a [paper](https://link.springer.com/article/10.1007/s11042-022-12364-7).

The paper discusses a method for hiding two watermarks in an 8bit bit deep grayscale image, and the process is reversible. A histogram shift and a method of flipping specific bits are used.

This repository modifies the process based on this paper, and as the simplest histogram shift method is chosen, an additional parameter passing step is required for the information extraction step (volume, P-value and Z-value of the hidden information). It has only been tested on strictly square images, and only supports the png file format. The test environment was macOS.

Uses the [lodepng library](https://github.com/lvandeve/lodepng)

## Usage

### Compile

Using CMake:

```
cd src
mkdir build
cd build
cmake ..
make
```

Generate an executable file called <code>rdh</code>

### Generate key image

Using lena image in the sample folder as example. Move the lena png to the build folder.


```
./rdh -g -i lena.png
```

Generate a key image that applies to the specified image

### Hide information and encrypt images

You need to create two separate files <code>message.txt</code> <code>watermark.txt</code> contain two watermarks <code>We</code> and <code>Ws</code>

```
./rdh -e -i lena.png -k key.png
```
Use the key for encryption and by default read the watermark from the two watermark files <code>message.txt</code> <code>watermark.txt</code>

```
./rdh -e -i lena.png -k key.png -w watermark.txt -m message.txt
```

When executed, the command line will contain the number of bytes of the hidden watermark and the **four sets** of <code>cap</code>, <code>p</code> and <code>z</code> values to be used in the extraction step

The processed image will be saved as <code>processed.png</code>

### Extraction of information and recovery of images

Use the same key file as for encryption

```
./rdh -d -i processed.png -k key.png
```

The default file for saving the two watermarks is <code>message.txt</code> <code>watermark.txt</code>

Similarly, two watermark saving files can be specified

```
./rdh -d -i processed.png -k key.png -w watermark.txt -m message.txt
```