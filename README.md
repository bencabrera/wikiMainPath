# WikiMainPath - Extracting the Main Path of Historic Events From Wikipedia

This repository contains source code related to the project presented in the scientific paper 

```Cabrera, B., König, B. (2017). Extracting the Main Path of Historic Events From Wikipedia. Proceedings of the Third European Network Intelligence Conference, ENIC 2017, Duisburg, Germany, September 11-12, 2017 [to appear].```

For bug reports and questions please contact ```benjamin.cabrera (at) uni-due.de```.

## Abstract of Paper

The large online encyclopedia of Wikipedia has become a valuable information resource. However, its large size and the interconnectedness of its pages can make it easy to get lost in detail and difficult to gain a good overview of a topic. As a solution we propose a procedure to extract, summarize and visualize large categories of historic Wikipedia articles. At the heart of this procedure we apply the method of main path analysis originally developed for citation networks to a modified network of linked Wikipedia articles. Beside the aggregation method itself, we describe our data mining process of the Wikipedia datasets and the considerations that guided the visualization of the article networks. Finally, we present our web app that allows to experiment with the procedure on an arbitrary Wikipedia category.

## Demo

A running version of the web app can be found under [http://wikimainpath.inf.uni-due.de/](http://wikimainpath.inf.uni-due.de/). However, if you want to either the parsers and/or the (backend of the) web app you have to compile the C++ sources yourself. 
This readme is meant to provide a brief description on the compilation process (as tested on Ubuntu 16.04).

## Dependencies

Th project depends on several libraries which are detailed below. 

### Parsers
- Several [boost libraries](http://www.boost.org/) including *boost.spirit, boost.program_options, boost.algorithm, boost.filesystem*
- [Apache Xerces XML parser for C/C++](http://xerces.apache.org/)
- Our [Wikipedia XML-dump library based on Xerces](https://github.com/bencabrera/wiki_xml_dump_xerces)

### HTTP Backend
- Several [boost libraries](http://www.boost.org/) including *boost.program_options, boost.algorithm, boost.filesystem*
- Several [POCO](https://pocoproject.org/) libraries including *Poco::Util, Poco::Net, Poco::XML, Poco::Foundation*
- Our [Main Path Analysis library](https://github.com/bencabrera/main_path_analysis)

## Compilation process (as performend under a fresh Ubuntu 16.04 installation)

The following steps should guide you through the compilation process of all components (parser and backend). Of course if you only need one you can skip some dependencies

#### 0. Preliminaries
If not already installed install the necessary packages
```
sudo apt install git libboost-all-dev libxerces-c-dev cmake
```

#### 1. Clone repository and initialize submodules

Clone the repository via SSH
```
$ git clone git@github.com:bencabrera/wikiMainPath.git
```

Initialize and update submodules
```
$ git submodule update --init --recursive
```

#### 2. Install POCO library 

Unfortunately because of [some bug](https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=856192) (tested Sep 14, 2017) the official package ```libpoco-dev``` is missing the POCO JSON library which is used by our backend. 

If the bugs has been fixed you can try:
```
sudo apt install git libpoco-dev
```

otherwise you have to compile the POCO libraries from source. To this end go to [their website](https://pocoproject.org/), download and extract the current source code. Go to its directory and run 
```
./configure
make
sudo make install
```

#### 3. Compile everything

Go back to the wikiMainPath repository folder. Now create a new folder (e.g. bin) in which to run cmake targeting the src folder. Finally run make. This could for example look like
```
cd [wikiMainPath_repository]
mkdir bin
cd bin
cmake ../src
make
```

Now everything should be compiled and you should find several binaries in the bin folder. How to use them is described in the following section.

## Usage

The three binaries that start with s1, s2, and s3 are part of the parsing process and have to be executed in ascending order. Each of them can be run with the --help argument to show the kind of input they need.
The following two arguments are the most important ones.
```
  --input-xml-folder arg The folder that should be scanned for wikidump .xml files.
  --output-folder arg    The folder in which the results (articlesWithDates.txt, categories.txt, redirects.txt) should be stored.
```

Given that we have a folder ```dump``` (in this example assumed in the bin folder) a full parsing could look like the following:
```
./count_pages -i dumps/ -o counts.txt
./s1_articlesAndCategories --input-xml-folder dumps/ --page-counts-file counts.txt --output-folder out
./s2_sortArticlesAndCategories --output-folder out/
./s3_parseAllLinks --input-xml-folder dumps/ --page-counts-file counts.txt --output-folder out
```
Now the extracted datasets can be found in the ```out``` folder.

To now start the backend 
```
./backend out/
```

To run the web app you somehow have to host the src/webapp/ folder (e.g. using an apache webserver).

#### Compatible Wikipedia XML Dumps

Compatible dumps can be found on the [official Wikipedia dump website](https://www.wikidata.org/wiki/Wikidata:Database_download#XML_dumps). Go to XML dumps, pick a date and then choose the dumps named ``` All pages, current versions only.```. Finally you have to decompress all of the bzip2 files.










