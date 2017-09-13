# WikiMainPath - Extracting the Main Path of Historic Events From Wikipedia

This repository contains source code related to the project presented in the scientific paper 

```Cabrera, B., König, B. (2017). Extracting the Main Path of Historic Events From Wikipedia. Proceedings of the Third European Network Intelligence Conference, ENIC 2017, Duisburg, Germany, September 11-12, 2017 [to appear].```

## Abstract of Paper

The large online encyclopedia of Wikipedia has become a valuable information resource. However, its large size and the interconnectedness of its pages can make it easy to get lost in detail and difficult to gain a good overview of a topic. As a solution we propose a procedure to extract, summarize and visualize large categories of historic Wikipedia articles. At the heart of this procedure we apply the method of main path analysis originally developed for citation networks to a modified network of linked Wikipedia articles. Beside the aggregation method itself, we describe our data mining process of the Wikipedia datasets and the considerations that guided the visualization of the article networks. Finally, we present our web app that allows to experiment with the procedure on an arbitrary Wikipedia category.

## Usage

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