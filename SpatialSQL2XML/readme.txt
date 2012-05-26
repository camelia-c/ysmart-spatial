Readme 

Suppose we have the following folders structure:
 |--SpatialSQL2XML
 |---- YSmartSpatial.g
 |---- YSmartSpatialMain.c
 |---- antlr-3.3.tar.gz
 |---- antlr-3.3 (after preparing the environment)
 |--test
 |---- Q.sql
 |---- output


0) Preparing the environment - one-time setup

tar xzvf antlr-3.3.tar.gz

cd ./antlr-3.3/runtime/C/dist

tar xzvf libantlr3c-3.1.4-SNAPSHOT.tar.gz

cd libantlr3c-3.1.4-SNAPSHOT/

./configure --prefix=/ANTLR3_HOME

make install 


1) Generating the executable

cd ./SpatialSQL2XML

java -Xms32m -Xmx512m -jar antlr-3.3/lib/antlr-3.3-complete.jar YSmartSpatial.g

gcc -m32 -o YSmartSpatialFront.exe  YSmartSpatialMain.c YSmartSpatialLexer.c YSmartSpatialParser.c /ANTLR3_HOME/lib/libantlr3c.a -I . -I /ANTLR3_HOME/include


2) Testing the module

./YSmartSpatialFront.exe ../test/Q.sql > ../test/output/Q_XML.xml